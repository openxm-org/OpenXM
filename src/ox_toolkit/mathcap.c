/* -*- mode: C; coding: euc-japan -*- */
/* $OpenXM: OpenXM/src/ox_toolkit/mathcap.c,v 1.1 2000/10/10 05:23:20 ohara Exp $ */

/* This module includes functions for handling mathcap databases. */

#include <stdlib.h>
#include "ox_toolkit.h"

typedef struct {
    int tag;
    int flag;
} mcdb;

static mcdb *mcdb_lookup(mcdb *db, int tag);
static void mcdb_ctl(mcdb *db, int tag, int flag);
static void mcdb_ctl_all(mcdb *db, int flag);
static cmo_list *mcdb_get_allow_all(mcdb *db);

static char *new_string(char *s);
static cmo_list *cmo_mathcap_get_cmotypes(cmo_mathcap *mc);
static cmo_list *get_messagetypes(cmo_list *ob, int type);
static cmo_list *mathcap_sm_get_all();
static cmo_list *mathcap_sysinfo_get_all();
static int mathcap_cmo_isallow_cmo_list(cmo_list *ob);
static int mathcap_cmo_isallow_cmo_mathcap(cmo_mathcap *ob);
static int mathcap_cmo_isallow_cmo_monomial32(cmo_monomial32 *ob);
static int mathcap_cmo_isallow_tag(int tag);
static void mathcap_cmo_allow(int tag);
static void mathcap_cmo_allow_all();
static void mathcap_cmo_deny(int tag);
static void mathcap_cmo_deny_all();
static void mathcap_cmo_update(cmo_list* types);

static mcdb mathcap_cmo[] = {
    {CMO_NULL,       MATHCAP_FLAG_ALLOW},
    {CMO_INT32,      MATHCAP_FLAG_ALLOW},
    {CMO_DATUM,      MATHCAP_FLAG_ALLOW},
    {CMO_STRING,     MATHCAP_FLAG_ALLOW},
    {CMO_MATHCAP,    MATHCAP_FLAG_ALLOW},
    {CMO_LIST,       MATHCAP_FLAG_ALLOW},
    {CMO_MONOMIAL32, MATHCAP_FLAG_ALLOW},
    {CMO_ZZ,         MATHCAP_FLAG_ALLOW},
    {CMO_ZERO,       MATHCAP_FLAG_ALLOW},
    {CMO_DMS_GENERIC, MATHCAP_FLAG_ALLOW},
    {CMO_RING_BY_NAME, MATHCAP_FLAG_ALLOW},
    {CMO_INDETERMINATE, MATHCAP_FLAG_ALLOW},
    {CMO_DISTRIBUTED_POLYNOMIAL, MATHCAP_FLAG_ALLOW},
    {CMO_ERROR2,     MATHCAP_FLAG_ALLOW},
    {0,              MATHCAP_FLAG_DENY}
};

static mcdb mathcap_sm[] = {
    {SM_popCMO,	    MATHCAP_FLAG_ALLOW},
    {SM_popString,	MATHCAP_FLAG_ALLOW},
    {SM_mathcap,	MATHCAP_FLAG_ALLOW},
    {SM_pops,		MATHCAP_FLAG_ALLOW},
    {SM_executeStringByLocalParser,		MATHCAP_FLAG_ALLOW},
    {SM_executeFunction,	MATHCAP_FLAG_ALLOW},
    {SM_setMathCap,		    MATHCAP_FLAG_ALLOW},
    {SM_shutdown,		    MATHCAP_FLAG_ALLOW},
    {SM_control_kill,		MATHCAP_FLAG_ALLOW},
    {SM_control_reset_connection,		MATHCAP_FLAG_ALLOW},
    {0,              MATHCAP_FLAG_DENY}
};

typedef struct {
	mcdb *cmo_db;
	mcdb *sm_db;
} mathcap;

mathcap default_mathcap = {mathcap_cmo, mathcap_sm};

