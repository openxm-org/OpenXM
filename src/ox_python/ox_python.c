/* $OpenXM: OpenXM/src/ox_python/ox_python.c,v 1.6 2022/01/02 00:23:25 takayama Exp $
*/

#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <math.h>
#include "ox_python.h"

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

#define OX_PYTHON_VERSION 2018090701
#define ID_STRING  "2018/09/07 17:47:00"

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
    SM_executeStringByLocalParser,
    SM_executeFunction,
    SM_setMathCap,
    SM_shutdown,
    SM_control_kill,
    SM_control_reset_connection,
    SM_control_spawn_server,
    SM_control_terminate_server,
    0};
    mathcap_init(OX_PYTHON_VERSION, ID_STRING, "ox_python", available_cmo,available_sm_command);
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
    char s_tmp[256]; 
    s = s_tmp;
    n = 5;
    argv = (cmo **) GC_malloc(sizeof(cmo *)*n);
    ms = (cmo *)new_cmo_string("error"); argv[0] = ms; 
    if (reason != NULL) {s = (char *)GC_malloc(strlen(reason)+3);
      strcpy(s,"\"");strcat(s,reason);strcat(s,"\"");
    }else strcpy(s,"0");
    ms = (cmo *) new_cmo_string(s); argv[1] = ms;
    if (fname != NULL) {s = (char *)GC_malloc(strlen(fname)+3);
      strcpy(s,"\"");strcat(s,fname);strcat(s,"\"");
    }else strcpy(s,"0");
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


int sm_executeFunction()
{
    cmo_string *func = (cmo_string *)pop();
    if (func->tag != CMO_STRING) {
        push(make_error2("sm_executeFunction, not CMO_STRING",NULL,0,-1));
        return -1;
    }
    // Test functions
    if (strcmp(func->s, "add_int32") == 0) {
        my_add_int32();
    }else if (strcmp(func->s,"add_double")==0) {
        my_add_double();
    }else if (strcmp(func->s,"show_double_list")==0) {
        show_double_list();
    }else if (strcmp(func->s,"restart")==0) {
        pop(); restart();
    }else if (strcmp(func->s,"PyRun_String")==0) {
        my_PyRun_String();
    }else if (strcmp(func->s,"eval")==0) {
        my_eval2();
    }else {
        push(make_error2("sm_executeFunction, unknown function",NULL,0,-1));
        return -1;
    } 
    return(0);
}

