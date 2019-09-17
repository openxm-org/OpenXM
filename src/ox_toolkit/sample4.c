/* $OpenXM$
 */

/*  Example:
   OpenXM_HOME etc should be properly set by OpenXM/rc/dot.bashrc
   python2
import ctypes
lib = ctypes.cdll.LoadLibrary('./sample4.so');
lib.eval_string_by_ox_asir.argtypes = (ctypes.c_char_p,)
lib.eval_string_by_ox_asir.restype = ctypes.c_char_p
lib.open_ox_asir()
s = lib.eval_string_by_ox_asir('fctr(x^10-1)')
print(s)
s = lib.eval_string_by_ox_asir('print_tex_form(poly_factor((x^10-1))')
print(s)
lib.close_ox_asir()
 */

/*
  gcc -shared -fPIC -o sample4.so sample4.c -I${OpenXM_HOME}/include -L${OpenXM_HOME}/lib -lox -lgmp -lmpfr -lgc
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

int open_ox_asir()
{
  char *server = "ox_asir";
  sv = ox_start("localhost", "ox", server);  
  if (sv == NULL) {
    ox_printf("testclient:: I cannot connect to servers.\n");
    exit(1);
  }
  printf("Input size is limited to %d bytes.\n",SIZE_CMDLINE-1);
  ox_stderr_init(stderr);
  return 0;
}
char *eval_string_by_ox_asir(char *cmdline) 
{
  ox* m = NULL;
  cmo* c = NULL;
  char cmd1[SIZE_CMDLINE+1024];
  int code;
    
  if (strlen(cmdline)>SIZE_CMDLINE-1) return "Error: too long command. Increase SIZE_CMDLINE";
  strcpy(cmd1,"(OX_DATA,(CMO_STRING,\"if(1){");
  strcat(cmd1,cmdline);
  strcat(cmd1,";}else{};\"))\n");
  m = ox_parse_lisp(cmd1);
  // if (m == NULL) ; //  error, not implemented
  send_ox(sv, m);
  m = ox_parse_lisp("(OX_COMMAND,(SM_executeStringByLocalParser))\n");
  send_ox(sv, m);
  m = ox_parse_lisp("(OX_COMMAND,(SM_popString))\n");
  send_ox(sv, m);
  code = ((ox_command *)m)->command;
  if (code >= 1024) {
    ; // error, not implemented
  }else if (code == SM_popCMO || code == SM_popString) {
    receive_ox_tag(sv);
    c = receive_cmo(sv);
    ox_printf("testclient:: cmo received.\n");
    //print_cmo(c);
    if (c->tag == CMO_STRING)  return ((cmo_string *)c)->s;
    else ; // error
  }
  return NULL;
}

int close_ox_asir() {
  ox_reset(sv);
  ox_printf("The testclient resets.\n");
  ox_close(sv);
  ox_printf("The testclient halts.\n");
  return(0);
}

