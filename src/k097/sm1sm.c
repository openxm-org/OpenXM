/* $OpenXM: OpenXM/src/k097/sm1sm.c,v 1.4 2015/10/10 11:29:46 takayama Exp $ */
/* This is imported from kxx/sm1stackmachine.c */
#include <stdio.h>
#include <setjmp.h>
#include <string.h>
#include <stdlib.h>
#include "../kxx/ox_kan.h"
#include "../kxx/serversm.h"
#include "ox_k0.h"

extern int OXprintMessage;

/*  server stack machine */


void *Sm1_mathcap() {
  int n,i;
  struct mathCap *mathcap;
  mathcap = KSmathCapByStruct();
  return((void *)mathcap);
}

int Sm1_setMathCap(ox_stream os) {
  /* Set the mathcap data of the client in the server. */
  /*  sm1 <====== ox_sm1  mathcap is set in ox_stream = FILE2 * */
  /* The mathcap data is on the stack. */
  struct object ob = OINIT;
  struct object ob2 = OINIT;
  int n,i;
  ob = KSpop();
  KSpush(ob);  KSexecuteString(" (mathcap data is ) message message ");
  Kan_setMathCapToStream(os,ob);
  /* set the math cap data associated to the ox_stream. */
}
void Sm1_pops(void) {
  char data[100];
  sprintf(data," 1 1 3 -1 roll { pop pop } for ");
  KSexecuteString(data);
}
int Sm1_executeStringByLocalParser(void) {
  int i;
  char *s;
  s = Sm1_popString();
  if (s == NULL) {
    printf("NULL argument for executeString.\n");
    return(-1);
  }else{
    if (OXprintMessage) fprintf(stderr,"KSexecuteString(%s)\n",s);
    i = KSexecuteString(s);
    return(i);
  }
}
char *Sm1_popString(void) {
  char *KSpopString();
  KSexecuteString(" toString ");
  return(KSpopString());
}


int Sm1_setName(void)
{
  char *s;
  struct object ob = OINIT;
  s = Sm1_popString();
  if (s == NULL) {
    printf("NULL argument for setName.\n");
    return(-1);
  }else{
    ob = KSpop();
    printf("/%s tag=%d def\n",s,ob.tag);
    KputUserDictionary(s,ob);
    return(0);
  }
}

int Sm1_evalName(void)
{
  char *s;
  struct object ob = OINIT;
  s = Sm1_popString();
  if (s == NULL) {
    printf("NULL argument for evalName.\n");
    return(-1);
  }else{
    ob = KfindUserDictionary(s);
    if (ob.tag == -1) {
      printf("findUserDictionary(%s)--> tag=%d Not found.\n",s,ob.tag);
      return(-1);
    }
    printf("findUserDictionary(%s)--> tag=%d\n",s,ob.tag);
    KSpush(ob);
    return(0);
  }
}

int Sm1_pushCMO(ox_stream fp)
{
  return(Kan_pushCMOFromStream(fp));
}
int Sm1_popCMO(ox_stream fp,int serial)
{
  return(Kan_popCMOToStream(fp,serial));
}

int Sm1_pushError2(int serial, int no, char *s)
{
  struct object ob = OINIT;
  ob = KnewErrorPacket(serial,no,s);
  KSpush(ob);
}

char *Sm1_popErrorMessage(char *s) {
  char *e;
  char *a;
  extern int ErrorMessageMode;
  /* Set ErrorMessageMode = 2 to use this function. */
  if (ErrorMessageMode != 2) return(s);
  e = popErrorStackByString();
  if (e == NULL ) {
    a = (char *) sGC_malloc(sizeof(char)*(strlen(s)+80));
    if (a == NULL) {
      fprintf(stderr,"No more memory in Sm1_popErrorMessage.\n");
      exit(10);
    }
    strcpy(a,s); strcat(a,"No error message on the error stack.");
    return(a);
  }else{
    a = (char *) sGC_malloc(sizeof(char)*(strlen(s)+strlen(e)+2));
    if (a == NULL) {
      fprintf(stderr,"No more memory in Sm1_popErrorMessage.\n");
      exit(10);
    }
    strcpy(a,s); strcat(a,e);
    return(a);
  }
}

void Sm1_getsp(void) {
  KSpush(KpoInteger(KSstackPointer()));
}

void Sm1_dupErrors(void) {
  KSpush(KSdupErrors());
}

void Sm1_pushCMOtag(int serial) {
  struct object obj = OINIT;
  int t;
  obj = KSpeek(0);
  t = KgetCmoTagOfObject(obj);
  if (t != -1) {
    KSpush(KpoInteger(t));
  }else{
    Sm1_pushError2(serial,-1,"The top object on the server stack cannot be translated to cmo.");
  }
}


