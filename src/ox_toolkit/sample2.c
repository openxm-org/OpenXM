/* -*- mode: C -*- */
/* $OpenXM: OpenXM/src/ox_toolkit/sample2.c,v 1.1 1999/12/16 06:55:07 ohara Exp $ */

/*
  This program explains how to get
  int, char *, and so on from the received data
  by using low-level functions of OpenXM C library.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "ox_toolkit.h"

int explain_cmo(cmo *);

int explain_cmo_list(cmo_list *c)
{
	int len=length_cmo_list(c);
	int i=0;
	printf("{");
	for(i=0; i<len; i++) {
		explain_cmo(nth_cmo_list(c, i));
		printf(", ");
	}
	printf("}");
}

int explain_cmo_int32(cmo_int32 *c)
{
	printf("%d", c->i);
}

int explain_cmo_string(cmo_string *c)
{
	printf("%s", c->s);
}

int explain_cmo_zz(cmo_zz *c)
{
	printf("%s", new_string_set_cmo((cmo *)c));
}

int explain_cmo(cmo *c)
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
	ox_file_t s;
	cmo_list *c;
	cmo *d;
	int len;
	int i;

	/* starting an OpenXM server */
    s = ox_start("localhost", "ox", "ox_sm1"); 

	/* making a list. */
	c = new_cmo_list();
	append_cmo_list(c, (cmo *)new_cmo_int32(10000));
	append_cmo_list(c, (cmo *)new_cmo_string("Hello"));
	append_cmo_list(c, (cmo *)new_cmo_zz_set_string("3141592653289793238462643383279"));

	/* The following equals to ox_push_cmo(s, (cmo *)c)	*/
	send_ox_tag(s->stream, OX_DATA);
	send_cmo(s->stream, (cmo *)c);

	/* The following equals to ox_push_cmd(s, SM_popCMO) */
	send_ox_tag(s->stream, OX_COMMAND);
	send_int32(s->stream, SM_popCMO);

	/* The following equals to ox_get(s) */
	receive_ox_tag(s->stream);
	d = receive_cmo(s->stream);
	explain_cmo(d);
	printf("\n");

    ox_close(s);
    return 0;
}
