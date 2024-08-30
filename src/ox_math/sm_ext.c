/* -*- mode: C; coding: euc-japan -*- */
/* $OpenXM: OpenXM/src/ox_math/sm_ext.c,v 1.7 2003/02/04 14:27:43 ohara Exp $ */

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
#include <signal.h>
#include <ox_toolkit.h>
#include "sm.h"
#include "mlo.h"

static struct { int (*func_ptr)(); int key; } tbl_smcmd[] = {
    {sm_executeFunction, SM_executeFunction}, 
    {sm_executeStringByLocalParser, SM_executeStringByLocalParser}, 
    {sm_executeStringByLocalParser, SM_executeStringByLocalParserInBatchMode}, 
    {sm_mathcap,         SM_mathcap},
    {sm_set_mathcap,     SM_setMathCap},
    {sm_popString,       SM_popString},
    {sm_popCMO,          SM_popCMO},
    {sm_pops,            SM_pops},
	{shutdown,           SM_shutdown},
    {NULL, NULL}
};

extern OXFILE *stack_oxfp;

static sigset_t mask;
static int flag_state_interrupting = 0;

/* state management for the OpenXM robust interruption */
void sm_state_set_interrupting()
{
    ml_state_set(RESERVE_INTERRUPTION);
}

int sm_state_interrupting()
{
    return ml_state(RESERVE_INTERRUPTION);
}

void sm_state_clear_interrupting()
{
    ml_state_clear_all();
}

/* handling OpenXM signals */
static void sm_sighandler()
{
    sm_state_set_interrupting();
}

/* generating the mask pattern */
void sm_siginit()
{
    signal(SIGUSR1, sm_sighandler);
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
}

void sm_sigmask()
{
    sigprocmask(SIG_BLOCK, &mask, NULL);
}

void sm_sigunmask()
{
    sigprocmask(SIG_UNBLOCK, &mask, NULL);
}

int (*sm_search_f(int code))()
{
    int i;
    for (i=0; tbl_smcmd[i].key != NULL; i++) {
        if (code == tbl_smcmd[i].key) {
            return tbl_smcmd[i].func_ptr;
        }
    }
    return NULL;
}

/* MathLink dependent */
void sm_popString()
{
    char *s;
    cmo *err;
    cmo *m = pop();
    if (m->tag == CMO_STRING) {
        send_ox_cmo(stack_oxfp, m);
    }else if ((s = new_string_set_cmo(m)) != NULL) {
        send_ox_cmo(stack_oxfp, (cmo *)new_cmo_string(s));
    }else {
        err = make_error_object(SM_popString, m);
        send_ox_cmo(stack_oxfp, err);
    }
}

int local_execute(char *s)
{
    extern int flag_mlo_symbol;

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
void sm_executeStringByLocalParser()
{
    cmo* m = pop();
    char *s = NULL;
    if (m->tag == CMO_STRING
        && strlen(s = ((cmo_string *)m)->s) != 0) {
        if (s[0] == ':') {
            local_execute(++s);
        }else {
            /* for mathematica */
            /* Sending the string `s' to mathematica for its evaluation. */
            ml_evaluateStringByLocalParser(s);
            sm_sigunmask();
            ml_select();
            sm_sigmask();
            m = ml_return();
            push(m);
        }
    }else {
		ox_printf(" <%s>", get_symbol_by_tag(m->tag));
		push_error(SM_executeStringByLocalParser, m);
	}
}

void sm_executeFunction()
{
    int i, argc;
    cmo **argv;
    char* func;
    cmo* m;

    if ((m = pop()) == NULL || m->tag != CMO_STRING) {
        push_error(SM_executeFunction, m);
    }
    func = ((cmo_string *)m)->s;

    if ((m = pop()) == NULL || m->tag != CMO_INT32) {
        push_error(SM_executeFunction, m);
    }

    argc = ((cmo_int32 *)m)->i;
    argv = malloc(argc*sizeof(cmo *));
    for (i=0; i<argc; i++) {
        argv[i] = pop();
    }
    ml_executeFunction(func, argc, argv);
    sm_sigunmask();
    ml_select();
    sm_sigmask();
    m = ml_return();
    push(m);
}

void sm_mathcap()
{
    push((cmo *)oxf_cmo_mathcap(stack_oxfp));
}

void sm_set_mathcap()
{
    cmo_mathcap *m = (cmo_mathcap *)pop();
    if (m->tag == CMO_MATHCAP) {
        oxf_mathcap_update(stack_oxfp, m);
    }else {
        push_error(-1, m);
        /* an error object must be pushed */
    }
}
