/* -*- mode: C -*- */
/* $OpenXM: OpenXM/src/ox_toolkit/bconv.c,v 1.1 1999/12/15 05:21:25 ohara Exp $ */

/* bconv can convert an OX expression or a CMO expression to a byte stream. */
/* Any expressions, as a string, must have shorter length than 8192.*/

#include <stdio.h>
#include <stdlib.h>
#include "ox.h"
#include "parse.h"

static int display(ox *m)
{
    int i;
    int len = 0;
    unsigned char* d_buff;

    switch(m->tag) {
    case OX_DATA:
        len = sizeof(int) + sizeof(int) + cmolen_cmo(((ox_data *)m)->cmo);
        d_buff = malloc(len);
        init_dump_buffer(d_buff);
        dump_ox_data((ox_data *)m);
        break;
    case OX_COMMAND:
        len = sizeof(int) + sizeof(int) + sizeof(int);
        d_buff = malloc(len);
        init_dump_buffer(d_buff);
        dump_ox_command((ox_command *)m);
        break;
    default:
        len = cmolen_cmo((cmo *)m);
        d_buff = malloc(len);
        init_dump_buffer(d_buff);
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

static int prompt()
{
    fprintf(stdout, "> ");
    fgets(cmdline, SIZE_CMDLINE, stdin);
    init_parser(cmdline);
}

int main()
{
    cmo *m;
    setbuf(stderr, NULL);
    setbuf(stdout, NULL);

    setflag_parse(PFLAG_ADDREV);
    for(prompt(); (m = parse()) != NULL; prompt()) {
        display(m);
    }
    return 0;
}
