/* -*- mode: C; coding: euc-japan -*- */
/* $OpenXM$ */

/* This module is needed by bconv.c */

/* 
The cmolen_cmo() function returns the length of the binary encoded
object for a given cmo.
cmolen_cmo_XXX() functions return the length of the binary encoded cmo
except its tag.
*/

#include "ox_toolkit.h"

static int          cmolen_cmo_int32(cmo_int32* c);
static int          cmolen_cmo_list(cmo_list* c);
static int          cmolen_cmo_mathcap(cmo_mathcap* c);
static int          cmolen_cmo_null(cmo_null* c);
static int          cmolen_cmo_string(cmo_string* c);
static int          cmolen_cmo_zz(cmo_zz* c);
static int          cmolen_cmo_monomial32(cmo_monomial32* c);

__inline__
static int cmolen_cmo_null(cmo_null* c)
{
    return 0;
}

static int cmolen_cmo_int32(cmo_int32* c)
{
    return sizeof(int);
}

static int cmolen_cmo_string(cmo_string* c)
{
    return sizeof(int)+strlen(c->s);
}

static int cmolen_cmo_mathcap(cmo_mathcap* c)
{
    return cmolen_cmo(c->ob);
}

static int cmolen_cmo_list(cmo_list* c)
{
    int size = sizeof(int);
    cell* cp;
    for(cp = list_first(c); !list_endof(c, cp); cp = list_next(cp)) {
        size += cmolen_cmo(cp->cmo);
    }
    return size;
}

static int cmolen_cmo_monomial32(cmo_monomial32* c)
{
    int len = (c->length + 1)*sizeof(int);
    return len + cmolen_cmo(c->coef);
}

static int cmolen_cmo_zz(cmo_zz* c)
{
    int len = abs(c->mpz->_mp_size);
    return sizeof(int) + len*sizeof(int);
}

static int cmolen_cmo_distributed_polynomial(cmo_distributed_polynomial* c)
{
    return cmolen_cmo_list((cmo_list *)c) + cmolen_cmo(c->ringdef);
}

/* a function calculating the length of the byte stream of a given CMO.  */
int cmolen_cmo(cmo* c)
{
    int size = sizeof(int);

    switch(c->tag) {
    case CMO_NULL:
    case CMO_ZERO:
    case CMO_DMS_GENERIC:
        size += cmolen_cmo_null(c);
        break;
    case CMO_INT32:
        size += cmolen_cmo_int32((cmo_int32 *)c);
        break;
    case CMO_STRING:
        size += cmolen_cmo_string((cmo_string *)c);
        break;
    case CMO_MATHCAP:
    case CMO_RING_BY_NAME:
    case CMO_INDETERMINATE:
    case CMO_ERROR2:
        size += cmolen_cmo_mathcap((cmo_mathcap *)c);
        break;
    case CMO_LIST:
        size += cmolen_cmo_list((cmo_list *)c);
        break;
    case CMO_MONOMIAL32:
        size += cmolen_cmo_monomial32((cmo_monomial32 *)c);
        break;
    case CMO_ZZ:
        size += cmolen_cmo_zz((cmo_zz *)c);
        break;
    case CMO_DISTRIBUTED_POLYNOMIAL:
        size += cmolen_cmo_distributed_polynomial((cmo_distributed_polynomial *)c);
        break;
    default:
    }
    return size;
}

