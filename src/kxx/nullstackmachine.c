#include <stdio.h>
#include "ox_kan.h"
#include "serversm.h"
#include <setjmp.h>
jmp_buf EnvOfStackMachine;  /* dummy data. */

int SerialCurrent = -1;
int Quiet = 0;

void *GC_malloc(n) {
  return((void *)malloc(n));
}

/* internal use. */
int Sm1_popInt();

Sm1_start() {
  fprintf(stderr,"nullstackmachine: sleep, pstack\n");
}

int nullCmoGetInt32(ox_stream ostream)
{
  char d[4];
  int i;
  for (i=0; i<4; i++) {
    d[i] = fp2fgetc(ostream);
  }
  return(ntohl(* ( (int *)d)));
}

/*  server stack machine */
static CMO_Object *LocalStack[200];
static int Stackp = 0;
void Sm1_pushToLocalStack(CMO_Object *op) {
  if (Stackp < 200) {
    LocalStack[Stackp++] = op;
  }else{
    fprintf(stderr,"Stack Overflow.\n");
  }
}
CMO_Object *Sm1_popFromLocalStack() {
  fprintf(stderr,"Stackp=%d\n",Stackp);
  if (Stackp <= 0) {
    fprintf(stderr,"Stack underflow.\n");
    return(NULL);
  }
  Stackp--;
  return(LocalStack[Stackp]);
}
  
CMO_Object *CMO_new_string(char *s) {
  CMO_string_object *op;
  int i;
  op = (CMO_string_object *)mymalloc(sizeof(CMO_string_object)+strlen(s));
  op->tag = htonl(CMO_STRING);
  op->size = htonl(strlen(s)+1);
  for (i=0; i< strlen(s); i++) {
    (op->data)[i] = s[i];
    (op->data)[i+1] = '\0';
  }
  return( (CMO_Object *)op);
}
CMO_Object *CMO_new_int32(int k) {
  CMO_int32_object *op;
  int i;
  op = (CMO_int32_object *)mymalloc(sizeof(CMO_int32_object));
  op->tag = htonl(CMO_INT32);
  op->n = k;
  return( (CMO_Object *)op);
}
void printCMO_object(FILE *fp,CMO_Object *op)
{
  int n,i;
  if (op == NULL) {
    fprintf(fp,"null");
  }else{
    switch(ntohl(op->tag)) {
    case CMO_INT32:
      fprintf(fp,"%d",((CMO_int32_object *)op)->n);
      break;
    case CMO_STRING:
      n = ntohl(((CMO_string_object *)op)->size);
      fprintf(stderr,"n=%d :"); fflush(NULL);
      for (i=0; i<n; i++) {
        fprintf(fp,"%c",((CMO_string_object *)op)->data[i]);
      }
      break;
    default:
      fprintf(fp,"Unknown object: tag=%d ",ntohl(op->tag));
      break;
    }
  }
}

void *Sm1_mathcap(void) {
  int n,i;
  struct mathCap *mathcap;
  mathcap = (struct mathCap *) malloc(sizeof(struct mathCap));
  strcpy(mathcap->name,"nullserver00 Version=0.1");
  mathcap->version = 199901160;
  mathcap->cmo[0] = CMO_ERROR2;
  mathcap->cmo[1] = CMO_NULL;
  mathcap->cmo[2] = CMO_INT32;
  mathcap->cmo[3] = CMO_STRING;
  mathcap->cmo[4] = CMO_LIST;
  mathcap->n =  5;
  return((void *)mathcap);
}
int Sm1_setMathCap(ox_stream os) {
  fprintf(stderr,"setMathCap is not implemented.\n");
  return(-1);
}
void Sm1_pops(void) {
  int n;
  n = Sm1_popInt32();
  Stackp -= n;
  if (Stackp < 0) Stackp = 0;
}
int Sm1_executeStringByLocalParser(void) {
  char *s;
  CMO_Object *op;
  int i;
  s = Sm1_popString();
  if (s != NULL) {
    if (strcmp(s,"sleep") == 0) {
      while (1) {
        fprintf(stderr,"Sleeping...  "); fflush(NULL);
        sleep(10);
      }
    }else if (strcmp(s,"pstack") == 0) {
      fprintf(stderr,"pstack -------------- Stackp = %d\n",Stackp);
      for (i=Stackp-1; i>=0; i--) {
        printCMO_object(stdout,LocalStack[i]); fprintf(stderr,"\n");
      }
      fprintf(stderr,"\n--------------------\n");
    }else{
      fprintf(stderr,"Unknown operator: %s\n",s);
    }
  }else {
    fprintf(stderr,"nullstackmachine.c: pop the null string.");
  }
  /* Sm1_pushToLocalStack(CMO_new_string(s)); */
  return(0);
}
char *Sm1_popString() {
  CMO_Object *op;
  CMO_string_object *sop;
  char *c;
  op = Sm1_popFromLocalStack();
  if (op != NULL) {
    switch(ntohl(op->tag)) {
    case CMO_INT32:
      c = (char *)malloc(30);
      sprintf(c,"%d",((CMO_int32_object *)op)->n);
      return(c);
      break;
    case CMO_STRING:
      sop = (CMO_string_object *)op;
      return(sop->data);
      break;
    default:
      fprintf(stderr,"tag error \n");
      return(NULL);
    }
  }else{
    return(NULL);
  }
}

