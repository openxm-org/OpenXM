/* -*- mode: C; coding: euc-japan -*- */
/* $OpenXM: OpenXM/src/ox_math/math2ox.c,v 1.23 2005/07/20 17:48:03 ohara Exp $ */

/* 
   Copyright (C) Katsuyoshi OHARA, 2000.
   Portions copyright 1999 Wolfram Research, Inc. 

   You must see OpenXM/Copyright/Copyright.generic.
   The MathLink Library is licensed from Wolfram Research Inc..
   See OpenXM/Copyright/Copyright.mathlink for detail.
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <mathlink.h>
#include <unistd.h>
#include <signal.h>
#include <mathlink.h>
#include <ox_toolkit.h>

static char *host    = "localhost";
static char *ctlserv = "ox";
static char *oxprog  = "ox_sm1";

static OXFILE **ss = NULL;
static int len_ss = 0;
static int max_process = 0;

/* The following functions are called from Mathematica.
   See math2.tm for detail. */
void OX_get(int id)
{
    cmo *c = NULL;

    receive_ox_tag(ss[id]);
    c = receive_cmo(ss[id]);
    ox_printf("ox message is received in OxGet[].\n");
    print_cmo(c);
    send_mlo(c);
	ml_flush();
}

int OX_execute_string(int id, const char *str)
{
    ox_execute_string(ss[id], str);
    return 0;
}

char *OX_popString(int id)
{
    return ox_popString(ss[id]);
}

void OX_popCMO(int id)
{
    cmo *c = ox_pop_cmo(ss[id]);
    send_mlo(c);
	ml_flush();
}

int OX_close(int id)
{
    ox_close(ss[id]);
    return 0;
}

int OX_reset(int id)
{
    ox_reset(ss[id]);
    return 0;
}

/* This function is compatible with previous version of math2ox. */
/* Parsing s and sending its cmo to an OX server. */
int OX_parse(int id, char *s)
{
	return OX_sendMessage(id, s);
}

/* After creating an OX message by parsing a string s, 
   the function send the message to the OX server id. */
int OX_sendMessage(int id, char *s)
{
    cmo *m = ox_parse_lisp(s);
    if(m != NULL) {
        send_ox(ss[id], m);
        return 0;
    }
    return -1; /* if we failed. */
}

int OX_start(char* s)
{
    if (s != NULL && s[0] != '\0') {
        oxprog = s;
    }
	if (++max_process < len_ss) {
		ss[max_process] = ox_start(host, ctlserv, oxprog);
		ox_printf("open (%s)\n", "localhost");
		return max_process;
	}
	return -1;
}

int OX_start_remote_ssh(char *s, char *host)
{
    if (s != NULL && s[0] != '\0') {
        oxprog = s;
    }
    if (host != NULL || host[0] == '\0') {
        host = "localhost";
    }
	if (++max_process < len_ss) {
		ss[max_process] = ox_start_remote_with_ssh(oxprog, host);
		ox_printf("open (%s)\n", host);
		return max_process;
	}
    return -1;
}

int OX_start_insecure(char *host, int portCtl, int portDat)
{
    if (host != NULL || host[0] == '\0') {
        host = "localhost";
    }
    if (portCtl == 0) {
        portCtl = 1200;
    }
    if (portDat == 0) {
        portDat = 1300;
    }
    
	if (++max_process < len_ss) {
		ss[max_process] = ox_start_insecure(host, portCtl, portDat);
		ox_printf("math2ox :: connect to \"%s\" with (ctl, dat) = (%d, %d)\n", host, portCtl, portDat);
		return max_process;
	}

    return -1;
}

static char *cp_str(char *src)
{
    char *dest = malloc(strlen(src)+1);
    strcpy(dest, src);
    return dest;
}

int OX_setClientParam(char *h, char* c, char* p)
{
    host    = cp_str(h);
    ctlserv = cp_str(c);
    oxprog  = cp_str(p);
    return 0;
}

static OXFILE **new_sstack(int size)
{
	max_process = 0;
	len_ss = size;
	return (OXFILE **)malloc(sizeof(OXFILE *)*len_ss);
}

static OXFILE *ss_id(int id)
{
	return ss[id];
}

static int  ss_id_stream(int id)
{
	return ss[id];
}

#define VERSION "2000/11/29"

int main(int argc, char *argv[])
{
    ox_stderr_init(NULL);

    ss = new_sstack(20);
    mathcap_init(20001129, VERSION, "math2ox", NULL, NULL);
    
    MLMain(argc, argv);
}
