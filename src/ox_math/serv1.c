/* -*- mode: C; coding: euc-japan -*- */
/* $OpenXM: OpenXM/src/ox_math/serv1.c,v 1.22 2003/02/13 01:29:38 ohara Exp $ */

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

static int exchange_ox_sync_ball(OXFILE *oxfp)
{
    int tag;
	ox_printf("exchanging OX_SYNC_BALL\n");
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

/* (Heisei)15/02/01 */
#define VERSION  "2003/02/01"

int main()
{
    OXFILE* sv;

    ox_stderr_init(stderr);
    ml_init();
    mathcap_init(20030201, VERSION, "ox_math", NULL, NULL);

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
        ox_printf("illeagal OX message(%d)\n", tag);
        break;
    }
    return 1;
}

int sm(OXFILE *oxfp)
{
    int i=0;
    fd_set fdmask;
    stack_oxfp = oxfp;
    stack_extend();
    sm_siginit();

    FD_ZERO(&fdmask);
    FD_SET(oxf_fileno(oxfp), &fdmask);

    for (i=0; ; i++) {
        sm_sigunmask();
        ox_printf("phase%d: select\n",i);
        if (select(5, &fdmask, NULL, NULL, NULL) > 0) {
            sm_sigmask();
            ox_printf("phase%d: receiving\n",i);
            sm_receive_ox();
        }
        sm_sigmask();
        ox_printf("phase%d: clearing(%d)\n",i,sm_state_interrupting());
        if (sm_state_interrupting()) {
            exchange_ox_sync_ball(stack_oxfp);
            sm_state_clear_interrupting();
        }
    }
    ox_printf("ox_math::socket(%d) is closed.\n", stack_oxfp->fd);
}
