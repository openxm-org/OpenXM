/* $OpenXM: OpenXM/src/kan96xx/plugin/mysig.c,v 1.1 2016/03/30 08:25:43 takayama Exp $ */
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <setjmp.h>
#include <string.h>
#include "mysig.h"

/*
  sigset SIGINT, SIGCHLD, SIGUSR1
 */
int unblock_signal(int sigset[]) {
  struct sigaction act;
  sigset_t set;
  sigset_t oldset;
  int i;
  sigemptyset(&set);
  for (i=0; sigset[i] > 0; i++) sigaddset(&set,sigset[i]);
  sigprocmask(SIG_UNBLOCK,&set,&oldset);
  return(0);
}
/* Add (or) blocked signals, see sigprocmask */
int block_signal(int sigset[]) {
  struct sigaction act;
  sigset_t set;
  sigset_t oldset;
  int i;
  sigemptyset(&set);
  for (i=0; sigset[i] > 0; i++) sigaddset(&set,sigset[i]);
  sigprocmask(SIG_BLOCK,&set,&oldset);
  return(0);
}

int set_signal(int sig,void (*handler)(int m)) {
  struct sigaction act;
  struct sigaction oldact;
  act.sa_handler=handler;
  act.sa_flags=0;
  act.sa_flags |= SA_RESTART;
  sigemptyset(&act.sa_mask);
  return(sigaction(sig,&act,&oldact));
}

  
  

