/* -*- mode: C -*- */
/* $OpenXM: OpenXM/src/oxc/sm.c,v 1.6 2003/05/07 04:00:30 ohara Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <ox_toolkit.h>
#include "sm.h"

#define  receive_sm_command(x)   receive_int32((x))
 
/* WARNING: you must NOT use stack[stack_ptr]. */

static cmo **stack = NULL;
static int stack_ptr = 0;
static int stack_size = 0;
OXFILE *stack_oxfp = NULL;
int oxf_error(OXFILE *oxfp);

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

void push(cmo *ob)
{
    if (stack_ptr >= stack_size) {
        stack_extend();
    }
    stack[stack_ptr] = ob;
    stack_ptr++;
}

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
int sm_popCMO()
{
    cmo* m = pop();
    send_ox_cmo(stack_oxfp, m);
    return 0;
}

int sm_pops()
{
    cmo* m = pop();
    if (m->tag == CMO_INT32) {
        pops(((cmo_int32 *)m)->i);
    }else {
        push_error(-1, m); /* m is invalid. */
    }
    return 0;
}

void sm_run(int code)
{
    int (*func)(OXFILE *) = sm_search_f(code);
    if (func != NULL) {
        func(stack_oxfp);
    }else {
        ox_printf("oxc: unknown SM code(%d).\n", code);
    }
}

int sm_receive_ox()
{
    int tag;
    int code;

    tag = receive_ox_tag(stack_oxfp);
    if (oxf_error(stack_oxfp)) {
        return 0;
    }
    switch(tag) {
    case OX_DATA:
        push(receive_cmo(stack_oxfp));
        break;
    case OX_COMMAND:
        code = receive_sm_command(stack_oxfp);
        ox_printf("oxc: code = %d.\n", code);
        sm_run(code);
        break;
    default:
        ox_printf("illeagal message? ox_tag = (%d)\n", tag);
        return 0;
        break;
    }
    return 1;
}

int oxf_error(OXFILE *oxfp)
{
    int e = oxfp->error;
    if (e != 0) {
        oxfp->error = 0;
    }
    return e;
}

int sm(OXFILE *oxfp)
{
    stack_oxfp = oxfp;
    stack_extend();
    while (sm_receive_ox()) {
    }
    ox_printf("oxc: socket(%d) is closed.\n", stack_oxfp->fd);
    return 0;
}
