/* $OpenXM$ */
/************************************************************************
 *
 * float に対応できていない.
 *
 *
 * 以下対応したもの
 *  - 整数, 多項式, リスト
 * 
 *
 *********************************************************************** */

#include <stdio.h>
#include <string.h>

#include "oxstack.h"
#include "oxserv.h"


#include "maplec.h"
#include "oxmaple.h"

#define DPRINTF(x)	printf x; fflush(stdout)

/*==========================================================================*
 * Block interrupt input
 *==========================================================================*/

#define BLOCK_NEW_CMO()
#define UNBLOCK_NEW_CMO()

/*==========================================================================*
 * Gloval 
 *==========================================================================*/
static MKernelVector kv;  /* Maple kernel handle */

static cmo_string *maple_error_message;


/*==========================================================================*
 * wrapper
 *==========================================================================*/
cmo	*	
convert_maple2cmo_(ALGEB a)
{
	return (convert_maple2cmo(kv, a));
}

char	*
convert_maple2str_(ALGEB a) 
{
	return (convert_maple2str(kv, a));
}


/*==========================================================================*
 * main
 *==========================================================================*/
/* callback used for directing result output */


static void M_DECL textCallBack( void *data, int tag, char *output )
{
	printf("%s\n",output);
}


static void
oxmpl_print_err(void *data, int offset, char *msg)
{
	fprintf(stderr, "data=%p, offset=%d\n", data, offset);fflush(stderr);
	fprintf(stderr, "msg=%s\n", msg);fflush(stderr);
	maple_error_message = new_cmo_string(msg);
}

int oxmpl_init(int argc, char *argv[])
{
	
	char err[2048];  /* command input and error string buffers */
	MCallBackVectorDesc cb = {  textCallBack,
		oxmpl_print_err,   /* errorCallBack not used */
		0,   /* statusCallBack not used */
		0,   /* readLineCallBack not used */
		0,   /* redirectCallBack not used */
		0,   /* streamCallBack not used */
		0,   /* queryInterrupt not used */
		0	/* callBackCallBack not used */
	};
	ALGEB r;  /* Maple data-structures */
   


	/* initialize Maple */
	if( (kv=StartMaple(argc,argv,&cb,NULL,NULL,err)) == NULL ) {
		printf("Fatal error, %s\n",err);
		return (16);
	}

	/* example 1: find out where Maple is installed */
	r = MapleKernelOptions(kv,"mapledir",NULL);
	if( IsMapleString(kv,r) ) {
		printf("Maple directory = \"%s\"\n\n",MapleToString(kv,r));
		return (16);
	}

	return (0);
}




static inline void
convert_to_maple(oxstack_node *p)
{
	if (p->p == NULL) {
		p->p = convert_cmo_to_maple(kv, p->c, NULL);
	}
}

/*==========================================================================*
 * user function
 *==========================================================================*/

/****************************************************************************
 * add
 ****************************************************************************/
oxstack_node *
oxmpl_add(oxstack_node **arg, int argc)
{
	oxstack_node *p[2], *ans;
	const char *s[2];
	char *buf;
	ALGEB alg;
	int i;
	int len;

printf("call function 'add'\n");
	len = 0;
	for (i = 0; i < argc; i++) {
		convert_to_maple(arg[i]);
		p[i] = arg[i];
		s[i] = MapleToString(kv, p[i]->p);
		len += strlen(s[i]) + 1;
	}

	buf = MapleAlloc(kv, len);
	sprintf(buf, "%s+%s:", s[0], s[1]);


	alg = EvalMapleStatement(kv, buf);

	ans = oxstack_node_init(NULL);
	ans->p = alg;

	return (ans);
}

/****************************************************************************
 * whattype
 ****************************************************************************/
oxstack_node *
oxmpl_whattype(oxstack_node **arg, int argc)
{
	oxstack_node *ans;
	char *s;
	ALGEB alg;
	cmo *c;

	ans = oxstack_node_init(arg[0]->c);
	ans->p = arg[0]->p;
	oxstack_push(ans);

	if (arg[0]->p == NULL) {
		s = GC_MALLOC(30);
		sprintf(s, "cmo(%d=0x%08x)", arg[0]->c->tag, arg[0]->c->tag);
	} else {
		alg = EvalMapleStatement(kv, "whattype:");
		alg = EvalMapleProc(kv, alg, 1, arg[0]->p);
		alg = MapleALGEB_SPrintf(kv, "%a", alg);
		s = MapleToString(kv, alg);
	}

	c = (cmo *)new_cmo_string(s);
	ans = oxstack_node_init(c);
	return (ans);
}


/****************************************************************************
 * sleep
 ****************************************************************************/
#include <unistd.h>

