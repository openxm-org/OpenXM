/* -*- mode: C; coding: euc-japan -*- */
/* $OpenXM: OpenXM/src/ox_toolkit/print.c,v 1.1 2000/10/10 05:23:21 ohara Exp $ */

/*
Functions in this module print a given CMO to console.
These functions are used for debugging.
*/

#include <stdio.h>
#include <stdlib.h>
#include "ox_toolkit.h"
#include "parse.h"

static void print_cmo_int32(cmo_int32* c);
static void print_cmo_list(cmo_list* li);
static void print_cmo_mathcap(cmo_mathcap* c);
static void print_cmo_string(cmo_string* c);

void print_cmo(cmo* c)
{
    int tag = c->tag;

    symbol_t symp = lookup_by_tag(tag);
    if (symp != NULL) {
        fprintf(stderr, "(%s", symp->key);
    }else {     
        fprintf(stderr, "(%d", tag);
    }

    switch(tag) {
    case CMO_LIST:
        print_cmo_list((cmo_list *)c);
        break;
    case CMO_INT32:
        print_cmo_int32((cmo_int32 *)c);
        break;
    case CMO_MATHCAP:
    case CMO_INDETERMINATE:
    case CMO_RING_BY_NAME:
    case CMO_ERROR2:
        print_cmo_mathcap((cmo_mathcap *)c);
        break;
    case CMO_STRING:
        print_cmo_string((cmo_string *)c);
        break;
    case CMO_NULL:
    case CMO_ZERO:
    case CMO_DMS_GENERIC:
        fprintf(stderr, ")");
        break;
    default:
        fprintf(stderr, "\nprint_cmo() does not know how to print cmo of type %d.\n", tag);
    }
}

static void print_cmo_int32(cmo_int32* c)
{
    fprintf(stderr, ", %d)", c->i);
}

static void print_cmo_list(cmo_list* this)
{
    cell* cp = list_first(this);
	fprintf(stderr, "[%d]", list_length(this));
    while(!list_endof(this, cp)) {
        fprintf(stderr, ", ");
        print_cmo(cp->cmo);
        cp=list_next(cp);
    }
    fprintf(stderr, ")");
}

static void print_cmo_mathcap(cmo_mathcap* c)
{
    fprintf(stderr, ", ");
    print_cmo(c->ob);
    fprintf(stderr, ")");
}

static void print_cmo_string(cmo_string* c)
{
    fprintf(stderr, ", \"%s\")", c->s);
}

