/* -*- mode: C; coding: euc-japan -*- */
/* $OpenXM: OpenXM/src/ox_toolkit/mathcap.c,v 1.17 2016/08/23 02:24:19 ohara Exp $ */

/* This module includes functions for handling mathcap databases. */

#include <stdlib.h>
#include <string.h>
#include "ox_toolkit.h"

#define MATHCAP_FLAG_DENY   0
#define MATHCAP_FLAG_ALLOW  1

static void table_init(table *m, int key);
static table *new_table(int *src);
static table *table_lookup(table *tbl, int tag);
static void table_ctl(table *tbl, int tag, int flag);
static void table_ctl_all(table *tbl, int flag);
static cmo_list *table_get_all(table *tbl);
static void table_allow(table *tbl, int tag);
static void table_deny(table *tbl, int tag);
static void table_update(table *cmotbl, cmo_list* types);
static int table_allowQ_tag(table *tbl, int tag);
static int table_allowQ_cmo_list(table *cmotbl, cmo_list *ob);
static int table_allowQ_cmo_monomial32(table *cmotbl, cmo_monomial32 *ob);
static int table_allowQ_cmo_mathcap(table *cmotbl, cmo_mathcap *ob);
static int table_allowQ_cmo(table *cmotbl, cmo *ob);
static cmo_list *sysinfo_get();
static char *new_string(char *s);
static int *new_int_array(int *array);
static cmo_list *get_messagetypes(cmo_list *ob, int type);
static cmo_list *cmo_mathcap_get_cmotypes(cmo_mathcap *mc);

static int cmotbl_a[] = {
    CMO_NULL,
    CMO_INT32,
    CMO_DATUM,
    CMO_STRING,
    CMO_MATHCAP,
    CMO_LIST,
    CMO_MONOMIAL32,
    CMO_ZZ,
    CMO_QQ,
    CMO_BIGFLOAT32,
    CMO_COMPLEX,
    CMO_IEEE_DOUBLE_FLOAT,
    CMO_ZERO,
    CMO_DMS_GENERIC,
    CMO_RING_BY_NAME,
    CMO_INDETERMINATE,
    CMO_DISTRIBUTED_POLYNOMIAL,
    CMO_RECURSIVE_POLYNOMIAL,
    CMO_POLYNOMIAL_IN_ONE_VARIABLE,
    CMO_ERROR2,
    0,
};

static int smtbl_a[] = {
    SM_popCMO,
    SM_popString,
    SM_mathcap,
    SM_pops,
    SM_executeStringByLocalParser,
    SM_executeFunction,
    SM_setMathCap,
    SM_shutdown,
    SM_control_kill,
    SM_control_reset_connection,
    SM_control_spawn_server,
    SM_control_terminate_server,
    0,
};

static struct {
    int  version;
    char *version_string;
    char *sysname;
    char *hosttype;
    int  *cmo_tags;
    int  *sm_cmds;
	char **opts;
} sysinfo = {0, "NO VERSION", "NONAME", "UNKNOWN", cmotbl_a, smtbl_a, NULL};

__inline__
static void table_init(table *m, int key)
{
    m->tag  = key;
    m->flag = MATHCAP_FLAG_ALLOW;
}

static table *new_table(int *src)
{
    table *new;
    int len=0;
    int i;
    while (src[len++] != 0) {
    }
    new = MALLOC(sizeof(table)*len);
    for(i=0; i<len; i++) {
        table_init(new+i, src[i]);
    }
    return new;
}

/* looking for an item of the tag */
static table *table_lookup(table *tbl, int tag)
{
    while (tbl->tag != 0) {
        if (tbl->tag == tag) {
            return tbl;
        }
        tbl++;
    }
    return NULL;
}

/* controller about a cmo identified by the tag */
static void table_ctl(table *tbl, int tag, int flag)
{
    table *e = table_lookup(tbl, tag);
    if (e != NULL) {
        e->flag = flag;
    }
}

/* controller about all CMObjects */
static void table_ctl_all(table *tbl, int flag)
{
    while (tbl->tag != 0) {
        tbl->flag = flag;
        tbl++;
    }
}

