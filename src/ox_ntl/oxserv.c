/* $OpenXM: OpenXM/src/ox_ntl/oxserv.c,v 1.4 2003/11/17 09:55:52 iwane Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <setjmp.h>

#include "oxserv.h"
#include "oxstack.h"

#include "gc/gc.h"

#define DPRINTF(x)	printf x; fflush(stdout)

#define FP	stdout
#define EPRINTF(x)	fprintf x; fflush(FP)

#if 1
/*===========================================================================*
 * for DEBUG
 *===========================================================================*/
#include <stdarg.h>
void
dprintf(const char *fmt, ...)
{
	FILE *fp;
	va_list ap;
	va_start(ap, fmt);
	
	fp = fopen("error.txt", "a");

	vfprintf(fp, fmt, ap);

	fflush(fp);
	fclose(fp);

	va_end(ap);
}
#endif

/*===========================================================================*
 * MACRO
 *===========================================================================*/
#define oxserv_get_cmo_tag(m)	((G_getCmoTag == NULL) ? m->tag : G_getCmoTag(m))


#define oxserv_delete_cmo(c)         \
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
static void (*G_userExecuteFunction)(const char *, cmo **, int) = NULL;
static void (*G_userExecuteStringParser)(const char *) = NULL;

static cmo *(*G_convertCmo)(cmo *) = NULL; /* convert user object ==> cmo */

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
	oxstack_push((cmo *)new_cmo_error2_string(msg));
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
	cmo *m, *n;
	m = oxstack_pop();
	if (m == NULL) {
		EPRINTF((FP, "stack underflow in popCMO\n"));
		m = new_cmo_null();
	} else if (G_convertCmo) {
		n = G_convertCmo(m);
		if (m != n) {
			oxserv_delete_cmo(m);
			m = n;
		}
	}

	send_ox_cmo(fd, m);

	oxserv_delete_cmo(m);
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
	cmo *m = oxstack_pop();
	cmo_string *m_str;

	if (m == NULL) {
		EPRINTF((FP, "stack underflow in popString\n"));
		m = new_cmo_null();
	}

	str = new_string_set_cmo(m);

	m_str = new_cmo_string(str);

	send_ox_cmo(fd, (cmo *)m_str);

	oxserv_delete_cmo(m);
	oxserv_delete_cmo(m_str);

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
	cmo_int32 *c;
	cmo *m;
	int n;
	int i;

	c = (cmo_int32 *)oxstack_pop(); /* suppose */
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
		
	oxserv_delete_cmo(c);

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
	oxstack_push((cmo *)m);
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

	oxserv_delete_cmo(G_oxserv_mathcap);

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

	oxstack_push((cmo *)G_oxserv_mathcap);
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
	cmo_string *str = (cmo_string *)oxstack_peek();
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
	cmo_string *str = (cmo_string *)oxstack_pop();
	if (str == NULL) {
		oxserv_push_errormes("stack underflow in executeStringByLocalParser");
		return ;
	}
	G_userExecuteStringParser(str->s);
}



/*****************************************************************************
 * -- SM_executeFunction --
 * pop s as a function name, pop n as the number of arguments and to execute a
 * local function s with n arguments poped from the stack.
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
	cmo_string *name = (cmo_string *)oxstack_pop();
	cmo_int32 *cnt = (cmo_int32 *)oxstack_pop();
	cmo **arg;


	if (name == NULL || cnt == NULL) {
		oxserv_push_errormes("stack underflow in executeFunction");
		return ;
	}

	arg = (cmo **)malloc(cnt->i * sizeof(cmo *));
	for (i = 0; i < cnt->i; i++) {
		arg[i] = oxstack_pop();
		if (arg[i] == NULL) {
			oxserv_push_errormes("stack underflow in executeFunction");

			for (i--; i >= 0; i--)
				oxserv_delete_cmo(arg[i]);
			free(arg);
			return ;
		}
	}

	/* user function */
	G_userExecuteFunction(name->s, arg, cnt->i);


	for (i = 0; i < cnt->i; i++) {
		oxserv_delete_cmo(arg[i]);
	}

	oxserv_delete_cmo(name);
	oxserv_delete_cmo(cnt);

	free(arg);
}

/*****************************************************************************
 * -- SM_pushCMOtag --
 * push the CMO tag of the top object on the stack.
 *
 * PARAM : NONE
 * RETURN: NONE
 *****************************************************************************/
static void
oxserv_sm_pushCMOtag()
{
	cmo *c = oxstack_peek();
	cmo_int32 *tag = new_cmo_int32(oxserv_get_cmo_tag(c));
	oxstack_push((cmo *)tag);
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
	cmo_list *list;
	cmo *c;
	int i;

	list = new_cmo_list();

	for (i = 0; i < oxstack_get_stack_pointer(); i++) {
		c = oxstack_get(i);
		if (c->tag == CMO_ERROR2) {
			list_append(list, c);
		}
	}

	oxstack_push((cmo *)list);
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
		if (G_userExecuteFunction)
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

	switch (tag) {
	case OX_DATA:
		BLOCK_INPUT();
		c = receive_cmo(fd);
		UNBLOCK_INPUT();
		DPRINTF(("[CMO:%d=0x%x]", c->tag, c->tag));
		oxstack_push(c);
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

	ret = setjmp(G_jmpbuf);
	if (ret == 0) {
		DPRINTF(("setjmp first time -- %d\n", ret));
	} else {
		DPRINTF(("setjmp return from longjmp() -- %d -- \n", ret));
	}

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
		G_userExecuteFunction = (void (*)(const char *, cmo **, int))ptr;
		break;
	case OXSERV_SET_EXECUTE_STRING_PARSER:
		G_userExecuteStringParser = (void (*)(const char *))ptr;
		break;
	case OXSERV_SET_CONVERT_CMO:
		G_convertCmo = (cmo *(*)(cmo *))ptr;
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
	oxserv_delete_cmo(G_oxserv_mathcap);

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
int
main(int argc, char *argv[])
{
	int fd = 3;
	int i;
	int ret;

	OXFILE *oxfp = oxf_open(fd);

	ox_stderr_init(stderr);

        oxserv_set(OXSERV_SET_EXECUTE_FUNCTION, oxserv_executeFunction, NULL);

	oxserv_init(oxfp, 0, "$Date$", "oxserv", NULL, NULL);

	ret = oxserv_receive(oxfp);

	oxserv_dest();
	oxf_close(oxfp);

	return (0);
}

#endif


