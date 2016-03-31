/* $OpenXM: OpenXM/src/kan96xx/plugin/mysig.h,v 1.3 2016/03/30 21:34:03 takayama Exp $ */
int unblock_signal(int sigset[]);
int block_signal(int sigset[]);
int set_signal(int sig,void (*handler)(int m));
void *mysignal(int sig,void (*handler)(int m));


