/* $OpenXM$ */

/*  Example:
  sample3
  > fctr(x^10-1)
  > def foo(N) { return (2*N); }
  > foo(10)
  > ctrl-C
*/

/*
 gcc -o sample3 sample3.c -I${OpenXM_HOME}/include -L${OpenXM_HOME}/lib -lox -lgmp -lmpfr -lgc
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

#define SIZE_CMDLINE  8192

static int  size = SIZE_CMDLINE;
static char cmdline[SIZE_CMDLINE];

static void prompt()
{
    fprintf(stdout, "> ");
    fgets(cmdline, size, stdin);
}


int main(int argc, char* argv[])
{
    ox* m = NULL;
    cmo* c = NULL;
    char cmd1[SIZE_CMDLINE+1024];
    int code;
    char *server = "ox_asir";

    ox_stderr_init(stderr);

    sv = ox_start("localhost", "ox", server);  
    if (sv == NULL) {
        ox_printf("testclient:: I cannot connect to servers.\n");
        exit(1);
    }
    printf("Input size is limited to %d bytes.\n",SIZE_CMDLINE-1);
    while(prompt(),1) {
        strcpy(cmd1,"(OX_DATA,(CMO_STRING,\"if(1){");
	strcat(cmd1,cmdline);
	strcat(cmd1,";}else{};\"))\n");
        m = ox_parse_lisp(cmd1);
	if (m == NULL) break;
        send_ox(sv, m);
	m = ox_parse_lisp("(OX_COMMAND,(SM_executeStringByLocalParser))\n");
        send_ox(sv, m);
	m = ox_parse_lisp("(OX_COMMAND,(SM_popString))\n");
        send_ox(sv, m);
        if (m->tag == OX_COMMAND) {
            code = ((ox_command *)m)->command;
            if (code >= 1024) {
                break;
            }else if (code == SM_popCMO || code == SM_popString) {
                receive_ox_tag(sv);
                c = receive_cmo(sv);
                ox_printf("testclient:: cmo received.\n");
                //print_cmo(c);
		if (c->tag == CMO_STRING)  printf("%s",((cmo_string *)c)->s);
		else ; // error
            }
        }
    }

    ox_reset(sv);
    ox_printf("The testclient resets.\n");
    ox_close(sv);
    ox_printf("The testclient halts.\n");

    return 0;
}

