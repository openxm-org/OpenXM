/* -*- mode: C; coding: euc-japan -*- */
/* $OpenXM: OpenXM/src/ox_toolkit/print.c,v 1.7 2018/04/04 09:52:03 ohara Exp $ */

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
static void print_cmo_double(cmo_double* c);
static void print_cmo_bf(cmo_bf* c);
static void print_cmo_zz(cmo_zz* c);
static void print_cmo_qq(cmo_qq* c);
static void print_cmo_tree(cmo_tree* c);
static void print_cmo_polynomial_in_one_variable(cmo_polynomial_in_one_variable* c);
static void print_cmo_recursive_polynomial(cmo_recursive_polynomial* c);

void print_cmo(cmo* c)
{
    int tag = c->tag;
    char *s = get_symbol_by_tag(tag);
    if (s != NULL) {
        ox_printf("(%s", s);
    }else {     
        ox_printf("(%d", tag);
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
        ox_printf(")");
        break;
    case CMO_ZZ:
        print_cmo_zz((cmo_zz *)c);
        break;
    case CMO_QQ:
        print_cmo_qq((cmo_qq *)c);
        break;
    case CMO_BIGFLOAT:
        print_cmo_bf((cmo_bf *)c);
        break;
    case CMO_IEEE_DOUBLE_FLOAT:
    case CMO_64BIT_MACHINE_DOUBLE:
        print_cmo_double((cmo_double *)c);
        break;
    case CMO_TREE:
        print_cmo_tree((cmo_tree *)c);
        break;
    case CMO_POLYNOMIAL_IN_ONE_VARIABLE:
        print_cmo_polynomial_in_one_variable((cmo_polynomial_in_one_variable *)c);
        break;
    case CMO_RECURSIVE_POLYNOMIAL:
        print_cmo_recursive_polynomial((cmo_recursive_polynomial *)c);
        break;
    default:
        ox_printf("\nprint_cmo() does not know how to print cmo of type %d.\n", tag);
    }
}

static void print_cmo_int32(cmo_int32* c)
{
    ox_printf(", %d)", c->i);
}

static void print_cmo_list(cmo_list* this)
{
    cell* cp = list_first(this);
    ox_printf("[%d]", list_length(this));
    while(!list_endof(this, cp)) {
        ox_printf(", ");
        print_cmo(cp->cmo);
        cp=list_next(cp);
    }
    ox_printf(")");
}

static void print_cmo_mathcap(cmo_mathcap* c)
{
    ox_printf(", ");
    print_cmo(c->ob);
    ox_printf(")");
}

static void print_cmo_string(cmo_string* c)
{
    ox_printf(", \"%s\")", c->s);
}

static void print_cmo_double(cmo_double* c)
{
    ox_printf(", %.14f)", c->d);
}

static void print_cmo_zz(cmo_zz* c)
{
	char buf[4096];
    gmp_sprintf(buf, ", %Zd)", c->mpz);
    ox_printf("%s", buf);
}

static void print_cmo_qq(cmo_qq* c)
{
	char buf[4096];
    gmp_sprintf(buf, ", %Qd)", c->mpq);
    ox_printf("%s", buf);
}

static void print_cmo_bf(cmo_bf* c)
{
	char buf[4096];
    mpfr_sprintf(buf, ", %.12f)", c->mpfr);
    ox_printf("%s", buf);
}

static void print_cmo_tree(cmo_tree* c)
{
    ox_printf(", ");
    print_cmo((cmo*)c->name);
    ox_printf(", ");
    print_cmo((cmo*)c->attributes);
    ox_printf(", ");
    print_cmo((cmo*)c->leaves);
    ox_printf(")");
}


static void print_cmo_polynomial_in_one_variable(cmo_polynomial_in_one_variable* c)
{
    int i;
    cell *cc;
    ox_printf(", %d", c->var);
    for(i=0; i<c->length; i++) {
        cc = list_nth_cell((cmo_list *)c, i);
		ox_printf(", %d, ", cc->exp);
		print_cmo(cc->cmo);
    }
    ox_printf(")");
}

static void print_cmo_recursive_polynomial(cmo_recursive_polynomial* c)
{
    ox_printf(", ");
	print_cmo((cmo *)c->ringdef);
	ox_printf(", ");
	print_cmo(c->coef);
    ox_printf(")");
}

