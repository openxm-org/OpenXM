/* -*- mode: C; coding: euc-japan -*- */
/* $OpenXM: OpenXM/src/ox_toolkit/ox_Xsample.c,v 1.8 2005/07/26 12:52:05 ohara Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include "ox_toolkit.h"

OXFILE *fd_rw;

#define INIT_S_SIZE 2048
#define EXT_S_SIZE  2048

static int stack_size = 0;
static int stack_pointer = 0;
static cmo **stack = NULL;

int initialize_stack()
{
    stack_pointer = 0;
    stack_size = INIT_S_SIZE;
    stack = malloc(stack_size*sizeof(cmo*));
}

static int extend_stack()
{
    int size2 = stack_size + EXT_S_SIZE;
    cmo **stack2 = malloc(size2*sizeof(cmo*));
    memcpy(stack2, stack, stack_size*sizeof(cmo *));
    free(stack);
    stack = stack2;
    stack_size = size2;
}

int push(cmo* m)
{
    stack[stack_pointer] = m;
    stack_pointer++;
    if (stack_pointer >= stack_size) {
        extend_stack();
    }
}

cmo* pop()
{
    if (stack_pointer > 0) {
        stack_pointer--;
        return stack[stack_pointer];
    }
    return new_cmo_null();
}

void pops(int n)
{
    stack_pointer -= n;
    if (stack_pointer < 0) {
        stack_pointer = 0;
    }
}

#define OX_XSAMPLE_VERSION 0x11121400
#define ID_STRING  "1999/12/14 15:25:00"

int sm_mathcap()
{
    mathcap_init(OX_XSAMPLE_VERSION, ID_STRING, "ox_Xsample", NULL, NULL);
    push((cmo *)oxf_cmo_mathcap(fd_rw));
    return 0;
}

int sm_popCMO()
{
    cmo* m = pop();

    if (m != NULL) {
        send_ox_cmo(fd_rw, m);
        return 0;
    }
    return SM_popCMO;
}

cmo_error2 *make_error2(int code)
{
    return (cmo_error2 *)new_cmo_int32(-1);
}

int get_i()
{
    cmo *c = pop();
    if (c->tag == CMO_INT32) {
        return ((cmo_int32 *)c)->i;
    }else if (c->tag == CMO_ZZ) {
        return mpz_get_si(((cmo_zz *)c)->mpz);
    }
    make_error2(-1);
    return 0;
}

int get_xy(int *x, int *y)
{
    pop();
    *x = get_i();
    *y = get_i();
}

int my_setpixel()
{
    int x, y;
    get_xy(&x, &y);
    setpixel(x, y);
    push((cmo *)new_cmo_int32(0));
}

int my_moveto()
{
    int x, y;
    get_xy(&x, &y);
    moveto(x, y);
    push((cmo *)new_cmo_int32(0));
}

int my_lineto()
{
    int x, y;
    get_xy(&x, &y);
    lineto(x, y);
    push((cmo *)new_cmo_int32(0));
}

int my_clear()
{
    /* dummy */
    pop();
    push((cmo *)new_cmo_int32(0));
}

int sm_executeFunction()
{
    cmo_string *func = (cmo_string *)pop();
    if (func->tag != CMO_STRING) {
        push((cmo *)make_error2(0));
        return -1;
    }
    if (strcmp(func->s, "setpixel") == 0) {
        my_setpixel();
    }else if (strcmp(func->s, "moveto") == 0) {
        my_moveto();
    }else if (strcmp(func->s, "lineto") == 0) {
        my_lineto();
    }else if (strcmp(func->s, "clear") == 0) {
        my_clear();
    }else {
        push((cmo *)make_error2(0));
        return -1;
    }
}


int receive_and_execute_sm_command()
{
    int code = receive_int32(fd_rw);
    switch(code) {
    case SM_popCMO:
        sm_popCMO();
        break;
    case SM_executeFunction:
        sm_executeFunction();
        break;
    case SM_mathcap:
        sm_mathcap();
        break;
    case SM_setMathCap:
        pop();
        break;
    default:
		;
    }
}

int receive()
{
    int tag;

    tag = receive_ox_tag(fd_rw);
    switch(tag) {
    case OX_DATA:
        push(receive_cmo(fd_rw));
        break;
    case OX_COMMAND:
        receive_and_execute_sm_command();
        break;
    default:
		;
    }
    return 0;
}

int main()
{
    ox_stderr_init(stderr);
    initialize_stack();

	fd_rw = oxf_open(3);
	oxf_determine_byteorder_server(fd_rw);

    gopen();
    while(1) {
        receive();
        gFlush();
    }
    gclose();
}
