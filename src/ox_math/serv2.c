/* -*- mode: C; coding: euc-japan -*- */
/* $OpenXM: OpenXM/src/ox_math/serv2.c,v 1.4 1999/11/04 03:05:51 ohara Exp $ */

/* Open Mathematica サーバ */
/* ファイルディスクリプタ 3, 4 は open されていると仮定して動作する. */

/* MathLink との通信部分 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <gmp.h>
#include <mathlink.h>
#include "ox.h"
#include "serv2.h"

#define UNKNOWN_SM_COMMAND 50000
#define MATH_ERROR         50001

/* MLINK はポインタ型. */
MLINK lp = NULL;


typedef cmo mlo;
typedef cmo_string mlo_string;
typedef cmo_zz mlo_zz;

/* cmo_list の派生クラス*/
typedef struct {
	int tag;
	int length;
	cell head[1];
	char *function;
} mlo_function;


mlo *receive_mlo_zz()
{
	char *s;
	mlo  *m;

	fprintf(stderr, "--debug: MLO == MLTKINT.\n");
	MLGetString(lp, &s);
 	fprintf(stderr, "--debug: zz = %s.\n", s);
	m = (mlo *)new_cmo_zz_set_string(s);
	MLDisownString(lp, s);
	return m;
}

mlo *receive_mlo_string()
{
	char *s;
	mlo  *m;
	fprintf(stderr, "--debug: MLO == MLTKSTR.\n");
	MLGetString(lp, &s);
 	fprintf(stderr, "--debug: string = \"%s\".\n", s);
	m = (cmo *)new_cmo_string(s);
	MLDisownString(lp, s);
	return m;
}

cmo *receive_mlo_function()
{
	char *s;
	cmo *m;
    cmo  *ob;
    int  i,n;

	fprintf(stderr, "--debug: MLO == MLTKFUNC.\n");
	MLGetFunction(lp, &s, &n);
	fprintf(stderr, "--debug: Function = \"%s\", # of args = %d\n", s, n);
	m = new_cmo_list();
	append_cmo_list(m, new_cmo_string(s));

	for (i=0; i<n; i++) {
		fprintf(stderr, "--debug: arg[%d]\n", i);
		fflush(stderr);
		ob = receive_mlo();
		append_cmo_list(m, ob);
	}

	MLDisownString(lp, s);
	return m;
}

cmo *receive_mlo_symbol()
{
	cmo *ob;
	char *s;

	fprintf(stderr, "--debug: MLO == MLTKSYM.\n");
	MLGetSymbol(lp, &s);
	fprintf(stderr, "--debug: Symbol \"%s\".\n", s);

	ob = new_cmo_indeterminate(new_cmo_string(s)); 

	MLDisownString(lp, s);
	return ob;
}


/* Mathematica を起動する. */
int MATH_init()
{
    int argc = 2;
    char *argv[] = {"-linkname", "math -mathlink"};

    if(MLInitialize(NULL) == NULL 
	   || (lp = MLOpen(argc, argv)) == NULL) {
		fprintf(stderr, "Mathematica Kernel not found.\n");
		exit(1);
    }
	return 0;
}

int MATH_exit()
{
    /* quit Mathematica then close the link */
    MLPutFunction(lp, "Exit", 0);
    MLClose(lp);
}

cmo *MATH_getObject2()
{
    /* skip any packets before the first ReturnPacket */
    while (MLNextPacket(lp) != RETURNPKT) {
        usleep(10);
        MLNewPacket(lp);
    }
	return receive_mlo();
}

