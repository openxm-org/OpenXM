/* -*- mode: C; coding: euc-japan -*- */
/* $OpenXM$ */

/* Open Mathematica サーバ */
/* ファイルディスクリプタ 3, 4 は open されていると仮定して動作する. */

/* MathLink との通信部分 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <gmp.h>
#include <mathlink.h>
#include "ox.h"
#include "parse.h"
#include "serv2.h"

int flag_mlo_symbol = FLAG_MLTKSYM_IS_INDETERMINATE;

/* MLINK はポインタ型. */
MLINK stdlink;

typedef cmo mlo;
typedef cmo_string mlo_string;
typedef cmo_zz mlo_zz;

mlo *receive_mlo_zz()
{
    char *s;
    mlo  *m;

    MLGetString(stdlink, &s);
    fprintf(stderr, "--debug: MLO == MLTKINT (%s).\n", s);
    m = (mlo *)new_cmo_zz_set_string(s);
    MLDisownString(stdlink, s);
    return m;
}

mlo *receive_mlo_string()
{
    char *s;
    mlo  *m;
    MLGetString(stdlink, &s);
    fprintf(stderr, "--debug: MLO == MLTKSTR (\"%s\").\n", s);
    m = (cmo *)new_cmo_string(s);
    MLDisownString(stdlink, s);
    return m;
}

cmo *receive_mlo_function()
{
    char *s;
    cmo *m;
    cmo  *ob;
    int  i,n;

    MLGetFunction(stdlink, &s, &n);
    fprintf(stderr, "--debug: MLO == MLTKFUNC (%s[#%d]).\n", s, n);
    m = new_cmo_list();
    append_cmo_list((cmo_list *)m, new_cmo_string(s));

    for (i=0; i<n; i++) {
        fprintf(stderr, "  --debug: arg[%d]\n", i);
        fflush(stderr);
        ob = receive_mlo();
        append_cmo_list((cmo_list *)m, ob);
    }

    MLDisownString(stdlink, s);
    return m;
}

mlo_function *new_mlo_function(char *function)
{
    mlo_function *c = malloc(sizeof(mlo_function));
    c->tag = MLO_FUNCTION;
    c->length = 0;
    c->head->next = NULL;
    c->function = function;
    return c;
}

cmo *receive_mlo_function_newer()
{
    char *s;
    mlo_function *m;
    cmo  *ob;
    int  i,n;

    MLGetFunction(stdlink, &s, &n);
#ifdef DEBUG
    fprintf(stderr, "--debug: MLO == MLTKFUNC, (%s[#%d])\n", s, n);
#endif
    m = new_mlo_function(s);
    for (i=0; i<n; i++) {
        fprintf(stderr, "--debug: arg[%d]\n", i);
        fflush(stderr);
        ob = receive_mlo();
        append_cmo_list((cmo_list *)m, ob);
    }

    MLDisownString(stdlink, s);
    return (cmo *)m;
}

cmo *receive_mlo_symbol()
{
    cmo *ob;
    char *s;

    MLGetSymbol(stdlink, &s);
#ifdef DEBUG
    fprintf(stderr, "--debug: MLO == MLTKSYM, (%s).\n", s);
#endif
    if(flag_mlo_symbol == FLAG_MLTKSYM_IS_INDETERMINATE) {
        ob = new_cmo_indeterminate(new_cmo_string(s));
    }else {
        ob = new_cmo_string(s);
    }
    MLDisownString(stdlink, s);
    return ob;
}

/* Mathematica を起動する. */
int ml_init()
{
    int argc = 2;
    char *argv[] = {"-linkname", "math -mathlink"};

    if(MLInitialize(NULL) == NULL
       || (stdlink = MLOpen(argc, argv)) == NULL) {
        fprintf(stderr, "Mathematica Kernel not found.\n");
        exit(1);
    }
    return 0;
}

int ml_exit()
{
    /* quit Mathematica then close the link */
    MLPutFunction(stdlink, "Exit", 0);
    MLClose(stdlink);
}

cmo *ml_get_object()
{
    /* skip any packets before the first ReturnPacket */
    while (MLNextPacket(stdlink) != RETURNPKT) {
        usleep(10);
        MLNewPacket(stdlink);
    }
    return receive_mlo();
}

cmo *receive_mlo()
{
    char *s;
    int type;

    switch(type = MLGetNext(stdlink)) {
    case MLTKINT:
        return receive_mlo_zz();
    case MLTKSTR:
        return receive_mlo_string();
    case MLTKREAL:
        /* double はまだ... */
        fprintf(stderr, "--debug: MLO == MLTKREAL.\n");
        MLGetString(stdlink, &s);
        return (cmo *)new_cmo_string(s);
    case MLTKSYM:
        return receive_mlo_symbol();
    case MLTKFUNC:
        return receive_mlo_function();
    case MLTKERR:
        fprintf(stderr, "--debug: MLO == MLTKERR.\n");
        return (cmo *)make_error_object(ERROR_ID_FAILURE_MLINK, new_cmo_null());
    default:
        fprintf(stderr, "--debug: MLO(%d) is unknown.\n", type);
        MLGetString(stdlink, &s);
        fprintf(stderr, "--debug: \"%s\"\n", s);
        return (cmo *)new_cmo_string(s);
    }
}

int send_mlo_int32(cmo *m)
{
    MLPutInteger(stdlink, ((cmo_int32 *)m)->i);
}

int send_mlo_string(cmo *m)
{
    char *s = ((cmo_string *)m)->s;
    MLPutString(stdlink, s);
}

int send_mlo_zz(cmo *m)
{
    char *s;
    MLPutFunction(stdlink, "ToExpression", 1);
    s = convert_cmo_to_string(m);
    MLPutString(stdlink, s);
}

int send_mlo_list(cmo *c)
{
    char *s;
    cell *cp = ((cmo_list *)c)->head;
    int len = length_cmo_list((cmo_list *)c);

    MLPutFunction(stdlink, "List", len);
    while(cp->next != NULL) {
        send_mlo(cp->cmo);
        cp = cp->next;
    }
}

int ml_sendObject(cmo *m)
{
    send_mlo(m);
    MLEndPacket(stdlink);
}

int send_mlo(cmo *m)
{
    char *s;
    switch(m->tag) {
    case CMO_INT32:
        send_mlo_int32(m);
        break;
    case CMO_ZERO:
    case CMO_NULL:
        send_mlo_int32(new_cmo_int32(0));
        break;
    case CMO_STRING:
        send_mlo_string(m);
        break;
    case CMO_LIST:
        send_mlo_list(m);
        break;
    case CMO_MATHCAP:
        send_mlo(((cmo_mathcap *)m)->ob);
        break;
    case CMO_ZZ:
        send_mlo_zz(m);
        break;
    default:
        MLPutFunction(stdlink, "ToExpression", 1);
        s = convert_cmo_to_string(m);
        MLPutString(stdlink, s);
        break;
    }
}

int ml_evaluateStringByLocalParser(char *str)
{
    MLPutFunction(stdlink, "ToExpression", 1);
    MLPutString(stdlink, str);
    MLEndPacket(stdlink);
}

int ml_executeFunction(char *function, int argc, cmo *argv[])
{
    int i;
    MLPutFunction(stdlink, function, argc);
    for (i=0; i<argc; i++) {
        send_mlo(argv[i]);
    }
    MLEndPacket(stdlink);
}
