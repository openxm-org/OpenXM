/* -*- mode: C; coding: euc-japan -*- */
/* $OpenXM: OpenXM/src/ox_toolkit/mysocket.h,v 1.2 2000/10/10 05:23:20 ohara Exp $ */

#ifndef _MYSOCKET_H_

#define _MYSOCKET_H_

int mysocketListen(char *hostname, int *portp);
int mysocketOpen(char* hostname, int port);
int mysocketAccept(int s_waiting);
#if 0
int mypipe(char *program, int fd1, int fd2);
#endif

#endif  /* _MYSOCKET_H_ */
