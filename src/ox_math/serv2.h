/* -*- mode: C; coding: euc-japan -*- */
/* $OpenXM: OpenXM/src/ox_math/serv2.h,v 1.2 1999/11/02 06:11:58 ohara Exp $ */

#ifndef _SERV2_H_
#define _SERV2_H_

#include "ox.h"

int  MATH_evaluateStringByLocalParser(char *str);
int  MATH_init();
int  MATH_exit();
cmo  *MATH_get_object();
int  MATH_executeFunction(char *function, int argc, cmo *argv[]);

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
#endif
