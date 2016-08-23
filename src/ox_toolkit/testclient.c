/* -*- mode: C -*- */
/* $OpenXM: OpenXM/src/ox_toolkit/testclient.c,v 1.16 2005/10/12 04:03:37 takayama Exp $ */

/* A sample implementation of an OpenXM client with OpenXM C library */
/* Sample input
  (OX_DATA, (CMO_STRING,"printf(\"%a\");"))
  (OX_COMMAND,(SM_executeStringByLocalParser))
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#include "ox_toolkit.h"

extern OXFILE *ox_start(char* host, char* prog1, char* prog2);
OXFILE *sv;

int dumpx(OXFILE *oxfp, int n)
{
    unsigned char buff[2048]; 
    int i;
    int len = oxf_read(buff, 1, n, oxfp);

    ox_printf("I have read %d byte from socket(%d).\n", len, oxfp->fd);
    for(i = 0; i < len; i++) {
        ox_printf("%02x ", buff[i]);
        if (i%20 == 19) {
            ox_printf("\n");
        }
    }
    ox_printf("\n");
    return len;
}

#define SIZE_CMDLINE  8192

static int  size = SIZE_CMDLINE;
static char cmdline[SIZE_CMDLINE];

static void prompt()
{
    fprintf(stdout, "> ");
    fgets(cmdline, size, stdin);
}

#define VERSION 0x11121500
#define ID_STRING  "v0.11121500"

void test_0()
{
    cmo* c = NULL;
    ox_printf("testclient:: calling ox_mathcap().\n");
    c = (cmo *)ox_mathcap(sv);
    ox_printf("testclient:: cmo received.(%p)\n", c);
    print_cmo(c);

    mathcap_init(VERSION, ID_STRING, "testclient", NULL, NULL);
    send_ox_cmo(sv, (cmo *)oxf_cmo_mathcap(sv));

    ox_reset(sv);
    send_ox_cmo(sv, (cmo *)new_cmo_string("N[ArcTan[1]]"));
    send_ox_command(sv, SM_executeStringByLocalParser);
    send_ox_command(sv, SM_popCMO);
    receive_ox_tag(sv);
    c = receive_cmo(sv);
    ox_printf("testclient:: cmo received.\n");
    print_cmo(c);
}

void test_1()
{
    cmo *c, *m;

	mathcap_init(1000, "test!", "testclient", NULL, NULL);
	m = (cmo *)oxf_cmo_mathcap(sv);
    ox_printf("testclient:: test cmo_mathcap.\n");
    send_ox_cmo(sv, m);
    send_ox_command(sv, SM_popCMO);
    receive_ox_tag(sv);
    c = receive_cmo(sv);
    ox_printf("testclient:: cmo received.(%p)\n", c);
    print_cmo(c);
	ox_printf("\n");
}

/*  Example:
  testclient
  >(OX_DATA,(CMO_INT32,123))
  >(OX_COMMAND,(SM_popCMO))
 */

int main(int argc, char* argv[])
{
    ox* m = NULL;
    cmo* c = NULL;
    int code;
    char *server = "ox_asir";

    ox_stderr_init(stderr);

    if (argc>1) {
        server = argv[1];
    }
    ox_printf("testclient:: I use %s as an OX server.\n", server);
    sv = ox_start("localhost", "ox", server);  
    if (sv == NULL) {
        ox_printf("testclient:: I cannot connect to servers.\n");
        exit(1);
    }

    if (strcmp(server, "ox_math")==0) {
        test_1();
    }

    while(prompt(), (m = ox_parse_lisp(cmdline)) != NULL) {
        send_ox(sv, m);
        if (m->tag == OX_COMMAND) {
            code = ((ox_command *)m)->command;
            if (code >= 1024) {
                break;
            }else if (code == SM_popCMO || code == SM_popString) {
                receive_ox_tag(sv);
                c = receive_cmo(sv);
                ox_printf("testclient:: cmo received.\n");
                print_cmo(c);
            }
        }
    }

    ox_reset(sv);
    ox_printf("The testclient resets.\n");
    ox_close(sv);
    ox_printf("The testclient halts.\n");

    return 0;
}
