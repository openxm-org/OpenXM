/* -*- mode: C; coding: euc-japan -*- */
/* $OpenXM: OpenXM/src/ox_math/serv2.h,v 1.4 1999/11/18 21:56:44 ohara Exp $ */

#ifndef _SERV2_H_
#define _SERV2_H_

#include "ox.h"

#define FLAG_MLTKSYM_IS_INDETERMINATE   0
#define FLAG_MLTKSYM_IS_STRING          1

#define ERROR_ID_UNKNOWN_SM 10
#define ERROR_ID_FAILURE_MLINK         11

int  ml_evaluateStringByLocalParser(char *str);
int  ml_init();
int  ml_exit();
cmo  *ml_get_object();
int  ml_executeFunction(char *function, int argc, cmo *argv[]);

int  initialize_stack();
int  push(cmo *m);
cmo  *pop();
int  sm_popCMO(int fd_write);
int  sm_popString_old(int fd_write);
int  sm_popString(int fd_write);
int  sm_pops(int fd_write);
int  sm_executeStringByLocalParser();
int  sm_executeFunction(int fd_write);

int  receive_sm_command(int fd_read);
int  execute_sm_command(int fd_write, int code);

int shutdown();
#endif
