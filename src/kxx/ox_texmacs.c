/* $OpenXM: OpenXM/src/kxx/ox_texmacs.c,v 1.4 2004/03/01 07:55:38 takayama Exp $ */

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

/*
#define DEBUG 
*/
#define DEBUG2

#ifdef DEBUG
#define DATA_BEGIN_V  "<S type=verbatim>"     /* "\002verbatim:" */
#define DATA_BEGIN_L  "<S type=latex>"        /* "\002latex:" */
#define DATA_BEGIN_P  "<S type=prompt>"        /* "\002channel:prompt " */
#define DATA_END      "</S>"    /* "\005" */
#else
#define DATA_BEGIN_V  "\002verbatim:"
#define DATA_BEGIN_L  "\002latex:"
#define DATA_BEGIN_P  "\002prompt:"
#define DATA_END      "\005" 
#endif

/*
#define END_OF_INPUT  '#'
*/
#define END_OF_INPUT '\n'

/* Table for the engine type. */
#define ASIR          1
#define SM1           2
#define K0            3

extern int Quiet;
extern JMP_BUF EnvOfStackMachine;
int Format=1;  /* 1 : latex mode */
int OutputLimit_for_TeXmacs = (1024*10);

int TM_Engine  = ASIR ;
int TM_asirStarted = 0;
int TM_sm1Started  = 0;
int TM_k0Started  = 0;

void ctrlC();
struct object KpoString(char *s);
char *KSpopString(void);

static char *readString(FILE *fp,char *prolog, char *eplog);
static void printv(char *s);
static void printl(char *s);
static void printp(char *s);
static void printCopyright(char *s);
static int startEngine(int type,char *msg);

/* tail -f /tmp/debug-texmacs.txt 
   Debug output to understand the timing problem of pipe interface.
*/
FILE *Dfp;

main() {
  char *s;
  char *r;
  char *sys;
  struct object ob;
  int irt=0;
  int vmode=1;

#ifdef DEBUG2
  Dfp = fopen("/tmp/debug-texmacs.txt","w");
#endif
  
  /* Set consts */
  sys = "asir> ";
  Quiet = 1;

  /* Initialize kanlib (gc is also initialized) */
  KSstart();

  /* Main loop */
  printf("%s",DATA_BEGIN_V);
  printCopyright("");

  /* Load ox engine here */
  /* engine id should be set to ox.engine */
  KSexecuteString(" [(parse) (ox.sm1) pushfile] extension ");
  startEngine(TM_Engine," ");

  if (signal(SIGINT,SIG_IGN) != SIG_IGN) {
	signal(SIGINT,ctrlC);
  }

  irt = 0;
  while(1) {
	/* printp(sys);  no prompt */
	if (SETJMP(EnvOfStackMachine)) {
	  printv("Syntax error or an interruption\n");
	  KSexecuteString(" ctrlC-hook "); /* Execute User Defined functions. */
	  if (signal(SIGINT,SIG_IGN) != SIG_IGN) {
		signal(SIGINT,ctrlC);
	  }
	  irt = 1;
	  continue;
	} else {  }
	if (!irt) {
	  printf("%s",DATA_END); fflush(stdout);
	}
	irt = 0;
	if (TM_Engine == K0) {
	  s=readString(stdin, " ", " "); /* see test data */
	}else if (TM_Engine == SM1) {
	  s=readString(stdin, " ", " "); /* see test data */
	}else{
	  s=readString(stdin, "if (1) { ", " ; }else{ }"); /* see test data */
	}

	if (s == NULL) { irt = 1; continue; }
	printf("%s",DATA_BEGIN_V);
    KSexecuteString(" ox.engine ");
    ob = KpoString(s);
	KSpush(ob);
	KSexecuteString(" oxsubmit ");
	
    /* Get the result in string. */
	if (Format == 1) {
	  /* translate to latex form */
	  KSexecuteString(" ox.engine oxpushcmotag ox.engine oxpopcmo ");
	  ob = KSpop();
	  vmode = 0;
	  /* printf("id=%d\n",ob.tag); bug: matrix return 17 instead of Sinteger
	   or error. */
	  if (ob.tag == Sinteger) {
		/* printf("cmotag=%d\n",ob.lc.ival);*/
		if (ob.lc.ival == CMO_ERROR2) {
		  vmode = 1;
		}
		if (ob.lc.ival == CMO_STRING) {
		  vmode = 1;
		}
	  }
	  if (vmode) {
		KSexecuteString(" ox.engine oxpopstring ");
		r = KSpopString();
	  }else{
		KSexecuteString(" ox.engine 1 oxpushcmo ox.engine (print_tex_form) oxexec  ");
		KSexecuteString(" ox.engine oxpopstring ");
		r = KSpopString();
	  }
	  if (strlen(r) < OutputLimit_for_TeXmacs) {
		if (vmode) printv(r); else printl(r);
	  } else printv("Output is too large.\n");
	}else{
	  KSexecuteString(" ox.engine oxpopstring ");
	  r = KSpopString();
	  if (strlen(r) < OutputLimit_for_TeXmacs) printv(r);
	  else printv("Output is too large.\n");
	}
  }
}

