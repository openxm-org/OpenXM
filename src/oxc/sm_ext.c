/* -*- mode: C; coding: euc-japan -*- */
/* $OpenXM: OpenXM/src/oxc/sm_ext.c,v 1.6 2000/11/30 10:27:02 ohara Exp $ */

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
    {lf_oxc_open, "spawn"}, 
    {NULL, NULL}
};

static db db_sm[] = {
    {sm_control_spawn,     SM_control_spawn_server},
    {sm_control_terminate, SM_control_terminate_server},
    {sm_executeFunction, SM_executeFunction}, 
    {sm_mathcap,         SM_mathcap},
    {sm_set_mathcap,     SM_setMathCap},
    {sm_popCMO,          SM_popCMO},
    {sm_pops,            SM_pops},
	{sm_control_reset_pid, SM_control_reset_connection_server},
	{sm_control_kill, SM_control_kill},
    {NULL, NULL}
};

extern OXFILE *stack_oxfp;

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
void sm_executeFunction()
{
    int (*func)();
    int retcode = 0;
    cmo *ob = pop();
    if (ob->tag == CMO_STRING) {
        func = lookup_localfunction(((cmo_string *)ob)->s);
        if (func != NULL) {
            if ((retcode = func()) > 0) {
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

/* Process Table */
static pid_t *pids = NULL;
static int pid_ptr = 0;
static int pid_size = 0;

static void pids_extend()
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

cmo_error2 *type_checker(cmo *ob, int type)
{
/*	cmo_error2 *err_ob; */
	if (ob->tag != type) {
		/* push and return an error object */
	}
	return NULL;
}

int sm_control_spawn_main(int argc, cmo *argv[])
{
    char *cmd = ((cmo_string *)argv[0])->s;
    int  port = ((cmo_int32 *)argv[1])->i;
    pid_t pid = lf_oxc_open_main(cmd, port);
    if (pid > 0) {
        push(new_cmo_int32(pid));
        pid_regist(pid);
    }
    return pid;
}

int lf_oxc_open()
{
    cmo **argv;
	if (getargs(&argv) != 2 || 
		type_checker(argv[0], CMO_STRING) != NULL 
		|| type_checker(argv[0], CMO_INT32) != NULL) {
        fprintf(stderr, "oxc: invalid arguments\n");
        return -1;
    }
	return sm_control_spawn_main(2, argv);
}

int sm_control_spawn()
{
    cmo *argv[2];
	argv[0] = pop();
	argv[1] = pop();

	if (type_checker(argv[0], CMO_STRING) != NULL 
		|| type_checker(argv[0], CMO_INT32) != NULL) {
        fprintf(stderr, "oxc: invalid arguments\n");
        return -1;
    }
	return sm_control_spawn_main(2, argv);
}

void sm_mathcap()
{
    push((cmo *)oxf_cmo_mathcap(stack_oxfp));
}

void sm_set_mathcap()
{
    cmo_mathcap *m = (cmo_mathcap *)pop();
    if (m->tag == CMO_MATHCAP) {
        oxf_mathcap_update(stack_oxfp, m);
    }else {
        push_error(-1, m);
        /* an error object must be pushed */
    }
}

void sm_control_kill()
{
	pid_kill_all();
}

void sm_control_terminate()
{
    cmo_int32 *m = (cmo_int32 *)pop();
    pid_t pid = m->i;
    if (m->tag != CMO_INT32 || !pid_kill(pid)) {
        push_error(-1, m);
    }
}

void sm_control_reset_pid()
{
    cmo_int32 *m = (cmo_int32 *)pop();
    pid_t pid = m->i;
    if (m->tag != CMO_INT32 || !pid_reset(pid)) {
        push_error(-1, m);
		return;
    }
	/* ... */
}

static int intcmp(int key1, int key2)
{
    return key1 != key2;
}

int (*sm_search_f(int code))()
{
    return db_search(code, db_sm, intcmp);
}
