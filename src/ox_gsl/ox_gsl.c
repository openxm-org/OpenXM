/* $OpenXM: OpenXM/src/ox_gsl/ox_gsl.c,v 1.17 2019/10/23 07:00:43 takayama Exp $
*/

#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <math.h>
#include "ox_gsl.h"
#include "call_gsl.h" // need only when you bind call_gsl functions.
#include "call_gsl_sf.h"

OXFILE *fd_rw;

#define INIT_S_SIZE 2048
#define EXT_S_SIZE  2048

static int stack_size = 0;
static int stack_pointer = 0;
static cmo **stack = NULL;

int Debug=1;

void show_stack_top() {
  cmo *data;
  if (stack_pointer > 0) {
    data=stack[stack_pointer-1];
    print_cmo(data); printf("\n");
  }else {
    printf("The stack is empty.\n");
  }
}

void *gc_realloc(void *p,size_t osize,size_t nsize)
{ return (void *)GC_realloc(p,nsize);}

void gc_free(void *p,size_t size)
{ GC_free(p);}

void init_gc()
{ GC_INIT();
  mp_set_memory_functions(GC_malloc,gc_realloc,gc_free);
  init_dic();  // initialize ox_eval.c
}

void initialize_stack()
{
    stack_pointer = 0;
    stack_size = INIT_S_SIZE;
    stack = GC_malloc(stack_size*sizeof(cmo*));
}

static void extend_stack()
{
    int size2 = stack_size + EXT_S_SIZE;
    cmo **stack2 = malloc(size2*sizeof(cmo*));
    memcpy(stack2, stack, stack_size*sizeof(cmo *));
    free(stack);
    stack = stack2;
    stack_size = size2;
}

void push(cmo* m)
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
  int available_cmo[]={
    CMO_NULL,
    CMO_INT32,
//    CMO_DATUM,
    CMO_STRING,
    CMO_MATHCAP,
    CMO_LIST,
//    CMO_MONOMIAL32,
    CMO_ZZ,
//    CMO_QQ,
    CMO_BIGFLOAT32,
    CMO_COMPLEX,
    CMO_IEEE_DOUBLE_FLOAT,
    CMO_ZERO,
//    CMO_DMS_GENERIC,
//    CMO_RING_BY_NAME,
//    CMO_INDETERMINATE,
//    CMO_DISTRIBUTED_POLYNOMIAL,
//    CMO_RECURSIVE_POLYNOMIAL,
//    CMO_POLYNOMIAL_IN_ONE_VARIABLE,
    CMO_TREE,
    CMO_ERROR2,
    0};
  int available_sm_command[]={
    SM_popCMO,
    SM_popString,
    SM_mathcap,
    SM_pops,
//    SM_executeStringByLocalParser,
    SM_executeFunction,
    SM_setMathCap,
    SM_shutdown,
    SM_control_kill,
    SM_control_reset_connection,
    SM_control_spawn_server,
    SM_control_terminate_server,
    0};
    mathcap_init(OX_GSL_VERSION, ID_STRING, "ox_gsl", available_cmo,available_sm_command);
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

cmo *make_error2(const char *reason,const char *fname,int line,int code)
{
// gsl_error_handler_t void handler(const char *reason,const char *file,int line, int gsl_errno)
    cmo *ms;
    cmo *err;
    cmo *m;
    cmo **argv;
    int n;
    char *s;
    n = 5;
    argv = (cmo **) GC_malloc(sizeof(cmo *)*n);
    ms = (cmo *)new_cmo_string("Error"); argv[0] = ms; 
    if (reason != NULL) {s = (char *)GC_malloc(strlen(reason)+1); strcpy(s,reason);
    }else strcpy(s,"");
    ms = (cmo *) new_cmo_string(s); argv[1] = ms;
    if (fname != NULL) {s = (char *)GC_malloc(strlen(fname)+1); strcpy(s,fname);
    }else strcpy(s,"");
    ms = (cmo *) new_cmo_string(s); argv[2] = ms;
    err = (cmo *)new_cmo_int32(line); argv[3] = err;
    err = (cmo *)new_cmo_int32(code); argv[4] = err;

    m = (cmo *)new_cmo_list_array((void *)argv,n);
    return (m);
}