int Sm1_popInt32() {
  CMO_Object *op;
  CMO_int32_object *sop;
  op = Sm1_popFromLocalStack();
  if (op != NULL) {
    if (op->tag != htonl(CMO_INT32)) {
      fprintf(stderr,"Object on the stack is not CMO_INT32. \n");
      return(0);
    }
    sop = (CMO_int32_object *)op;
    return(sop->n);
  }else{
    return(0);
  }
}


int Sm1_setName(void)
{
  char *s;
  s = Sm1_popString();
  if (s != NULL) fprintf(stderr,"setName %s\n",s);
  return(-1);
}

int Sm1_evalName(void)
{
  char *s;
  s = Sm1_popString();
  if (s != NULL) fprintf(stderr,"evalName : %s");
  return(-1);
}

static int isData(FILE2 *fp)
{
  if (fp->readpos < fp->readsize) return(1);
  else {
    return(oxSocketSelect0(fp->fd,0));
  }
}

int Sm1_pushCMO(ox_stream ostream) /* old one went to junk.c */
{
  int size;
  char data[1000];
  int i;
  int c,n;
  if (ostream == NULL || ostream->initialized != 1) {
    fprintf(stderr,"pushCMO,  ostream is not initialized or null.\n");
    return(-1);
  }
  /* Read data from ostream */
  fprintf(stderr,"----------- CMO data from stream -----------------\n");fflush(NULL);
  if (isData(ostream) || oxSocketSelect0(ostream->fd,-1)) {
    c = nullCmoGetInt32(ostream);
    fprintf(stderr,"cmo tag=%d : ",c);
    switch(c) {
    case CMO_ERROR2: fprintf(stderr,"CMO_ERROR2 ;"); break;
    case CMO_ERROR: fprintf(stderr,"CMO_ERROR ;"); break;
    case CMO_INT32: fprintf(stderr,"CMO_INT32 ;"); break;
    case CMO_STRING: fprintf(stderr,"CMO_STRING ;"); break;
    default: fprintf(stderr,"Unknown"); break;
    }
    switch(c) {
    case CMO_ERROR:
      break;
    case CMO_INT32: 
      n = nullCmoGetInt32(ostream);
      fprintf(stderr,"%d",n);
      Sm1_pushToLocalStack(CMO_new_int32(n));
      break;
    case CMO_STRING: 
      n = nullCmoGetInt32(ostream);
      fprintf(stderr,"size=%d ",n);
      if (n > 1000-2) {
        fprintf(stderr," size is too large. \n");
      }else{
        for (i=0; i<n; i++) {
          data[i] = fp2fgetc(ostream);
          data[i+1] = '\0';
        }
        fprintf(stderr," string=%s ",data);
        Sm1_pushToLocalStack(CMO_new_string(data));
      }
      break;
    default: 
      do {
        if ((c = fp2fgetc(ostream)) == EOF) {
          fprintf(stderr,"pushCMOFromStrem: Select returns 0, but there is no data or unexpected EOF.\n");
          return(-1);
        }
        fprintf(stderr,"%2x ",c);
      }while(isData(ostream));
    }
  }
  fprintf(stderr,"\n-------------------------------------------------\n"); fflush(NULL);
  return(0);
}

int Sm1_popCMO(ox_stream os,int serial)
{
  FILE *fp2;
  extern int errno;
  int c;
  int p;
  char data[1000];

  fp2 = fopen("ex1.cmo","r");
  if (fp2 == NULL) {
    fprintf(stderr,"popCMO : file ex1.cmo is not found.\n");
    return(-1);
  }
  p = 0;
  while ((c=fgetc(fp2)) != EOF) {
    data[p] = c; p++;
    if (p >= 1000) {fp2write(os,data,1000); p=0;}
    fprintf(stderr," %2x ",c);
  }
  if (p>0) { fp2write(os,data,p); }
  fp2fflush(os);

  return(0);
}

int Sm1_pushError2(int serial,int no,char *s)
{
  fprintf(stderr,"Sm1_pushError2 : [%d,%d,%s] \n",serial,no,s);
}


/* These are dummy.  It is defined in stackmachine.c */
unlockCtrlCForOx() { ; }
restoreLockCtrlCForOx() { ; }

