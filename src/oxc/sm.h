/* -*- mode: C -*- */
/* $OpenXM: OpenXM/src/oxc/sm.h,v 1.4 2000/11/30 10:27:02 ohara Exp $ */

#ifndef _SM_ERRNO_H_
#define _SM_ERRNO_H_

#include <ox_toolkit.h>

#define ERRNO_SM_ILLEGAL      100
#define ERRNO_SM_LF_NOT_FOUND 101

void push(cmo *ob);
cmo *pop();
void pops(int n);
void push_error(int errcode, cmo* pushback);
void sm_popCMO();
void sm_pops();
void sm_executeFunction();
void sm_mathcap();
void sm_set_mathcap();
void sm_run(int code);
int  sm_receive_ox();
int  sm(OXFILE *oxfp);

int (*sm_search_f(int code))();
int oxc_open(char *cmd, short port);
int lf_oxc_open();

#endif
