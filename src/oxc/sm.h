/* -*- mode: C -*- */
/* $OpenXM: OpenXM/src/oxc/sm.h,v 1.3 2000/11/28 18:11:42 ohara Exp $ */

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
int  sm_control_spawn();
void sm_control_terminate();
void sm_control_kill();
void sm_control_reset_pid();
void sm_run(int code);
int  sm_receive_ox();
int  sm(OXFILE *oxfp);

void pid_extend();
int  pid_lookup(pid_t pid);
int  pid_registed(pid_t pid);
void pid_regist(pid_t pid);
void pid_delete(pid_t pid);
int  pid_reset(pid_t pid);
int  pid_kill(pid_t pid);
void pid_kill_all();

typedef struct {
    int (*func_ptr)();
    void *key;
} db;

int (*sm_search_f(int code))();
int oxc_open(char *cmd, short port);
int lf_oxc_open();

#endif