oxstack_node *
oxmpl_sleep(oxstack_node **arg, int argc)
{
	oxstack_node *ans;
	int i;
	mpz_ptr len;
	mpz_t m;
	cmo_error2 *err;

printf("call function 'sleep'\n");
	for (i = 0; i < argc; i++) {
		if (arg[i]->c->tag != CMO_INT32 &&
		    arg[i]->c->tag != CMO_ZZ) {
			err = new_cmo_error2((cmo *)new_cmo_string("invalid 1st argument: not integer"));
			ans = oxstack_node_init((cmo *)err);
			return (ans);
			
		}
	}

	if (arg[0]->c->tag == CMO_ZZ) {
		len = ((cmo_zz *)arg[0]->c)->mpz;
	} else {
		mpz_init(m);
		len = m;
		mpz_set_si(m, ((cmo_int32 *)arg[0]->c)->i);
	}
	if (mpz_cmp_si(len, 0) > 0) {
		if (mpz_cmp_si(len, 0x1000000) > 0) {
			/* too large */
			i = 0x10000000;
		} else {
			i = (mpz_get_ui(len));
		}
		printf("sleep(%d=0x%x)\n", i, i);
		sleep(i);
	}

	ans = oxstack_node_init(arg[0]->c);
	ans->p = arg[0]->p;

	return (ans);
}

/****************************************************************************
 * func
 * maple 上の任意の関数を実行する
 ****************************************************************************/
oxstack_node *
oxmpl_func(oxstack_node **arg, int argc)
{
	oxstack_node *ans;
	oxstack_node **args2;
	ALGEB *args3;
	char *buf, *ff;
	cmo_error2 *err;
	ALGEB alg;
	ALGEB f;
	int i;

printf("call function 'func' argc=%d\n", argc);

	if (arg[0]->c->tag != CMO_STRING) {
		err = new_cmo_error2((cmo *)new_cmo_string("invalid 1st argument: not string"));
		ans = oxstack_node_init((cmo *)err);
		return (ans);
	}

	for (i = 1; i < argc; i++) {
		convert_to_maple(arg[i]);
		if (arg[i]->p == NULL) {
			err = new_cmo_error2((cmo *)new_cmo_string("convert failed"));
			ans = oxstack_node_init((cmo *)err);
			return (ans);
			
		}
	}

	ff = ((cmo_string *)arg[0]->c)->s;
	buf = MapleAlloc(kv, strlen(ff) + 10);
	sprintf(buf, "%s:", ff);
	f = EvalMapleStatement(kv, buf);


#define ARG_N 20
	args2 = MapleAlloc(kv, sizeof(oxstack_node *) * ARG_N);
	for (i = 0; i < argc - 1; i++) {
		args2[i] = arg[i+1];
	}
	for (; i < ARG_N; i++) {
		args2[i] = NULL;
	}

	/* 何故かわからないがこうやらないと動かない. */
	args3 = MapleAlloc(kv, sizeof(ALGEB) * ARG_N);
	for (i = 0; i < argc - 1; i++) {
		args3[i] = args2[i]->p;
	}


	/* どうすっぺかな */
	if (argc < ARG_N) {
#define ARG(n) args3[n]
		alg = EvalMapleProc(kv, f, argc - 1, 
		    ARG( 0), ARG( 1), ARG( 2), ARG( 3), ARG( 4), 
		    ARG( 5), ARG( 6), ARG( 7), ARG( 8), ARG( 9), 
		    ARG(10), ARG(11), ARG(12), ARG(13), ARG(14),
		    ARG(15), ARG(16), ARG(17), ARG(18), ARG(19));
printf("EvalMapleProg: alg=%p\n", alg);
		if (alg == NULL) {
			err = new_cmo_error2((cmo *)maple_error_message);
		}
#undef ARG
	} else {
		/* .... @@TODO */
		alg = NULL;
		err = new_cmo_error2((cmo *)new_cmo_string(
		    "too much argument"));
	}


	ans = oxstack_node_init(NULL);
	ans->p = alg;
	if (alg == NULL) {
		ans->c = (cmo *)err;
	}

	MapleDispose(kv, (ALGEB)buf);
	MapleDispose(kv, (ALGEB)args2);

	return (ans);
}



void 
oxmpl_executeStringParser(char *str)
{
	ALGEB alg;
	oxstack_node *ans;

	DPRINTF(("StringParser start [%s]\n", str));
	alg = EvalMapleStatement(kv, str);

	DPRINTF(("end maple eval\n"));
	ans = oxstack_node_init(NULL);
	ans->p = alg;

	oxstack_push(ans);
}


/*==========================================================================*
 * debug
 *==========================================================================*/
#include <unistd.h>
#include <stdarg.h>
void
oxmpl_debug_callfunc(
	const char *funcname, 
	int argc, ...)
{
	int i;
	va_list ap;
	ALGEB name;
	ALGEB *argv;
	char buf[1024];

	argv = GC_MALLOC(sizeof(ALGEB) * 10);

	sprintf(buf, "%s:", funcname);
	name = EvalMapleStatement(kv, buf);
	
	va_start(ap, argc);
	for (i = 0; i < argc; i++) {
		sprintf(buf, "%s:", va_arg(ap, char *));
		argv[i] = EvalMapleStatement(kv, buf);
	}
	for (; i < 10; i++) {
		argv[i] = NULL;
	}
	va_end(ap);

	
printf("eval proc\n");
	name = EvalMapleProc(kv, name, argc, argv[0], argv[1], argv[2]);
MapleALGEB_Printf(kv, "ret = %a\n", name);
}

