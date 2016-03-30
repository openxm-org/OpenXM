/* $OpenXM: OpenXM/src/kan96xx/plugin/mysig.h,v 1.2 2016/03/30 09:20:40 takayama Exp $ */
int unblock_signal(int sigset[]);
int block_signal(int sigset[]);
int set_signal(int sig,void (*handler)(int m));
int mysignal(int sig,void (*handler)(int m));


