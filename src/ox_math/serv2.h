/* -*- mode: C; coding: euc-japan -*- */
/* $OpenXM: OpenXM/src/ox_math/serv2.h,v 1.8 2000/10/10 19:58:30 ohara Exp $ */

#ifndef _SERV2_H_
#define _SERV2_H_

#include <ox_toolkit.h>

#define FLAG_MLTKSYM_IS_INDETERMINATE   0
#define FLAG_MLTKSYM_IS_STRING          1

#define ERROR_ID_UNKNOWN_SM 10
#define ERROR_ID_FAILURE_MLINK         11

int  push(cmo *m);
cmo  *pop();
int  sm_popCMO(OXFILE *oxfp);
int  sm_popString(OXFILE *oxfp);
int  sm_pops(OXFILE *oxfp);
int  sm_executeStringByLocalParser();
int  sm_executeFunction(OXFILE *oxfp);

int  receive_sm_command(OXFILE *oxfp);
int  execute_sm_command(OXFILE *oxfp, int code);

int shutdown();
#endif