mcdb *new_mcdb(mcdb *src)
{
	mcdb *new;
	int len=0;
	while ((src+(len++))->tag != 0) {
	}
	new = malloc(sizeof(mcdb)*len);
	memcpy(new, src, sizeof(mcdb)*len);
	return new;
}

mathcap *new_mathcap()
{
	mathcap *new = malloc(sizeof(mathcap));
	new->cmo_db = new_mcdb(mathcap_cmo);
	new->sm_db  = new_mcdb(mathcap_sm);
	return new;
}

/* 次の tag についてのキーを探す */
static mcdb *mcdb_lookup(mcdb *db, int tag)
{
    while (db->tag != 0) {
        if (db->tag == tag) {
            return db;
        }
        db++;
    }
    return NULL;
}

/* tag に対する送信制御 */
static void mcdb_ctl(mcdb *db, int tag, int flag)
{
    mcdb *e = mcdb_lookup(db, tag);
    if (e != NULL) {
        e->flag = flag;
    }
}

/* 全データに対する送信制御 */
static void mcdb_ctl_all(mcdb *db, int flag)
{
    while (db->tag != 0) {
        db->flag = flag;
        db++;
    }
}

/* 送信許可されている tag のリストを得る */
static cmo_list *mcdb_get_allow_all(mcdb *db)
{
    cmo_list *list = new_cmo_list();
    while (db->tag != 0) {
        if (db->flag == MATHCAP_FLAG_ALLOW) {
            list_append(list, (cmo *)new_cmo_int32(db->tag));
        }
        db++;
    }
    return list;
}

static struct {
    int  version;
    char *version_string;
    char *sysname;
    char *hosttype;
} mathcap_sysinfo = {0, "NO VERSION", "NONAME", "UNKNOWN"};

/* 次の tag をもつ cmo の送信が許可されているかを調べる */
static int mathcap_cmo_isallow_tag(int tag)
{
    mcdb *e = mathcap_cmo;
    while (e->tag != 0 && e->tag != tag) {
        e++;
    }
    return e->flag;
}

static int mathcap_cmo_isallow_cmo_list(cmo_list *ob)
{
    cell *el;
    if (mathcap_cmo_isallow_tag(ob->tag)) {
        el = list_first(ob);
        while (!list_endof(ob, el)) {
            if (!mathcap_cmo_isallow_cmo(el->cmo)) {
                return MATHCAP_FLAG_DENY;
            }
            el = list_next(el);
        }
        return MATHCAP_FLAG_ALLOW;
    }
    return MATHCAP_FLAG_DENY;
}

__inline__
static int mathcap_cmo_isallow_cmo_monomial32(cmo_monomial32 *ob)
{
    return mathcap_cmo_isallow_tag(ob->tag)
        && mathcap_cmo_isallow_cmo(ob->coef);
}

__inline__
static int mathcap_cmo_isallow_cmo_mathcap(cmo_mathcap *ob)
{
    return mathcap_cmo_isallow_tag(ob->tag)
        && mathcap_cmo_isallow_cmo(ob->ob);
}

/* 次の cmo の送信が許可されているかを調べる */
int mathcap_cmo_isallow_cmo(cmo *ob)
{
    int tag = ob->tag;
    switch(tag) {
    case CMO_LIST:
    case CMO_DISTRIBUTED_POLYNOMIAL:
        return mathcap_cmo_isallow_cmo_list((cmo_list *)ob);
    case CMO_MATHCAP:
    case CMO_ERROR2:
    case CMO_RING_BY_NAME:
    case CMO_INDETERMINATE:
        return mathcap_cmo_isallow_cmo_mathcap((cmo_mathcap *)ob);
    case CMO_MONOMIAL32:
        return mathcap_cmo_isallow_cmo_monomial32((cmo_monomial32 *)ob);
    default:
        return mathcap_cmo_isallow_tag(tag);
    }
}

