/* -*- mode: C; coding: euc-japan -*- */
/* $OpenXM: OpenXM/src/ox_toolkit/dump.c,v 1.7 2015/08/27 03:03:33 ohara Exp $ */

/* 
   This module includes functions for sending/receiveng CMO's.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#if !defined(_MSC_VER)
#include <sys/param.h>
#else
#include <winsock2.h>
#endif
#include "ox_toolkit.h"

static void dump_cmo_int32(cmo_int32* m);
static void dump_cmo_list(cmo_list* m);
static void dump_cmo_mathcap(cmo_mathcap* m);
static void dump_cmo_null(cmo_null* m);
static void dump_cmo_string(cmo_string* m);
static void dump_cmo_monomial32(cmo_monomial32* c);
static void dump_cmo_zz(cmo_zz* c);
static void dump_mpz(mpz_ptr mpz);
static void dump_string(void *s, int len);
static void dump_integer(int x);

/* functions encoding cmo to binary */

static int  d_ptr;
static char *d_buf;

void dump_buffer_init(char *s)
{
    d_buf = s;
    d_ptr = 0;
}

static void dump_string(void *s, int len)
{
    memcpy(&d_buf[d_ptr], s, len);
    d_ptr += len;
}

static void dump_integer(int x)
{
    x = htonl(x);
    dump_string(&x, sizeof(int));
}

static void dump_mpz(mpz_ptr mpz)
{
    int i;
    int len = abs(mpz->_mp_size);
    dump_integer(mpz->_mp_size);
    for(i=0; i<len; i++) {
        dump_integer(mpz->_mp_d[i]);
    }
}

__inline__
static void dump_cmo_null(cmo_null* m)
{
    return;
}

static void dump_cmo_int32(cmo_int32* m)
{
    dump_integer(m->i);
}

static void dump_cmo_string(cmo_string* m)
{
    int len = strlen(m->s);
    dump_integer(len);
    dump_string(m->s, len);
}

static void dump_cmo_mathcap(cmo_mathcap* c)
{
    dump_cmo(c->ob);
}

static void dump_cmo_list(cmo_list* m)
{
    cell* cp = list_first(m);
    int len  = list_length(m);
    dump_integer(len);
    while(!list_endof(m, cp)) {
        dump_cmo(cp->cmo);
        cp = list_next(cp);
    }
}

static void dump_cmo_monomial32(cmo_monomial32* c)
{
    int i;
    int length = c->length;
    dump_integer(c->length);
    for(i=0; i<length; i++) {
        dump_integer(c->exps[i]);
    }
    dump_cmo(c->coef);
}

static void dump_cmo_zz(cmo_zz* c)
{
    dump_mpz(c->mpz);
}

static void dump_cmo_distributed_polynomial(cmo_distributed_polynomial* m)
{
    cell* cp;
    dump_integer(list_length((cmo_list *)m));
    dump_cmo(m->ringdef);
    for(cp = list_first((cmo_list *)m); !list_endof((cmo_list *)m, cp); cp = list_next(cp)) {
        dump_cmo(cp->cmo);
    }
}

/* after its tag is sent, we invoke each functions. */
void dump_cmo(cmo* m)
{
    dump_integer(m->tag);
    switch(m->tag) {
    case CMO_NULL:
    case CMO_ZERO:
    case CMO_DMS_GENERIC:
        dump_cmo_null(m);
        break;
    case CMO_INT32:
        dump_cmo_int32((cmo_int32 *)m);
        break;
    case CMO_STRING:
        dump_cmo_string((cmo_string *)m);
        break;
    case CMO_MATHCAP:
    case CMO_RING_BY_NAME:
    case CMO_INDETERMINATE:
    case CMO_ERROR2:
        dump_cmo_mathcap((cmo_mathcap *)m);
        break;
    case CMO_LIST:
        dump_cmo_list((cmo_list *)m);
        break;
    case CMO_MONOMIAL32:
        dump_cmo_monomial32((cmo_monomial32 *)m);
        break;
    case CMO_ZZ:
        dump_cmo_zz((cmo_zz *)m);
        break;
    case CMO_DISTRIBUTED_POLYNOMIAL:
        dump_cmo_distributed_polynomial((cmo_distributed_polynomial *)m);
        break;
    default:
		;
    }
}

void dump_ox_data(ox_data* m)
{
    dump_integer(OX_DATA);
    dump_integer(-1);
    dump_cmo(m->cmo);
}

void dump_ox_command(ox_command* m)
{
    dump_integer(OX_COMMAND);
    dump_integer(-1);
    dump_integer(m->command);
}