#define SB_SIZE 1024
#define INC_BUF 	if (n >= limit-3) { \
	  tmp = s; \
	  limit *= 2;  \
	  s = (char *) sGC_malloc(limit); \
	  if (s == NULL) { \
		fprintf(stderr,"No more memory.\n"); \
		exit(10); \
	  } \
	  strcpy(s,tmp); \
	} 
/*   */
static char *readString(FILE *fp, char *prolog, char *epilog) {
  int n = 0;
  static int limit = 0;
  static char *s;
  int c;
  char *tmp;
  int i;
  int m;
  int start;
  if (limit == 0) {
	limit = 1024;
	s = (char *)sGC_malloc(limit);
	if (s == NULL) {
	  fprintf(stderr,"No more memory.\n");
	  exit(10);
	}
  }
  s[0] = 0; n = 0; m = 0;
  for (i=0; i < strlen(prolog); i++) {
	s[n++] = prolog[i];  s[n] = 0;
    INC_BUF ;
  }
  start = n;
  while ((c = fgetc(fp)) != EOF) {
#ifdef DEBUG2
	fprintf(Dfp,"[%x] ",c); fflush(Dfp); 
#endif
	if (c == END_OF_INPUT) {
	  if (oxSocketSelect0(0,1)) {
		/* If there remains data in the stream,
		   read the remaining data. */
		if (c == '\n') c=' ';
		s[n++] = c; s[n] = 0;  m++;
		INC_BUF ;
		continue;
	  }
	  break;
	}
	if (c == '\n') c=' ';
	s[n++] = c; s[n] = 0;  m++;
    INC_BUF ;
  }
  /* Check the escape sequence */
  if (strcmp(&(s[start]),"!quit;") == 0) {
	printv("Terminated the process ox_texmacs.\n"); 
	exit(0);
  }
  /* Check the escape sequence to change the globa env. */
  if (strcmp(&(s[start]),"!verbatim;") == 0) {
	printv("Output mode is changed to verbatim mode.");
	Format=0;
	return NULL;
  }
  if (strcmp(&(s[start]),"!latex;") == 0) {
	printv("Output mode is changed to latex/verbose.");
	Format = 1;
	return NULL;
  }
  if (strcmp(&(s[start]),"!asir;") == 0) {
	Format=1;
	TM_Engine=ASIR; startEngine(TM_Engine,"Asir");
	return NULL;
  }
  if (strcmp(&(s[start]),"!sm1;") == 0) {
	Format=0;
	TM_Engine=SM1; startEngine(TM_Engine,"sm1");
	return NULL;
  }
  if (strcmp(&(s[start]),"!k0;") == 0) {
	Format=0;
	TM_Engine=K0; startEngine(TM_Engine,"k0");
	return NULL;
  }

  for (i=0; i < strlen(epilog); i++) {
	s[n++] = epilog[i];  s[n] = 0;
    INC_BUF ;
  }
  return s;
}

static void printv(char *s) {
  int i;
  printf("%s",DATA_BEGIN_V);
  printf("%s",s);
  printf("%s",DATA_END);
#ifdef DEBUG2
  fprintf(Dfp,"<%s>",s); fflush(Dfp);
#endif
  fflush(NULL);
}
static void printl(char *s) {
  printf("%s",DATA_BEGIN_L);
  printf(" $ %s $ ",s);
  printf("%s",DATA_END);
  fflush(NULL);
}
static void printp(char *s) {
  printf("%s",DATA_BEGIN_P);
  printf("%s",DATA_END);
  printf("%s] ",s);
  fflush(NULL);
}
static void printCopyright(char *s) {
  printf("%s",DATA_BEGIN_V);
  printf("OpenXM engine (ox engine) interface for TeXmacs\n2004 (C) openxm.org");
  printf(" under the BSD licence.  !asir, !sm1, !k0.");
  printf("%s",s);
  printf("%s",DATA_END);
  fflush(NULL);
}

static int startEngine(int type,char *msg) {
  struct object ob;
  printf("%s",DATA_BEGIN_V);
  if (type == SM1) {
    if (!TM_sm1Started) KSexecuteString(" sm1connectr ");
	KSexecuteString(" /ox.engine oxsm1.ccc def ");
	TM_sm1Started = 1;
	printf("%s\n",msg);
  }else if (type == K0) {
    if (!TM_k0Started) KSexecuteString(" k0connectr ");
	KSexecuteString(" /ox.engine oxk0.ccc def ");
	TM_k0Started = 1;
	printf("%s\n",msg);
  }else{
    if (!TM_asirStarted) KSexecuteString(" asirconnectr ");
	KSexecuteString(" /ox.engine oxasir.ccc def ");
	TM_asirStarted = 1;
	printf("%s\n",msg);
	KSexecuteString(" oxasir.ccc (copyright();) oxsubmit oxasir.ccc oxpopstring ");
	ob = KSpop();
	if (ob.tag == Sdollar) {
	  printf("%s",ob.lc.str);
	}
  }
  printf("%s",DATA_END);
  fflush(NULL);
}

/* test data

1.  print("hello"); print("afo");

    1+2;

2. def foo(N) { for (I=0; I<10; I++) {   --> error

   3+5;

4.  print("hello"); shift+return print("afo");

*/


  
