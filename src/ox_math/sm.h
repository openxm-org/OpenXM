/* -*- mode: C; coding: euc-japan -*- */
/* $OpenXM: OpenXM/src/ox_math/sm.h,v 1.2 2000/12/03 22:09:34 ohara Exp $ */

#ifndef _SERV2_H_
#define _SERV2_H_

#include <ox_toolkit.h>

#define FLAG_MLTKSYM_IS_INDETERMINATE   0
#define FLAG_MLTKSYM_IS_STRING          1

#define ERROR_ID_UNKNOWN_SM 10
#define ERROR_ID_FAILURE_MLINK         11

/* sm.c */
void sm_sigmask();
void sm_sigunmask();
void sm_siginit();

void push_error(int errcode, cmo* pushback);
int  sm_receive_ox();
int  oxf_error(OXFILE *oxfp);

void push(cmo *m);
cmo  *pop();
void pops(int n);
void sm_popCMO();
void sm_popString();
void sm_pops();
void sm_executeStringByLocalParser();
void sm_executeFunction();
void sm_run(int code);
int  shutdown();

/* sm_ext.c */
int (*sm_search_f(int code))();
void sm_popString();
int local_execute(char *s);
void sm_executeStringByLocalParser();
void sm_executeFunction();
void sm_mathcap();
void sm_set_mathcap();

#define  receive_sm_command(x)   receive_int32((x))

#endif