/* getting the list of tags of all allowed objects */
static cmo_list *table_get_all(table *tbl)
{
    cmo_list *list = new_cmo_list();
    while (tbl->tag != 0) {
        if (tbl->flag == MATHCAP_FLAG_ALLOW) {
            list_append(list, (cmo *)new_cmo_int32(tbl->tag));
        }
        tbl++;
    }
    return list;
}

/* giving a permission to send objects identified by the tag. */
__inline__
static void table_allow(table *tbl, int tag)
{
    table_ctl(tbl, tag, MATHCAP_FLAG_ALLOW);
}

/* taking a permission to send objects identified by the tag. */
__inline__
static void table_deny(table *tbl, int tag)
{
    table_ctl(tbl, tag, MATHCAP_FLAG_DENY);
}

static void table_update(table *cmotbl, cmo_list* types)
{
    cell *el = list_first(types);
    cmo_int32 *ob;
    while(!list_endof(types, el)) {
        ob = (cmo_int32 *)el->cmo;
        if (ob->tag == CMO_INT32) {
            table_allow(cmotbl, ob->i);
        }
        el = list_next(el);
    }
}

/* getting a permission to send objects identified by the tag. */
static int table_allowQ_tag(table *tbl, int tag)
{
    while (tbl->tag != 0 && tbl->tag != tag) {
        tbl++;
    }
    return tbl->flag;
}

static int table_allowQ_cmo_list(table *cmotbl, cmo_list *ob)
{
    cell *el;
    if (table_allowQ_tag(cmotbl, ob->tag)) {
        el = list_first(ob);
        while (!list_endof(ob, el)) {
            if (!table_allowQ_cmo(cmotbl, el->cmo)) {
                return MATHCAP_FLAG_DENY;
            }
            el = list_next(el);
        }
        return MATHCAP_FLAG_ALLOW;
    }
    return MATHCAP_FLAG_DENY;
}

__inline__
static int table_allowQ_cmo_monomial32(table *cmotbl, cmo_monomial32 *ob)
{
    return table_allowQ_tag(cmotbl, ob->tag)
        && table_allowQ_cmo(cmotbl, ob->coef);
}

__inline__
static int table_allowQ_cmo_mathcap(table *cmotbl, cmo_mathcap *ob)
{
    return table_allowQ_tag(cmotbl, ob->tag)
        && table_allowQ_cmo(cmotbl, ob->ob);
}

/* getting a permission to send the following object. */
static int table_allowQ_cmo(table *cmotbl, cmo *ob)
{
    int tag = ob->tag;
    switch(tag) {
    case CMO_LIST:
    case CMO_DISTRIBUTED_POLYNOMIAL:
        return table_allowQ_cmo_list(cmotbl, (cmo_list *)ob);
    case CMO_MATHCAP:
    case CMO_ERROR2:
    case CMO_RING_BY_NAME:
    case CMO_INDETERMINATE:
        return table_allowQ_cmo_mathcap(cmotbl, (cmo_mathcap *)ob);
    case CMO_MONOMIAL32:
        return table_allowQ_cmo_monomial32(cmotbl, (cmo_monomial32 *)ob);
    default:
        return table_allowQ_tag(cmotbl, tag);
    }
}

/* getting the System Information */
static cmo_list *sysinfo_get()
{
    cmo_list *syslist = new_cmo_list();
    cmo_int32 *ver    = new_cmo_int32(sysinfo.version);
    cmo_string *vers  = new_cmo_string(sysinfo.version_string);
    cmo_string *host  = new_cmo_string(sysinfo.hosttype);
    cmo_string *sname = new_cmo_string(sysinfo.sysname);
    return list_appendl(syslist, ver, sname, vers, host, NULL);
}

static char *new_string(char *s)
{
    char *t = MALLOC(strlen(s)+1);
    strcpy(t, s);
    return t;
}

static int *new_int_array(int *array)
{
    int *new_array;
    int length = 0;
    while(array[length++] != 0)
        ;
    new_array = MALLOC(sizeof(int)*length);
    return memcpy(new_array, array, sizeof(int)*length);
}

void mathcap_init(int ver, char *vstr, char *sysname, int cmos[], int sms[])
{
    mathcap_init2(ver, vstr, sysname, cmos, sms, NULL);
}

