/* $OpenXM$
*/

#include <stdio.h>
#include <stdlib.h>
#include "ox_toolkit.h"

OXFILE *fd_rw;

#define INIT_S_SIZE 2048
#define EXT_S_SIZE  2048

static int stack_size = 0;
static int stack_pointer = 0;
static cmo **stack = NULL;

int Debug=1;

int show_stack_top() {
  cmo *data;
  if (stack_pointer > 0) {
    data=stack[stack_pointer-1];
    printf("tag=%d\n",data->tag);
  }else {
    printf("The stack is empty.\n");
  }
}

void init_gc()
{
  GC_INIT();
}

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

#define OX_GSL_VERSION 2018032901
#define ID_STRING  "2018/03/29 13:56:00"

int sm_mathcap()
{
    mathcap_init(OX_GSL_VERSION, ID_STRING, "ox_gsl", NULL, NULL);
    push(oxf_cmo_mathcap(fd_rw));
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
    return new_cmo_int32(-1);
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

int my_add_int32()
{
    int x, y;
    get_xy(&x, &y);
    push(new_cmo_int32(x+y));
}


int sm_executeFunction()
{
    cmo_string *func = (cmo_string *)pop();
    if (func->tag != CMO_STRING) {
        push(make_error2(0));
        return -1;
    }
    if (strcmp(func->s, "add_int32") == 0) {
        my_add_int32();
    }else {
        push(make_error2(0));
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
        if (Debug) show_stack_top();
        break;
    case OX_COMMAND:
        if (Debug) show_stack_top();
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
    init_gc();

    fd_rw = oxf_open(3);
    oxf_determine_byteorder_server(fd_rw);

    while(1) {
        receive();
    }
}
