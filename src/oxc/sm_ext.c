/* -*- mode: C; coding: euc-japan -*- */
/* $OpenXM: OpenXM/src/oxc/sm_ext.c,v 1.7 2000/12/01 01:53:34 ohara Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <ox_toolkit.h>
#include "sm.h"

static int  sm_control_spawn();
static void sm_control_terminate();
static void sm_control_kill();
static void sm_control_reset_pid();

static void pid_extend();
static int  pid_lookup(pid_t pid);
static int  pid_registed(pid_t pid);
static void pid_regist(pid_t pid);
static void pid_delete(pid_t pid);
static int  pid_reset(pid_t pid);
static int  pid_kill(pid_t pid);
static void pid_kill_all();

/* ultra loose data base. */
static struct { int (*func_ptr)(); char *key; } tbl_lfunc[] = {
    {lf_oxc_open, "spawn"}, 
    {lf_oxc_open, "oxc_open"}, 
    {NULL, NULL}
};

static struct { int (*func_ptr)(); int key; } tbl_smcmd[] = {
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

int (*sm_search_f(int code))()
{
    int i;
    for (i=0; tbl_smcmd[i].key != NULL; i++) {
        if (code == tbl_smcmd[i].key) {
            return tbl_smcmd[i].func_ptr;
        }
    }
    return NULL;
}

static int (*lookup_localfunction(char *name))()
{
    int i;
    for (i=0; tbl_lfunc[i].key != NULL; i++) {
        if (strcmp(name, tbl_lfunc[i].key) == 0) {
            return tbl_lfunc[i].func_ptr;
        }
    }
    return NULL;
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

static int pid_lookup(pid_t pid)
{
    int i;
    for(i=0; i<pid_ptr; i++) {
        if (pids[i] == pid) {
            return i;
        }
    }
    return -1;
}

static int pid_registed(pid_t pid)
{
    return pid_lookup(pid)+1;
}

static void pid_regist(pid_t pid)
{
    if (pid_ptr >= pid_size) {
        pids_extend();
    }
    pids[pid_ptr++] = pid;
}

static void pid_delete(pid_t pid)
{
    int i = pid_lookup(pid);
    if (i >= 0 && i != --pid_ptr) {
        pids[i] = pids[pid_ptr];
    }
}

static int pid_reset(pid_t pid)
{
    if (pid_registed(pid)) {
        kill(pid, SIGUSR1);
        return 1;
    }
    return 0;
}

static int pid_kill(pid_t pid)
{
    if (pid_registed(pid)) {
        kill(pid, SIGKILL);
        pid_delete(pid);
        return 1;
    }
    return 0;
}

/* Killing all child processes */
static void pid_kill_all()
{
    while(pid_ptr > 0) {
        kill(pids[--pid_ptr], SIGKILL);
    }
}

cmo_error2 *type_checker(cmo *ob, int type)
{
/*  cmo_error2 *err_ob; */
    if (ob->tag != type) {
        /* push and return an error object */
    }
    return NULL;
}

static int sm_control_spawn_main(int argc, cmo *argv[])
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

static int sm_control_spawn()
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

static void sm_control_kill()
{
    pid_kill_all();
}

static void sm_control_terminate()
{
    cmo_int32 *m = (cmo_int32 *)pop();
    pid_t pid = m->i;
    if (m->tag != CMO_INT32 || !pid_kill(pid)) {
        push_error(-1, m);
    }
}

static void sm_control_reset_pid()
{
    cmo_int32 *m = (cmo_int32 *)pop();
    pid_t pid = m->i;
    if (m->tag != CMO_INT32 || !pid_reset(pid)) {
        push_error(-1, m);
        return;
    }
    /* ... */
}
