/* -*- mode: C; coding: euc-japan -*- */
/* $OpenXM$ */
/* $Id$ */

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

/* Mathematica を起動する. */
int MATH_init()
{
    int argc = 2;
    char *argv[] = {"-linkname", "math -mathlink"};

    if(MLInitialize(NULL) != NULL) {
        lp = MLOpen(argc, argv);
        if(lp != NULL) {
            return 0;
        }
    }
    exit(1);
}

int MATH_exit()
{
    /* quit Mathematica then close the link */
    MLPutFunction(lp, "Exit", 0);
    MLClose(lp);
}

char *MATH_getObject()
{
    char *s;

    /* skip any packets before the first ReturnPacket */
    while (MLNextPacket(lp) != RETURNPKT) {
        usleep(10);
        MLNewPacket(lp);
    }
    /* いまはタイプにかかわらず文字列を取得する. */
    switch(MLGetNext(lp)) {
    case MLTKINT:
        fprintf(stderr, "type is INTEGER.\n");
        MLGetString(lp, &s);
        break;
    case MLTKSTR:
        fprintf(stderr, "type is STRING.\n");
        MLGetString(lp, &s);
        break;
    default:
        MLGetString(lp, &s);
    }
    return s;
}

cmo *MATH_getObject2()
{
    char *s;
    cmo  *m;
    char **sp;
    int  i,n;

    /* skip any packets before the first ReturnPacket */
    while (MLNextPacket(lp) != RETURNPKT) {
        usleep(10);
        MLNewPacket(lp);
    }
    /* いまはタイプにかかわらず文字列を取得する. */
    switch(MLGetNext(lp)) {
    case MLTKINT:
        fprintf(stderr, "type is INTEGER.\n");
        MLGetString(lp, &s);
        m = (cmo *)new_cmo_zz_set_string(s);
        break;
    case MLTKSTR:
        fprintf(stderr, "type is STRING.\n");
        MLGetString(lp, &s);
        m = (cmo *)new_cmo_string(s);
        break;
    case MLTKERR:
        fprintf(stderr, "type is ERROR.\n");
        m = gen_error_object(MATH_ERROR);
        break;
    case MLTKSYM:
        fprintf(stderr, "MLTKSYM.\n");
        MLGetString(lp, s);
        m = (cmo *)new_cmo_string(s);
        break;
    case MLTKFUNC:
        fprintf(stderr, "MLTKFUNC.\n");
#if DEBUG
        MLGetString(lp, s);
        m = (cmo *)new_cmo_string(s);
        break;
#endif
        MLGetFunction(lp, sp, &n);
        fprintf(stderr, "n = %d\n", n);
        for (i=0; i<=n; i++) {
            fprintf(stderr, "%s ");
        }
        fprintf(stderr, "\n");
        m = (cmo *)new_cmo_string(s[0]);
        break;
    case MLTKREAL:
        fprintf(stderr, "MLTKREAL is not supported: we use MLTKSTR.\n");
        MLGetString(lp, &s);
        m = (cmo *)new_cmo_string(s);
        break;
    default:
        fprintf(stderr, "unknown type: we use STRING.\n");
        MLGetString(lp, &s);
        m = (cmo *)new_cmo_string(s);
    }
    return m;
}

int MATH_sendObject(cmo *m)
{
    char *s;
    switch(m->tag) {
    case CMO_INT32:
        MLPutInteger(lp, ((cmo_int32 *)m)->i);
        break;
    case CMO_STRING:
        s = ((cmo_string *)m)->s;
        MLPutString(lp, s);
        fprintf(stderr, "put %s.", s);
        break;
    default:
        MLPutFunction(lp, "ToExpression", 1);
        s = CONVERT_CMO_TO_CSTRING(m);
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
        MATH_sendObject(argv[i]);
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
    fprintf(stderr, "server:: a cmo is pushed: tag == %d.\n", m->tag);
    if (m->tag == CMO_STRING) {
        fprintf(stderr, "server:: %s\n", ((cmo_string *)m)->s);
    }
#endif
    Operand_Stack[Stack_Pointer] = m;
    Stack_Pointer++;
    if (Stack_Pointer >= SIZE_OPERAND_STACK) {
        fprintf(stderr, "stack over flow.\n");
        exit(1);
    }
}

/* エラーのときは NULL を返す */
/* gen_error_object(SM_popCMO); */
/* CMO_ERROR2 */

cmo* pop()
{
    if (Stack_Pointer > 0) {
        Stack_Pointer--;
        return Operand_Stack[Stack_Pointer];
    }
    return NULL;
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

    fprintf(stderr, "code: SM_popCMO.\n");
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
    fprintf(stderr, "code: SM_popString.\n");
#endif

    if ((m = pop()) != NULL && (s = CONVERT_CMO_TO_CSTRING(m)) != NULL) {
        send_ox_cmo(fd_write, new_cmo_string(s));
        return 0;
    }
    return SM_popString;
}

/* この関数はサーバに依存する. */
int sm_executeStringByLocalParser(int fd_write)
{
    cmo* m = NULL;
#ifdef DEBUG
    fprintf(stderr, "code: SM_executeStringByLocalParser.\n");
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
    case SM_setMathcap:
        pop();  /* 無視する */
        break;
    default:
        fprintf(stderr, "unknown command: %d.\n", code);
        err = UNKNOWN_SM_COMMAND;
    }

    if (err != 0) {
        push(gen_error_object(err));
    }
}
