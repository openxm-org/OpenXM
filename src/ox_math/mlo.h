#ifndef _MLO_H_

#define _MLO_H_

#include "ox.h"

/* Mathematica でのみ使用される Local Object の定義 */
typedef struct {
    int tag;
    int length;
    cell head[1];
    char *function;
} mlo_function;

typedef cmo mlo;
typedef cmo_string mlo_string;
typedef cmo_zz mlo_zz;

mlo_function *new_mlo_function(char *function);

cmo *receive_mlo();
mlo *receive_mlo_zz();
mlo *receive_mlo_string();
cmo *receive_mlo_function();
cmo *convert_mlo_to_cmo(mlo *m);
cmo *receive_mlo_function_newer();
cmo *receive_mlo_symbol();

int send_mlo(cmo *m);
int send_mlo_int32(cmo *m);
int send_mlo_string(cmo *m);
int send_mlo_zz(cmo *m);
int send_mlo_list(cmo *c);

int ml_init();
int ml_exit();
int ml_flush();
int ml_select();
int ml_evaluateStringByLocalParser(char *str);
int ml_executeFunction(char *function, int argc, cmo *argv[]);

#endif
