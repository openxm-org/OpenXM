/* $OpenXM: OpenXM/src/kxx/sample_kanlib.c,v 1.5 2016/03/31 05:27:34 takayama Exp $ */
/*
This is a sample program to use kanlib.a
gcc -g -O2 -g -D_BSD_SOURCE sample_kanlib.c -o sample_kanlib  -ldl   ../kan96xx/Kan/kanlib.a -L../../lib -lgmp -lgc
*/

#include <stdio.h>
#include <setjmp.h>
#include <signal.h>
#include "mysig.h"

// serversm.h is not included.
#if defined(__CYGWIN__) || defined(__MSYS__)
#define JMP_BUF sigjmp_buf
#define SETJMP(env)  _setjmp(env)
#define LONGJMP(env,p)  _longjmp(env,p)
#else
#define JMP_BUF jmp_buf
#define SETJMP(env)  setjmp(env)
#define LONGJMP(env,p)  longjmp(env,p)
#endif


extern int Quiet;
extern JMP_BUF EnvOfStackMachine;
extern Calling_ctrlC_hook;
extern int RestrictedMode, RestrictedMode_saved;

void ctrlC();

main() {
  char s[1024];
  int r;
  Quiet = 0;
  KSstart();  

  if (mysignal(SIGINT,SIG_IGN) != SIG_IGN) {
	mysignal(SIGINT,ctrlC);
  }
  while( fgets(s,1023,stdin) != NULL) {
	if (SETJMP(EnvOfStackMachine)) {
      if (!Calling_ctrlC_hook) {
        Calling_ctrlC_hook = 1; RestrictedMode = 0; 
        KSexecuteString(" ctrlC-hook "); /* Execute User Defined functions. */ 
        RestrictedMode = RestrictedMode_saved;
      }
      Calling_ctrlC_hook = 0;
      KSexecuteString(" (Computation is interrupted.) ");
	  continue;
	} else {  }
	r=KSexecuteString(s);
	fprintf(stderr,"result code=%d\n",r);
  }
}
