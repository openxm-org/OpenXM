/* $OpenXM: OpenXM/src/kan96xx/plugin/mysig.h,v 1.1 2016/03/30 08:25:43 takayama Exp $ */
int unblock_signal(int sigset[]);
int block_signal(int sigset[]);
int set_signal(int sig,void (*handler)(int m));


