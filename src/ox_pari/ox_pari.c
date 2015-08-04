/*	$OpenXM$	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gmp.h"
#include "ox_toolkit.h"
OXFILE *fd_rw;

static int stack_size = 0;
static int stack_pointer = 0;
static cmo **stack = NULL;
extern int debug_print;

void init_pari(void);

#define INIT_S_SIZE 2048
#define EXT_S_SIZE  2048

void init_pari()
{
}

int initialize_stack()
{
	stack_pointer = 0;
 	stack_size = INIT_S_SIZE;
	stack = MALLOC(stack_size*sizeof(cmo*));
	return 0;
}

static int extend_stack()
{
	int size2 = stack_size + EXT_S_SIZE;
	cmo **stack2 = MALLOC(size2*sizeof(cmo*));
	memcpy(stack2, stack, stack_size*sizeof(cmo *));
	free(stack);
	stack = stack2;
	stack_size = size2;
	return 0;
}

int push(cmo* m)
{
 	stack[stack_pointer] = m;
	stack_pointer++;
	if(stack_pointer >= stack_size) {
		extend_stack();
	}
	return 0;
}

cmo* pop()
{
	if(stack_pointer > 0) {
		stack_pointer--;
		return stack[stack_pointer];
	}
	return new_cmo_null();
}

void pops(int n)
{
	stack_pointer -= n;
	if(stack_pointer < 0) {
		stack_pointer = 0;
	}
}

#define OX_PARI_VERSION 20150731
#define ID_STRING  "2015/07/31 15:00:00"

int sm_mathcap()
{
	mathcap_init(OX_PARI_VERSION, ID_STRING, "ox_cdd", NULL, NULL);
	push((cmo*)oxf_cmo_mathcap(fd_rw));
	return 0;
}

int sm_popCMO()
{
	cmo* m = pop();

	if(m != NULL) {
		send_ox_cmo(fd_rw, m);
		return 0;
	}
	return SM_popCMO;
}

cmo_error2 *make_error2(int code)
{
	return (cmo_error2 *) new_cmo_int32(-1);
}

int get_i()
{
	cmo *c = pop();
	if(c->tag == CMO_INT32) {
		return ((cmo_int32 *)c)->i;
	}else if(c->tag == CMO_ZZ) {
		return mpz_get_si(((cmo_zz *)c)->mpz);
	}
	make_error2(-1);
	return 0;
}

char *get_str()
{
	cmo *c = pop();
	if(c->tag == CMO_STRING) {
		return ((cmo_string *)c)->s;
	}
	make_error2(-1);
	return "";
}

int cmo2int(cmo *c)
{
	if(c->tag == CMO_INT32) {
		return ((cmo_int32 *)c)->i;
	}else if(c->tag == CMO_ZZ) {
		return mpz_get_si(((cmo_zz *)c)->mpz);
	} else if(c->tag == CMO_NULL){
		return 0;
	}

	return 0;
}

int sm_executeFunction()
{
	cmo_string *func = (cmo_string *)pop();
	if(func->tag != CMO_STRING) {
		printf("sm_executeFunction : func->tag is not CMO_STRING");fflush(stdout);
		push((cmo*)make_error2(0));
		return -1;
	}

    if(strcmp(func->s, "factor" ) == 0) {
		printf("afo\n");fflush(stdout);
		return 0;
	} else if( strcmp( func->s, "exit" ) == 0 ){
		pop();
		exit(0);
		return 0;
	} else {
		push((cmo*)make_error2(0));
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
		printf("receive_and_execute_sm_command : code=%d\n",code);fflush(stdout);
		break;
	}
	return 0;
}

int receive()
{
	int tag;

	tag = receive_ox_tag(fd_rw);
	switch(tag) {
	case OX_DATA:
		printf("receive : ox_data\n",tag);fflush(stdout);
		push(receive_cmo(fd_rw));
		break;
	case OX_COMMAND:
		printf("receive : ox_command\n",tag);fflush(stdout);
		receive_and_execute_sm_command();
		break;
	default:
		printf("receive : tag=%d\n",tag);fflush(stdout);
	}
	return 0;
}

int main()
{
	GC_INIT();
	ox_stderr_init(stderr);
	initialize_stack();
	init_pari();

	fprintf(stderr,"ox_pari\n");

	fd_rw = oxf_open(3);
	oxf_determine_byteorder_server(fd_rw);

	while(1){
		receive();
	}
}
