/* $OpenXM: OpenXM/src/ox_ntl/oxserv.c,v 1.8 2008/09/19 10:55:40 iwane Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <setjmp.h>

#include "oxserv.h"
#include "oxstack.h"

#include "gc/gc.h"

#define DPRINTF(x)	printf x; (void)fflush(stdout)

#define FP	stdout
#define EPRINTF(x)	fprintf x; (void)fflush(FP)



/*===========================================================================*
 * MACRO
 *===========================================================================*/
#define oxserv_get_cmo_tag(m)	((G_getCmoTag == NULL) ? (m)->tag : G_getCmoTag(m))


#define oxserv_delete_cmo(C)         \
do {                                 \
        if (C != NULL) {             \
		if ((C)->c) { FREE((C)->c); } \
		oxserv_delete_cmo_usr(((C)->p)); \
		oxserv_free(C, 0);             \
		C = NULL;            \
	}                            \
} while (0)

#define oxserv_delete_cmo_usr(c)         \
do {                                 \
        if (c != NULL) {             \
            if (G_DeleteCmo != NULL) \
                G_DeleteCmo((cmo *)c);      \
            else                     \
                c = NULL;            \
        }                            \
} while (0)


/*===========================================================================*
 * Global Variables.
 *===========================================================================*/
/* ox */
static OXFILE *G_oxfilep = NULL;
static cmo_mathcap *G_oxserv_mathcap = NULL;

/* signal */
int		G_oxserv_sigusr1flag = 0;
int		G_oxserv_sigusr1cnt = 0;
static jmp_buf  G_jmpbuf;

/* User Function */
static void (*G_userExecuteFunction)(const char *, oxstack_node **, int) = NULL;
static void (*G_userExecuteStringParser)(const char *) = NULL;

static cmo  *(*G_convertCmo)(void *) = NULL; /* convert user object ==> cmo */
static char *(*G_convertStr)(void *) = NULL; /* convert user object ==> string */

static void (*G_DeleteCmo)(cmo *) = NULL;

static int  (*G_getCmoTag)(cmo *) = NULL;


/*===========================================================================*
 * CMO_ERROR2
 *===========================================================================*/
#define new_cmo_error2_string(msg) new_cmo_error2((cmo *)new_cmo_string(msg))


static void
oxserv_push_errormes(char *msg)
{
	EPRINTF((FP, "%s\n", msg));
	oxstack_push_cmo((cmo *)new_cmo_error2_string(msg));
}

/*===========================================================================*
 * synchronized malloc
 *===========================================================================*/
void *
oxserv_malloc(size_t size)
{
	void *ptr;

	BLOCK_INPUT();
	ptr = GC_malloc(size);
	UNBLOCK_INPUT();

	return (ptr);
}

void
oxserv_free(void *ptr, size_t size)
{
	/* nothing */
}

void *
oxserv_realloc(void *org, size_t old, size_t size)
{
	void *ptr;

	BLOCK_INPUT();
	ptr = GC_realloc(org, size);
	UNBLOCK_INPUT();
	
	return (ptr);
}



/*===========================================================================*
 * OX_SERVER
 *===========================================================================*/

/*****************************************************************************
 * -- SM_popCMO --
 * pop an object from the stack, convert it into a serialized from according
 * to the standard CMO encoding scheme, and send it to the stream
 *
 * PARAM : fd : OXFILE
 * RETURN: NONE
 *****************************************************************************/
static void
oxserv_sm_popCMO(OXFILE *fd)
{
	cmo *m;
	oxstack_node *p;
	int flag = 0;
	p = oxstack_pop();
	if (p == NULL) {
		EPRINTF((FP, "stack underflow in popCMO\n"));
		m = new_cmo_null();
		/* asir の動きに従う */
	} else {
		if (p->c != NULL) {
			m = p->c;
		} else if (p->p != NULL) {
			if (G_convertCmo) {
				m = G_convertCmo(p->p);
			} else {
				m = NULL;
			}
			if (m == NULL) {
				m = (cmo *)new_cmo_error2((cmo *)new_cmo_string("convert failed"));
				flag = 1;

			}
		} else {
			EPRINTF((FP, "converter not defined\n"));
			m = (cmo *)new_cmo_error2((cmo *)new_cmo_string("converter not defined"));
			flag = 1;
		}

		if (flag) {
			/* 
			 * せっかく結果があるのに消してしまうのは
			 * もったいないので残すことにする.
			 */
			oxstack_push(p);
		} else {
			oxserv_delete_cmo(p);
		}
	}


	send_ox_cmo(fd, m);

	FREE(m);
}

