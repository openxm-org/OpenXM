/* -*- mode: C; coding: euc-japan -*- */
/* $OpenXM: OpenXM/src/ox_toolkit/mathcap.c,v 1.2 2000/10/12 15:40:41 ohara Exp $ */

/* This module includes functions for handling mathcap databases. */

#include <stdlib.h>
#include "ox_toolkit.h"

/* 送信許可されている cmo, sm のリストを得る */
#define smdb_get_allow_all(x)   mcdb_get_allow_all((x)->sm_db)
#define cmodb_get_allow_all(x)  mcdb_get_allow_all((x)->cmo_db)

/* 全ての種類の cmo の送信を許可/不許可にする */
#define cmodb_deny_all(x)   mcdb_ctl_all((x), MATHCAP_FLAG_DENY)
#define cmodb_allow_all(x)  mcdb_ctl_all((x), MATHCAP_FLAG_ALLOW)

typedef struct {
    int tag;
    int flag;
} mcdb;

typedef struct {
    mcdb *cmo_db;
    mcdb *sm_db;
} mathcap;

static char *new_string(char *s);
static cmo_list *cmo_mathcap_get_cmotypes(cmo_mathcap *mc);
static cmo_list *get_messagetypes(cmo_list *ob, int type);
static cmo_list *mcdb_get_allow_all(mcdb *db);
static cmo_list *sysinfo_get_all();
static int cmodb_isallow_cmo(mcdb *cmo_db, cmo *ob);
static int cmodb_isallow_cmo_list(mcdb *cmo_db, cmo_list *ob);
static int cmodb_isallow_cmo_mathcap(mcdb *cmo_db, cmo_mathcap *ob);
static int cmodb_isallow_cmo_monomial32(mcdb *cmo_db, cmo_monomial32 *ob);
static int cmodb_isallow_tag(mcdb *cmo_db, int tag);
static mcdb *mcdb_lookup(mcdb *db, int tag);
static mcdb *new_mcdb(int *src);
static void cmodb_allow(mcdb *cmo_db, int tag);
static void cmodb_deny(mcdb *cmo_db, int tag);
static void cmodb_update(mcdb *cmo_db, cmo_list* types);
static void mcdb_ctl(mcdb *db, int tag, int flag);
static void mcdb_ctl_all(mcdb *db, int flag);
static void mcdb_init(mcdb *m, int key);
void sysinfo_set(int version, char *version_string, char *sysname);

static int cmodb_d[] = {
    CMO_NULL,
    CMO_INT32,
    CMO_DATUM,
    CMO_STRING,
    CMO_MATHCAP,
    CMO_LIST,
    CMO_MONOMIAL32,
    CMO_ZZ,
    CMO_ZERO,
    CMO_DMS_GENERIC,
    CMO_RING_BY_NAME,
    CMO_INDETERMINATE,
    CMO_DISTRIBUTED_POLYNOMIAL,
    CMO_ERROR2,
    0,
};

static int smdb_d[] = {
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
    0,
};

__inline__
static void mcdb_init(mcdb *m, int key)
{
    m->tag  = key;
    m->flag = MATHCAP_FLAG_ALLOW;
}

static mcdb *new_mcdb(int *src)
{
    mcdb *new;
    int len=0;
    int i;
    while (src[len++] != 0) {
    }
    new = malloc(sizeof(mcdb)*len);
    for(i=0; i<len; i++) {
        mcdb_init(new+i, src[i]);
    }
    return new;
}

mathcap *new_mathcap_d()
{
    mathcap *new = malloc(sizeof(mathcap));
    new->cmo_db = new_mcdb(cmodb_d);
    new->sm_db  = new_mcdb(smdb_d);
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
} sysinfo = {0, "NO VERSION", "NONAME", "UNKNOWN"};

/* 次の tag をもつ cmo の送信が許可されているかを調べる */
static int cmodb_isallow_tag(mcdb *cmo_db, int tag)
{
    while (cmo_db->tag != 0 && cmo_db->tag != tag) {
        cmo_db++;
    }
    return cmo_db->flag;
}

static int cmodb_isallow_cmo_list(mcdb *cmo_db, cmo_list *ob)
{
    cell *el;
    if (cmodb_isallow_tag(cmo_db, ob->tag)) {
        el = list_first(ob);
        while (!list_endof(ob, el)) {
            if (!cmodb_isallow_cmo(cmo_db, el->cmo)) {
                return MATHCAP_FLAG_DENY;
            }
            el = list_next(el);
        }
        return MATHCAP_FLAG_ALLOW;
    }
    return MATHCAP_FLAG_DENY;
}

__inline__
static int cmodb_isallow_cmo_monomial32(mcdb *cmo_db, cmo_monomial32 *ob)
{
    return cmodb_isallow_tag(cmo_db, ob->tag)
        && cmodb_isallow_cmo(cmo_db, ob->coef);
}

