/* -*- mode: C -*- */
/* $OpenXM: OpenXM/src/ox_toolkit/bconv.c,v 1.6 2003/02/04 20:43:54 ohara Exp $ */

/* bconv can convert an OX expression or a CMO expression to a byte stream. */
/* Any expressions, as a string, must have shorter length than 8192.*/

#include <stdio.h>
#include <stdlib.h>

#include "ox_toolkit.h"

static void display(ox *m)
{
    int i;
    int len = 0;
    unsigned char* d_buff;

    switch(m->tag) {
    case OX_DATA:
        len = sizeof(int) + sizeof(int) + cmolen_cmo(((ox_data *)m)->cmo);
        d_buff = malloc(len);
        dump_buffer_init(d_buff);
        dump_ox_data((ox_data *)m);
        break;
    case OX_COMMAND:
        len = sizeof(int) + sizeof(int) + sizeof(int);
        d_buff = malloc(len);
        dump_buffer_init(d_buff);
        dump_ox_command((ox_command *)m);
        break;
    default:
        len = cmolen_cmo((cmo *)m);
        d_buff = malloc(len);
        dump_buffer_init(d_buff);
        dump_cmo((cmo *)m);
    }

    for(i=0; i<len; i++) {
        fprintf(stdout, "%02x ", d_buff[i]);
        if(i%20 == 19) {
            fprintf(stdout, "\n");
        }
    }
    if(i%20 != 19) {
        fprintf(stdout, "\n");
    }
    free(d_buff);
}

#define SIZE_CMDLINE  8192

static char cmdline[SIZE_CMDLINE];

static void prompt()
{
    fprintf(stdout, "> ");
    fgets(cmdline, SIZE_CMDLINE, stdin);
}

int main()
{
    cmo *m;
    ox_stderr_init(stderr);
    setbuf(stdout, NULL);

    for(prompt(); (m = ox_parse_lisp(cmdline)) != NULL; prompt()) {
        display(m);
    }
    return 0;
}
