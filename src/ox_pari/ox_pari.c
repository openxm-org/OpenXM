/*  $OpenXM: OpenXM/src/ox_pari/ox_pari.c,v 1.3 2015/08/17 05:18:35 noro Exp $  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pari/pari.h"
#include "pari/paripriv.h"
#include "gmp.h"
#include "gmp-impl.h"
#include "mpfr.h"
#include "ox_toolkit.h"
OXFILE *fd_rw;

#define MPFR_PREC(x)      ((x)->_mpfr_prec)
#define MPFR_EXP(x)       ((x)->_mpfr_exp)
#define MPFR_MANT(x)      ((x)->_mpfr_d)
#define MPFR_LAST_LIMB(x) ((MPFR_PREC (x) - 1) / GMP_NUMB_BITS)
#define MPFR_LIMB_SIZE(x) (MPFR_LAST_LIMB (x) + 1)

static int stack_size = 0;
static int stack_pointer = 0;
static cmo **stack = NULL;
extern int debug_print;
long paristack=10000000;

void init_pari(void);
cmo *GEN_to_cmo(GEN z);
cmo_zz *GEN_to_cmo_zz(GEN z);
cmo_bf *GEN_to_cmo_bf(GEN z);
cmo_list *GEN_to_cmo_list(GEN z);
GEN cmo_to_GEN(cmo *c);
GEN cmo_zz_to_GEN(cmo_zz *c);
GEN cmo_bf_to_GEN(cmo_bf *c);

#define INIT_S_SIZE 2048
#define EXT_S_SIZE  2048

void *gc_realloc(void *p,size_t osize,size_t nsize)
{
  return (void *)GC_realloc(p,nsize);        
}

void gc_free(void *p,size_t size)
{
  GC_free(p);
}

void init_gc()
{
  GC_INIT();
  mp_set_memory_functions(GC_malloc,gc_realloc,gc_free);
}

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

GEN cmo_bf_to_GEN(cmo_bf *c)
{
  mpfr_ptr mpfr;
  GEN z;
  int sgn,len,j;
  long exp;
  long *ptr;

  mpfr = c->mpfr;
  sgn = MPFR_SIGN(mpfr);
  exp = MPFR_EXP(mpfr)-1;
  len = MPFR_LIMB_SIZE(mpfr);
  ptr = (long *)MPFR_MANT(mpfr);
  z = cgetr(len+2);
  for ( j = 0; j < len; j++ )
    z[len-j+1] = ptr[j];
  z[1] = evalsigne(sgn)|evalexpo(exp);
  setsigne(z,sgn);
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

cmo_bf *GEN_to_cmo_bf(GEN z)
{
  cmo_bf *c;
  int len,prec,j;
  long *ptr;

  c = new_cmo_bf();
  len = lg(z)-2;
  prec = len*sizeof(long)*8;
  mpfr_init2(c->mpfr,prec);
  ptr = (long *)MPFR_MANT(c->mpfr);
  for ( j = 0; j < len; j++ )
    ptr[j] = z[len-j+1];
  MPFR_EXP(c->mpfr) = (long long)(expo(z)+1);
  MPFR_SIGN(c->mpfr) = gsigne(z);
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
    return gen_0;
  case CMO_ZZ: /* int */
    return cmo_zz_to_GEN((cmo_zz *)c);
  case CMO_BIGFLOAT: /* bigfloat */
    return cmo_bf_to_GEN((cmo_bf *)c);
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
  case 2: /* bigfloat */
    return (cmo *)GEN_to_cmo_bf(z);
  case 17: case 18: /* vector */
    return (cmo *)GEN_to_cmo_list(z);
  case 19: /* matrix */
    return (cmo *)GEN_to_cmo_list(shallowtrans(z));
  default:
    return (cmo *)make_error2(typ(z));
  }
}

