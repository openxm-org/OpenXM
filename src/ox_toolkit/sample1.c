/* -*- mode: C -*- */
/* $OpenXM: OpenXM/src/ox_toolkit/sample1.c,v 1.6 2003/03/23 20:17:35 ohara Exp $ */

/*
   This sample program explains how to construct
     (OX_DATA,(CMO_INT,32))
     (OX_DATA,(CMO_STRING,"Hello"))
     (OX_DATA,(CMO_LIST,1,2,3))
     (OX_COMMAND,(SM_popCMO))
     etc
   by using C functions.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "ox_toolkit.h"

/* This program uses High-level functions of OpenXM C library. */
int main()
{
    OXFILE *s;
    cmo_list *c;

    ox_stderr_init(stderr);
    /* starting an OpenXM server */
    s = ox_start("localhost", "ox", "ox_sm1"); 

    ox_push_cmo(s, (cmo *)new_cmo_int32(32));
    ox_push_cmo(s, (cmo *)new_cmo_string("Hello"));

    /* making a list {10000, -2342, 3141592653289793238462643383279}.
       the elements are a bignum. */
    c = new_cmo_list();

    list_append(c, new_cmo_zz_set_si(10000));
    list_append(c, new_cmo_zz_set_si(-2342));
    list_append(c, new_cmo_zz_set_string("3141592653289793238462643383279"));

    /* sending the list above. */
    ox_push_cmo(s, (cmo *)c);

    ox_push_cmd(s, SM_popCMO);
    ox_push_cmd(s, SM_popCMO);
    ox_push_cmd(s, SM_popCMO);

    ox_close(s);
    return 0;
}
