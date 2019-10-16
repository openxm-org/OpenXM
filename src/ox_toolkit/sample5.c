/* $OpenXM$
 */

/* oxf_select, ox_reset examples. See oxf.c, readme*.txt
  See also OpenXM/include/ox/*.h as for ox stackmachine commands.

 Example:
   OpenXM_HOME etc should be properly set by OpenXM/rc/dot.bashrc
   OX_XTERM_GEOMETRY=80x20+0+0
   python2
import ctypes
lib = ctypes.cdll.LoadLibrary('./sample5.so');
lib.submit_string_to_ox_asir.argtypes = (ctypes.c_char_p,)
lib.submit_string_to_ox_asir.restype = ctypes.c_char_p
lib.pop_string_from_ox_asir.restype = ctypes.c_char_p
lib.myselect.restype = ctypes.c_int
lib.reset_ox_asir.restype = ctypes.c_int
lib.open_ox_asir()
s = lib.submit_string_to_ox_asir('fctr(x^10-1)')
lib.myselect()
lib.pop_string_from_ox_asir()
s = lib.submit_string_to_ox_asir('fctr(x^1000-y^1000)')
lib.myselect()
lib.reset_ox_asir()

s = lib.submit_string_to_ox_asir('fctr(x^10-1)')
lib.myselect()
lib.pop_string_from_ox_asir()

lib.close_ox_asir()
 */

/*
  gcc -shared -fPIC -o sample5.so sample5.c -I${OpenXM_HOME}/include -L${OpenXM_HOME}/lib -lox -lgmp -lmpfr -lgc
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

char *submit_string_to_ox_asir(char *cmdline) 
{
  ox* m = NULL;
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
  return NULL;
}
char *pop_string_from_ox_asir() {
  ox* m = NULL;
  cmo* c = NULL;
  int code;
  m = ox_parse_lisp("(OX_COMMAND,(SM_popString))\n");
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

/*
 It returns non-zero number when the data is ready.
 It return 0 when the data is not ready.
*/
int myselect() {
  fd_set readfds;
  struct timeval timeout;
  FD_ZERO(&readfds);
  FD_SET(sv->fd,&readfds);
  timeout.tv_sec = 0;
  timeout.tv_usec = (long) 1;
  return select((sv->fd)+1,&readfds, NULL,NULL, &timeout);
}

int reset_ox_asir() {
  ox_reset(sv);
  return 0;
}

int close_ox_asir() {
  ox_reset(sv);
  ox_printf("The testclient resets.\n");
  ox_close(sv);
  ox_printf("The testclient halts.\n");
  return(0);
}

