/* -*- mode: C -*- */
/* $OpenXM: OpenXM/src/ox_math/mlo.c,v 1.7 2000/10/10 19:58:29 ohara Exp $ */

/* 
   Copyright (C) Katsuyoshi OHARA, 2000.
   Portions copyright 1999 Wolfram Research, Inc. 

   You must see OpenXM/Copyright/Copyright.generic.
   The MathLink Library is licensed from Wolfram Research Inc..
   See OpenXM/Copyright/Copyright.mathlink for detail.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mathlink.h>
#include <ox_toolkit.h>
#include "mlo.h"
#include "serv2.h"

/* If this flag sets then we identify MLTKSYM to CMO_INDETERMINATE. */
int flag_mlo_symbol = FLAG_MLTKSYM_IS_INDETERMINATE;

/* MLINK is a indentifier of MathLink connection. */
MLINK stdlink;

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
    list_append((cmo_list *)m, new_cmo_string(s));

    for (i=0; i<n; i++) {
        fprintf(stderr, "  --debug: arg[%d]\n", i);
        fflush(stderr);
        ob = receive_mlo();
        list_append((cmo_list *)m, ob);
    }

    MLDisownString(stdlink, s);
    return m;
}

#if 0
cmo *convert_mlo_to_cmo(mlo *m)
{
	if (m->tag == MLO_FUNCTION) {
		if (strcmp(((mlo_function *)m)->function, "List") == 0) {
			return convert_mlo_function_list_to_cmo_list(m);
		}
	}
	return m;
}
#endif

#define MLO_FUNCTION   (CMO_PRIVATE+1)

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
        list_append((cmo_list *)m, ob);
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

/* starting a MathLink connection. */
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

/* closing a MathLink connection. */
int ml_exit()
{
    /* quit Mathematica then close the link */
    MLPutFunction(stdlink, "Exit", 0);
    MLClose(stdlink);
}

/* Never forget call ml_select() before calling receive_mlo(). */
int ml_select()
{
    /* skip any packets before the first ReturnPacket */
    while (MLNextPacket(stdlink) != RETURNPKT) {
        usleep(10);
        MLNewPacket(stdlink);
    }
}

/* Never forget call ml_flush() after calling send_mlo(). */
int ml_flush()
{
    MLEndPacket(stdlink);
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
        /* Yet we have no implementation of CMO_DOUBLE... */
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
    s = new_string_set_cmo(m);
    MLPutString(stdlink, s);
}

int send_mlo_list(cmo *c)
{
    char *s;
    cell *cp = list_first((cmo_list *)c);
    int len = list_length((cmo_list *)c);

    MLPutFunction(stdlink, "List", len);
    while(!list_endof(c, cp)) {
        send_mlo(cp->cmo);
        cp = list_next(cp);
    }
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
        s = new_string_set_cmo(m);
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
