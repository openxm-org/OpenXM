/* -*- mode: C -*- */
/* $OpenXM: OpenXM/src/oxc/sm.h,v 1.2 2000/11/28 04:52:05 ohara Exp $ */

#ifndef _SM_ERRNO_H_
#define _SM_ERRNO_H_

#include <ox_toolkit.h>

#define ERRNO_SM_ILLEGAL      100
#define ERRNO_SM_LF_NOT_FOUND 101

void extend_stack();
void push(cmo *ob);
cmo *pop();
void pops(int n);
void push_error(int errcode, cmo* pushback);
void sm_popCMO(OXFILE *oxfp);
void sm_pops(OXFILE *oxfp);
void sm_executeFunction(OXFILE *oxfp);
void sm_mathcap(OXFILE *oxfp);
void sm_set_mathcap(OXFILE *oxfp);
void sm_control_kill(OXFILE *oxfp);
void sm_control_kill_pid(OXFILE *oxfp);
void sm_control_reset_pid(OXFILE *oxfp);
void sm_run(OXFILE *oxfp, int code);
int sm(OXFILE *oxfp);

void pid_extend();
int  pid_lookup(pid_t pid);
int  pid_registed(pid_t pid);
void pid_regist(pid_t pid);
void pid_delete(pid_t pid);
int  pid_reset(pid_t pid);
int  pid_kill(pid_t pid);
void pid_kill_all();

int receive_sm_command(OXFILE *oxfp);
int receive_ox(OXFILE *oxfp);

typedef struct {
    int (*func_ptr)();
    void *key;
} db;

int (*sm_search_f(int code))();
int oxc_open(char *cmd, short port);
int lf_oxc_open();

#endif