struct parif {
  char *name;
  GEN (*f)();
  int type;
} parif_tab[] = {
/* (ulong)allocatemoremem(ulong) */
  {"allocatemem",(GEN (*)())allocatemoremem,0},
/* num/num */
  {"abs",gabs,1},
  {"erfc",gerfc,1},
  {"arg",garg,1},
  {"isqrt",racine,1},
  {"gamma",ggamma,1},
  {"zeta",gzeta,1},
  {"floor",gfloor,1},
  {"frac",gfrac,1},
  {"imag",gimag,1},
  {"conj",gconj,1},
  {"ceil",gceil,1},
  {"isprime",gisprime,2},
  {"bigomega",gbigomega,1},
  {"denom",denom,1},
  {"numer",numer,1},
  {"lngamma",glngamma,1},
  {"logagm",glogagm,1},
  {"classno",classno,1},
  {"classno2",classno2,1},
  {"dilog",dilog,1},
  {"disc",discsr,1},
  {"discf",discf,1},
  {"nextprime",nextprime,1},
  {"eintg1",eint1,1},
  {"eta",eta,1},
  {"issqfree",gissquarefree,1},
  {"issquare",gcarreparfait,1},
  {"gamh",ggamd,1},
  {"hclassno",classno3,1},

  /* num/array */
  {"binary",binaire,1},
  {"factorint",factorint,2},
  {"factor",Z_factor,1},
  {"cf",gcf,1},
  {"divisors",divisors,1},
  {"smallfact",smallfact,1},

  /* mat/mat */
  {"adj",adj,1},
  {"lll",lll,1},
  {"lllgen",lllgen,1},
  {"lllgram",lllgram,1},
  {"lllgramgen",lllgramgen,1},
  {"lllgramint",lllgramint,1},
  {"lllgramkerim",lllgramkerim,1},
  {"lllgramkerimgen",lllgramkerimgen,1},
  {"lllint",lllint,1},
  {"lllkerim",lllkerim,1},
  {"lllkerimgen",lllkerimgen,1},
  {"trans",gtrans,1},
  {"eigen",eigen,1},
  {"hermite",hnf,1},
  {"mat",gtomat,1},
  {"matrixqz2",matrixqz2,1},
  {"matrixqz3",matrixqz3,1},
  {"hess",hess,1},

  /* mat/poly */
  {"det",det,1},
  {"det2",det2,1},

  /* poly/poly */
  {"centerlift",centerlift,1},
  {"content",content,1},

  /* poly/array */
  {"galois",galois,1},
  {"roots",roots,1},

};

#define PARI_MAX_AC 64

struct parif *search_parif(char *name)
{
  int tablen,i;

  tablen = sizeof(parif_tab)/sizeof(struct parif);
  for ( i = 0; i < tablen; i++ ) {
    if ( !strcmp(parif_tab[i].name,name) )
      return &parif_tab[i];
  }
  return 0;
}

int sm_executeFunction()
{
  long ltop,lbot;
  int ac,i;
  cmo_int32 *c;
  cmo *av[PARI_MAX_AC];
  cmo *ret;
  GEN z,m;
  struct parif *parif;

  if ( setjmp(GP_DATA->env) ) {
    printf("sm_executeFunction : an error occured.\n");fflush(stdout);
    push((cmo*)make_error2(0));
    return -1;
  }
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
  if( strcmp( func->s, "exit" ) == 0 )
    exit(0);

  parif =search_parif(func->s);
  if ( !parif ) {
    push((cmo*)make_error2(0));
    return -1;
 } else if ( parif->type == 0 ) {
    /* one long int variable */ 
    int a = cmo_to_int(av[0]);
    a = (int)(parif->f)(a);
    ret = (cmo *)new_cmo_int32(a);
    push(ret);
    return 0;
  } else if ( parif->type == 1 ) {
    /* one variable possibly with prec */ 
    unsigned long prec;

    ltop = avma;
    z = cmo_to_GEN(av[0]); 
    if ( ac == 2 ) {
      prec = cmo_to_int(av[1])*3.32193/32+3;
    } else
      prec = precreal;
    m = (*parif->f)(z,prec);
    lbot = avma;
    ret = GEN_to_cmo(m);
//    gerepile(ltop,lbot,0);
    push(ret);
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
  init_gc();
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
