/* $OpenXM$ */
/*
This is a sample program to use kanlib.a
gcc -g -O2 -g -D_BSD_SOURCE sample_kanlib.c -o sample_kanlib  -ldl   ../kan96xx/Kan/kanlib.a -L../../lib -lgmp -lgc
*/

#include <stdio.h>
#include <setjmp.h>
#include <signal.h>

#if defined(__CYGWIN__)
#define JMP_BUF sigjmp_buf
#define SETJMP(env)  sigsetjmp(env,1)
#define LONGJMP(env,p)  siglongjmp(env,p)
#else
#define JMP_BUF jmp_buf
#define SETJMP(env)  setjmp(env)
#define LONGJMP(env,p)  longjmp(env,p)
#endif


extern int Quiet;
extern JMP_BUF EnvOfStackMachine;

void ctrlC();

main() {
  char s[1024];
  Quiet = 0;
  KSstart();  

  if (signal(SIGINT,SIG_IGN) != SIG_IGN) {
	signal(SIGINT,ctrlC);
  }
  while( fgets(s,1023,stdin) != NULL) {
	if (SETJMP(EnvOfStackMachine)) {
	  KSexecuteString(" ctrlC-hook "); /* Execute User Defined functions. */
      KSexecuteString(" (Computation is interrupted.) ");
	  continue;
	} else {  }
	KSexecuteString(s);
  }
}
