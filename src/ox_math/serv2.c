/* -*- mode: C; coding: euc-japan -*- */
/* $OpenXM: OpenXM/src/ox_math/serv2.c,v 1.17 2000/11/28 20:16:03 ohara Exp $ */

/* 
   Copyright (C) Katsuyoshi OHARA, 2000.
   Portions copyright 1999 Wolfram Research, Inc. 

   You must see OpenXM/Copyright/Copyright.generic.
   The MathLink Library is licensed from Wolfram Research Inc..
   See OpenXM/Copyright/Copyright.mathlink for detail.
*/

/* 
   Remarks: 
   file descripter 3 and 4 are already opened by the parent process.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <gmp.h>
#include <mathlink.h>
#include <ox_toolkit.h>
#include "serv2.h"

extern int flag_mlo_symbol;

/* MathLink independent */
static cmo **stack = NULL;
static int stack_size = 0;
static int stack_ptr = 0;
OXFILE *stack_oxfp = NULL;

#define DIFFERENCE_OF_STACK  1024

static void stack_extend()
{
    int newsize = stack_size + DIFFERENCE_OF_STACK;
    cmo **newstack = (cmo **)malloc(sizeof(cmo *)*newsize);
    if (stack != NULL) {
        memcpy(newstack, stack, sizeof(cmo *)*stack_size);
        free(stack);
    }
    stack_size = newsize;
    stack = newstack;
}

int push(cmo* m)
{
#if DEBUG
    symbol_t symp;

    if (m->tag == CMO_STRING) {
        fprintf(stderr, "ox_math:: a CMO_STRING(%s) was pushed.\n", ((cmo_string *)m)->s);
    }else {
        symp = lookup_by_tag(m->tag);
        fprintf(stderr, "ox_math:: a %s was pushed.\n", symbol_get_key(symp));
    }
#endif
    if (stack_ptr >= stack_size) {
        stack_extend();
    }
    stack[stack_ptr] = m;
    stack_ptr++;
}

/* if the stack is empty, then pop() returns (CMO_NULL). */
cmo* pop()
{
    if (stack_ptr > 0) {
        return stack[--stack_ptr];
    }
    return new_cmo_null();
}

void pops(int n)
{
    stack_ptr -= n;
    if (stack_ptr < 0) {
        stack_ptr = 0;
    }
}

void push_error(int errcode, cmo* pushback)
{
    return push((cmo *)make_error_object(errcode, pushback));
}

/* 
   if error occurs, then a sm_*() function returns non-zero and 
   an error obect is set by a function which calls sm_*().
*/
int sm_popCMO(OXFILE* oxfp)
{
    cmo* m = pop();
#ifdef DEBUG
    symbol_t symp = lookup_by_tag(m->tag);
    fprintf(stderr, "ox_math:: opecode = SM_popCMO. (%s)\n", symbol_get_key(symp));
#endif

    if (m != NULL) {
        send_ox_cmo(oxfp, m);
        return 0;
    }
    return SM_popCMO;
}

int sm_pops(OXFILE* oxfp)
{
    cmo* m = pop();
    if (m != NULL && m->tag == CMO_INT32) {
        pops(((cmo_int32 *)m)->i);
        return 0;
    }
    return ERROR_ID_UNKNOWN_SM;
}

/* MathLink dependent */
int sm_popString(OXFILE* oxfp)
{
    char *s;
    cmo *err;
    cmo *m;

#ifdef DEBUG
    fprintf(stderr, "ox_math:: opecode = SM_popString.\n");
#endif

    m = pop();
    if (m->tag == CMO_STRING) {
        send_ox_cmo(oxfp, m);
    }else if ((s = new_string_set_cmo(m)) != NULL) {
        send_ox_cmo(oxfp, (cmo *)new_cmo_string(s));
    }else {
        err = make_error_object(SM_popString, m);
        send_ox_cmo(oxfp, err);
    }
    return 0;
}

int local_execute(char *s)
{
    if(*s == 'i') {
        switch(s[1]) {
        case '+':
            flag_mlo_symbol = FLAG_MLTKSYM_IS_STRING;
            break;
        case '-':
        case '=':
        default:
            flag_mlo_symbol = FLAG_MLTKSYM_IS_INDETERMINATE;
        }
    }
    return 0;
}

/* The following function is depend on an implementation of a server. */
int sm_executeStringByLocalParser(OXFILE* oxfp)
{
    symbol_t symp;
    cmo* m = pop();
    char *s = NULL;
#ifdef DEBUG
    fprintf(stderr, "ox_math:: opecode = SM_executeStringByLocalParser.\n");
#endif

    if (m->tag == CMO_STRING
        && strlen(s = ((cmo_string *)m)->s) != 0) {
        if (s[0] == ':') {
            local_execute(++s);
        }else {
            /* for mathematica */
            /* Sending the string `s' to mathematica for its evaluation. */
            ml_evaluateStringByLocalParser(s);
            ml_select();
            push(receive_mlo());
        }
        return 0;
    }
#ifdef DEBUG
    symp = lookup_by_tag(m->tag);
    fprintf(stderr, "ox_math:: error. the top of stack is %s.\n", symbol_get_key(symp));
#endif
    return SM_executeStringByLocalParser;
}

int sm_executeFunction(OXFILE* oxfp)
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
    argv = malloc(argc*sizeof(cmo *));
    for (i=0; i<argc; i++) {
        argv[i] = pop();
    }
    ml_executeFunction(func, argc, argv);
    ml_select();
    push(receive_mlo());
    return 0;
}

int sm_mathcap(OXFILE* oxfp)
{
    push((cmo *)oxf_cmo_mathcap(oxfp));
    return 0;
}

void sm_set_mathcap(OXFILE *oxfp)
{
    cmo_mathcap *m = (cmo_mathcap *)pop();
    if (m->tag == CMO_MATHCAP) {
        oxf_mathcap_update(oxfp, m);
    }else {
        push_error(-1, m);
        /* an error object must be pushed */
    }
}

int receive_sm_command(OXFILE* oxfp)
{
    return receive_int32(oxfp);
}

int execute_sm_command(OXFILE* oxfp, int code)
{
    int err = 0;
#ifdef DEBUG    
    symbol_t sp = lookup_by_tag(code);
    fprintf(stderr, "ox_math:: %s received.\n", symbol_get_key(sp));
#endif

    switch(code) {
    case SM_popCMO:
        err = sm_popCMO(oxfp);
        break;
    case SM_popString:
        err = sm_popString(oxfp);
        break;
    case SM_mathcap:
        err = sm_mathcap(oxfp);
        break;
    case SM_pops:
        err = sm_pops(oxfp);
        break;
    case SM_executeStringByLocalParser:
    case SM_executeStringByLocalParserInBatchMode:
        err = sm_executeStringByLocalParser(oxfp);
        break;
    case SM_executeFunction:
        err = sm_executeFunction(oxfp);
        break;
    case SM_shutdown:
        shutdown();
        break;
    case SM_setMathCap:
        pop();  /* ignore */
        break;
    default:
        fprintf(stderr, "unknown command: %d.\n", code);
        err = ERROR_ID_UNKNOWN_SM;
    }

    if (err != 0) {
        push((cmo *)make_error_object(err, new_cmo_null()));
    }
}
