/* $OpenXM$ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "oxserv.h"


#define DPRINTF(x)	printf x; fflush(stdout)

#define delete_cmo(m)

#define OXSERV_INIT_STACK_SIZE 2048
#define OXSERV_EXT_STACK_SIZE  2048

#if 0
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
 * Global Variables.
 *===========================================================================*/
/* cmo stack */
static int G_ox_stack_size = 0;
static int G_ox_stack_pointer = 0;
static cmo **G_ox_stack = NULL;

/* ox */
static cmo_mathcap *G_oxserv_mathcap = NULL;

/* User Function */
static cmo *(*G_userExecuteFunction)(const char *, cmo **, int) = NULL;
static cmo *(*G_userExecuteStringParser)(const char *) = NULL;

static cmo *(*G_convertCmo)(cmo *) = NULL; /* convert user object ==> cmo */

/*===========================================================================*
 * CMO STACK FUNCTIONs
 *===========================================================================*/
/*****************************************************************************
 * return the number of cmo in the stack.
 * PARAM   : NONE
 * RETURN  : the number of cmo in the stack.
 *****************************************************************************/
#define oxserv_get_stack_pointer()  G_ox_stack_pointer


/*****************************************************************************
 * initialize stack.
 * PARAM   : NONE
 * RETURN  : if success return OXSERV_SUCCESS, else OXSERV_FAILURE.
 *****************************************************************************/
static int
oxserv_init_stack(void)
{
	free(G_ox_stack);

	G_ox_stack_pointer = 0;
	G_ox_stack_size = OXSERV_INIT_STACK_SIZE;
	G_ox_stack = (cmo **)malloc(G_ox_stack_size * sizeof(cmo *));
	if (G_ox_stack == NULL) {
		DPRINTF(("server: %d: %s\n", errno, strerror(errno)));
		return (OXSERV_FAILURE);
	}
	return (OXSERV_SUCCESS);
}

/*****************************************************************************
 * 
 * PARAM   : NONE
 * RETURN  : if success return OXSERV_SUCCESS, else OXSERV_FAILURE.
 *****************************************************************************/
static int
oxserv_extend_stack(void)
{
	int size2 = G_ox_stack_size + OXSERV_EXT_STACK_SIZE;
	cmo **stack2 = (cmo **)malloc(size2 * sizeof(cmo *));
	if (stack2 == NULL) {
		DPRINTF(("server: %d: %s\n", errno, strerror(errno)));
		return (OXSERV_FAILURE);
	}

	memcpy(stack2, G_ox_stack, G_ox_stack_size * sizeof(cmo *));
	free(G_ox_stack);

	G_ox_stack = stack2;
	G_ox_stack_size = size2;

	return (OXSERV_SUCCESS);
}

/*****************************************************************************
 * push a cmo onto the topof the stack.
 * PARAM   : m  : the cmo to be pushed on the stack.
 * RETURN  : if success return OXSERV_SUCCESS, else OXSERV_FAILURE.
 *****************************************************************************/
static int
oxserv_push(cmo *m)
{
	int ret;

	if (G_ox_stack_pointer >= G_ox_stack_size) {
		ret = oxserv_extend_stack();
		if (ret != OXSERV_SUCCESS)
			return (ret);
	}

	G_ox_stack[G_ox_stack_pointer] = m;
	G_ox_stack_pointer++;

	return (OXSERV_SUCCESS);
}

/*****************************************************************************
 * remove thd CMO at the top of this stack and
 * returns that cmo as the value of this function.
 * PARAM   : NONE
 * RETURN  : CMO at the top of the stack.
 *****************************************************************************/
static cmo *
oxserv_pop(void)
{
	cmo *c;
	if (G_ox_stack_pointer > 0) {
		G_ox_stack_pointer--;
		c = G_ox_stack[G_ox_stack_pointer];
		G_ox_stack[G_ox_stack_pointer] = NULL;
		return (c);
	}
	return (NULL);
}

/*****************************************************************************
 * return the cmo at the specified position in the stack without removing it from the stack.
 * PARAM : NONE
 * RETURN: thd cmo at the specified position in the stack.
 *****************************************************************************/
static cmo *
oxserv_get(int i)
{
	if (i < G_ox_stack_pointer && i >= 0) {
		return (G_ox_stack[i]);
	}
	return (NULL);
}


/*****************************************************************************
 * return the cmo at the top of the stack without removing it from the stack.
 * PARAM : NONE
 * RETURN: the cmo at the top of the stack.
 *****************************************************************************/
static cmo *
oxserv_peek(void)
{
	return (oxserv_get(G_ox_stack_pointer - 1));
}

/*****************************************************************************
 *
 * defined for SM_pops
 * 
 * PARAM : n : remove count
 * RETURN: NONE
 *****************************************************************************/
