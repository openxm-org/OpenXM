/* -*- mode: C; coding: euc-japan -*- */
/* $OpenXM: OpenXM/src/ox_math/sm.c,v 1.4 2003/02/04 14:27:43 ohara Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <mathlink.h>
#include <ox_toolkit.h>
#include "sm.h"

/* MathLink independent */

/* WARNING: you must NOT use stack[stack_ptr]. */
static cmo **stack = NULL;
static int stack_ptr = 0;
static int stack_size = 0;
OXFILE *stack_oxfp = NULL;

#define DIFFERENCE_OF_STACK  1024

void stack_extend()
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

void push(cmo *ob)
{
    ox_printf("push<%s>.\n", get_symbol_by_tag(ob->tag));
    if (stack_ptr >= stack_size) {
        stack_extend();
    }
    stack[stack_ptr] = ob;
    stack_ptr++;
}

/* if the stack is empty, then pop() returns (CMO_NULL). */
cmo *pop()
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
If error occurs, then
an sm_* function, called by sm_run, pushes an error object.
*/
void sm_popCMO()
{
    cmo* m = pop();
    ox_printf(" <%s>", get_symbol_by_tag(m->tag));
    send_ox_cmo(stack_oxfp, m);
}

void sm_pops()
{
    cmo* m = pop();
    if (m->tag == CMO_INT32) {
        pops(((cmo_int32 *)m)->i);
    }else {
        push_error(ERROR_ID_UNKNOWN_SM, m);
    }
}

void sm_run(int code)
{
    int (*func)(OXFILE *) = sm_search_f(code);
    ox_printf("opecode=<%s>[%d] ", get_symbol_by_tag(code), code);
    if (func != NULL) {
        func(stack_oxfp);
    }else {
        push_error(ERROR_ID_UNKNOWN_SM, new_cmo_null());
    }
    ox_printf("\n");
}

int oxf_error(OXFILE *oxfp)
{
    int e = oxfp->error;
    if (e != 0) {
        oxfp->error = 0;
    }
    return e;
}