/*****************************************************************************
 * -- SM_popString --
 * pop an cmo from stack, convert it into a string according to the
 * output format of the local system, and send the string.
 *
 * PARAM : fd : OXFILE
 * RETURN: NONE
 *****************************************************************************/
static void
oxserv_sm_popString(OXFILE *fd)
{
	char *str;
	oxstack_node *p;
	cmo *m;
	cmo_string *m_str;

	p = oxstack_pop();
	if (p == NULL) {
		/* @@TODO
		 * http://www.math.kobe-u.ac.jp/OpenXM/1.2.1/html/OpenXM-ja/OpenXM/node12.html
		 * スタックが空の場合には (char *)NULL を返す.
		 * CMO の用語で書かれていないから何を返すべきかわからない.
		 */
		m = new_cmo_null();
		str = new_string_set_cmo(m);
	} else {
		if (p->c) {
			m = NULL;
			str = new_string_set_cmo(p->c);
		} else if (p->p) {
			if (G_convertStr) {
				str = G_convertStr(p->p);
				m = NULL;
			} else {
				if (G_convertCmo) {
					m = G_convertCmo(p->p);
				} else {
					/* @@TODO
					 * 変換できない場合は...  CMO_ERROR2 を戻すべきである?
					 * そのときの stack の状態はどうすべき？
					 */
					m = NULL;
				}
				str = new_string_set_cmo(m);
			}
		} else {
			m = new_cmo_null();
			str = new_string_set_cmo(m);
		}

		oxserv_delete_cmo(p);
	}

	m_str = new_cmo_string(str);

	send_ox_cmo(fd, (cmo *)m_str);

	FREE(m);
	FREE(m_str);

	/* free(str); */
}

/*****************************************************************************
 * -- SM_pops --
 * pop n and to discard element from the stack
 *
 * PARAM : NONE
 * RETURN: NONE
 *****************************************************************************/
static void
oxserv_sm_pops()
{
	oxstack_node *p, *m;
	cmo_int32 *c;
	int n;
	int i;

	p = oxstack_pop();
	if (p == NULL) {
		EPRINTF((FP, "stack underflow in pops\n"));
		return ;
	}

	c = (cmo_int32 *)p->c; /* suppose */
	if (c == NULL) {
		EPRINTF((FP, "stack underflow in pops\n"));
		return ;
	}

	n = oxstack_get_stack_pointer();
	if (c->i < n)
		n = c->i;

	for (i = 0; i < n; i++) {
		m = oxstack_pop();
		oxserv_delete_cmo(m);
	}
		
	oxserv_delete_cmo(p);

}

/*****************************************************************************
 * -- SM_getsp --
 * push the current stack pointer onto the stack.
 *
 * PARAM : fd : OXFILE
 * RETURN: NONE
 *****************************************************************************/
static void
oxserv_sm_getsp()
{
	cmo_int32 *m = new_cmo_int32(oxstack_get_stack_pointer());
	oxstack_push_cmo((cmo *)m);
}

/*****************************************************************************
 * 
 * PARAM : ver    : 
 *       : vstr   : 
 *       : sysname:
 *       : cmos   :
 *       : sms    :
 * RETURN: NONE
 * SEE   : oxserv_set();
 *****************************************************************************/
