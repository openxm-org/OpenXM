/* -*- mode: C; coding: euc-japan -*- */
/* $OpenXM$ */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
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
    {sm_popCMO,          SM_popCMO},
    {sm_pops,            SM_pops},
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
    cmo_int32 *m = pop();

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

static int pids[1024] = {0};
static int pid_ptr = 0;

int regist_pid(int pid)
{
	if (pid_ptr < 1024) {
		pids[pid_ptr++] = pid;
		return pid;
	}
	return 0;
}

int lf_oxc_open()
{
    cmo **argv;
    char *cmd;
    int port;
    int pid;

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
		regist_pid(pid);
	}
	return pid;
}

void sm_mathcap(OXFILE *oxfp)
{
	cmo_mathcap *m = pop();
	if (m->tag == CMO_MATHCAP) {
		mathcap_update(m);
	}else {
		push_error(-1, m);
		/* an error object must be pushed */
	}
}

static int intcmp(int key1, int key2)
{
    return key1 != key2;
}


int (*sm_search_f(int code))()
{
    return db_search(code, db_sm, intcmp);
}
