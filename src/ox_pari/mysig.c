/* $OpenXM: OpenXM/src/ox_pari/mysig.c,v 1.1 2018/06/04 06:28:05 ohara Exp $
 */
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <setjmp.h>
#include <string.h>
#include "mysig.h"

static void *Old_handler;
/*
  sigset SIGINT, SIGCHLD, SIGUSR1
 */
int unblock_signal(int sigset[]) {
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
  sigset_t set;
  sigset_t oldset;
  int i;
  sigemptyset(&set);
  for (i=0; sigset[i] > 0; i++) sigaddset(&set,sigset[i]);
  sigprocmask(SIG_BLOCK,&set,&oldset);
  return(0);
}

#if !defined(SA_RESTART) && defined(__FreeBSD__) 
#define SA_RESTART  0x0002
#endif

int set_signal(int sig,void (*handler)(int m)) {
  struct sigaction act;
  struct sigaction oldact;
  int val;
  act.sa_handler=handler;
  act.sa_flags=0;
  act.sa_flags |= SA_RESTART;
  sigemptyset(&act.sa_mask);
  val=sigaction(sig,&act,&oldact);
  Old_handler = oldact.sa_handler;
  return(val);
}
/* 
  my own emulation of signal for portability.
 */
void *mysignal(int sig,void (*handler)(int m)) {
  int sigset[2];
  if (handler == SIG_IGN) {
    /*    sigset[0] = sig; sigset[1]=0;
    block_signal(sigset);
    return(SIG_IGN);*/
    return(signal(sig,handler));
  }else if (handler == SIG_DFL) {
    return(signal(sig,handler));
  }
  /* on unix system, you may simply call signal(3) here.  */
  set_signal(sig,handler);
  /* unblock is necessary on cygwin, ..., cf. misc-2015/12/misc */
  sigset[0] = sig; sigset[1]=0; unblock_signal(sigset);
  return((void *)Old_handler);
}

