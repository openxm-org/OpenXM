/* $OpenXM: OpenXM/src/kan96xx/Kan/output.c,v 1.2 2000/01/16 07:55:40 takayama Exp $ */
#include <stdio.h>
#include "datatype.h"
#include "stackm.h"
#include "extern.h"
#include "extern2.h"


#define OUTPUT_QUEUE_SIZE 10


struct outputQueue {
  int oqP;
  int oqLimit;
  char **outputQueue;
};
static void putstr(char *s,struct outputQueue *oq);

static void putstr(s,oq)
     char *s;
     struct outputQueue *oq;
{
  int i;
  char **tmp;
  int oqP,oqLimit;
  char **outputQueue;
  oqP = oq->oqP; oqLimit = oq->oqLimit; outputQueue = oq->outputQueue;

  if (oqP < 0) errorOutput("(putstr) Invalid value of OutputQueue Pointer.");
  if (oqP <oqLimit) {
    outputQueue[oqP++] = s;
  }else{
    oqLimit = 2*oqLimit;
#ifndef NOGC    
    outputQueue =
      (char **)sGC_realloc((void *)outputQueue,oqLimit*sizeof(char **));
#else
    tmp = (char **)sGC_malloc(oqLimit*sizeof(char **));
    for (i=0; i<oqP; i++) {
      tmp[i] = outputQueue[i];  
    }
    outputQueue = tmp;
#endif
    outputQueue[oqP++] = s;
  }
  oq->outputQueue = outputQueue;
  oq->oqP = oqP;
  oq->oqLimit = oqLimit;
}

#define multStr(c) (c==' '?" ":(c=='\0'?"":"*"))

char *POLYToString(f,multSym,brace)
     POLY f;
     int multSym;
     int brace;
{
  extern int Wrap;
  int i,j,jj,fi;
  int printed = 0;
  int vi; /* index for variables */
  char *contStr = " \\\n";
  int length = 0;
  int lengthLimit = Wrap; /* e.g. = 15 */
  char *s;
  int size,sp;
  struct outputQueue oq;
  struct ring *ringp;
  char **xnames;
  char **dnames;
  int *xout;  /* output order */
  int n = 0;
  
  oq.oqP = 0;
  oq.oqLimit = OUTPUT_QUEUE_SIZE;
  oq.outputQueue = (char **)sGC_malloc(OUTPUT_QUEUE_SIZE*sizeof(char **));

  

  fi = 0;
  if (brace) putstr("(",&oq);
  if (f == POLYNULL) {
    putstr("0",&oq);
    printed = 1;
    xnames = dnames = (char **)NULL;
  }else{
    ringp = f->m->ringp; xnames = f->m->ringp->x; dnames = f->m->ringp->D;
    n = ringp->n;
    xout = ringp->outputOrder;
  }
  while (f != POLYNULL) {
    printed = 1;
    /*print coefficient*/
    if (fi == 0) {
      if (isConstant(f)) {
        putstr(coeffToString(f->coeffp),&oq);
      }else if (isOne(f->coeffp)) {
        /* do nothing */
      }else if (isMinusOne(f->coeffp)) {
        putstr("-",&oq);
      }else{
        putstr(coeffToString(f->coeffp),&oq);
        putstr(multStr(multSym),&oq);
      }
    }else{
      if (isConstant(f)) {
        if (isNegative(f->coeffp)) {
          putstr(coeffToString(f->coeffp),&oq);
        }else{
          putstr("+",&oq);
          putstr(coeffToString(f->coeffp),&oq);
        }
      } else if (isOne(f->coeffp)) {
        putstr("+",&oq);
      }else if (isMinusOne(f->coeffp)) {
        putstr("-",&oq);
      }else{
        if (!isNegative(f->coeffp)) putstr("+",&oq);
        putstr(coeffToString(f->coeffp),&oq);
        putstr(multStr(multSym),&oq);
      }
    }
    /* output variables */
    vi = 0;
    for (jj=0; jj<n*2; jj++) {
      j = xout[jj];
      if (j <n) {
        if (f->m->e[j].x) {
          vi++;
          if (vi != 1) putstr(multStr(multSym),&oq);
          putstr(xnames[j],&oq);
          if (f->m->e[j].x >= 2) {
            putstr("^",&oq);
            putstr(intToString(f->m->e[j].x),&oq);
          }else if (f->m->e[j].x < 0) {
            putstr("^(",&oq);
            putstr(intToString(f->m->e[j].x),&oq);
            putstr(")",&oq);
          }
        }
      }else {
        j = j-n;  
        if (f->m->e[j].D) {
          vi++;
          if (vi != 1) putstr(multStr(multSym),&oq);
          putstr(dnames[j],&oq);
          if (f->m->e[j].D >= 2) {
            putstr("^",&oq);
            putstr(intToString(f->m->e[j].D),&oq);
          }else if (f->m->e[j].D < 0) {
            putstr("^(",&oq);
            putstr(intToString(f->m->e[j].D),&oq);
            putstr(")",&oq);
          }
        }
      }
    }
    fi++;
    f = f->next;
    length += n/3+1;
    if (lengthLimit > 0  && length > lengthLimit) {
      length = 0;
      putstr(contStr,&oq);
    }
  }
  if (!printed) putstr("0",&oq);

  if (brace) putstr(")",&oq);
  
  size = 0;
  for (i=0; i<oq.oqP;i++) {
    size += strlen(oq.outputQueue[i]);
  }
  s = (char *)sGC_malloc(sizeof(char *)*(size + 2));
  if (s == (char *)NULL) errorOutput("No more memory.");
  sp = 0;
  for (i=0; i<oq.oqP; i++) {
    strcpy(&(s[sp]),oq.outputQueue[i]);
    sp += strlen(oq.outputQueue[i]);
  }

  return(s);
}

char *KPOLYToString(f)
     POLY f;
{
  extern int OutputStyle;
  return(POLYToString(f,OutputStyle,0));
}

isOne(c)
     struct coeff *c;
{
  switch(c->tag) {
  case INTEGER:
    if (c->val.i == 1) return(1); else return(0);
    break;
  case POLY_COEFF:
    return(0);
    break;
  case MP_INTEGER:
    if (mpz_cmp_si(c->val.bigp,(long) 1) == 0) return(1);
    else return(0);
    break;
  default:
    errorCoeff("not yet");
  }
}
isMinusOne(c)
     struct coeff *c;
{
  switch(c->tag) {
  case INTEGER:
    if (c->val.i == -1) return(1); else return(0);
    break;
  case POLY_COEFF:
    return(0);
    break;
  case MP_INTEGER:
    if (mpz_cmp_si(c->val.bigp,(long) -1) == 0) return(1);
    return(0);
  default:
    errorCoeff("not yet");
  }

}
isNegative(c)
     struct coeff *c;
{
  switch(c->tag) {
  case INTEGER:
    if (c->val.i < 0) return(1); else return(0);
    break;
  case POLY_COEFF:
    return(0);
    break;
  case MP_INTEGER:
    if (mpz_cmp_si(c->val.bigp,(long) 1) < 0) return(1);
    else return(0);
    break;
  default:
    errorCoeff("not yet");
  }
}

isConstant(f)
     POLY f;
{
  int i;
  int n;
  if (f == POLYNULL) return(1);
  n = f->m->ringp->n;
  for (i=0; i<n; i++) {
    if (f->m->e[i].x  ||  f->m->e[i].D) return(0);
  }
  return(1);
}

void errorOutput(s)
     char *s;
{
  fprintf(stderr,"Error(output.c):%s\n",s);
  exit(15);
}
