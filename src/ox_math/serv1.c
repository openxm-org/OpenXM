/* -*- mode: C; coding: euc-japan -*- */
/* $OpenXM: OpenXM/src/ox_math/serv1.c,v 1.14 2000/12/03 22:09:34 ohara Exp $ */

/* 
   Copyright (C) Katsuyoshi OHARA, 2000.
   Portions copyright 1999 Wolfram Research, Inc. 

   You must see OpenXM/Copyright/Copyright.generic.
   The MathLink Library is licensed from Wolfram Research Inc..
   See OpenXM/Copyright/Copyright.mathlink for detail.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <mathlink.h>
#include <ox_toolkit.h>
#include "sm.h"

extern OXFILE *stack_oxfp;

/* if in_critical equals to 1 then we do not permit an interrupt. */
static int flag_sigusr1 = 0;

/* SM_control_reset_connection */
static void handler()
{
    fprintf(stderr, "signal received.\n");
    signal(SIGUSR1, handler);
    flag_sigusr1++;
}

static int exchange_ox_sync_ball(OXFILE *oxfp)
{
    int tag;
    send_ox_tag(oxfp, OX_SYNC_BALL);
    while((tag = receive_ox_tag(oxfp)) != OX_SYNC_BALL) {
        if (tag == OX_DATA) {
            receive_cmo(oxfp);
        }else if (tag == OX_COMMAND) {
            receive_int32(oxfp);
        }
    }
}

int shutdown()
{
    oxf_close(stack_oxfp);
    ml_exit();
    exit(0);
}

#define VERSION 0x11121400
#define ID_STRING  "2000/11/29"

int main()
{
    OXFILE* sv;

    ml_init();
    mathcap_init(VERSION, ID_STRING, "ox_math", NULL, NULL);

    signal(SIGUSR1, handler);

    sv = oxf_open(3);
    oxf_determine_byteorder_server(sv);
    sm(sv);
    shutdown();
}

/* a part of stack machine. */
int sm_receive_ox()
{
    int tag;
    int code;

    tag = receive_ox_tag(stack_oxfp);
    if (oxf_error(stack_oxfp)) {
        return 0;
    }
    switch(tag) {
    case OX_DATA:
        push(receive_cmo(stack_oxfp));
        break;
    case OX_COMMAND:
        code = receive_sm_command(stack_oxfp);
        sm_run(code);
        break;
    default:
        fprintf(stderr, "illeagal message? ox_tag = (%d)\n", tag);
        break;
    }
    return 1;
}

int sm(OXFILE *oxfp)
{
	int mask;
    stack_oxfp = oxfp;
    stack_extend();

    mask = sigsetmask(sigmask(SIGUSR1));
    while(sm_receive_ox()) {
        sigsetmask(mask);
        usleep(1);
        sigsetmask(sigmask(SIGUSR1));
        if(flag_sigusr1) {
            exchange_ox_sync_ball(oxfp);
            flag_sigusr1 = 0;
        }
    }
    fprintf(stderr, "SM: socket(%d) is closed.\n", stack_oxfp->fd);
}
