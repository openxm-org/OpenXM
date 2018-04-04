/* $OpenXM: OpenXM/src/ox_gsl/ox_gsl.h,v 1.3 2018/03/30 08:48:23 takayama Exp $
*/
// Todo, misc-2017/A3/kanazawa/ox_gsl.h.for_obj
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
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
cmo_tree *get_tree();
void print_tree(cmo_tree *c);