static void
oxserv_mathcap_init(int ver, char *vstr, char *sysname, int *cmos, int *sms)
{
	int i;

	int local_sms[] = {
		SM_popCMO,
		SM_mathcap,
		SM_setMathCap,
		SM_pops,
		SM_getsp,
		SM_popString,
		SM_pushCMOtag,
		0,
		SM_executeFunction,
		SM_dupErrors,
		SM_executeStringByLocalParser,
		SM_executeStringByLocalParserInBatchMode,
		SM_shutdown,
		0,
	};

	/* depend on ox_toolkit */
	int local_cmos[] = {
		CMO_ERROR2,
		CMO_NULL,
		CMO_INT32,
		CMO_STRING,
		CMO_MATHCAP,
		CMO_LIST,
		CMO_MONOMIAL32,
		CMO_ZZ,
		CMO_ZERO,
		CMO_RECURSIVE_POLYNOMIAL,
		CMO_DISTRIBUTED_POLYNOMIAL,
		CMO_POLYNOMIAL_IN_ONE_VARIABLE,
		CMO_DMS_GENERIC,
		CMO_RING_BY_NAME,
		CMO_INDETERMINATE,
		CMO_TREE,
		CMO_LAMBDA,
		0,
		CMO_QQ,
		CMO_ATTRIBUTE_LIST,
		CMO_DMS,
		CMO_DMS_OF_N_VARIABLES,
		CMO_LIST_R,
		CMO_INT32COEFF,
		CMO_RATIONAL,
		CMO_DATUM,
		0,
	};

	if (sms == NULL) {
		sms = local_sms;

		for (i = 0; sms[i] != 0; i++)
			;
		if (G_userExecuteFunction != NULL)
			sms[i++] = SM_executeFunction;

		if (G_userExecuteStringParser != NULL) {
			sms[i++] = SM_executeStringByLocalParser;
			sms[i++] = SM_executeStringByLocalParserInBatchMode;
		}

		sms[i] = 0;
	}
	if (cmos == NULL)
		cmos = local_cmos;

	mathcap_init(ver, vstr, sysname, cmos, sms);

	if (G_oxserv_mathcap) {
		FREE(G_oxserv_mathcap);
	}

	G_oxserv_mathcap = mathcap_get(new_mathcap());
}

/*****************************************************************************
 * -- SM_mathcap --
 * push the mathcap of the server.
 *
 * PARAM : NONE
 * RETURN: NONE
 *****************************************************************************/
static void
oxserv_sm_mathcap()
{
	if (G_oxserv_mathcap == NULL) {
		oxserv_mathcap_init(0, "", "oxserv", NULL, NULL);
	}

	oxstack_push_cmo((cmo *)G_oxserv_mathcap);
}

/*****************************************************************************
 * -- SM_executeStringByLocalParserInBatchMode --
 * peek a character string s, parse it by the local parser of the stack machine,
 * and interpret by the local interpreter.
 *
 * PARAM : NONE
 * RETURN: NONE
 *****************************************************************************/
static void
oxserv_sm_executeStringByLocalParserInBatchMode(void)
{
	oxstack_node *p;
	cmo_string *str;

	p = oxstack_peek();
	if (p == NULL) {
		oxserv_push_errormes("stack underflow in executeStringByLocalParserInBatchMode");
		return ;
	}

	str = (cmo_string *)p->c;
	if (str == NULL) {
		oxserv_push_errormes("stack underflow in executeStringByLocalParserInBatchMode");
		return ;
	}
	G_userExecuteStringParser(str->s);
}

/*****************************************************************************
 * -- SM_executeStringByLocalParser --
 * pop a character string s, parse it by the local parser of the stack machine,
 * and interpret by the local interpreter.
 *
 * PARAM : NONE
 * RETURN: NONE
 *****************************************************************************/
static void
oxserv_sm_executeStringByLocalParser(void)
{
	oxstack_node *p;
	cmo_string *str;

	p = oxstack_pop();
	if (p == NULL) {
		oxserv_push_errormes("stack underflow in executeStringByLocalParserInBatchMode");
		return ;
	}

	str = (cmo_string *)p->c;
	if (str == NULL) {
		oxserv_push_errormes("stack underflow in executeStringByLocalParser");
		return ;
	}
	G_userExecuteStringParser(str->s);
	oxserv_delete_cmo(p);
}



