/* $OpenXM$ */

#include <stdio.h>
#include <setjmp.h>
#include <signal.h>
#include "ox_kan.h"
#include "serversm.h"

#if defined(__CYGWIN__)
#define JMP_BUF sigjmp_buf
#define SETJMP(env)  sigsetjmp(env,1)
#define LONGJMP(env,p)  siglongjmp(env,p)
#else
#define JMP_BUF jmp_buf
#define SETJMP(env)  setjmp(env)
#define LONGJMP(env,p)  longjmp(env,p)
#endif

#define DATA_BEGIN_V  "<S type=verbatim>"     /* "\0x2verbatim:" */
#define DATA_BEGIN_L  "<S type=latex>"        /* "\0x2latex:" */
#define DATA_BEGIN_P  "<S type=prompt>"        /* "\0x2prompt:" */
#define DATA_END      "</S>"    /* "\0x5" */

extern int Quiet;
extern JMP_BUF EnvOfStackMachine;
int Format=0;

void ctrlC();
struct object KpoString(char *s);
char *KSpopString(void);

static char *readString(FILE *fp);
static void printv(char *s);
static void printl(char *s);
static void printp(char *s);

main() {
  char *s;
  char *r;
  char *sys;
  struct object ob;

  /* Set consts */
  sys = "asir> ";
  Quiet = 1;

  /* Initialize kanlib (gc is also initialized) */
  KSstart();

  /* Load ox engine here */
  /* engine id should be set to ox.engine */
  KSexecuteString(" [(parse) (ox.sm1) pushfile] extension ");
  KSexecuteString(" asirconnectr /ox.engine oxasir.ccc def ");

  if (signal(SIGINT,SIG_IGN) != SIG_IGN) {
	signal(SIGINT,ctrlC);
  }
  /* Main loop */
  while(1) {
	printp(sys);
	if (SETJMP(EnvOfStackMachine)) {
	  printv("Syntax error or an interruption\n");
	  KSexecuteString(" ctrlC-hook "); /* Execute User Defined functions. */
	  if (signal(SIGINT,SIG_IGN) != SIG_IGN) {
		signal(SIGINT,ctrlC);
	  }
	  continue;
	} else {  }
	s=readString(stdin);
	if (s[0] == 0 || (strcmp(s,"quit;")==0)) break;
    KSexecuteString(" ox.engine ");
    ob = KpoString(s);
	KSpush(ob);
	KSexecuteString(" oxsubmit ");
	
    /* Get the result in string. */
	if (Format == 1) {
	  /* translate to latex form */
	}else{
	  KSexecuteString(" ox.engine oxpopstring ");
	  r = KSpopString();
	  printv(r);
	}
  }
}


#define SB_SIZE 1024
static char *readString(FILE *fp) {
  int n = 0;
  static int limit = 0;
  static char *s;
  int c;
  char *tmp;
  if (limit == 0) {
	limit = 1024;
	s = (char *)sGC_malloc(limit);
	if (s == NULL) {
	  fprintf(stderr,"No more memory.\n");
	  exit(10);
	}
  }
  s[0] = 0; n = 0;
  while ((c = fgetc(fp)) != EOF) {
	if (c == '\n') {
	  return s;
	}
	s[n++] = c; s[n] = 0;
	if (n >= limit-3) {
	  tmp = s;
	  limit *= 2;
	  s = (char *) sGC_malloc(limit);
	  if (s == NULL) {
		fprintf(stderr,"No more memory.\n");
		exit(10);
	  }
	  strcpy(s,tmp);
	}
  }
  return s;
}

static void printv(char *s) {
  int i;
  printf("%s",DATA_BEGIN_V);
  printf("%s",s);
  printf("%s",DATA_END);
  /* for debug "hello;
  for (i=0; i<strlen(s); i++) {
	printf("%x ",s[i]);
  }
  printf("\n");
  */

  fflush(NULL);
}
static void printl(char *s) {
  printf("%s",DATA_BEGIN_L);
  printf("%s",s);
  printf("%s",DATA_END);
  fflush(NULL);
}
static void printp(char *s) {
  printf("%s",DATA_BEGIN_P);
  printf("%s",s);
  printf("%s",DATA_END);
  fflush(NULL);
}
