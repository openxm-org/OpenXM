/* $OpenXM: OpenXM/src/ox_math/mlo.h,v 1.8 2003/03/23 21:56:11 ohara Exp $ */
#ifndef _MLO_H_

#define _MLO_H_

#include <ox_toolkit.h>

#define RESERVE_INTERRUPTION 1
#define INTERRUPTED          2
#define RESERVE_ABORTION     4
#define ABORTED              8

/* definitions of Mathlink Local Objects. */
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
mlo *receive_mlo_error();
mlo *receive_mlo_real();

int send_mlo(cmo *m);

int ml_abort();
int ml_interrupt();
int ml_next_packet();
int ml_new_packet();
int ml_read_packet();
int ml_init();
int ml_exit();
int ml_flush();
int ml_select();
int ml_evaluateStringByLocalParser(char *str);
int ml_executeFunction(char *function, int argc, cmo *argv[]);
mlo *ml_return();

/* state management for the OpenXM robust interruption */
unsigned ml_state_set(unsigned fl);
unsigned ml_state_clear(unsigned fl);
unsigned ml_state(unsigned fl);
void     ml_state_clear_all();
#endif