/*****************************************************************************
 * -- SM_executeFunction --
 * pop s as a function name, pop n as the number of arguments and to execute a
 * local function s with n arguments popped from the stack.
 *
 * suppose G_userExecuteFunction not equal NULL
 *
 * PARAM : NONE
 * RETURN: NONE
 *****************************************************************************/
static void
oxserv_sm_executeFunction(void)
{
	int i;
	oxstack_node *p1, *p2;
	cmo_string *name;
	cmo_int32 *cnt;
	int total;
	oxstack_node **arg;

	if (G_userExecuteFunction == NULL) {
		oxserv_push_errormes("G_userExecuteFunction not defined");
		return ;
	}

	p1 = oxstack_pop();
	p2 = oxstack_pop();

	name = (cmo_string *)p1->c;
	cnt = (cmo_int32 *)p2->c;



	if (name == NULL || cnt == NULL) {
		oxserv_push_errormes("stack underflow in executeFunction[name,cnt]");
		return ;
	}
	if (name->tag != CMO_STRING) {
		oxstack_push(p2);
		oxstack_push(p1);
		oxserv_push_errormes("invalid parameter #1 not cmo_string");
		return ;
	}

	if (cnt->tag != CMO_INT32) {
		oxstack_push(p2);
		oxstack_push(p1);
printf("command name=%s\n", name->s);
		oxserv_push_errormes("invalid parameter #2 not cmo_int32");
		return ;
	}

	total = cnt->i;

	arg = (oxstack_node **)oxserv_malloc(total * sizeof(oxstack_node *));
	for (i = 0; i < total; i++) {
		arg[i] = oxstack_pop();
		if (arg[i] == NULL) {
			oxserv_push_errormes("stack underflow in executeFunction");

			for (i--; i >= 0; i--) {
				oxserv_delete_cmo(arg[i]);
			}
			oxserv_free(arg, 0);
			return ;
		}
	}

	/* user function */
	G_userExecuteFunction(name->s, arg, total);


	for (i = 0; i < total; i++) {
		oxserv_delete_cmo(arg[i]);
	}

	oxserv_delete_cmo(p1);
	oxserv_delete_cmo(p2);
	oxserv_free(arg, 0);
}

/*****************************************************************************
 * -- SM_pushCMOtag --
 * push the CMO tag of the top object on the stack.
 *
 *
 *
 * PARAM : NONE
 * RETURN: NONE
 *****************************************************************************/
static void
oxserv_sm_pushCMOtag()
{
	oxstack_node *p = oxstack_peek();
	cmo *c = p->c;
	cmo_int32 *tag;

	if (c == NULL) {
		if (p->p != NULL && G_convertCmo) {
			c = p->c = G_convertCmo(p->p);
		}
	}

	if (c == NULL) {
		tag = new_cmo_int32(oxserv_get_cmo_tag(c));
	} else {
		tag = (cmo_int32 *)new_cmo_error2_string("convert from MathObj to CMO failed");
	}
	oxstack_push_cmo((cmo *)tag);
}


/*****************************************************************************
 * -- SM_dupErrors --
 *
 * PARAM : NONE
 * RETURN: NONE
 *****************************************************************************/
static void
oxserv_sm_dupErrors()
{
	oxstack_node *p;
	cmo_list *list;
	int i;

	list = new_cmo_list();

	for (i = 0; i < oxstack_get_stack_pointer(); i++) {
		p = oxstack_get(i);
		if (p->c && p->c->tag == CMO_ERROR2) {
			list_append(list, p->c);
		}
	}

	oxstack_push_cmo((cmo *)list);
}




/*****************************************************************************
 * -- SM_control_reset_connection -- signal handler for SIGUSR1 --
 *
 * PARAM : NONE
 *
 *       : if (sig == 0) called UNBLOCK_INPUT()
 * RETURN: NONE
 *****************************************************************************/