cmo *receive_mlo()
{
    char *s;
	int type;

    switch(type = MLGetNext(lp)) {
    case MLTKINT:
		return receive_mlo_zz();
    case MLTKSTR:
		return receive_mlo_string();
    case MLTKREAL:
		/* double はまだ... */
        fprintf(stderr, "--debug: MLO == MLTKREAL.\n");
        MLGetString(lp, &s);
        return new_cmo_string(s);
    case MLTKSYM:
        return receive_mlo_symbol();
    case MLTKFUNC:
		return receive_mlo_function();
    case MLTKERR:
        fprintf(stderr, "--debug: MLO == MLTKERR.\n");
        return gen_error_object(MATH_ERROR);
    default:
        fprintf(stderr, "--debug: MLO(%d) is unknown.\n", type);
        MLGetString(lp, &s);
		fprintf(stderr, "--debug: \"%s\"\n", s);
        return new_cmo_string(s);
    }
}


int send_mlo_int32(cmo *m)
{
	MLPutInteger(lp, ((cmo_int32 *)m)->i);
}

int send_mlo_string(cmo *m)
{
	char *s = ((cmo_string *)m)->s;
	MLPutString(lp, s);
	fprintf(stderr, "ox_math:: put %s.", s);
}

int send_mlo_zz(cmo *m)
{
	char *s;
	MLPutFunction(lp, "ToExpression", 1);
	s = convert_cmo_to_string(m);
	MLPutString(lp, s);
	fprintf(stderr, "put %s.", s);
}

int send_mlo_list(cmo *c)
{
	char *s;
	cell *cp = ((cmo_list *)c)->head;
	int len = length_cmo_list((cmo_list *)c);

	fprintf(stderr, "ox_math:: put List with %d args.\n", len);
	MLPutFunction(lp, "List", len);
	while(cp->next != NULL) {
		send_mlo(cp->cmo);
		cp = cp->next;
	}
}

int MATH_sendObject(cmo *m)
{
	send_mlo(m);
	MLEndPacket(lp);
}

int send_mlo(cmo *m)
{
    char *s;
    switch(m->tag) {
    case CMO_INT32:
		send_mlo_int32(m);
        break;
    case CMO_STRING:
		send_mlo_string(m);
        break;
	case CMO_LIST:
		send_mlo_list(m);
        break;
    default:
        MLPutFunction(lp, "ToExpression", 1);
        s = convert_cmo_to_string(m);
        MLPutString(lp, s);
        fprintf(stderr, "put %s.", s);
        break;
    }
}

int MATH_evaluateStringByLocalParser(char *str)
{
    MLPutFunction(lp, "ToExpression", 1);
    MLPutString(lp, str);
    MLEndPacket(lp);
}

int MATH_executeFunction(char *function, int argc, cmo *argv[])
{
    int i;
    MLPutFunction(lp, function, argc);
    for (i=0; i<argc; i++) {
        send_mlo(argv[i]);
    }
    MLEndPacket(lp);
}

/* MathLink 非依存部分 */

#define SIZE_OPERAND_STACK   2048

static cmo* Operand_Stack[SIZE_OPERAND_STACK];
static int Stack_Pointer = 0;

int initialize_stack()
{
    Stack_Pointer = 0;
} 

int push(cmo* m)
{
#if DEBUG
    if (m->tag == CMO_STRING) {
        fprintf(stderr, "ox_math:: a cmo_string(%s) was pushed.\n", ((cmo_string *)m)->s);
    }else {
		fprintf(stderr, "ox_math:: a cmo(%d) was pushed.\n", m->tag);
	}
#endif
    Operand_Stack[Stack_Pointer] = m;
    Stack_Pointer++;
    if (Stack_Pointer >= SIZE_OPERAND_STACK) {
        fprintf(stderr, "stack over flow.\n");
        exit(1); /* 手抜き */
    }
}

/* スタックが空のときは, (CMO_NULL) をかえす. */
cmo* pop()
{
    if (Stack_Pointer > 0) {
        Stack_Pointer--;
        return Operand_Stack[Stack_Pointer];
    }
    return new_cmo_null();
}

void pops(int n)
{
    Stack_Pointer -= n;
    if (Stack_Pointer < 0) {
        Stack_Pointer = 0;
    }
}

