/* -*- mode: C -*- */
/* $OpenXM$ */

/* Example:
   maple
   read `simple2.ml`;
   ox_start_asir();
   ox_execute_string("fctr(x^1000-y^1000");");
   ox_pop_string();   
   ctrl-C      ===> ox_asir is restarted.
   ox_execute_string("1+2;");
   ox_pop_string();
 */

/* A sample implementation for oxserver invoked from maple 

   NOTE:  
      Maple has the structure 
         try --- catch  --- finally ---
      or
          traperror(---);
      However, they cannot catch the interruption.
      Under this restriction, any implementation of ox_reset by ctrl-C
      will become difficult.
     [ I've not yet tested signal() in simple2.c. If it works, then there 
       is a hope. ]

      This sample implementation gives a partial solution for the problem;
      we restart ox servers after ctrl-C.
 */

#include <stdio.h>
#include <sys/socket.h>
#include <sys/time.h>

#include "ox_toolkit.h"

extern OXFILE *ox_start(char* host, char* prog1, char* prog2);
OXFILE *sv = NULL;
int In_ox = 0;

#define ox_restart() if (sv != NULL && In_ox) { ox_shutdown(sv); In_ox=0; sv=NULL; ml_start_asir();}


int oxselect(OXFILE *serv) {
  fd_set readfds;
  struct timeval timeout;
  int fd;
  fd = serv->fd;
  FD_ZERO(&readfds);
  FD_SET(fd,&readfds);
  timeout.tv_sec = 0;
  timeout.tv_usec = (long) 1;
  if (select(fd+1,&readfds,(fd_set *)NULL,(fd_set *)NULL,&timeout)<0) {
	fprintf(stderr,"Select error.\n");
	return (-1);
  }
  /* Maple seems to accept ctrl-C in select (and in sleep), 
     but in the other parts,
     it does not accept ctrl-C out of select.
  */
  if (FD_ISSET(fd,&readfds)) return 1;
  else return 0;
}

int ml_init() {
  static int p = 1;
  In_ox = 1;
  if (p) {
	GC_init();
    ox_stderr_init(stderr);
    p = 0;
  }
  In_ox = 0;
}

int ml_start_asir()
{
    char *server = "ox_asir";
	ml_init();

    sv = ox_start("localhost", "ox", server);  
    if (sv == NULL) {
        ox_printf("simple:: I cannot connect to servers.\n");
        return -1;
    }
	return 0;
}

int ml_push_int(int i) {
  if (sv == NULL) ml_start_asir();
  ox_restart();
  In_ox=1;
  ox_push_cmo(sv,(cmo *)new_cmo_int32(i));
  In_ox=0;
  return i;
}

int ml_execute_string(char *s) {
  if (sv == NULL) ml_start_asir();
  ox_restart();
  In_ox=1;
  ox_execute_string(sv,s);
  In_ox=0;
  return 0;
}

int ml_pop_string0() {
  if (sv == NULL) ml_start_asir();
  ox_restart();
  In_ox = 1;
  send_ox_command(sv,SM_popString);
  return 1;
}
char *ml_get_string() {
  char *s;
  receive_ox_tag(sv); /* OX_DATA */
  s = ((cmo_string *) receive_cmo(sv))->s;
  In_ox = 0;
  return s;
}

int ml_select() {
  return oxselect(sv);
}
