/* ki.c    ( kx interpreter )  */

#include <stdio.h>
#include "datatype.h"
#include "stackm.h"
#include "extern.h"
#include "extern2.h"
#include "lookup.h"
#include "matrix.h"
#include "gradedset.h"
#include <setjmp.h>
#include <signal.h>

char *getLOAD_K_PATH();  /* from d.h */

#ifdef CALLASIR
#include "ak0.h"
#endif

extern jmp_buf KCenvOfParser;

char Ktmp[10240];
int Ksize = 10240;
char *Kbuff = Ktmp;
int Kpt = 0;
int DebugCompiler = 0;  /* 0:   , 1: Displays sendKan[ .... ] */
int K00_verbose = 0;

extern int DebugMode;

sendKan(int p) {
  static int n = 2;
  extern int Interactive;
  struct object obj;
  int result;
  signal(SIGINT,SIG_IGN); /* Don't jump to ctrlC(). */
  if (p == 10) {printf("In(%d)=",n++); return;}
  if (p == 0 && DebugCompiler) printf("sendKan[%s]\n",Kbuff); 
  /* printf("sendKan[%s]\n",Kbuff);   */
  if (strlen(Kbuff) != 0) {
    signal(SIGINT,SIG_DFL);
    result = KSexecuteString(Kbuff);
    /* fprintf(stderr,"r=%d ",result);  */
    signal(SIGINT,SIG_IGN); /* Reset SIGINT. Don't jump to ctrlC(). */
  }
  /* fprintf(stderr,"r=%d ",result); */
  if (result == -1) {
    K00recoverFromError();
    fprintf(stderr,"--- Engine error or interrupt : ");
    if (DebugMode) {
      signal(SIGINT,SIG_DFL);
      KSexecuteString("db.DebugStack setstack ");
      signal(SIGINT,SIG_IGN); /* Reset SIGINT. Don't jump to ctrlC(). */
      obj = KSpop();
      signal(SIGINT,SIG_DFL);
      KSexecuteString("stdstack ");
      signal(SIGINT,SIG_IGN); /* Reset SIGINT. Don't jump to ctrlC(). */
      if (obj.tag == Sdollar) {
	fprintf(stderr,"%s\n",obj.lc.str);
	fprintf(stderr,"\n");
      }else{
	fprintf(stderr,"The error occured on the top level.\n");
      }
      fprintf(stderr,"Type in Cleards() to exit the debug mode and Where() to see the stack trace.\n");
    }
  }
#define AFO
#ifdef AFO
  if (setjmp(KCenvOfParser)) {
    fprintf(stderr,"Error: Goto the top level.\n");
    parseAfile(stdin);
    KCparse();  
    /* Call KCparse() recursively when there is error. */
    /* This is the easiest way to handle errors. */
    /* However, it should be rewrited in a future. */
  }else{ /* fprintf(stderr,"setjmp\n"); */ }
#endif
  if (p == 0 && Interactive) printf("In(%d)=",n++);
  Kpt=0; Kbuff[0] = '\0';
}


void pkkan(s)
char *s;
{
  char *t;
  if (strlen(s)+Kpt >= Ksize) {
    Ksize = Ksize*2;
    t = (char *)GC_malloc(sizeof(char)*Ksize);
    if (t == (char *)NULL) { fprintf(stderr,"No memory."); exit();}
    strcpy(t,Kbuff); Kbuff = t;
  }
  strcpy(&(Kbuff[Kpt]),s);
  Kpt += strlen(s);
}

void pkdebug(char *s0,char *s1, char *s2,char *s3) {
  if (DebugMode) {
    pkkan(" db.DebugStack setstack $");
    pkkan(s0); pkkan(s1); pkkan(s2); pkkan(s3);
    pkkan("$  stdstack \n");
  }
}
    
void pkdebug2(void) {
  if (DebugMode) {
    pkkan(" db.DebugStack setstack pop stdstack \n");
  }
}
    

void *mymalloc(int n)
{
  return((void *)GC_malloc(n));
}

execFile(char *s)
{
  FILE *fp;
#define TMP_SIZE 1024
  char tmp[TMP_SIZE+1];
  char tname[1024];
  char tname2[1024];
  char tname3[1024];
  char tname4[1024];
  int c;
  if ((fp = fopen(s,"r")) == (FILE *) NULL) {
    strcpy(tname,LOAD_SM1_PATH2);
    strcat(tname,s);
    strcpy(tname2,tname);
    if ((fp = fopen(tname,"r")) == (FILE *) NULL) {
      strcpy(tname,getLOAD_K_PATH());
      strcat(tname,s);
      strcpy(tname3,tname);
      if ((fp = fopen(tname,"r")) == (FILE *) NULL) {
	strcpy(tname,LOAD_K_PATH);
	strcat(tname,s);
	strcpy(tname4,tname);
	if ((fp = fopen(tname,"r")) == (FILE *) NULL) {
	  strcpy(tname,getLOAD_K_PATH());
	  strcat(tname,"../kan96xx/Kan/");
	  strcat(tname,s);
	  if ((fp = fopen(tname,"r")) == (FILE *) NULL) {
	    fprintf(stderr,"Fatal error: Cannot open the system macro %s in %s, %s, %s nor %s.\n",
		    s,tname2,tname3,tname4,tname);
	    exit(11);
	    return;
	  }
	}
      }
    }
  }
  /* printf("Reading\n"); fflush(stdout);  */
  while (fgets(tmp,TMP_SIZE,fp) != NULL) {
    pkkan(tmp);
  }
  /* printf("Done.\n"); fflush(stdout); */
  sendKan(1);
  /* printf("sendKan, done.\n"); fflush(stdout); */
}

/*Tag:  yychar = YYEMPTY; Put the following line in simple.tab.c */
/* It makes segmentation fault. */
/*
#include <setjmp.h>
  extern jmp_buf KCenvOfParser;
  if (setjmp(KCenvOfParser)) {
    parseAfile(stdin);
    fprintf(stderr,"Error: Goto the top level.\n");
  }else{   }
*/


void testNewFunction(objectp op)
{
  fprintf(stderr,"This is testNewFunction of NOT CALLASIR.\n");
  if (op->tag != Sstring) {
    fprintf(stderr,"The argument must be given as an argment of load.\n");
    return;
  }
  fprintf(stderr,"Now execute .. <<%s>> \n",op->lc.str);
  parseAstring(op->lc.str);
  fprintf(stderr,"\nDone.\n");
}







