/*  $OpenXM: OpenXM/src/ox_pari/ox_pari.c,v 1.23 2021/03/25 07:03:21 noro Exp $  */

#include <signal.h>
#include "ox_pari.h"

OXFILE *fd_rw;

static int stack_size = 0;
static int stack_pointer = 0;
static cmo **stack = NULL;
extern int debug_print;
extern unsigned long precreal;
long paristack=10000000;

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
}

void init_pari()
{
  pari_init(paristack,2);
  mp_set_memory_functions(GC_malloc,gc_realloc,gc_free);
  gmp_check();
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
#if 0
  char *opts[] = {"no_ox_reset", NULL};
  mathcap_init2(OX_PARI_VERSION, ID_STRING, "ox_pari", NULL, NULL, opts);
#else
  mathcap_init2(OX_PARI_VERSION, ID_STRING, "ox_pari", NULL, NULL, NULL);
#endif
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
  return new_cmo_error2((cmo *)new_cmo_string(message));
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
  }
  if( strcmp( func->s, "exit" ) == 0 )
    exit(0);

  if ( !strcmp(func->s,"allocatemem") ) {
    paristack =  cmo_to_int(av[0]);
    pari_close();
    init_pari();
    return 0;
  } 
  if ( !strcmp(func->s,"pari_setprec") ) {
    long n,prec;

    n = cmo_to_int(av[0]);
    setrealprecision(n,&prec);
    return 0;
  } 
  parif =search_parif(func->s);
  if ( !parif ) {
    sprintf(buf,"%s : not implemented",func->s);
    push((cmo*)make_error2(buf));
    return -1;
  } else if ( parif->type <= 2 ) {
    /* type=1 => one GEN argument possibly with prec */ 
    /* type=2 => one GEN argument with optarg */ 
    /* type=3 => one GEN, return ulong */

    av0 = avma;
    z = cmo_to_GEN(av[0]); 
    prec = ac==2 ? ndec2prec(cmo_to_int(av[1])) : nbits2prec(precreal);
    if ( ismatrix(z) ) {
      int i,len;
      len = lg(z); 
      for ( i = 1; i < len; i++ )
        settyp(z[i],t_COL);
      settyp(z,t_MAT);
      z = shallowtrans(z);
    }
    pari_CATCH(CATCH_ALL) {
      GEN E = pari_err_last();
      long code = err_get_num(E);
      char *err = pari_err2str(E);
      if ( code == e_MEM || code == e_STACK ) {
        sprintf(buf,"%s\nIncrease PARI stack by pari(allocatemem,size).",err);
      } else 
        sprintf(buf,"An error occured in PARI :%s",err);
      push((cmo*)make_error2(buf));
      pari_CATCH_reset();
      avma = av0;
      return -1;
    }
    pari_TRY {
      ret = 0;
      if ( parif->type == 0 ) {
        gp_allocatemem(z);
        ret = av[0];
        /* allocatemem */
      } else if ( parif->type == 1 ) {
        m = (*parif->f)(z,prec);
        ret = GEN_to_cmo(m);
      } else if ( parif->type == 2 ) {
        m = (*parif->f)(z,parif->opt);
        ret = GEN_to_cmo(m);
      } else if ( parif->type == 3 ) {
        /* XXX */
        unsigned long a;
        a = (unsigned long)(*parif->f)(z);
        ret = (cmo *)new_cmo_int32((int)a);
      }
      avma = av0;
      push(ret);
      return 0;
    }
    pari_ENDCATCH
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
  case SM_shutdown:
    exit(0);
    break;
  default:
    ox_printf("receive_and_execute_sm_command : code=%d\n",code);
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
    ox_printf("receive : ox_data %d\n",tag);
    push(receive_cmo(fd_rw));
    break;
  case OX_COMMAND:
    ox_printf("receive : ox_command %d\n",tag);
    receive_and_execute_sm_command();
    break;
  default:
    ox_printf("receive : tag=%d\n",tag);
  }
  return 0;
}

#if defined(ANDROID)
jmp_buf ox_env;
#else
sigjmp_buf ox_env;
#endif

void usr1_handler(int sig)
{
#if defined(ANDROID)
  _longjmp(ox_env,1);
#else
  siglongjmp(ox_env,1);
#endif
}

#if defined(USE_OXPARI_LOG)
#define LOGFILE "/tmp/oxpari.log"
#endif

int main()
{
#if defined(ANDROID)
  if ( _setjmp(ox_env) ) {
#else
  if ( sigsetjmp(ox_env,~0) ) {
#endif
    ox_printf("resetting libpari and sending OX_SYNC_BALL...");
    init_pari();
    initialize_stack();
    send_ox_tag(fd_rw,OX_SYNC_BALL);
    ox_printf("done\n");
  } else {
    init_gc();

#if defined(LOGFILE)
    ox_stderr_init(fopen(LOGFILE,"a"));
#endif
    init_pari();
    initialize_stack();
  
    ox_printf("ox_pari\n");

    fd_rw = oxf_open(3);
    oxf_determine_byteorder_server(fd_rw);
  }

#if defined(__CYGWIN__)
  void *mysignal(int sig,void (*handler)(int m));
  mysignal(SIGUSR1,usr1_handler);
#else
  signal(SIGUSR1,usr1_handler);
#endif

  while(1){
    receive();
  }
}
