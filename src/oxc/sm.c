/* -*- mode: C -*- */
/* $OpenXM$ */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <ox_toolkit.h>
#include "sm.h"

/* WARNING: you must be use stack[stack_ptr]. */

static cmo **stack = NULL;
static int stack_ptr = 0;
static int sizeof_stack = 0;
#define DIFFERENCE_OF_STACK  1024

void extend_stack()
{
    int newsize = sizeof_stack + DIFFERENCE_OF_STACK;
    cmo **newstack = (cmo **)malloc(sizeof(cmo *)*newsize);
    if (stack != NULL) {
        memcpy(newstack, stack, sizeof(cmo *)*sizeof_stack);
        free(stack);
    }
    sizeof_stack = newsize;
    stack = newstack;
}

void push(cmo *ob)
{
    if (stack_ptr >= sizeof_stack) {
        extend_stack();
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
an sm_* function, called by sm_run, pushes an error obect.
*/
void sm_popCMO(OXFILE* oxfp)
{
    cmo* m = pop();
    send_ox_cmo(oxfp, m);
}

void sm_pops(OXFILE* oxfp)
{
    cmo* m = pop();
    if (m->tag == CMO_INT32) {
        pops(((cmo_int32 *)m)->i);
    }else {
		push_error(-1, m); /* m is invalid. */
	}
}

int receive_sm_command(OXFILE* oxfp)
{
    return receive_int32(oxfp);
}

void sm_run(OXFILE* oxfp, int code)
{
    int (*func)(OXFILE *) = sm_search_f(code);
    if (func != NULL) {
        func(oxfp);
    }else {
        fprintf(stderr, "oxc: unknown SM code(%d).\n", code);
    }
}

int receive_ox(OXFILE *oxfp)
{
    int tag;
    int code;

    tag = receive_ox_tag(oxfp);
    if (oxf_error(oxfp)) {
        return 0;
    }
    switch(tag) {
    case OX_DATA:
        push(receive_cmo(oxfp));
        break;
    case OX_COMMAND:
        code = receive_sm_command(oxfp);
        fprintf(stderr, "oxc: oxfp(%d), code = %d.\n", oxfp->fd, code);
        sm_run(oxfp, code);
        break;
    default:
        fprintf(stderr, "illeagal message? ox_tag = (%d)\n", tag);
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
    extend_stack();
    while (receive_ox(oxfp)) {
    }
    fprintf(stderr, "oxc: socket(%d) is closed.\n", oxfp->fd);
}