void
oxserv_sm_control_reset_connection(int sig)
{
	int tag;
	OXFILE *fd = G_oxfilep;

	if (G_oxserv_sigusr1cnt > 0) {
		G_oxserv_sigusr1flag = 1;
		return ;
	}


	DPRINTF(("reset -- start ==> "));
	G_oxserv_sigusr1flag = 0;

	send_ox_tag(fd, OX_SYNC_BALL);

	oxstack_init_stack();

	for (;;) {
		tag = receive_ox_tag(fd);
		DPRINTF(("[OX:%d=0x%x]", tag, tag));
		if (tag == OX_SYNC_BALL)
			break;
		if (tag == OX_DATA)
			receive_cmo(fd);
		else
			receive_int32(fd);
	}
	DPRINTF((" <== end.\n"));


	longjmp(G_jmpbuf, sig);
}

/*****************************************************************************
 * execute sm command
 *
 * PARAM : NONE
 * RETURN: NONE
 *****************************************************************************/
static int
oxserv_execute_sm_command(OXFILE *fd, int code)
{

	DPRINTF(("[SM:%d=0x%x]", code, code));

	switch (code) {
	case SM_popCMO: /* 262 */
		oxserv_sm_popCMO(fd);
		break;
	case SM_executeStringByLocalParser: /* 268 */
		if (G_userExecuteStringParser)
			oxserv_sm_executeStringByLocalParser();
		break;
	case SM_executeStringByLocalParserInBatchMode: /* 274 */
		if (G_userExecuteStringParser)
			oxserv_sm_executeStringByLocalParserInBatchMode();
		break;
	case SM_pops: /* 265 */
		oxserv_sm_pops();
		break;
	case SM_popString: /* 263 */
		oxserv_sm_popString(fd);
		break;
	case SM_getsp: /* 275 */
		oxserv_sm_getsp();
		break;
	case SM_mathcap: /* 264 */
		oxserv_sm_mathcap();
		break;
	case SM_setMathCap: /* 273 */
		/* dont support */
		oxstack_pop();
		break;
	case SM_executeFunction: /* 269 */
		oxserv_sm_executeFunction();
		break;
	case SM_pushCMOtag: /* 277 */
		oxserv_sm_pushCMOtag();
		break;
	case SM_dupErrors: /* 276 */
		oxserv_sm_dupErrors();
		break;
	case SM_popSerializedLocalObject:
	case SM_setName:
	case SM_evalName:
	case SM_beginBlock:
	case SM_endBlock:
	case SM_shutdown:
	case SM_executeFunctionAndPopCMO:
	case SM_executeFunctionAndPopSerializedLocalObject:
	case SM_control_reset_connection:
	case SM_control_reset_connection_server:
	default:
		break;
	}
	return (OXSERV_SUCCESS);
}



/*****************************************************************************
 * reveice ox_data
 *
 * PARAM : fd : OXFILE
 * RETURN: NONE
 *****************************************************************************/
static int
oxserv_ox_receive(OXFILE *fd)
{
	int tag;
	cmo *c;
	int ret = OXSERV_SUCCESS;
	int code;

	tag = receive_ox_tag(fd);
printf("get ox=[%d=0x%x]\n", tag, tag); fflush(stdout);

	switch (tag) {
	case OX_DATA:
		BLOCK_INPUT();
		c = receive_cmo(fd);
		UNBLOCK_INPUT();
		DPRINTF(("[CMO:%d=0x%x]", c->tag, c->tag));
		oxstack_push_cmo(c);
		break;

	case OX_COMMAND:
		code = receive_int32(fd);
		ret = oxserv_execute_sm_command(fd, code);
		break;

	default:
		DPRINTF(("receive unknown ox_tag: %d=0x%x\n", tag, tag));
		return (OXSERV_FAILURE);
	}

	return (ret);
}

int
oxserv_receive(OXFILE *fd)
{
	int i = 0;
	int ret;


	/*-----------------------------------------*
	 * initialize
	 *-----------------------------------------*/

	ret = setjmp(G_jmpbuf);
	if (ret == 0) {
		DPRINTF(("setjmp first time -- %d\n", ret));
	} else {
		DPRINTF(("setjmp return from longjmp() -- %d -- \n", ret));
	}

	/*-----------------------------------------*
	 * main loop
	 *-----------------------------------------*/
	for (;; i++) {
		ret = oxserv_ox_receive(fd);
		if (ret != OXSERV_SUCCESS)
			break;
	}

	return (ret);
}