/* src must be terminated by NULL */
static char **clone_str_list(char **src)
{
    int i,len;
    char **new = NULL;
    if(src) {
        for(len=0; src[len]!=NULL; len++) {
        }
        new = (char **)MALLOC(sizeof(char *)*(len+1));
        new[len] = NULL;
        for(i=0; i<len; i++) {
            new[i] = (char *)MALLOC(strlen(src[i])+1);
            strcpy(new[i], src[i]);
        }
    }
    return new;
}

/* options must be terminated by NULL */
void mathcap_init2(int ver, char *vstr, char *sysname, int cmos[], int sms[], char *options[])
{
    char *host = getenv("HOSTTYPE");
    sysinfo.hosttype = (host != NULL)? new_string(host): "UNKNOWN";
    sysinfo.sysname  = new_string(sysname);
    sysinfo.version_string = new_string(vstr);
    sysinfo.version  = ver;
    if (cmos != NULL) {
        sysinfo.cmo_tags = new_int_array(cmos);
    }
    if (sms != NULL) {
        sysinfo.sm_cmds = new_int_array(sms);
    }
    sysinfo.opts = clone_str_list(options);
}

mathcap *new_mathcap()
{
    mathcap *new = MALLOC(sizeof(mathcap));
    new->cmotbl = new_table(sysinfo.cmo_tags);
    new->smtbl  = new_table(sysinfo.sm_cmds);
    new->opts   = clone_str_list(sysinfo.opts);
    return new;
}

/* generating a cmo_mathcap by a local database. */
cmo_mathcap* mathcap_get(mathcap *this)
{
    cmo_list *mc = new_cmo_list();
    cmo_list *l3 = new_cmo_list();
    cmo_list *si = sysinfo_get();
    cmo_list *sm=  table_get_all(this->smtbl);
    cmo_list *opts;
    int i;

    list_append(l3, (cmo *)list_appendl(new_cmo_list(),
                                 new_cmo_int32(OX_DATA),
                                 table_get_all(this->cmotbl), NULL));
    if(this->opts) {
        opts = new_cmo_list();
        for(i=0; this->opts[i]!=NULL; i++) {
            list_append(opts, (cmo *)new_cmo_string(this->opts[i]));
        }
        list_appendl(mc, (cmo *)si, (cmo *)sm, (cmo *)l3, (cmo *)opts, NULL);
    }else {
        list_appendl(mc, (cmo *)si, (cmo *)sm, (cmo *)l3, NULL);
    }
    return new_cmo_mathcap((cmo *)mc);
}

/* ( ..., ( type, (...) ), (cmo_int32, (...) ), ... )  */
/*                ^^^^^ Here!                          */
static cmo_list *get_messagetypes(cmo_list *ob, int type)
{
    cmo_list  *c;
    cell *el;

    for (el = list_first(ob); !list_endof(ob, el); el = list_next(el)) {
        c = (cmo_list *)el->cmo;
        if (((cmo_int32 *)list_nth(c, 0))->i == type) {
            return (cmo_list *)list_nth(c, 1);
        }
    }
    return NULL;
}

/* cmo_mathcap->ob = ( (...), (...), ( ( cmo_int32, (...) ), ...), ...) */
/*                                              ^^^^^ Here!         */
__inline__
static cmo_list *cmo_mathcap_get_cmotypes(cmo_mathcap *mc)
{
    cmo_list *ob = (cmo_list *)list_nth((cmo_list *)mc->ob, 2);
    return get_messagetypes(ob, OX_DATA);
}

/* The mathcap_update integrates received cmo_mathcap into the mathcap
   database. If this == NULL, then an instance of mathcap is generated. */
mathcap *mathcap_update(mathcap *this, cmo_mathcap *mc)
{
    cmo_list *types;
    types = cmo_mathcap_get_cmotypes(mc);
    if (types != NULL) {
        table_ctl_all(this->cmotbl, MATHCAP_FLAG_DENY);
        table_update(this->cmotbl, types);
    }

    return this;
}

int mathcap_allowQ_cmo(mathcap *this, cmo *ob)
{
    return table_allowQ_cmo(this->cmotbl, ob);
}
