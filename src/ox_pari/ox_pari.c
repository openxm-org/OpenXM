/*	$OpenXM: OpenXM/src/ox_pari/ox_pari.c,v 1.1 2015/08/04 05:24:44 noro Exp $	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pari/pari.h"
#include "gmp.h"
#include "gmp-impl.h"
#include "ox_toolkit.h"
OXFILE *fd_rw;

static int stack_size = 0;
static int stack_pointer = 0;
static cmo **stack = NULL;
extern int debug_print;
long paristack=10000000;

void init_pari(void);
cmo *GEN_to_cmo(GEN z);
cmo_zz *GEN_to_cmo_zz(GEN z);
cmo_list *GEN_to_cmo_list(GEN z);
GEN cmo_to_GEN(cmo *c);
GEN cmo_zz_to_GEN(cmo_zz *c);

#define INIT_S_SIZE 2048
#define EXT_S_SIZE  2048

void init_pari()
{
  pari_init(paristack,2);
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
	mathcap_init(OX_PARI_VERSION, ID_STRING, "ox_pari", NULL, NULL);
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
	return (cmo_error2 *) new_cmo_int32(code);
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

GEN cmo_zz_to_GEN(cmo_zz *c)
{
  mpz_ptr mpz;
  GEN z;
  long *ptr;
  int j,sgn,len;

  mpz = c->mpz;
  sgn = mpz_sgn(mpz);
  len = ABSIZ(mpz);
  ptr = (long *)PTR(mpz);
  z = cgeti(len+2);
  for ( j = 0; j < len; j++ )
    z[len-j+1] = ptr[j];
  setsigne(z,sgn);
  setlgefint(z,lg(z));
  return z;
}

cmo_zz *GEN_to_cmo_zz(GEN z)
{
  cmo_zz *c;

  c = new_cmo_zz();
  mpz_import(c->mpz,lgef(z)-2,1,sizeof(long),0,0,&z[2]);
  if ( signe(z) < 0 )
    mpz_neg(c->mpz,c->mpz);
  return c;
}

cmo_list *GEN_to_cmo_list(GEN z)
{
  cmo_list *c;
  cmo *ob;
  int i,len;

  c = new_cmo_list();
  len = lg(z)-1;
  for ( i = 1; i <= len; i++ ) {
    ob = GEN_to_cmo((GEN)z[i]);
    c = list_append(c,ob);
  }
  return c;
}


GEN cmo_to_GEN(cmo *c)
{
  switch ( c->tag ) {
  case CMO_ZERO:
  case CMO_ZZ: /* int */
    return cmo_zz_to_GEN((cmo_zz *)c);
  default:
    return 0;
  }
}

cmo *GEN_to_cmo(GEN z)
{
  if ( gcmp0(z) )
    return new_cmo_zero();
  switch ( typ(z) ) {
  case 1: /* int */
    return (cmo *)GEN_to_cmo_zz(z);
  case 17: case 18: /* vector */
    return (cmo *)GEN_to_cmo_list(z);
  case 19: /* matrix */
    return (cmo *)GEN_to_cmo_list(shallowtrans(z));
  default:
    return (cmo *)make_error2(typ(z));
  }
}


#define PARI_MAX_AC 64

int sm_executeFunction()
{
  int ac,i;
  cmo_int32 *c;
  cmo *av[PARI_MAX_AC];
  cmo *ret;
  GEN z,m;

	cmo_string *func = (cmo_string *)pop();
	if(func->tag != CMO_STRING) {
		printf("sm_executeFunction : func->tag is not CMO_STRING");fflush(stdout);
		push((cmo*)make_error2(0));
		return -1;
	}

	c = (cmo_int32 *)pop();
  ac = c->i;
  if ( ac > PARI_MAX_AC ) {
		push((cmo*)make_error2(0));
		return -1;
  }
  for ( i = 0; i < ac; i++ ) {
    av[i] = (cmo *)pop();
    fprintf(stderr,"arg%d:",i);
    print_cmo(av[i]);
    fprintf(stderr,"\n");
  }
  if(strcmp(func->s, "factor") == 0) {
    z = cmo_to_GEN(av[0]); 
    m = Z_factor(z);
    ret = GEN_to_cmo(m);
    push(ret);
		return 0;
  } else if(strcmp(func->s, "nextprime") == 0) {
    z = cmo_to_GEN(av[0]); 
    m = nextprime(z);
    ret = GEN_to_cmo(m);
    push(ret);
		return 0;
  } else if(strcmp(func->s, "det") == 0) {
    z = cmo_to_GEN(av[0]); 
    m = det(z);
    ret = GEN_to_cmo(m);
    push(ret);
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
		printf("receive : ox_data %d\n",tag);fflush(stdout);
		push(receive_cmo(fd_rw));
		break;
	case OX_COMMAND:
		printf("receive : ox_command %d\n",tag);fflush(stdout);
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