/*****************************************************************************
 * initialize oxserver
 *
 * PARAM : see oxserv_mathcap_init()
 * RETURN: success : OXSERV_SUCCESS
 *       : failure : OXSERV_FAILURE
 * SEE   : oxserv_mathcap_init()
 *       : oxserv_set();
 *****************************************************************************/
int
oxserv_init(OXFILE *oxfp, int ver, char *vstr, char *sysname, int *cmos, int *sms)
{
	int ret;

	DPRINTF(("init start\n"));

	ret = oxstack_init_stack();
	if (ret != OXSERV_SUCCESS)
		return (ret);

	G_oxfilep = oxfp;

	oxserv_mathcap_init(ver, vstr, sysname, cmos, sms);

	signal(SIGUSR1, oxserv_sm_control_reset_connection);

	/* initialize GMP memory functions. */
	mp_set_memory_functions(oxserv_malloc, oxserv_realloc, oxserv_free);

	/* session start */
	oxf_determine_byteorder_server(oxfp);

	return (OXSERV_SUCCESS);
}


/*****************************************************************************
 * set oxserver behavior
 *
 * PARAM : mode : mode
 *              :
 *       : ptr  :
 *       : rsv  : reserve space.
 * RETURN: success : OXSERV_SUCCESS
 *       : failure : OXSERV_FAILURE
 * SEE   : 
 *****************************************************************************/
int
oxserv_set(int mode, void *ptr, void *rsv)
{
	switch (mode) {
	case OXSERV_SET_EXECUTE_FUNCTION:
		G_userExecuteFunction = (void (*)(const char *, oxstack_node **, int))ptr;
		break;
	case OXSERV_SET_EXECUTE_STRING_PARSER:
		G_userExecuteStringParser = (void (*)(const char *))ptr;
		break;
	case OXSERV_SET_CONVERT_CMO:
		G_convertCmo = (cmo *(*)(void *))ptr;
		break;
	case OXSERV_SET_CONVERT_STR:
		G_convertStr = (char *(*)(void *))ptr;
		break;
	case OXSERV_SET_DELETE_CMO:
		G_DeleteCmo = (void (*)(cmo *))ptr;
		break;
	case OXSERV_SET_GET_CMOTAG:
		G_getCmoTag = (int (*)(cmo *))ptr;
		break;
	default:
		return (OXSERV_FAILURE);
	}


	return (OXSERV_SUCCESS);
}


/*****************************************************************************
 * destroy
 *
 * PARAM : NONE
 * RETURN: NONE
 *****************************************************************************/
void
oxserv_dest()
{
	FREE(G_oxserv_mathcap);
	oxstack_dest();
}
	

#if __OXSERV_DEBUG
/*===========================================================================*
 * DEBUG
 *===========================================================================*/


void
oxserv_executeFunction(const char *func, cmo **arg, int argc)
{
	int i;

	printf("%s()\n", func);

	for (i = 0; i < argc; i++) {
		printf("\t%2d: %s\n", i, new_string_set_cmo(arg[i]));
	}

	return ;
}

/*****************************************************************************
 * main
 *
 * PARAM : NONE
 * RETURN: NONE
 *****************************************************************************/
static FILE *dfp = NULL;
int
main(int argc, char *argv[])
{
	int fd = 10;

	dfp = fopen("/tmp/oxserv,log", "w");
	OXFILE *oxfp = oxf_open(fd);

	fprintf(dfp, "stderr.init start\n"); fflush(dfp);
	ox_stderr_init(dfp);

	fprintf(dfp, "set start\n"); fflush(dfp);
	oxserv_set(OXSERV_SET_EXECUTE_FUNCTION, oxserv_executeFunction, NULL);

	fprintf(dfp, "init start\n"); fflush(dfp);
	oxserv_init(oxfp, 0, "$Date$", "oxserv", NULL, NULL);

	fprintf(dfp, "recv start\n"); fflush(dfp);
	oxserv_receive(oxfp);

	oxserv_dest();
	oxf_close(oxfp);

	return (0);
}

#endif


