/* $OpenXM$
*/
// Todo, misc-2017/A3/kanazawa/ox_gsl.h.for_obj
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
void pops(int n);
void show_double_list();
void usr1_handler(int sig);
void show_stack_top();
void initialize_stack();
void push(cmo* m);
void get_xy(int *x, int *y);
void my_add_int32();
void my_add_double();

char *get_string();
cmo *pop();
