/* -*- mode: C; coding: euc-japan -*- */
/* $OpenXM: OpenXM/src/ox_math/serv1.c,v 1.2 1999/11/02 06:11:58 ohara Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <gmp.h>
#include <mathlink.h>
#include "ox.h"
#include "serv2.h"

static int send_ox_sync_ball(int fd);

extern MLINK lp;
static int sv_read  = 3;
static int sv_write = 4;

static int flag_sigusr1 = 0;
static int flag_sigusr2 = 0;

/* 1 のとき割り込み禁止 */
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
            send_ox_sync_ball(sv_write);
            already_send_ox_sync_ball = 1;
        }
    }
}

static int handler_kill()
{
    close(3);
    close(4);
    exit(1);
}

static int send_ox_sync_ball(int fd)
{
    fprintf(stderr, "sending a sync_ball.\n");
    send_ox_tag(fd, OX_SYNC_BALL);
}

static int exchange_ox_syncball(int fd)
{
    int tag;

    while((tag = receive_ox_tag(fd)) != OX_SYNC_BALL) {
        /* skipping a message. */
        if (tag == OX_DATA) {
            receive_cmo(fd);
        }else {
            receive_int32(fd);
        }
    }
    fprintf(stderr, "received a sync_ball.\n");
}

/* スタックマシン部分 */
int receive_ox(int fd_read, int fd_write)
{
    int tag;
    int code;

    tag = receive_ox_tag(fd_read);
    switch(tag) {
    case OX_DATA:
        push(receive_cmo(fd_read));
        break;
    case OX_COMMAND:
        code = receive_sm_command(fd_read);
        set_critical();
        execute_sm_command(fd_write, code);
        unset_critical();
        break;
    default:
        fprintf(stderr, "illeagal message? ox_tag = (%d)\n", tag);
        return -1;
        break;
    }
    return 0;
}

int main()
{
    MATH_init();
    initialize_stack();

    signal(SIGUSR1, handler_reset1);
    signal(SIGKILL, handler_kill);

    /* バイトオーダの決定 */
    decideByteOrderServer(sv_read, 0);

    while(1) {
        receive_ox(sv_read, sv_write);
        if(flag_sigusr1) {
            if (!already_send_ox_sync_ball) {
              send_ox_sync_ball(sv_write);
                already_send_ox_sync_ball = 1;
            }
            exchange_ox_syncball(sv_read);
            flag_sigusr1 = 0;
            already_send_ox_sync_ball = 0;
        }
    }
    MATH_exit();
}
