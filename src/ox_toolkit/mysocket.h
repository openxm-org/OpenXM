/* -*- mode: C; coding: euc-japan -*- */
/* $OpenXM: OpenXM/src/ox_toolkit/mysocket.h,v 1.1 1999/12/09 22:44:56 ohara Exp $ */

#ifndef _MYSOCKET_H_

#define _MYSOCKET_H_

int mysocketListen(char *hostname, short *portp);
int mysocketOpen(char* hostname, short port);
int mysocketAccept(int s_waiting);
#if 0
int mypipe(char *program, int fd1, int fd2);
#endif

#endif  /* _MYSOCKET_H_ */
