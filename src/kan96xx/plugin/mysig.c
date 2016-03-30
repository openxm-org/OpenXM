/* $OpenXM$ */
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <setjmp.h>
#include <string.h>
#include "mysig.h"

int unblock_sigchild_sigint(void) {
  struct sigaction act;
  sigset_t set;
  sigset_t oldset;
  sigemptyset(&set);
  sigaddset(&set,SIGINT);
  sigaddset(&set,SIGCHLD);
  sigprocmask(SIG_UNBLOCK,&set,&oldset);
  return(0);
}

int set_sigchild(void (*handler)(void)) {
  struct sigaction act;
  struct sigaction oldact;
  act.sa_handler=handler;
  act.sa_flags=0;
  act.sa_flags |= SA_RESTART;
  sigemptyset(&act.sa_mask);
  return(sigaction(SIGCHLD,&act,&oldact));
}

  
  

