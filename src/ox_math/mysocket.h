/* -*- mode: C; coding: euc-japan -*- */
/* $OpenXM: OpenXM/src/ox_math/mysocket.h,v 1.2 1999/11/02 06:11:57 ohara Exp $ */

#ifndef _MYSOCKET_H_

#define _MYSOCKET_H_

int mysocketListen(char *hostname, short *portp);
int mysocketOpen(char* hostname, short port);
int mypipe(char *program, int fd1, int fd2);

#endif  /* _MYSOCKET_H_ */