__inline__
static int cmodb_isallow_cmo_mathcap(mcdb *cmo_db, cmo_mathcap *ob)
{
    return cmodb_isallow_tag(cmo_db, ob->tag)
        && cmodb_isallow_cmo(cmo_db, ob->ob);
}

/* 次の cmo の送信が許可されているかを調べる */
static int cmodb_isallow_cmo(mcdb *cmo_db, cmo *ob)
{
    int tag = ob->tag;
    switch(tag) {
    case CMO_LIST:
    case CMO_DISTRIBUTED_POLYNOMIAL:
        return cmodb_isallow_cmo_list(cmo_db, (cmo_list *)ob);
    case CMO_MATHCAP:
    case CMO_ERROR2:
    case CMO_RING_BY_NAME:
    case CMO_INDETERMINATE:
        return cmodb_isallow_cmo_mathcap(cmo_db, (cmo_mathcap *)ob);
    case CMO_MONOMIAL32:
        return cmodb_isallow_cmo_monomial32(cmo_db, (cmo_monomial32 *)ob);
    default:
        return cmodb_isallow_tag(cmo_db, tag);
    }
}

/* 次の tag をもつ cmo の送信を許可する */
__inline__
static void cmodb_allow(mcdb *cmo_db, int tag)
{
    mcdb_ctl(cmo_db, tag, MATHCAP_FLAG_ALLOW);
}

/* 次の tag をもつ cmo の送信を不許可にする */
__inline__
static void cmodb_deny(mcdb *cmo_db, int tag)
{
    mcdb_ctl(cmo_db, tag, MATHCAP_FLAG_DENY);
}

/* システム情報を得る */
static cmo_list *sysinfo_get_all()
{
    cmo_list *syslist = new_cmo_list();
    cmo_int32 *ver    = new_cmo_int32(sysinfo.version);
    cmo_string *vers  = new_cmo_string(sysinfo.version_string);
    cmo_string *host  = new_cmo_string(sysinfo.hosttype);
    cmo_string *sname = new_cmo_string(sysinfo.sysname);
    return list_appendl(syslist, ver, sname, vers, host, NULL);
}

__inline__
static char *new_string(char *s)
{
    char *t = malloc(sizeof(s)+1);
    strcpy(t, s);
    return t;
}

void sysinfo_set(int version, char *vstr, char *sysname)
{
    char *host = getenv("HOSTTYPE");
    sysinfo.hosttype = (host != NULL)? new_string(host): "UNKNOWN";
    sysinfo.sysname  = new_string(sysname);
    sysinfo.version_string = new_string(vstr);
    sysinfo.version  = version;
}

void mathcap_global_init(int ver, char *vstr, char *sysname, int cmos[], int sms[])
{
	sysinfo_set(ver, vstr, sysname); /* global! */
	if (cmos == NULL) {
		cmos = cmodb_d;
	}
	if (sms == NULL) {
		sms = smdb_d;
	}
	/* ... */
}

/* データベースから cmo_mathcap を生成する */
cmo_mathcap* mathcap_get(mathcap *this)
{
    cmo_list *mc = new_cmo_list();
    cmo_list *l3 = new_cmo_list();
    list_append(l3, list_appendl(new_cmo_list(),
                                 new_cmo_int32(OX_DATA),
                                 cmodb_get_allow_all(this), NULL));
    list_appendl(mc, (cmo *)sysinfo_get_all(),
                 (cmo *)smdb_get_allow_all(this), (cmo *)l3, NULL);
    return new_cmo_mathcap((cmo *)mc);
}

static void cmodb_update(mcdb *cmo_db, cmo_list* types)
{
    cell *el = list_first(types);
    cmo_int32 *ob;
    while(!list_endof(types, el)) {
        ob = (cmo_int32 *)el->cmo;
        if (ob->tag == CMO_INT32) {
            cmodb_allow(cmo_db, ob->i);
        }
        el = list_next(el);
    }
}

/* cmo_mathcap が妥当な構造を持つかを調べる.  (未実装) */
int cmo_mathcap_isvalid(cmo_mathcap *mc)
{
	/* print_cmo(mc); */
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
/* this == NULL のとき、はじめて mathcap* オブジェクトをせいせいする */
mathcap *mathcap_update(mathcap *this, cmo_mathcap *mc)
{
    cmo_list *types;
    if (cmo_mathcap_isvalid(mc)) {
        types = cmo_mathcap_get_cmotypes(mc);
        if (types != NULL) {
            cmodb_deny_all(this->cmo_db); /* すべての cmo の送信を禁止 */
            cmodb_update(this->cmo_db, types);
        }
    }
	return this;
}
