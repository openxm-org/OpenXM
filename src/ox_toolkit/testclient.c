/* -*- mode: C -*- */
/* $OpenXM: OpenXM/src/ox_toolkit/testclient.c,v 1.1 1999/12/15 05:21:25 ohara Exp $ */

/* A sample implementation of an OpenXM client with OpenXM C library */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include "ox.h"

ox_file_t sv;

int dumpx(int fd, int n)
{
    unsigned char buff[2048]; 
    int i;
    int len = read(fd, buff, n);

    fprintf(stderr, "I have read %d byte from socket.\n", len);
    for(i = 0; i < len; i++) {
        fprintf(stderr, "%02x ", buff[i]);
        if (i%20 == 19) {
            fprintf(stderr, "\n");
        }
    }
    fprintf(stderr, "\n");
    return len;
}

#define VERSION 0x11121500
#define ID_STRING  "testclient version 0.11121500"

int prompt()
{
    printf("> ");
}

int test_0()
{
    cmo* c = NULL;
#ifdef DEBUG
    fprintf(stderr, "testclient:: calling ox_mathcap().\n");
    c = ox_mathcap(sv);
    fprintf(stderr, "testclient:: cmo received.(%p)\n", c);
#else
    c = ox_mathcap(sv);
#endif
    print_cmo(c);
    fflush(stderr);
    send_ox_cmo(sv->stream, make_mathcap_object(VERSION, ID_STRING));

    ox_reset(sv);
    send_ox_cmo(sv->stream, new_cmo_string("N[ArcTan[1]]"));
    send_ox_command(sv->stream, SM_executeStringByLocalParser);
    send_ox_command(sv->stream, SM_popCMO);
    receive_ox_tag(sv->stream);
    c = receive_cmo(sv->stream);
    fprintf(stderr, "testclient:: cmo received.\n");
    print_cmo(c);
}

int test_1()
{
    cmo *c = NULL;
    cmo *m = make_mathcap_object(1000, "test!");
    fprintf(stderr, "testclient:: test cmo_mathcap.\n");
    send_ox_cmo(sv->stream, m);
    send_ox_command(sv->stream, SM_popCMO);
    receive_ox_tag(sv->stream);
    c = receive_cmo(sv->stream);
    fprintf(stderr, "testclient:: cmo received.(%p)\n", c);
    print_cmo(c);
    fputc('\n', stderr);
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
    char *server = "ox_sm1";

    setbuf(stderr, NULL);

    if (argc>1) {
        server = argv[1];
    }
    fprintf(stderr, "testclient:: I use %s as an OX server.\n", server);
    sv = ox_start("localhost", "ox", server); 
    if (sv == NULL) {
        fprintf(stderr, "testclient:: I cannot connect to servers.\n");
        exit(1);
    }

    if (strcmp(server, "ox_math")==0) {
        test_1();
    }

    while(prompt(), (m = parse()) != NULL) {
        send_ox(sv->stream, m);
        if (m->tag == OX_COMMAND) {
            code = ((ox_command *)m)->command;
            if (code >= 1024) {
                break;
            }else if (code == SM_popCMO || code == SM_popString) {
                receive_ox_tag(sv->stream);
                c = receive_cmo(sv->stream);
                fprintf(stderr, "testclient:: cmo received.\n");
                print_cmo(c);
            }
        }
    }

    ox_reset(sv);
    fprintf(stderr, "The testclient resets.\n");
    ox_close(sv);
    fprintf(stderr, "The testclient halts.\n");

    return 0;
}