/* sm_XXX 関数群は、エラーのときは 0 以外の値を返し、呼び出し元で
   エラーオブジェクトをセットする */
int sm_popCMO(int fd_write)
{
    cmo* m = pop();

    fprintf(stderr, "ox_math:: opecode = SM_popCMO. (tag = %d)\n", m->tag);
    if (m != NULL) {
        send_ox_cmo(fd_write, m);
        return 0;
    }
    return SM_popCMO;
}

int sm_pops(int fd_write)
{
    cmo* m = pop();
    if (m != NULL && m->tag == CMO_INT32) {
        pops(((cmo_int32 *)m)->i);
        return 0;
    }
    return UNKNOWN_SM_COMMAND;
}

/* MathLink 依存部分 */
int sm_popString(int fd_write)
{
    char* s;
    cmo*  m;

#ifdef DEBUG
    fprintf(stderr, "ox_math:: opecode = SM_popString.\n");
#endif

    if ((m = pop()) != NULL && (s = convert_cmo_to_string(m)) != NULL) {
        send_ox_cmo(fd_write, (cmo *)new_cmo_string(s));
        return 0;
    }
    return SM_popString;
}

/* この関数はサーバに依存する. */
int sm_executeStringByLocalParser(int fd_write)
{
    cmo* m = NULL;
#ifdef DEBUG
    fprintf(stderr, "ox_math:: opecode = SM_executeStringByLocalParser.\n");
#endif
    if ((m = pop()) != NULL && m->tag == CMO_STRING) {
        /* for mathematica */
        /* mathematica に文字列を送って評価させる */
        MATH_evaluateStringByLocalParser(((cmo_string *)m)->s);
        push(MATH_getObject2());
        return 0;
    }
    fprintf(stderr, "cannot execute: top of stack is not string!(%p, %d)\n", m, m->tag);
    return SM_executeStringByLocalParser;
}

int sm_executeFunction(int fd_write)
{
    int i, argc;
    cmo **argv;
    char* func;
    cmo* m;

    if ((m = pop()) == NULL || m->tag != CMO_STRING) {
        return SM_executeFunction;
    }
    func = ((cmo_string *)m)->s;

    if ((m = pop()) == NULL || m->tag != CMO_INT32) {
        return SM_executeFunction;
    }
    argc = ((cmo_int32 *)m)->i;
    argv = malloc(sizeof(cmo *)*argc);
    for (i=0; i<argc; i++) {
        if ((argv[i] = pop()) == NULL) {
            return SM_executeFunction;
        }
    }
    MATH_executeFunction(func, argc, argv);
    push(MATH_getObject2());
    return 0;
}

/* 平成11年10月13日 */
#define VERSION 0x11102700
#define ID_STRING  "ox_math server 1999/10/28 17:29:25"

int sm_mathcap(int fd_write)
{
    cmo* c = make_mathcap_object(VERSION, ID_STRING);
    push(c);
    return 0;
}

int receive_sm_command(int fd_read)
{
    return receive_int32(fd_read);
}

int execute_sm_command(int fd_write, int code)
{
    int err = 0;

    switch(code) {
    case SM_popCMO:
        err = sm_popCMO(fd_write);
        break;
    case SM_popString:
        err = sm_popString(fd_write);
        break;
    case SM_mathcap:
        err = sm_mathcap(fd_write);
        break;
    case SM_pops:
        err = sm_pops(fd_write);
        break;
    case SM_executeStringByLocalParser:
        err = sm_executeStringByLocalParser(fd_write);
        break;
    case SM_executeFunction:
        err = sm_executeFunction(fd_write);
        break;
    case SM_setMathCap:
        pop();  /* 無視する */
        break;
    default:
        fprintf(stderr, "unknown command: %d.\n", code);
        err = UNKNOWN_SM_COMMAND;
    }

    if (err != 0) {
        push((cmo *)gen_error_object(err));
    }
}