int get_i()
{
    cmo *c = pop();
    if (c->tag == CMO_INT32) {
        return ((cmo_int32 *)c)->i;
    }else if (c->tag == CMO_ZZ) {
        return mpz_get_si(((cmo_zz *)c)->mpz);
    }else if (c->tag == CMO_NULL) {
        return(0);
    }else if (c->tag == CMO_ZERO) {
        return(0);
    }
    myhandler("get_i: not an integer",NULL,0,-1);
    return 0;
}

void get_xy(int *x, int *y)
{
    pop();
    *x = get_i();
    *y = get_i();
}

void my_add_int32()
{
    int x, y;
    get_xy(&x, &y);
    push((cmo *)new_cmo_int32(x+y));
}

double get_double()
{
#define mympz(c) (((cmo_zz *)c)->mpz)
    cmo *c = pop();
    if (c->tag == CMO_INT32) {
        return( (double) (((cmo_int32 *)c)->i) );
    }else if (c->tag == CMO_IEEE_DOUBLE_FLOAT) {
        return (((cmo_double *)c)->d);  // see ox_toolkit.h
    }else if (c->tag == CMO_ZZ) {
       if ((mpz_cmp_si(mympz(c),(long int) 0x7fffffff)>0) ||
           (mpz_cmp_si(mympz(c),(long int) -0x7fffffff)<0)) {
	 myhandler("get_double: out of int32",NULL,0,-1);
         return(NAN);
       }
       return( (double) mpz_get_si(((cmo_zz *)c)->mpz));
    }else if (c->tag == CMO_NULL) {
        return(0);
    }else if (c->tag == CMO_ZERO) {
        return(0);
    }
    myhandler("get_double: not a double",NULL,0,-1);
    return(NAN);
}
/* get_double() will be obsolted and will be replaced by cmo2double(c) */
double cmo2double(cmo *c)
{
#define mympz(c) (((cmo_zz *)c)->mpz)
  if (c == NULL) c = pop();
    if (c->tag == CMO_INT32) {
        return( (double) (((cmo_int32 *)c)->i) );
    }else if (c->tag == CMO_IEEE_DOUBLE_FLOAT) {
        return (((cmo_double *)c)->d);  // see ox_toolkit.h
    }else if (c->tag == CMO_ZZ) {
       if ((mpz_cmp_si(mympz(c),(long int) 0x7fffffff)>0) ||
           (mpz_cmp_si(mympz(c),(long int) -0x7fffffff)<0)) {
	 myhandler("get_double: out of int32",NULL,0,-1);
         return(NAN);
       }
       return( (double) mpz_get_si(((cmo_zz *)c)->mpz));
    }else if (c->tag == CMO_NULL) {
        return(0);
    }else if (c->tag == CMO_ZERO) {
        return(0);
    }
    myhandler("cmo2double: not a double",NULL,0,-1);
    return(NAN);
}

void my_add_double() {
  double x,y;
  pop();
  y = get_double();
  x = get_double();
  push((cmo *)new_cmo_double(x+y));
}

