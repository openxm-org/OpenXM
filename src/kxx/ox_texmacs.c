/* $OpenXM: OpenXM/src/kxx/ox_texmacs.c,v 1.1 2004/02/29 03:00:37 takayama Exp $ */

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

/* #define DEBUG */
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

extern int Quiet;
extern JMP_BUF EnvOfStackMachine;
int Format=0;

void ctrlC();
struct object KpoString(char *s);
char *KSpopString(void);

static char *readString(FILE *fp,char *prolog, char *eplog);
static void printv(char *s);
static void printl(char *s);
static void printp(char *s);
static void printCopyright(char *s);

/* tail -f /tmp/debug-texmacs.txt 
   Debug output to understand the timing problem of pipe interface.
*/
FILE *dfp;

main() {
  char *s;
  char *r;
  char *sys;
  struct object ob;

  dfp = fopen("/tmp/debug-texmacs.txt","w");
  
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
  printCopyright("");
  while(1) {
	/* printp(sys);  no prompt */
	if (SETJMP(EnvOfStackMachine)) {
	  printv("Syntax error or an interruption\n");
	  KSexecuteString(" ctrlC-hook "); /* Execute User Defined functions. */
	  if (signal(SIGINT,SIG_IGN) != SIG_IGN) {
		signal(SIGINT,ctrlC);
	  }
	  continue;
	} else {  }
	s=readString(stdin, "if (1) { ", " ; }else{ };"); /* see test data */
	if (s == NULL) break;
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
  while ((c = fgetc(fp)) != EOF) {
	fprintf(dfp,"[%x] ",c); fflush(dfp); 
	if (c == END_OF_INPUT) {
	  break;
	}
	if (c == '\n') c=' ';
	s[n++] = c; s[n] = 0;  m++;
    INC_BUF ;
  }
  if (m == 0) return (char *) NULL;
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
  fprintf(dfp,"<%s>",s); fflush(dfp);
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
  printf("%s",DATA_END);
  printf("%s] ",s);
  fflush(NULL);
}
static void printCopyright(char *s) {
  printf("%s",DATA_BEGIN_P);
  printf("OpenXM engine (ox engine) interface for TeXmacs\n2004 (C) openxm.org\n");
  printf("%s",s);
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


  
