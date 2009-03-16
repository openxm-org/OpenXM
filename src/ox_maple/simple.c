/* -*- mode: C -*- */
/* $OpenXM: OpenXM/src/ox_maple/simple.c,v 1.1 2004/06/21 12:46:04 takayama Exp $ */

/* A sample implementation for maple 
   of an OpenXM client with OpenXM C library */

#include <stdio.h>

#include "ox_toolkit.h"

extern OXFILE *ox_start(char* host, char* prog1, char* prog2);
OXFILE *sv = NULL;

int ml_init() {
  static int p = 1;
  if (p) {
	GC_init();
    ox_stderr_init(stderr);
    p = 0;
  }
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

int ml_start_maple()
{
    char *server = "ox_maple";

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
  ox_push_cmo(sv,(cmo *)new_cmo_int32(i));
  return i;
}

int ml_execute_string(char *s) {
  if (sv == NULL) ml_start_asir();
  ox_execute_string(sv,s);
  return 0;
}

char *ml_pop_string() {
  if (sv == NULL) ml_start_asir();
  return ox_popString(sv);
  /* Is it protected from GC? */
}