double *get_double_list(int *length) {
  cmo *c;
  cmo *entry;
  cell *cellp;
  double *d;
  int n,i;
  c = pop();
  if (c->tag != CMO_LIST) {
//    make_error2("get_double_list",NULL,0,-1);
    *length=-1; return(0);
  }
  n = *length = list_length((cmo_list *)c);
  d = (double *) GC_malloc(sizeof(double)*(*length+1));
  cellp = list_first((cmo_list *)c);
  entry = cellp->cmo;
  for (i=0; i<n; i++) {
    if (Debug) {
      printf("entry[%d]=",i); print_cmo(entry); printf("\n");
    }
    if (entry->tag == CMO_INT32) {
      d[i]=( (double) (((cmo_int32 *)entry)->i) );
    }else if (entry->tag == CMO_IEEE_DOUBLE_FLOAT) {
      d[i]=((cmo_double *)entry)->d;  
    }else if (entry->tag == CMO_ZZ) {
      d[i]=( (double) mpz_get_si(((cmo_zz *)entry)->mpz));
    }else if (entry->tag == CMO_NULL) {
      d[i]= 0;
    }else {
      fprintf(stderr,"entries of the list should be int32 or zz or double\n");
      *length = -1;
      myhandler("get_double_list",NULL,0,-1);
      return(NULL);
    }
    cellp = list_next(cellp);
    entry = cellp->cmo;
  }
  return(d);
}
/* get_double_list will be obsolted and will be replaced by cmo2double_list() */
double *cmo2double_list(int *length,cmo *c) {
  cmo *entry;
  cell *cellp;
  double *d;
  int n,i;
  if (c == NULL) c = pop();
  if (c->tag != CMO_LIST) {
//    make_error2("get_double_list",NULL,0,-1);
    *length=-1; return(0);
  }
  n = *length = list_length((cmo_list *)c);
  d = (double *) GC_malloc(sizeof(double)*(*length+1));
  cellp = list_first((cmo_list *)c);
  entry = cellp->cmo;
  for (i=0; i<n; i++) {
    if (Debug) {
      printf("entry[%d]=",i); print_cmo(entry); printf("\n");
    }
    if (entry->tag == CMO_INT32) {
      d[i]=( (double) (((cmo_int32 *)entry)->i) );
    }else if (entry->tag == CMO_IEEE_DOUBLE_FLOAT) {
      d[i]=((cmo_double *)entry)->d;  
    }else if (entry->tag == CMO_ZZ) {
      d[i]=( (double) mpz_get_si(((cmo_zz *)entry)->mpz));
    }else if (entry->tag == CMO_NULL) {
      d[i]= 0;
    }else {
      fprintf(stderr,"entries of the list should be int32 or zz or double\n");
      *length = -1;
      myhandler("get_double_list",NULL,0,-1);
      return(NULL);
    }
    cellp = list_next(cellp);
    entry = cellp->cmo;
  }
  return(d);
}
void show_double_list() {
  int n;
  double *d;
  int i;
  pop(); // pop argument number;
  d = get_double_list(&n);
  if (n < 0) fprintf(stderr,"Error in the double list\n");
  printf("show_double_list: length=%d\n",n);
  for (i=0; i<n; i++) {
    printf("%lg, ",d[i]);
  }
  printf("\n");
}

char *get_string() {
  cmo *c;
  c = pop();
  if (c->tag == CMO_STRING) {
    return (((cmo_string *)c)->s);
  }
  // make_error2(-1);
  return(NULL);
}

void test_ox_eval() {
  cmo *c;
  double d=0;
  pop();
  c=pop();
  if (Debug) {
    ox_printf("cmo *c="); print_cmo(c); ox_printf("\n");
  }
  init_dic();
  register_entry("x",1.25);
  if (eval_cmo(c,&d) == 0) myhandler("eval_cmo failed",NULL,0,-1);
  push((cmo *)new_cmo_double(d));
}

