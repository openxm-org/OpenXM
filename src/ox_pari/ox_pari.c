/*  $OpenXM: OpenXM/src/ox_pari/ox_pari.c,v 1.7 2015/08/18 05:04:35 noro Exp $  */

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
cmo_qq *GEN_to_cmo_qq(GEN z);
cmo_bf *GEN_to_cmo_bf(GEN z);
cmo_list *GEN_to_cmo_list(GEN z);
cmo_complex *GEN_to_cmo_cmo_complex(GEN z);
cmo_polynomial_in_one_variable *GEN_to_cmo_up(GEN z);
cmo_recursive_polynomial *GEN_to_cmo_rp(GEN z);

GEN cmo_to_GEN(cmo *c);
GEN cmo_int32_to_GEN(cmo_int32 *c);
GEN cmo_zz_to_GEN(cmo_zz *c);
GEN cmo_qq_to_GEN(cmo_qq *c);
GEN cmo_bf_to_GEN(cmo_bf *c);
GEN cmo_list_to_GEN(cmo_list *c);
GEN cmo_rp_to_GEN(cmo_recursive_polynomial *c);
GEN cmo_up_to_GEN(cmo_polynomial_in_one_variable *c);
GEN cmo_complex_to_GEN(cmo_complex *c);


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

cmo_error2 *make_error2(char *message)
{
  return (cmo_error2 *) new_cmo_string(message);
}

int get_i()
{
  cmo *c = pop();
  if(c->tag == CMO_INT32) {
    return ((cmo_int32 *)c)->i;
  }else if(c->tag == CMO_ZZ) {
    return mpz_get_si(((cmo_zz *)c)->mpz);
  }
  make_error2("get_i : invalid object");
  return 0;
}

char *get_str()
{
  cmo *c = pop();
  if(c->tag == CMO_STRING) {
    return ((cmo_string *)c)->s;
  }
  make_error2("get_str : invalid object");
  return "";
}