int sm_executeStringByLocalParser()
{
    int status;
    cmo_string *cmd = (cmo_string *)pop();
    if (cmd->tag != CMO_STRING) {
        push(make_error2("sm_executeStringByLocalParser, not CMO_STRING",NULL,0,-1));
        return -1;
    }
/***    {FILE *fp; fp=fopen("tmp-debug.txt","a"); fprintf(fp,"strlen=%d\n%s\n",strlen(cmd->s),cmd->s);fclose(fp); } ***/
    status=PyRun_SimpleString(cmd->s);
//     push(make_error2("sm_executeStringByLocalParser",NULL,0,-1));
    push((cmo *)new_cmo_int32(status));
    return(0);
/* Todo, set the flag by Py_InspectFlag to avoid exit after exception.
   See PyRun_SimpleStringFlags, https://docs.python.jp/2.7/c-api/veryhigh.html
*/
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
    case SM_executeStringByLocalParser:
      sm_executeStringByLocalParser();
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
  sprintf(logname,"/tmp/ox_python-%d.txt",(int) getpid());
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
  char cmd[BUF_SIZE+256];
  char file[BUF_SIZE];
  char reason[BUF_SIZE];
  int gsl_errno, line;
  cmo *m;
  fprintf(stderr,"push_error_from_file()\n");
  sprintf(logname,"/tmp/ox_python-%d.txt",(int) getpid());
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
int main(int argc,char *argv[])
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

    /* Initialize python */
    Py_SetProgramName((wchar_t *) argv[0]);  /* optional but recommended */
    Py_Initialize();

  
  while(1) {
    receive();
  }
  Py_Finalize();
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

int my_PyRun_String() {
  static PyObject *py_main=NULL;
  static PyObject *py_dict=NULL;
  PyObject *pyRes;
  char *cmd;
  pop();   // pop argc
  cmd = get_string();
  if (cmd == NULL) {
    push(make_error2("my_PyRun_Sring: argument is not a string",NULL,0,-1));
    return(-1);
  }
  printf("cmd=%s\n",cmd);
  if (py_main == NULL) py_main = PyImport_AddModule("__main__");
  if (py_dict == NULL) py_dict = PyModule_GetDict(py_main);
//  pyRes = PyRun_String(cmd,Py_single_input,py_dict,py_dict);
  pyRes = PyRun_String(cmd,Py_eval_input,py_dict,py_dict);
  if (pyRes==NULL) {
    push(make_error2("PyRun_String: exception",NULL,0,-1));
    PyRun_SimpleString("\n");
  /* https://stackoverflow.com/questions/12518435/pyrun-string-stop-sending-result-to-stdout-after-any-error 
  */
    return(-1);
  }
  return push_python_result(pyRes);
}

int push_python_result(PyObject *pyRes) {
  if (PyUnicode_Check(pyRes)) {
    push((cmo *)new_cmo_string(PyBytes_AsString(PyUnicode_AsEncodedString(pyRes,"UTF-8","strict"))));
    return(0);
  }else if (PyLong_Check(pyRes)) {
    push((cmo *)new_cmo_int32((int) PyLong_AsLong(pyRes)));
    return(0);
  }else {
    push((cmo *)new_cmo_string(PyBytes_AsString(PyUnicode_AsEncodedString(PyObject_Str(pyRes),"UTF-8","strict"))));
    return(0);
    /* push((cmo *)new_cmo_string(PyBytes_AsString(PyObject_Str(pyRes))));
    return(0); */
//    push(make_error2("PyRun_String returns an object which as not been implemented.",NULL,0,-1));
//    return(-1);
  }
}


int my_eval() {
  static PyObject *pName=NULL;
  static PyObject *pModule=NULL;
  static PyObject *pDict=NULL;
  static PyObject *pFunc=NULL;
  PyObject *pArgs, *pValue;
  char *cmd;
  int i;
  pop();   // pop argc
  cmd = get_string();
  if (cmd == NULL) {
    push(make_error2("my_eval: argument is not a string",NULL,0,-1));
    return(-1);
  }
  printf("my_eval cmd=%s\n",cmd);

  // code from https://docs.python.jp/2.7/extending/embedding.html
  if (pName==NULL) pName = PyBytes_FromString("__builtin__"); 
  if (pModule==NULL) pModule = PyImport_Import(pName);

  if (pModule != NULL) {
    if (pFunc==NULL) pFunc = PyObject_GetAttrString(pModule, "eval");
    if (pFunc && PyCallable_Check(pFunc)) {
      pArgs = PyTuple_New(3);
      PyTuple_SetItem(pArgs,0,PyBytes_FromString(cmd));
      PyTuple_SetItem(pArgs,1,PyEval_GetGlobals());
      PyTuple_SetItem(pArgs,2,PyEval_GetLocals());
      pValue = PyObject_CallObject(pFunc, pArgs);
      Py_DECREF(pArgs);
      if (pValue != NULL) {
        push_python_result(pValue);
        //              Py_DECREF(pValue);
        return(0);
      }
      else {
        PyErr_Print();
        push(make_error2("Fail to call PyObjedct_CallObject(eval,...)",NULL,0,-1));
        return(-1);
      }
    }
    else {
      if (PyErr_Occurred())
        PyErr_Print();
      fprintf(stderr, "Cannot find function eval\n");
    }
    return(-1);
  }
  else {
    PyErr_Print();
    fprintf(stderr, "Failed to load __builtin__\n");
    return -1;
  }
}

int my_eval2() {
  static PyObject *pName=NULL;
  static PyObject *pModule=NULL;
  static PyObject *pDict=NULL;
  static PyObject *pFunc=NULL;
  PyObject *pArgs, *pValue;
  char *cmd;
  int i;
  char *cmd2;
  pop();   // pop argc
  cmd = get_string();
  if (cmd == NULL) {
    push(make_error2("my_eval2: argument is not a string",NULL,0,-1));
    return(-1);
  }
  printf("my_eval2 cmd=%s\n",cmd);

  // code from https://stackoverflow.com/questions/48432577/extracting-value-from-python-after-its-embedded-in-c
  cmd2=(char *)GC_malloc(strlen(cmd)+256);
  sprintf(cmd2,"f = lambda x: eval(x)");
  PyRun_SimpleString(cmd2);
  pModule = PyImport_ImportModule("__main__");
  pFunc= PyObject_GetAttrString(pModule,"f");

  if (pModule != NULL) {
    if (pFunc && PyCallable_Check(pFunc)) {
      pArgs = PyTuple_New(1);
      PyTuple_SetItem(pArgs,0,PyBytes_FromString(cmd));
      pValue = PyObject_CallObject(pFunc, pArgs);
      Py_DECREF(pArgs);
      if (pValue != NULL) {
        push_python_result(pValue);
        //              Py_DECREF(pValue);
        return(0);
      }
      else {
        PyErr_Print();
        push(make_error2("Fail to call PyObjedct_CallObject(eval,...)",NULL,0,-1));
        return(-1);
      }
    }else {
      if (PyErr_Occurred())
        PyErr_Print();
      fprintf(stderr, "Cannot find function f\n");
    }
    return(-1);
  }
  else {
    PyErr_Print();
    fprintf(stderr, "Failed to load __builtin__\n");
    return -1;
  }
}