/* 次の tag をもつ cmo の送信を許可する */
__inline__
static void mathcap_cmo_allow(int tag)
{
	mcdb_ctl(mathcap_cmo, tag, MATHCAP_FLAG_ALLOW);
}

/* 次の tag をもつ cmo の送信を不許可にする */
__inline__
static void mathcap_cmo_deny(int tag)
{
	mcdb_ctl(mathcap_cmo, tag, MATHCAP_FLAG_DENY);
}

/* 全ての種類の cmo の送信を不許可にする */
__inline__
static void mathcap_cmo_deny_all()
{
	mcdb_ctl_all(mathcap_cmo, MATHCAP_FLAG_DENY);
}

/* 全ての種類の cmo の送信を許可する */
__inline__
static void mathcap_cmo_allow_all()
{
	mcdb_ctl_all(mathcap_cmo, MATHCAP_FLAG_ALLOW);
}

/* 送信許可されている cmo のリストを得る */
cmo_list *mathcap_cmo_get_allow_all()
{
	return mcdb_get_allow_all(mathcap_cmo);
}

/* 既知の sm コマンドのリストを得る */
__inline__
static cmo_list *mathcap_sm_get_all()
{
	return mcdb_get_allow_all(mathcap_sm);
}

/* システム情報を得る */
static cmo_list *mathcap_sysinfo_get_all()
{
    cmo_list *syslist = new_cmo_list();
    cmo_int32 *ver    = new_cmo_int32(mathcap_sysinfo.version);
    cmo_string *vers  = new_cmo_string(mathcap_sysinfo.version_string);
    cmo_string *host  = new_cmo_string(mathcap_sysinfo.hosttype);
    cmo_string *sname = new_cmo_string(mathcap_sysinfo.sysname);
    return list_appendl(syslist, ver, sname, vers, host);
}

static char *new_string(char *s)
{
    char *t = malloc(sizeof(s)+1);
    strcpy(t, s);
    return t;
}

void mathcap_sysinfo_set(int version, char *version_string, char *sysname)
{
    char *host = getenv("HOSTTYPE");

    mathcap_sysinfo.hosttype = (host != NULL)? new_string(host): "UNKNOWN";
    mathcap_sysinfo.sysname  = new_string(sysname);
    mathcap_sysinfo.version_string = new_string(version_string);
    mathcap_sysinfo.version  = version;
}

/* データベースから cmo_mathcap を生成する */
cmo_mathcap* mathcap_get()
{
    cmo_list *mc = new_cmo_list();
    cmo_list *l3 = new_cmo_list();
    list_append(l3, list_appendl(new_cmo_list(), new_cmo_int32(OX_DATA), mathcap_cmo_get_allow_all(), NULL));
    list_appendl(mc, (cmo *)mathcap_sysinfo_get_all(), (cmo *)mathcap_sm_get_all(), (cmo *)l3, NULL);
    return new_cmo_mathcap((cmo *)mc);
}

static void mathcap_cmo_update(cmo_list* types)
{
    cell *el = list_first(types);
    cmo_int32 *ob;
    while(!list_endof(types, el)) {
        ob = (cmo_int32 *)el->cmo;
        if (ob->tag == CMO_INT32) {
            mathcap_cmo_allow(ob->i);
        }
        el = list_next(el);
    }
}

/* cmo_mathcap が妥当な構造を持つかを調べる.  (未実装) */
int cmo_mathcap_isvalid(cmo_mathcap *mc)
{
    return 1;
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

/* 受信した mathcap データを反映させる */
void mathcap_update(cmo_mathcap *mc)
{
    cmo_list *types;
    if (cmo_mathcap_isvalid(mc)) {
        types = cmo_mathcap_get_cmotypes(mc);
        if (types != NULL) {
            mathcap_cmo_deny_all(); /* すべての cmo の送信を禁止 */
            mathcap_cmo_update(types);
        }
    }
}
