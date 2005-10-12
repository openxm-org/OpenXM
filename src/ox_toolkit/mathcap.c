/* -*- mode: C; coding: euc-japan -*- */
/* $OpenXM: OpenXM/src/ox_toolkit/mathcap.c,v 1.12 2005/07/26 12:52:05 ohara Exp $ */

/* This module includes functions for handling mathcap databases. */
/* This version will be in the new-mathcap-branch. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ox_toolkit.h"

#define MATHCAP_1ST_FORMAT "(CMO_LIST (CMO_INT32 %d) (CMO_STRING \"%s\") (CMO_STRING \"%s\") (CMO_STRING \"%s\"))"

static int default_cmd[] = { 
    SM_popCMO,
    SM_popString,
    SM_mathcap,
    SM_pops,
    SM_executeStringByLocalParser,
    SM_executeFunction,
    SM_control_kill,
    SM_control_reset_connection,
    0 };
static int default_cmo[] = { 
    CMO_NULL,
    CMO_INT32,
    CMO_STRING,
    CMO_MATHCAP,
    CMO_LIST,
    CMO_MONOMIAL32,
    CMO_ZZ,
    CMO_QQ,
    CMO_ZERO,
    CMO_DMS_GENERIC,
    CMO_RING_BY_NAME,
    CMO_RECURSIVE_POLYNOMIAL,
    CMO_DISTRIBUTED_POLYNOMIAL,
    CMO_POLYNOMIAL_IN_ONE_VARIABLE,
    CMO_64BIT_MACHINE_DOUBLE, 
    CMO_IEEE_DOUBLE_FLOAT,
    CMO_INDETERMINATE,
    CMO_TREE,
    CMO_LAMBDA,
    CMO_ERROR2,
    0 };
static int default_oxtag[] = { OX_DATA, 0 };

static struct {
    int  ox_version;
    char *sysname;
    char *version;
    char *hosttype;
} sysinfo = {OX_PROTOCOL_VERSION, "ox_toolkit", OX_TOOLKIT_VERSION, "generic"};

mathcap default_mathcap = {default_cmd, default_cmo};

static int ilen(int a[]);
static int *icopy(int s[]);
static int *icopyn(int s[], int n);
static int cmo_int32_to_int(cmo_int32* m);
static int *imerge(int *base, int *diff);
static mathcap *mathcap_merge_io(mathcap *this, mathcap *diff);

mathcap *new_mathcap()
{
    mathcap *mcap = MALLOC(sizeof(mathcap));
    mcap->cmo = icopy(default_cmo);
    mcap->cmd = icopy(default_cmd);
    return mcap;
}

mathcap *new_mathcap_set(int *cmd, int *cmo)
{
    mathcap *mcap  = MALLOC(sizeof(mathcap));
	mcap->cmd = (cmd)? cmd: icopy(default_cmd);
	mcap->cmo = (cmo)? cmo: icopy(default_cmo);
    return mcap;
}

cmo_list *cmo_mathcap_1st() 
{
    char buffer[BUFSIZ];
    static char format[] = MATHCAP_1ST_FORMAT;
    int len = sizeof(format) + 32
		+ strlen(sysinfo.sysname)
		+ strlen(sysinfo.version)
		+ strlen(sysinfo.hosttype);
    if (len < BUFSIZ) {
        sprintf(buffer, format, sysinfo.ox_version, sysinfo.sysname, 
				sysinfo.version, sysinfo.hosttype);
        return (cmo_list *)ox_parse_lisp(buffer);
    }
    return NULL;
}

/* 0: terminator of array of integer. */
static int ilen(int a[])
{
    int i=0;
    if (a != NULL) {
        for( ; a[i] !=0; i++) {
        }
    }
    return i;
}

static int *icopy(int s[])
{
    int n = sizeof(int)*(ilen(s)+1);
    int *d = MALLOC(n);
    memcpy(d,s,n);
    return d;
}

static int *icopyn(int s[], int n)
{
    int *d = MALLOC((n = sizeof(int)*(n+1)));
    memcpy(d,s,n);
    return d;
}

cmo_mathcap *new_cmo_mathcap_by_mathcap(mathcap *mcap)
{
    cmo_list *cap1, *cap2, *cap3, *cap4;
    cap1 = cmo_mathcap_1st();
    cap2 = new_cmo_list_map(mcap->cmd, ilen(mcap->cmd), new_cmo_int32);
    cap3 = new_cmo_list_map(default_oxtag,
                            ilen(default_oxtag),
                            new_cmo_int32);
    cap4 = new_cmo_list_map(mcap->cmo, ilen(mcap->cmo), new_cmo_int32);
    /* new_cmo_mathcap([cap1, cap2, [cap3, cap4]]) */
    return new_cmo_mathcap(
        (cmo *)list_appendl(NULL, cap1, cap2, list_appendl(NULL, cap3, cap4, NULL)));
}

static int cmo_int32_to_int(cmo_int32* m)
{
    return m->i;
}

mathcap *new_mathcap_by_cmo_mathcap(cmo_mathcap *cap)
{
    int *cmd = list_to_array_map(list_nth(cap->ob, 1), cmo_int32_to_int);
    int *cmo = list_to_array_map(list_nth(list_nth(cap->ob, 2), 1), 
                                 cmo_int32_to_int);
    return new_mathcap_set(cmd, cmo);
}

/* if base is unsorted. */
static int *imerge(int *base, int *diff)
{
    int i,j,k;
    int n = ilen(base);
    int m = ilen(diff);
    int *t = ALLOCA(sizeof(int)*(n+1));
    int *ret;
    for(i=0,j=0; i<n; i++) {
        for(k=0; k<m; k++) {
            if (base[i] == diff[k]) {
                t[j++] = base[i];
                break;
            }
        }
    }
    t[j] = 0;
    ret = icopyn(t,j);
    return ret;
}

static mathcap *mathcap_merge_io(mathcap *this, mathcap *diff)
{
    int *tmp;
    tmp = imerge(this->cmo, diff->cmo);
    FREE(this->cmo); 
    this->cmo = tmp;
    return this;
}

/* for compatibility */
void mathcap_init(char *version, char *sysname)
{
    sysinfo.hosttype = getenv("HOSTTYPE");
    sysinfo.version  = version;
    sysinfo.sysname  = sysname;
}
cmo_mathcap* mathcap_get(mathcap *this)
{
    return new_cmo_mathcap_by_mathcap(this);
}
mathcap *mathcap_update(mathcap *this, cmo_mathcap *m)
{
    return mathcap_merge_io(this, new_mathcap_by_cmo_mathcap(m));
}