static void
oxserv_remove(int n)
{
	int i;
	cmo *m;

	if (n > G_ox_stack_size)
		n = G_ox_stack_size;

	for (i = 0; i < n; i++) {
		--G_ox_stack_pointer;
		m = G_ox_stack[G_ox_stack_pointer];
		G_ox_stack[G_ox_stack_pointer] = NULL;
		delete_cmo(m);
	}
}


/*===========================================================================*
 * OX_SERVER
 *===========================================================================*/

/*****************************************************************************
 * pop n and to discard element from the stack
 * PARAM : fd : OXFILE
 * RETURN: NONE
 *****************************************************************************/
static void
oxserv_sm_popCMO(OXFILE *fd)
{
	cmo *m = oxserv_pop();
	if (m == NULL) {
		m = new_cmo_null();
	} else if (G_convertCmo) {
		m = G_convertCmo(m);
	}

	send_ox_cmo(fd, m);

	delete_cmo(m);
}

/*****************************************************************************
 * pop an cmo from stack, convert it into a string according to the
 * output format of the local system, and send the string.
 * PARAM : fd : OXFILE
 * RETURN: NONE
 *****************************************************************************/
static void
oxserv_sm_popString(OXFILE *fd)
{
	char *str;
	cmo *m = oxserv_pop();
	cmo_string *m_str;

	if (m == NULL)
		m = new_cmo_null();

	str = new_string_set_cmo(m);

	m_str = new_cmo_string(str);

	send_ox_cmo(fd, (cmo *)m_str);

	delete_cmo(m);
	delete_cmo(m_str);

/* free(str); */
}

/*****************************************************************************
 * pop n and to discard element from the stack
 * PARAM : fd : OXFILE
 * RETURN: NONE
 *****************************************************************************/
static void
oxserv_sm_pops()
{
	cmo_int32 *c;

	c = (cmo_int32 *)oxserv_pop(); /* suppose */

	oxserv_remove(c->i)

	delete_cmo(c);
}

/*****************************************************************************
 * for SM_getsp
 *
 * PARAM : fd : OXFILE
 * RETURN: NONE
 *****************************************************************************/
static void
oxserv_sm_getsp()
{
	cmo_int32 *m = new_cmo_int32(oxserv_get_stack_pointer());
	oxserv_push((cmo *)m);
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

	delete_cmo(G_oxserv_mathcap);

	G_oxserv_mathcap = mathcap_get(new_mathcap());
}

static void
oxserv_sm_mathcap()
{
	if (G_oxserv_mathcap == NULL) {
		oxserv_mathcap_init(0, "", "oxserv", NULL, NULL);
	}

	oxserv_push((cmo *)G_oxserv_mathcap);
}

static void
oxserv_sm_executeStringByLocalParserInBatchMode(void)
{
	cmo_string *str = (cmo_string *)oxserv_peek();
	cmo *c;
	c = G_userExecuteStringParser(str->s);
	oxserv_push(c);
}

static void
oxserv_sm_executeStringByLocalParser(void)
{
	cmo_string *str = (cmo_string *)oxserv_pop();
	cmo *c;
	c = G_userExecuteStringParser(str->s);
	oxserv_push(c);
}



static void
oxserv_sm_executeFunction(void)
{
	int i;
	cmo_string *name = (cmo_string *)oxserv_pop();
	cmo_int32 *cnt = (cmo_int32 *)oxserv_pop();
	cmo **arg = (cmo **)malloc(cnt->i * sizeof(cmo *));
	cmo *ret;

	for (i = 0; i < cnt->i; i++) {
		arg[i] = oxserv_pop();
	}

	/* user function */
	ret = G_userExecuteFunction(name->s, arg, cnt->i);

	oxserv_push(ret);

	for (i = 0; i < cnt->i; i++) {
		delete_cmo(arg[i]);
	}

	delete_cmo(name);
	delete_cmo(cnt);

	free(arg);
}

static void
oxserv_sm_pushCMOtag()
{
	cmo *c = oxserv_peek();
	cmo_int32 *tag = new_cmo_int32(c->tag);

	oxserv_push((cmo *)tag);
}


static void
oxserv_sm_dupErrors()
{
	cmo_list *list;
	cmo *c;
	int i;

	list = new_cmo_list();

	for (i = 0; i < oxserv_get_stack_pointer(); i++) {
		c = oxserv_get(i);
		if (c->tag == CMO_ERROR2) {
			list_append(list, c);
		}
	}

	oxserv_push((cmo *)list);
}


