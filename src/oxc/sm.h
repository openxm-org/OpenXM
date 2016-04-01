/* -*- mode: C -*- */
/* $OpenXM: OpenXM/src/oxc/sm.h,v 1.5 2000/12/03 14:32:40 ohara Exp $ */

#ifndef _SM_ERRNO_H_
#define _SM_ERRNO_H_

#include <ox_toolkit.h>

#define ERRNO_SM_ILLEGAL      100
#define ERRNO_SM_LF_NOT_FOUND 101

void push(cmo *ob);
cmo *pop();
void pops(int n);
void push_error(int errcode, cmo* pushback);
int  sm_popCMO();
int  sm_pops();
int  sm_executeFunction();
int  sm_mathcap();
int  sm_set_mathcap();
void sm_run(int code);
int  sm_receive_ox();
int  sm(OXFILE *oxfp);

int (*sm_search_f(int code))();
int oxc_open(char *cmd, short port);
int lf_oxc_open_main(char *cmd, short port);
int lf_oxc_open();

#endif
