/* -*- mode: C; coding: euc-japan -*- */
/* $OpenXM: OpenXM/src/ox_math/serv2.h,v 1.5 1999/11/29 12:09:58 ohara Exp $ */

#ifndef _SERV2_H_
#define _SERV2_H_

#include "ox.h"

#define FLAG_MLTKSYM_IS_INDETERMINATE   0
#define FLAG_MLTKSYM_IS_STRING          1

#define ERROR_ID_UNKNOWN_SM 10
#define ERROR_ID_FAILURE_MLINK         11

int  initialize_stack();
int  push(cmo *m);
cmo  *pop();
int  sm_popCMO(int fd);
int  sm_popString(int fd);
int  sm_pops(int fd);
int  sm_executeStringByLocalParser();
int  sm_executeFunction(int fd);

int  receive_sm_command(int fd);
int  execute_sm_command(int fd, int code);

int shutdown();
#endif
