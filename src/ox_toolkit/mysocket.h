/* -*- mode: C; coding: euc-japan -*- */
/* $OpenXM$ */

#ifndef _MYSOCKET_H_

#define _MYSOCKET_H_

int mysocketListen(char *hostname, short *portp);
int mysocketOpen(char* hostname, short port);
#if 0
int mypipe(char *program, int fd1, int fd2);
#endif

#endif  /* _MYSOCKET_H_ */