int sm_executeFunction()
{
    cmo_string *func = (cmo_string *)pop();
    if (func->tag != CMO_STRING) {
        push(make_error2("sm_executeFunction, not CMO_STRING",NULL,0,-1));
        return -1;
    }
    init_dic();
    // Test functions
    if (strcmp(func->s, "add_int32") == 0) {
        my_add_int32();
    }else if (strcmp(func->s,"add_double")==0) {
        my_add_double();
    }else if (strcmp(func->s,"show_double_list")==0) {
        show_double_list();
    }else if (strcmp(func->s,"restart")==0) {
        pop(); restart();
    }else if (strcmp(func->s,"test_ox_eval")==0) {
        test_ox_eval();
    // The following functions are defined in call_gsl.c
    }else if (strcmp(func->s,"gsl_sf_lngamma_complex_e")==0) {
        call_gsl_sf_lngamma_complex_e();
    }else if (strcmp(func->s,"gsl_integration_qags")==0) {
        call_gsl_integration_qags();
    }else if (strcmp(func->s,"gsl_monte_plain_integrate")==0) {
        call_gsl_monte_plain_miser_vegas_integrate(0);
    }else if (strcmp(func->s,"gsl_monte_miser_integrate")==0) {
        call_gsl_monte_plain_miser_vegas_integrate(1);
    }else if (strcmp(func->s,"gsl_monte_vegas_integrate")==0) {
        call_gsl_monte_plain_miser_vegas_integrate(2);
    }else if (strcmp(func->s,"gsl_odeiv_step_rk4")==0) {
        call_gsl_odeiv_step("rk4");
    }else if (strcmp(func->s,"gsl_sf_gamma_inc")==0) {
        call_gsl_sf_gamma_inc();
    }else if (strcmp(func->s,"gsl_eigen_nonsymmv")==0) {
        call_gsl_eigen_nonsymmv();
    }else {
        push(make_error2("sm_executeFunction, unknown function",NULL,0,-1));
        return -1;
    } 
    return(0);
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
    return(0);
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

jmp_buf Ox_env;
int Ox_intr_usr1=0;
void usr1_handler(int sig)
{
  Ox_intr_usr1=1;
  longjmp(Ox_env,1);
}
void restart() {
  Ox_intr_usr1=0;
  longjmp(Ox_env,1);
}

void myhandler(const char *reason,const char *file,int line, int gsl_errno) {
  cmo *m;
  FILE *fp;
  char logname[1024];
  sprintf(logname,"/tmp/ox_gsl-%d.txt",(int) getpid());
  fp = fopen(logname,"w");
  fprintf(fp,"%d\n",gsl_errno);
  fprintf(fp,"%d\n",line);
  if (file != NULL) fprintf(fp,"%s\n",file); else fprintf(fp,"file?\n");
  if (reason != NULL) fprintf(fp,"%s\n",reason); else fprintf(fp,"reason?\n");
  fflush(NULL); fclose(fp); 
  // m = make_error2(reason,file,line,gsl_errno);
  //  send_ox_cmo(fd_rw, m);  ox_flush(fd_rw); 
  // send error packet even it is not asked. Todo, OK? --> no
  restart();
}
void push_error_from_file() {
  FILE *fp;
#define BUF_SIZE 1024
  char logname[BUF_SIZE];
  char cmd[BUF_SIZE+100];
  char file[BUF_SIZE];
  char reason[BUF_SIZE];
  int gsl_errno, line;
  cmo *m;
  fprintf(stderr,"push_error_from_file()\n");
  sprintf(logname,"/tmp/ox_gsl-%d.txt",(int) getpid());
  fp = fopen(logname,"r");
  if (fp == NULL) {
    fprintf(stderr,"open %s is failed\n",logname); return;
  }
  fgets(cmd,BUF_SIZE-2,fp); sscanf(cmd,"%d",&gsl_errno);
  fgets(cmd,BUF_SIZE-2,fp); sscanf(cmd,"%d",&line);
#define remove_newline(s) {char *tmp_pos; if ((tmp_pos=strchr(s,'\n')) != NULL) *tmp_pos = '\0';}
  fgets(file,BUF_SIZE-2,fp);  remove_newline(file);
  fgets(reason,BUF_SIZE-2,fp); remove_newline(reason);
  fclose(fp);
  m = make_error2(reason,file,line,gsl_errno);
  push(m);
  sprintf(cmd,"rm -f %s",logname);
  system(cmd);
}
int main()
{
  if ( setjmp(Ox_env) ) {
    fprintf(stderr,"resetting libgsl ..."); 
    initialize_stack();
    if (Ox_intr_usr1) {
      fprintf(stderr,"and sending OX_SYNC_BALL...");
      send_ox_tag(fd_rw,OX_SYNC_BALL);
    }
    fprintf(stderr,"done\n");
    Ox_intr_usr1=0;
    push_error_from_file();
  }else{
    ox_stderr_init(stderr);
    initialize_stack();
    init_gc();
    fd_rw = oxf_open(3);
    oxf_determine_byteorder_server(fd_rw);
  }
#if defined(__CYGWIN__)
  void *mysignal(int sig,void (*handler)(int m));
  mysignal(SIGUSR1,usr1_handler);
#else
  signal(SIGUSR1,usr1_handler);
#endif
  
  while(1) {
    receive();
  }
  return(0);
}

cmo *element_of_at(cmo *list,int k) {
  int length;
  static cmo * saved_list = NULL;
  static cmo **dic;
  int i;
  cell *cellp;
  if (list == NULL) {
    ox_printf("element_of_at: list is NULL.\n");
    return( (cmo *)NULL);
  }
  if (list->tag != CMO_LIST) {
    ox_printf("element_of_at: list is not list.\n");
    return((cmo *)NULL);
  }
  length = list_length((cmo_list *)list);
  if ((k < 0) || (k >= length)) {
    ox_printf("element_of_at: out of bound length=%d, k=%d.\n",length,k);
    return((cmo *)NULL);
  }
  if (list == saved_list) return(dic[k]);
  saved_list = list;
  dic = (cmo **)GC_malloc(sizeof(cmo *)*(length+1));
  if (dic == NULL) return((cmo *)NULL); // no more memory.
  cellp = list_first((cmo_list *)list);
  for (i=0; i<length; i++) {
    dic[i] = cellp->cmo;
    cellp = list_next(cellp);
  }
  return(dic[k]);  
}

int get_length(cmo *c) {
  if (c->tag != CMO_LIST) {
    return(-1);
  }
  return(list_length((cmo_list *)c));
}
