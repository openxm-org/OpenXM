/* -*- mode: C -*- */
/* $OpenXM: OpenXM/src/ox_toolkit/sample2.c,v 1.7 2003/03/30 08:05:23 ohara Exp $ */

/*
  This program explains how to get
  int, char *, and so on from the received data
  by using low-level functions of OpenXM C library.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "ox_toolkit.h"

OXFILE* ox_start(char* host, char* prog1, char* prog2);

void explain_cmo(cmo *);

void explain_cmo_list(cmo_list *c)
{
    int len=list_length(c);
    int i=0;
    printf("{");
    for(i=0; i<len; i++) {
        explain_cmo(list_nth(c, i));
        printf(", ");
    }
    printf("}");
}

void explain_cmo_int32(cmo_int32 *c)
{
    printf("%d", c->i);
}

void explain_cmo_string(cmo_string *c)
{
    printf("%s", c->s);
}

void explain_cmo_zz(cmo_zz *c)
{
    printf("%s", new_string_set_cmo((cmo *)c));
}

void explain_cmo(cmo *c)
{
    switch(c->tag) {
    case CMO_LIST:
        explain_cmo_list((cmo_list *)c);
        break;
    case CMO_INT32:
        explain_cmo_int32((cmo_int32 *)c);
        break;
    case CMO_STRING:
        explain_cmo_string((cmo_string *)c);
        break;
    case CMO_ZZ:
        explain_cmo_zz((cmo_zz *)c);
        break;
    default:
        printf("cmo");
    }
}

int main()
{
    OXFILE *s;
    cmo_list *c;
    cmo *d;

	ox_stderr_init(stderr);
    /* starting an OpenXM server */
    s = ox_start("localhost", "ox", "ox_sm1"); 

    /* making a list. */
    c = list_appendl(new_cmo_list(),
                     (cmo *)new_cmo_int32(10000), 
                     (cmo *)new_cmo_string("Hello"),
                     (cmo *)new_cmo_zz_set_string("3141592653289793238462643383279"),
                     NULL);

    ox_push_cmo(s, (cmo *)c);
    ox_push_cmd(s, SM_popCMO);

    /* The following equals to ox_get(s) */
    receive_ox_tag(s);
    d = receive_cmo(s);
    explain_cmo(d);
    printf("\n");

    ox_close(s);
    return 0;
}