GEN cmo_int32_to_GEN(cmo_int32 *c)
{
  GEN z;
  int i,sgn;

  i = c->i;
  if ( !i ) return gen_0;
  z = cgeti(3);
  sgn = 1;
  if ( i < 0 ) {
    i = -i;
    sgn = -1;
  }
  z[2] = i;
  setsigne(z,sgn);
  setlgefint(z,lg(z));
  return z;
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

GEN cmo_qq_to_GEN(cmo_qq *c)
{
  GEN z,nm,den;
  
  z = cgetg(3,t_FRAC);
  nm = cmo_zz_to_GEN(new_cmo_zz_set_mpz(mpq_numref(c->mpq)));
  den = cmo_zz_to_GEN(new_cmo_zz_set_mpz(mpq_denref(c->mpq)));
  z[1] = (long)nm;
  z[2] = (long)den;
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

/* list->vector */

GEN cmo_list_to_GEN(cmo_list *c)
{
  GEN z;
  int i;
  cell *cell;

  z = cgetg(c->length+1,t_VEC);
  for ( i = 0, cell = c->head->next; cell != c->head; cell = cell->next, i++ ) {
    z[i+1] = (long)cmo_to_GEN(cell->cmo);
  }
  return z;
}

GEN cmo_complex_to_GEN(cmo_complex *c)
{
  GEN z;

  z = cgetg(3,t_COMPLEX);
  z[1] = (long)cmo_to_GEN(c->re);
  z[2] = (long)cmo_to_GEN(c->im);
  return z;
}

GEN cmo_up_to_GEN(cmo_polynomial_in_one_variable *c)
{
  GEN z;
  int d,i;
  cell *cell;

  d = c->head->next->exp;
  z = cgetg(d+3,t_POL);
  setsigne(z,1);
  setvarn(z,c->var);
  setlgef(z,d+3);
  for ( i = 2; i <= d+2; i++ )
    z[i] = (long)gen_0;
  for ( cell = c->head->next; cell != c->head; cell = cell->next ) {
    z[2+cell->exp] = (long)cmo_to_GEN(cell->cmo);
  }
  return z;
}

cmo_list *current_ringdef;

void register_variables(cmo_list *ringdef)
{
  current_ringdef = ringdef;
}

GEN cmo_rp_to_GEN(cmo_recursive_polynomial *c)
{
  register_variables(c->ringdef);
  switch ( c->coef->tag ) {
  case CMO_ZERO:
  case CMO_NULL:
    return gen_0;
  case CMO_INT32:
    return cmo_int32_to_GEN((cmo_int32 *)c->coef);
  case CMO_ZZ:
    return cmo_zz_to_GEN((cmo_zz *)c->coef);
  case CMO_QQ:
    return cmo_qq_to_GEN((cmo_qq *)c->coef);
  case CMO_POLYNOMIAL_IN_ONE_VARIABLE:
    return cmo_up_to_GEN((cmo_polynomial_in_one_variable *)c->coef);
  default:
    return 0;
  }
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

cmo_qq *GEN_to_cmo_qq(GEN z)
{
  cmo_qq *c;
  GEN num,den;

  num = (GEN)z[1];
  den = (GEN)z[2];
  c = new_cmo_qq();
  mpz_import(mpq_numref(c->mpq),lgef(num)-2,1,sizeof(long),0,0,&num[2]);
  mpz_import(mpq_denref(c->mpq),lgef(num)-2,1,sizeof(long),0,0,&den[2]);
  if ( signe(num)*signe(den) < 0 )
    mpz_neg(mpq_numref(c->mpq),mpq_numref(c->mpq));
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

cmo_complex *GEN_to_cmo_complex(GEN z)
{
  cmo_complex *c;

  c = new_cmo_complex();
  c->re = GEN_to_cmo((GEN)z[1]);
  c->im = GEN_to_cmo((GEN)z[2]);
  return c;
}

cmo_polynomial_in_one_variable *GEN_to_cmo_up(GEN z)
{
  cmo_polynomial_in_one_variable *c;
  int i;
  cmo *coef;

  c = new_cmo_polynomial_in_one_variable(varn(z));
  for ( i = lg(z)-1; i >= 2; i-- )
    if ( (GEN)z[i] != gen_0 ) {
      coef = GEN_to_cmo((GEN)z[i]);
      list_append_monomial((cmo_list *)c, coef, i-2);
    }
  return c;
}

cmo_recursive_polynomial *GEN_to_cmo_rp(GEN z)
{
  cmo_recursive_polynomial *c;

  if ( !signe(z) ) return (cmo_recursive_polynomial *)new_cmo_zero();
  c = new_cmo_recursive_polynomial(current_ringdef,(cmo *)GEN_to_cmo_up(z));
  return c;
}

GEN cmo_to_GEN(cmo *c)
{
  switch ( c->tag ) {
  case CMO_ZERO:
  case CMO_NULL:
    return gen_0;
  case CMO_ZZ: /* int */
    return cmo_zz_to_GEN((cmo_zz *)c);
  case CMO_IEEE_DOUBLE_FLOAT:
     return dbltor(((cmo_double *)c)->d);
  case CMO_BIGFLOAT: /* bigfloat */
    return cmo_bf_to_GEN((cmo_bf *)c);
  case CMO_LIST:
    return cmo_list_to_GEN((cmo_list *)c);
  case CMO_RECURSIVE_POLYNOMIAL:
    return cmo_rp_to_GEN((cmo_recursive_polynomial *)c);
  case CMO_POLYNOMIAL_IN_ONE_VARIABLE:
    return cmo_up_to_GEN((cmo_polynomial_in_one_variable *)c);
  default:
    return 0;
  }
}

cmo *GEN_to_cmo(GEN z)
{
  char buf[BUFSIZ];

  if ( gcmp0(z) )
    return new_cmo_zero();
  switch ( typ(z) ) {
  case t_INT: /* int */
    return (cmo *)GEN_to_cmo_zz(z);
  case t_REAL: /* bigfloat */
    return (cmo *)GEN_to_cmo_bf(z);
  case t_FRAC: /* rational number */
    return (cmo *)GEN_to_cmo_qq(z);
  case t_COMPLEX: /* complex */
    return (cmo *)GEN_to_cmo_complex(z);
  case t_POL:
    return (cmo *)GEN_to_cmo_rp(z);
  case t_VEC: case t_COL: /* vector */
    return (cmo *)GEN_to_cmo_list(z);
  case t_MAT: /* matrix */
    return (cmo *)GEN_to_cmo_list(shallowtrans(z));
  default:
    sprintf(buf,"GEN_to_cmo : unsupported type=%d",(int)typ(z));
    return (cmo *)make_error2(buf);
  }
}
/* type=1 : num/poly arg, type=2 : matrix arg */

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

  /* poly/poly */
  {"centerlift",centerlift,1},
  {"content",content,1},

  /* poly/array */
  {"galois",galois,1},
  {"roots",roots,1},

  /* mat/mat */
  {"adj",adj,2},
  {"lll",lll,2},
  {"lllgen",lllgen,2},
  {"lllgram",lllgram,2},
  {"lllgramgen",lllgramgen,2},
  {"lllgramint",lllgramint,2},
  {"lllgramkerim",lllgramkerim,2},
  {"lllgramkerimgen",lllgramkerimgen,2},
  {"lllint",lllint,2},
  {"lllkerim",lllkerim,2},
  {"lllkerimgen",lllkerimgen,2},
  {"trans",gtrans,2},
  {"eigen",eigen,2},
  {"hermite",hnf,2},
  {"mat",gtomat,2},
  {"matrixqz2",matrixqz2,2},
  {"matrixqz3",matrixqz3,2},
  {"hess",hess,2},
  {"ker",ker,2},
  {"keri",keri,2},
  {"kerint",kerint,2},
  {"kerintg1",kerint1,2},

  /* mat/poly */
  {"det",det,2},
  {"det2",det2,2},

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

int ismatrix(GEN z)
{
  int len,col,i;

  if ( typ(z) != t_VEC ) return 0;
  if ( typ((GEN)z[1]) != t_VEC ) return 0;
  len = lg(z); col = lg((GEN)z[1]);
  for ( i = 2; i < len; i++ )
    if ( lg((GEN)z[i]) != col ) return 0;
  return 1;
}

int sm_executeFunction()
{
  pari_sp av0;
  int ac,i;
  cmo_int32 *c;
  cmo *av[PARI_MAX_AC];
  cmo *ret;
  GEN z,m;
  struct parif *parif;
  unsigned long prec;
  char buf[BUFSIZ];

  if ( setjmp(GP_DATA->env) ) {
    sprintf(buf,"sm_executeFunction : an error occured in PARI.");
    push((cmo*)make_error2(buf));
    return -1;
  }
  cmo_string *func = (cmo_string *)pop();
  if(func->tag != CMO_STRING) {
    sprintf(buf,"sm_executeFunction : func->tag=%d is not CMO_STRING",func->tag);
    push((cmo*)make_error2(buf));
    return -1;
  }

  c = (cmo_int32 *)pop();
  ac = c->i;
  if ( ac > PARI_MAX_AC ) {
    push((cmo*)make_error2("sm_executeFunction : too many arguments"));
    return -1;
  }
  for ( i = 0; i < ac; i++ ) {
    av[i] = (cmo *)pop();
//    fprintf(stderr,"arg%d:",i);
//    print_cmo(av[i]);
//    fprintf(stderr,"\n");
  }
  if( strcmp( func->s, "exit" ) == 0 )
    exit(0);

  parif =search_parif(func->s);
  if ( !parif ) {
    sprintf(buf,"%s : not implemented",func->s);
    push((cmo*)make_error2(buf));
    return -1;
 } else if ( parif->type == 0 ) {
    /* one long int variable */ 
    int a = cmo_to_int(av[0]);
    a = (int)(parif->f)(a);
    ret = (cmo *)new_cmo_int32(a);
    push(ret);
    return 0;
  } else if ( parif->type == 1 || parif->type == 2 ) {
    /* one number/poly/matrix argument possibly with prec */ 
    av0 = avma;
    z = cmo_to_GEN(av[0]); 
    prec = ac==2 ? cmo_to_int(av[1])*3.32193/32+3 : precreal;
    if ( ismatrix(z) ) {
      int i,len;
      len = lg(z); 
      for ( i = 1; i < len; i++ )
        settyp(z[i],t_COL);
      settyp(z,t_MAT);
      z = shallowtrans(z);
    }
    printf("input : "); output(z);
    m = (*parif->f)(z,prec);
    ret = GEN_to_cmo(m);
    avma = av0;
    push(ret);
    return 0;
  } else {
    sprintf(buf,"%s : not implemented",func->s);
    push((cmo*)make_error2(buf));
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
