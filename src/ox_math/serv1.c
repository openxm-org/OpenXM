/* -*- mode: C; coding: euc-japan -*- */
/* $OpenXM: OpenXM/src/ox_math/serv1.c,v 1.13 2000/12/03 21:46:52 ohara Exp $ */

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
static int in_critical = 0; 
static int already_send_ox_sync_ball = 0;
static int flag_sigusr1 = 0;

static int send_ox_sync_ball();

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
    oxf_close(stack_oxfp);
    exit(1);
}

static int send_ox_sync_ball()
{
    fprintf(stderr, "sending a sync_ball.\n");
    send_ox_tag(stack_oxfp, OX_SYNC_BALL);
}

static int exchange_ox_sync_ball()
{
    int tag;

    while((tag = receive_ox_tag(stack_oxfp)) != OX_SYNC_BALL) {
        /* skipping a message. */
        if (tag == OX_DATA) {
            receive_cmo(stack_oxfp);
        }else {
            receive_int32(stack_oxfp);
        }
    }
    fprintf(stderr, "received a sync_ball.\n");
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

    signal(SIGUSR1, handler_reset1);
    signal(SIGKILL, handler_kill);

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
        set_critical();
        sm_run(code);
        unset_critical();
        break;
    default:
        fprintf(stderr, "illeagal message? ox_tag = (%d)\n", tag);
        return 0;
        break;
    }
    return 1;
}

int sm(OXFILE *oxfp)
{
    stack_oxfp = oxfp;
    stack_extend();
    while(sm_receive_ox()) {
        if(flag_sigusr1) {
            if (!already_send_ox_sync_ball) {
              send_ox_sync_ball();
                already_send_ox_sync_ball = 1;
            }
            exchange_ox_sync_ball();
            flag_sigusr1 = 0;
            already_send_ox_sync_ball = 0;
        }
    }
    fprintf(stderr, "SM: socket(%d) is closed.\n", stack_oxfp->fd);
}
