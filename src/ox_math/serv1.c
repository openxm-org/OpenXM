/* -*- mode: C; coding: euc-japan -*- */
/* $OpenXM: OpenXM/src/ox_math/serv1.c,v 1.9 2000/03/10 12:38:47 ohara Exp $ */

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
#include "serv2.h"

static int send_ox_sync_ball();

static OXFILE *sv;

static int flag_sigusr1 = 0;
static int flag_sigusr2 = 0;

/* if in_critical equals to 1 then we do not permit an interrupt. */
static int in_critical = 0; 

static int set_critical()
{
    in_critical = 1;
}

static int unset_critical()
{
    in_critical = 0;
}

static int critical_p() {
    return in_critical;
}

static int already_send_ox_sync_ball = 0;

/* SM_control_reset_connection */
static int handler_reset1()
{
    fprintf(stderr, "signal received.\n");
    signal(SIGUSR1, handler_reset1);
    if (!flag_sigusr1) {
        flag_sigusr1 = 1;
        if(critical_p()) {
            send_ox_sync_ball();
            already_send_ox_sync_ball = 1;
        }
    }
}

static int handler_kill()
{
	oxf_close(sv);
    exit(1);
}

static int send_ox_sync_ball()
{
    fprintf(stderr, "sending a sync_ball.\n");
    send_ox_tag(sv, OX_SYNC_BALL);
}

static int exchange_ox_syncball()
{
    int tag;

    while((tag = receive_ox_tag(sv)) != OX_SYNC_BALL) {
        /* skipping a message. */
        if (tag == OX_DATA) {
            receive_cmo(sv);
        }else {
            receive_int32(sv);
        }
    }
    fprintf(stderr, "received a sync_ball.\n");
}

/* a part of stack machine. */
int receive_ox(OXFILE *oxfp)
{
    int tag;
    int code;

    tag = receive_ox_tag(oxfp);
    switch(tag) {
    case OX_DATA:
        push(receive_cmo(oxfp));
        break;
    case OX_COMMAND:
        code = receive_sm_command(oxfp);
        set_critical();
        execute_sm_command(oxfp, code);
        unset_critical();
        break;
    default:
        fprintf(stderr, "illeagal message? ox_tag = (%d)\n", tag);
        return -1;
        break;
    }
    return 0;
}

int shutdown()
{
    oxf_close(sv);
    ml_exit();
    exit(0);
}

int main()
{
	sv = oxf_open(3);

    ml_init();
    initialize_stack();

    signal(SIGUSR1, handler_reset1);
    signal(SIGKILL, handler_kill);

	oxf_determine_byteorder_server(sv);

    while(1) {
        receive_ox(sv);
        if(flag_sigusr1) {
            if (!already_send_ox_sync_ball) {
              send_ox_sync_ball();
                already_send_ox_sync_ball = 1;
            }
            exchange_ox_syncball();
            flag_sigusr1 = 0;
            already_send_ox_sync_ball = 0;
        }
    }
    shutdown();
}
