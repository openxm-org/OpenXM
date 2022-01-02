/* $OpenXM: OpenXM/src/ox_python/ox_python.h,v 1.2 2018/09/08 03:05:19 takayama Exp $
*/
// Todo, misc-2017/A3/kanazawa/ox_gsl.h.for_obj
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <Python.h>
#include "gmp.h"
#include "gmp-impl.h"
#include "ox_toolkit.h"

int sm_mathcap();
int sm_popCMO();
int get_i();
int sm_executeFunction();
int receive_and_execute_sm_command();
int receive();
int main();

double get_double();
double *get_double_list(int *length);
// They will be replaced by the following functions.
double cmo2double(cmo *c);
double *cmo2double_list(int *length,cmo *c);

void init_gc();
void *gc_realloc(void *p,size_t osize,size_t nsize);
void gc_free(void *p,size_t size);
void pops(int n);
void show_double_list();
void usr1_handler(int sig);
void show_stack_top();
void initialize_stack();
void push(cmo* m);
void get_xy(int *x, int *y);
void my_add_int32();
void my_add_double();
void restart();
void push_error_from_file();
void myhandler(const char *reason,const char *file,int line, int gsl_errno);

char *get_string();

cmo *pop();
cmo *make_error2(const char *reason,const char *fname,int line,int code);

int myfopen_w(char *fname);
int myfputs(int fd,const char *s);
int myfclose(int fd);

void test_ox_eval();
int eval_cmo(cmo *c,double *retval);
int register_entry(char *s,double v);
void init_dic();

cmo *element_of_at(cmo *list,int k);
int get_length(cmo *c);

int sm_executeStringByLocalParser();
int my_PyRun_String();
int push_python_result(PyObject *pyRes);
int my_eval();
int my_eval2();



