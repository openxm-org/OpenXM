/* -*- mode: C; coding: euc-japan -*- */
/* $OpenXM: OpenXM/src/oxc/sm_ext.c,v 1.4 2000/11/28 04:52:05 ohara Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <ox_toolkit.h>
#include "sm.h"

/* ultra loose data base. */
static db db_localfunc[] = {
    {lf_oxc_open, "oxc_open"}, 
    {NULL, NULL}
};

static db db_sm[] = {
    {sm_executeFunction, SM_executeFunction}, 
    {sm_mathcap,         SM_mathcap},
    {sm_set_mathcap,     SM_setMathCap},
    {sm_popCMO,          SM_popCMO},
    {sm_pops,            SM_pops},
	{sm_control_reset_pid, SM_control_reset_connection_pid},
	{sm_control_kill_pid, SM_control_kill_pid},
	{sm_control_kill, SM_control_kill},
    {NULL, NULL}
};

__inline__
static int (*db_search(void *key, db *dbs, int (*cmp)(void *, void *)))()
{
    while (dbs->key != NULL) {
        if (cmp(key, dbs->key) == 0) {
            return dbs->func_ptr;
        }
        dbs++;
    }
    return NULL;
}

static int (*lookup_localfunction(char *name))()
{
    return db_search(name, db_localfunc, strcmp);
}

/* 
Normally local functions push a return value to the stack.
but, if error occurs, then these return non-positive numbers and
the sm_executeFunction push an error object.
*/
void sm_executeFunction(OXFILE *oxfp)
{
    int (*func)(OXFILE *);
    int retcode = 0;
    cmo *ob = pop();
    if (ob->tag == CMO_STRING) {
        func = lookup_localfunction(((cmo_string *)ob)->s);
        if (func != NULL) {
            if ((retcode = func(oxfp)) > 0) {
                return;
            }
        }
    }
    push_error(retcode, ob);
}

/* getargs() set number of popped objects to argc. */
static int getargs(cmo ***args)
{
    cmo **argv;
    int i;
    int argc = -1;
    cmo_int32 *m = (cmo_int32 *)pop();

    if (m->tag != CMO_INT32 || (argc = m->i) < 0) {
        fprintf(stderr, "oxc: invalid arguments\n");
    }else {
        argv = (cmo **)malloc(sizeof(cmo *)*argc);
        for(i=0; i<argc; i++) {
            argv[i] = pop();
        }
        *args = argv;
    }
    return argc;
}

#define MAX_PROCESS 1024

static pid_t *pids = NULL;
static int pid_ptr = 0;
static int pid_size = 0;

void pids_extend()
{
    int size2 = pid_size + MAX_PROCESS;
    pid_t *pids2 = (pid_t *)malloc(sizeof(pid_t)*size2);
    if (pids != NULL) {
        memcpy(pids2, pids, sizeof(pid_t)*pid_size);
        free(pids);
    }
    pid_size = size2;
    pids = pids2;
}

int pid_lookup(pid_t pid)
{
    int i;
    for(i=0; i<pid_ptr; i++) {
        if (pids[i] == pid) {
            return i;
        }
    }
    return -1;
}

int pid_registed(pid_t pid)
{
    return pid_lookup(pid)+1;
}

void pid_regist(pid_t pid)
{
    if (pid_ptr >= pid_size) {
		pids_extend();
    }
	pids[pid_ptr++] = pid;
}

void pid_delete(pid_t pid)
{
    int i = pid_lookup(pid);
    if (i >= 0 && i != --pid_ptr) {
        pids[i] = pids[pid_ptr];
    }
}

int pid_reset(pid_t pid)
{
	if (pid_registed(pid)) {
		kill(pid, SIGUSR1);
		return 1;
	}
	return 0;
}

int pid_kill(pid_t pid)
{
	if (pid_registed(pid)) {
		kill(pid, SIGKILL);
		pid_delete(pid);
		return 1;
	}
	return 0;
}

/* Killing all child processes */
void pid_kill_all()
{
	while(pid_ptr > 0) {
        kill(pids[--pid_ptr], SIGKILL);
	}
}

int lf_oxc_open()
{
    cmo **argv;
    char *cmd;
    int port;
    pid_t pid;

    if (getargs(&argv) != 2 || argv[0]->tag != CMO_STRING
        || argv[1]->tag != CMO_INT32) {
        fprintf(stderr, "oxc: invalid arguments\n");
        return -1;
    }

    cmd  = ((cmo_string *)argv[0])->s;
    port = ((cmo_int32 *)argv[1])->i;
    pid = lf_oxc_open_main(cmd, port);
    if (pid > 0) {
        push(new_cmo_int32(pid));
        pid_regist(pid);
    }
    return pid;
}

void sm_mathcap(OXFILE *oxfp)
{
    push((cmo *)oxf_cmo_mathcap(oxfp));
}

void sm_set_mathcap(OXFILE *oxfp)
{
    cmo_mathcap *m = (cmo_mathcap *)pop();
    if (m->tag == CMO_MATHCAP) {
        oxf_mathcap_update(oxfp, m);
    }else {
        push_error(-1, m);
        /* an error object must be pushed */
    }
}

void sm_control_kill(OXFILE *oxfp)
{
	pid_kill_all();
}

void sm_control_kill_pid(OXFILE *oxfp)
{
    cmo_int32 *m = (cmo_int32 *)pop();
    pid_t pid = m->i;
    if (m->tag != CMO_INT32 || !pid_kill(pid)) {
        push_error(-1, m);
    }
}

void sm_control_reset_pid(OXFILE *oxfp)
{
    cmo_int32 *m = (cmo_int32 *)pop();
    pid_t pid = m->i;
    if (m->tag != CMO_INT32 || !pid_reset(pid)) {
        push_error(-1, m);
		return;
    }
	/* ... */
}

void sm_control_spawn_server(OXFILE *oxfp);
void sm_control_terminate_server(OXFILE *oxfp);

static int intcmp(int key1, int key2)
{
    return key1 != key2;
}


int (*sm_search_f(int code))()
{
    return db_search(code, db_sm, intcmp);
}