static int
oxserv_receive_and_execute_sm_command(OXFILE *fd)
{
	int code = receive_int32(fd);

	DPRINTF(("SM_CODE=%d=0x%x\n", code, code));

	switch (code) {
	case SM_popCMO:
		oxserv_sm_popCMO(fd);
		break;
	case SM_executeStringByLocalParser:
		if (G_userExecuteStringParser)
			oxserv_sm_executeStringByLocalParser();
		break;
	case SM_executeStringByLocalParserInBatchMode:
		if (G_userExecuteStringParser)
			oxserv_sm_executeStringByLocalParserInBatchMode();
		break;
	case SM_pops:
		oxserv_sm_pops();
		break;
	case SM_popString:
		oxserv_sm_popString(fd);
		break;
	case SM_getsp:
		oxserv_sm_getsp();
		break;
	case SM_mathcap:
		oxserv_sm_mathcap();
		break;
	case SM_setMathCap:
		/* dont support */
		oxserv_pop();
		break;
	case SM_executeFunction:
		if (G_userExecuteFunction)
			oxserv_sm_executeFunction();
		break;
	case SM_pushCMOtag:
		oxserv_sm_pushCMOtag();
		break;
	case SM_dupErrors:
		oxserv_sm_dupErrors();
		break;
	default:
		break;
	}
	return (OXSERV_SUCCESS);
}


/*****************************************************************************
 * reveice ox_data
 * PARAM : fd : OXFILE
 * RETURN: NONE
 *****************************************************************************/
int
oxserv_receive(OXFILE *fd)
{
	int tag;
	cmo *c;
	int ret = OXSERV_SUCCESS;

	tag = receive_ox_tag(fd);

	DPRINTF(("oxserv_TAG=%d=0x%x\n", tag, tag));
	switch (tag) {
	case OX_DATA:
		c = receive_cmo(fd);
		DPRINTF(("CMO_TAG=%d=0x%x\n", c->tag, c->tag));
		oxserv_push(c);
		break;

	case OX_COMMAND:
		ret = oxserv_receive_and_execute_sm_command(fd);
		break;

	default:
		DPRINTF(("receive unknown ox_tag: %d=0x%x\n", tag, tag));
		return (OXSERV_FAILURE);
	}

	return (ret);
}


/*****************************************************************************
 * initialize oxserver
 *
 * PARAM : see oxserv_mathcap_init()
 * RETURN: NONE
 * SEE   : oxserv_mathcap_init()
 *       : oxserv_set();
 *****************************************************************************/
int
oxserv_init(OXFILE *oxfp, int ver, char *vstr, char *sysname, int *cmos, int *sms)
{
	int ret;

	ret = oxserv_init_stack();
	if (ret != OXSERV_SUCCESS)
		return (ret);

	oxserv_mathcap_init(ver, vstr, sysname, cmos, sms);

	oxf_determine_byteorder_server(oxfp);

	return (OXSERV_SUCCESS);
}


/*****************************************************************************
 * set oxserver
 * PARAM : mode : mode
 *              :
 *       : ptr  :
 *       : rsv  : reserve space.
 * RETURN: NONE
 * SEE   : 
 *****************************************************************************/
int
oxserv_set(int mode, void *ptr, void *rsv)
{
	switch (mode) {
	case OXSERV_SET_EXECUTE_FUNCTION:
		G_userExecuteFunction = (cmo *(*)(const char *, cmo **, int))ptr;
		break;
	case OXSERV_SET_EXECUTE_STRING_PARSER:
		G_userExecuteStringParser = (cmo *(*)(const char *))ptr;
		break;
	case OXSERV_SET_CONVERT_CMO:
		G_convertCmo = (cmo *(*)(cmo *))ptr;
		break;
	default:
		return (OXSERV_FAILURE);
	}


	return (OXSERV_SUCCESS);
}


/*****************************************************************************
 * destroy
 * PARAM : mode : mode
 *              :
 *       : ptr  :
 *       : rsv  : reserve space.
 * RETURN: NONE
 * SEE   : 
 *****************************************************************************/
void
oxserv_dest()
{
	free(G_ox_stack);
}
	

#if 0

cmo *
oxserv_executeFunction(const char *func, cmo **arg, int argc)
{
	int i;

	printf("%s()\n", func);

	for (i = 0; i < argc; i++) {
		printf("\t%2d: %s\n", i, new_string_set_cmo(arg[i]));
	}

	return ((cmo *)new_cmo_int32(0));
}

int
main(int argc, char *argv[])
{
	int fd = 3;
	int i;
	int ret;

	OXFILE *oxfp = oxf_open(fd);

	ox_stderr_init(stderr);

	oxserv_init(0, "$Date$", "oxserv", NULL, NULL);

	DPRINTF(("main - start\n"));
	for (i = 0;; i++) {
		DPRINTF(("@"));
		ret = oxserv_receive(oxfp);
		if (ret != OXSERV_SUCCESS)
			break;
	}

	oxf_close(fd);
	delete_cmo(G_oxserv_mathcap);

	return (0);
}

#endif
