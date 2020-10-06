/*$OpenXM: OpenXM/src/kan96xx/plugin/cmo.c,v 1.17 2020/10/04 06:10:36 noro Exp $*/
#include <stdio.h>
#include <string.h>
/* #include <netinet/in.h> */
#include <stdlib.h>
#include <arpa/inet.h>
#include "datatype.h"
#include "stackm.h"
#include "extern.h"
#include "extern2.h"
#include "mathcap.h"
#include "kclass.h"
#include "oxMessageTag.h"
#include "oxFunctionId.h"

#include "file2.h"
#include "cmo.h"

#include "cmotag.htmp"   /* static char *cmotagToName(int tag) is defined
                            here. */

void warningCmo(char *s);
void errorCmo(char *s);
size_t cmoOutGMPCoeff(mpz_srcptr x); // defined in cmo-gmp.c
size_t cmoGetGMPCoeff(MP_INT *x, struct cmoBuffer *cb); 
int cmoCheckMathCap(struct object ob, struct object *mathcapObjp); // in oxmisc2.h


extern int OxVersion;

int CmoClientMode = 1;  /* This flag is used to translate names for
               indeterminates.
               It does not work well if ox_sm1 have a server, i.e.,
               sm1 --> ox_sm1 --> ox_sm1
               */

/* void *malloc(int s);
  #define GC_malloc(x) malloc(x) */
/**********************  What you have to do when you add new CMO_ **********
*  Add your new object to   cmoObjectToCmo00  ( for sending )
*  Add your new object to   cmoCmoToObject00  ( for receiving )
*  Edit KSmathCapByStruct();
*  Edit typeTrans in cmoCheck00 in oxmisc2.c , e.g.,
*       typeTrans[CLASSNAME_mathcap] = CMO_MATHCAP;
**************************************************************************/
/*  If you change the format of mathcap, do the follows.
    Mofify  cmoCheckMathCap in oxmisc2.c,
            oxSendMathCap  in oxmisc.c,
        newMathCap in cmo.c,
    oxReq, SM_setMathCap: in oxmisc2.c, and
    grep mathCap and make all modifications.
*/

extern struct ring *CurrentRingp;
extern struct ring *SmallRingp;
extern int CmoDMSOutputOption;

struct object NullObjectInCmo = OINIT;

extern int SerialCurrent;
extern int DebugCMO;

#define BUFFERSIZE  1024
struct cmoBuffer *cmoOutputToBuf(cmoAction a,void *data, int size)
{
  static struct cmoBuffer b;
  static int bufferIsInitialized = 0;
  void *tmp;
  struct cmoBuffer *cb;
  int i;
  if (!bufferIsInitialized) {
    NullObjectInCmo.tag = Snull;
    bufferIsInitialized = 1;
    b.size = BUFFERSIZE;
    b.pos = 0;
    b.rpos = 0;
    b.isStream = 0;
    b.fp = (FILE2 *)NULL;
    b.buf = sGC_malloc(BUFFERSIZE);
    if (b.buf == NULL) errorCmo("cmoOutputToBuf: no memory.");
  }
  if (b.isStream) {
    switch(a) {
    case CMOINIT:
      b.pos = 0;
      b.rpos = 0;  /* added, 1997, 12/5 */
      b.isStream = 0;
      b.fp = (FILE2 *)data ;
      b.errorno = 0;
      break;
    case CMOINITSTREAM:
      b.pos = 0;
      b.isStream = 1;
      b.rpos = 0;  /* added, 1997, 12/5 */
      b.fp = (FILE2 *)data;
      b.errorno = 0;
      break;
    case CMOPUT:
      for (i=0; i<size; i++) {
        fp2fputc((int) ((char *)data)[i], b.fp);
      }
      break;
    case CMOFLUSH:
      if (fp2fflush(b.fp)<0) {
        errorCmo("cmoOutputToBuf: CMOFLUSH failed in stream mode.");
      }
      cb = (struct cmoBuffer *)sGC_malloc(sizeof(struct cmoBuffer));
      cb->isStream = b.isStream;
      cb->size = b.pos;
      cb->pos = b.pos;
      cb->buf = NULL;
      return(cb);
      break;
    case CMOERROR:
      b.errorno = size;
      break;
    default:
      errorCmo("Unknown action.");
      break;
    }
    return(NULL);
  }else{
    switch(a) {
    case CMOINIT:
      b.pos = 0;
      b.rpos = 0;  /* added, 1997, 12/5 */
      b.isStream = 0;
      b.errorno = 0;
      break;
    case CMOINITSTREAM:
      b.pos = 0;
      b.isStream = 1;
      b.rpos = 0;  /* added, 1997, 12/5 */
      b.fp = (FILE2 *)data;
      b.errorno = 0;
      break;
    case CMOPUT:
      if (b.pos + size >= b.size) {
        tmp = sGC_malloc((b.size)*2+size);
        memcpy(tmp,b.buf,b.pos);
        b.buf = tmp;
        b.size = (b.size)*2+size;
      }
      memcpy((void *) &(((char *)(b.buf))[b.pos]),data,size);
      b.pos += size;
      break;
    case CMOFLUSH:
      cb = (struct cmoBuffer *)sGC_malloc(sizeof(struct cmoBuffer));
      cb->isStream = b.isStream;
      cb->size = b.pos;
      cb->pos = b.pos;
      cb->buf = sGC_malloc((b.pos<=0?1:b.pos));
      memcpy(cb->buf,b.buf,b.pos);
      return(cb);
      break;
    case CMOERROR:
      b.errorno = size;
      break;
    default:
      errorCmo("Unknown action.");
      break;
    }
    return(NULL);
  }
}

int dumpCmoBuf(struct cmoBuffer *cb)
{
  int i,size, tag;
  char *s;
  if (cb->isStream) {
    printf("cmoBuffer is directed to a stream.\n");
    return 0;
  }
  size = cb->pos;
  s = (char *)(cb->buf);
  tag = htonl(*((int *) s));
  printf("CMO StandardEncoding: size = %d, size/sizeof(int) = %d, tag=%s \n",size,size/((int)sizeof(int)),cmotagToName(tag));
  for (i=0; i<size; i++) {
    if (i % 20 == 0) putchar('\n');
    printf("%3x",(int)(unsigned char)s[i]);
  }
  putchar('\n');
  return 0;
}

/* This obsolete function is used to write data
   in cmoBuffer (cmo object in kan)
   to a stream */
int cmoToStream(struct object cmoObj,struct object of)
{
  int i,size;
  struct cmoBuffer *cb;
  char *s;
  int file2=0;
  if (!(cmoObj.tag == Sclass && cmoObj.lc.ival == CMO)) {
    errorCmo("cmoToStream: the first argument is not cmoObject.");
  }
  if (of.tag != Sfile) {
    errorCmo("cmoToStream: the second argument is not file object.");
  }
  if (strcmp(of.lc.str,MAGIC2) == 0) {
    file2 = 1;
  }
  cb = cmoObj.rc.voidp;
  size = cb->pos;
  s = (char *)(cb->buf);
  for (i=0; i<size; i++) {
    if (file2) {
      fp2fputc((int) s[i],(FILE2 *)(of.rc.voidp));
    }else{
      fputc((int) s[i],of.rc.file);
    }
  }
}
/* This obsolete function is used to store data from the stream
   to cmoBuffer in the raw form. (cmo object in kan).
   cf. cmoObjectFromStream, cmoObjectToStream: these function 
       directly transmit objects of kan to a stream in CMO format.
*/
struct object streamToCmo(struct object of)
{
  int c;
  unsigned char s[1];
  struct object ob = OINIT;
  int file2 = 0;
  if (of.tag == Sfile) {

  }else{
    errorCmo("streamToCmo: no file is opened.");
  }
  if (strcmp(of.lc.str,MAGIC2) == 0) {
    file2 = 1;
  }
  cmoOutputToBuf(CMOINIT,NULL,0);
  /* Raw reading to the buffer from file. No cmoOutHeader(). */
  if (file2) {
    while ((c=fp2fgetc((FILE2 *)of.rc.voidp)) != EOF) {
      s[0] = c;
      cmoOutputToBuf(CMOPUT,s,1);
    }
  }else{
    while ((c=fgetc(of.rc.file)) != EOF) {
      s[0] = c;
      cmoOutputToBuf(CMOPUT,s,1);
    }
  }
  /* Raw reading to the buffer from file. No cmoOutTail(). */
  ob.tag = Sclass;
  ob.lc.ival = CMO;
  ob.rc.voidp = cmoOutputToBuf(CMOFLUSH,NULL,0);
  return(ob);
}


void cmoOutCmoNull() {
  cmoint tmp[1];
  tmp[0] = htonl((cmoint) CMO_NULL);
  cmoOutputToBuf(CMOPUT,tmp,sizeof(cmoint));
}

  


/* unsigned short int must be 32 bits */
void cmoOutInt32(int k)
{
  cmoint tmp[2];
  tmp[0] = htonl((cmoint) CMO_INT32);
  tmp[1] = htonl((cmoint ) k);
  cmoOutputToBuf(CMOPUT,tmp,2*sizeof(cmoint));
}

int cmoOutString(char *d,int size) {
  cmoint tmp[2];
  tmp[0] = htonl((cmoint) CMO_STRING);
  tmp[1] = htonl((cmoint ) size);
  cmoOutputToBuf(CMOPUT,tmp,2*sizeof(cmoint));
  cmoOutputToBuf(CMOPUT,d,size);
  return 0;
}


int cmoOutMonomial32(POLY cell)
{
  cmoint tmp[3+N0*2];
  extern int ReverseOutputOrder;
  int i,nn,tt;
  if (cell == POLYNULL) {
    tmp[0] = htonl(CMO_ZERO);
    cmoOutputToBuf(CMOPUT,tmp,sizeof(cmoint));
    return(0);
  }
  tmp[0] = htonl(CMO_MONOMIAL32);
  nn = cell->m->ringp->n;
  tmp[1] = htonl(nn*2);
  if (ReverseOutputOrder) {
    for (i=0; i<nn; i++) {
      tmp[2+i] = htonl(cell->m->e[nn-i-1].x);
      tmp[2+nn+i] = htonl(cell->m->e[nn-i-1].D);
    }
  }else{
    for (i=0; i<nn; i++) {
      tmp[2+i] = htonl(cell->m->e[i].x);
      tmp[2+nn+i] = htonl(cell->m->e[i].D);
    }
  }
  cmoOutputToBuf(CMOPUT,tmp,sizeof(cmoint)*(2+2*nn));
  switch(cell->coeffp->tag) {
  case INTEGER:
    cmoOutInt32(cell->coeffp->val.i);
    return(0);
    break;
  case MP_INTEGER:
    /* errorCmo("Not implemented."); */
    cmoOutGMPCoeff(cell->coeffp->val.bigp);
    return(0); 
    break;
  default:
    cmoOutputToBuf(CMOERROR,NULL,-1); /* fatal, need reset_connection */
    errorCmo("cmoOutMonomial32(): unknown coefficient tag.");
    return(-1);
    break;
  }
}
    
int cmoOutDMS() 
{
  cmoint tmp[1];
  tmp[0] = htonl(CMO_DMS);
  cmoOutputToBuf(CMOPUT,tmp,sizeof(cmoint));
  return(0);
}

int cmoOutPolynomial(POLY f) 
{
  int size;
  cmoint tmp[2];
  if (f == POLYNULL) {
    return(cmoOutMonomial32(f));
  }
  size = pLength(f);
  tmp[0] = htonl(CMO_LIST);
  tmp[1] = htonl(size+2);
  cmoOutputToBuf(CMOPUT,tmp,sizeof(cmoint)*2);
  cmoOutDMS();
  cmoOutRingDefinition(f->m->ringp,CmoDMSOutputOption);
  while (f != POLYNULL) {
    cmoOutMonomial32(f);
    f = f->next;
  }
  return(0);
}

int cmoOutPolynomial2(POLY f) 
{
  int size;
  cmoint tmp[2];
  if (f == POLYNULL) {
    return(cmoOutMonomial32(f));
  }
  size = pLength(f);
  tmp[0] = htonl(CMO_DISTRIBUTED_POLYNOMIAL);
  tmp[1] = htonl(size);
  cmoOutputToBuf(CMOPUT,tmp,sizeof(cmoint)*2);
  cmoOutRingDefinition2(f->m->ringp,CmoDMSOutputOption);
  while (f != POLYNULL) {
    cmoOutMonomial32(f);
    f = f->next;
  }
  return(0);
}

void cmoOutRingDefinition(struct ring *rp,int option)
{
  cmoint tmp[3];
  /* minimal information */
  switch(option) {
  case 1: /* RING_BY_NAME */
    cmoOutRawInt(CMO_RING_BY_NAME);
    cmoOutString(rp->name,strlen(rp->name));
    break;
  case 2: 
    tmp[0] = htonl(CMO_DMS_OF_N_VARIABLES);
    tmp[1] = htonl(CMO_LIST);
    tmp[2] = htonl(2);
    cmoOutputToBuf(CMOPUT,tmp,sizeof(cmoint)*3);
    cmoOutInt32((rp->n)*2); /* number of variables */
    cmoOutInt32(rp->p);     /* coefficient field.
                               CMO_INT32 or CMO_DMS_OF_N_VARIABLES */
    /* Optional arguments are  name of variables, weight_vector, output_order */
    break;
  default:   /* including 0. */
    cmoOutInt32(CMO_DMS_GENERIC);
    break;
  }
  
}

void cmoOutRingDefinition2(struct ring *rp,int option)
{
  cmoint tmp[3];
  /* minimal information */
  switch(option) {
  case 1: /* RING_BY_NAME */
    cmoOutRawInt(CMO_RING_BY_NAME);
    cmoOutString(rp->name,strlen(rp->name));
    break;
  case 2: 
    tmp[0] = htonl(CMO_DMS_OF_N_VARIABLES);
    tmp[1] = htonl(CMO_LIST);
    tmp[2] = htonl(2);
    cmoOutputToBuf(CMOPUT,tmp,sizeof(cmoint)*3);
    cmoOutInt32((rp->n)*2); /* number of variables */
    cmoOutInt32(rp->p);     /* coefficient field.
                               CMO_INT32 or CMO_DMS_OF_N_VARIABLES */
    /* Optional arguments are  list of indeterminates (name of variables),
       weight_vector by list , output_order by list. */
    break;
  default:   /* including 0. */
    cmoOutRawInt(CMO_DMS_GENERIC);
    break;
  }
  
}

/* ------------------------------ */
int cmoGetIntFromBuf(cmoAction a,struct cmoBuffer *cb)
{
  cmoint tmp[1];
  char data[4];
  int i;
  int cc;
  if (cb->isStream) {
    switch(a) {
    case CMOGET:
      for (i=0; i<4; i++) {
        cc = fp2fgetc(cb->fp);
        if (cc < 0) {
          return(-1);
          errorCmo("cmoGetIntFromBuf CMOGET: unexpected EOF.\n");
        }
        data[i] = cc;
      }
      return( (int) ntohl( *((cmoint *) data) ));
      break;
    case CMOGETBYTE:
      cc = fp2fgetc(cb->fp);
      if (cc < 0) {
        return(-1);
        errorCmo("cmoGetIntFromBuf CMOGETBYTE: unexpected EOF.\n");
      }
      return(cc);
      break;
    case CMOINIT:
      fprintf(stderr,"Invalid action CMOINIT for cmoBuffer in the stream mode.\n");
      cb->rpos = 0;
      cb->errorno = 0;
      break;
    case CMOINITSTREAM:
      cb->errorno = 0;
      break;
    case CMOERROR:
      cb->errorno = 1;
      break;
    case CMOERROR2:
      cb->errorno = -1;
      break;
    }
    return(0);
  }else{
    switch(a) {
    case CMOGET:
      if (cb->rpos + sizeof(cmoint) > cb->pos) {
        fprintf(stderr,"No more data in the buffer. Returns -1.\n");
        return(-1);
      }
      memcpy(tmp,(void *) &(((char *)(cb->buf))[cb->rpos]),
             sizeof(cmoint));
      cb->rpos += sizeof(cmoint);
      return( (int) ntohl(tmp[0]));
      break;
    case CMOGETBYTE:
      if (cb->rpos + 1 > cb->pos) {
        fprintf(stderr,"No more data in the buffer. Returns -1.\n");
        return(-1);
      }
      tmp[0] = ((unsigned char *)(cb->buf))[cb->rpos];
      cb->rpos += 1;
      return( (int) tmp[0]);
      break;
    case CMOINIT:
      cb->rpos = 0;
      cb->errorno = 0;
      break;
    case CMOINITSTREAM:
      cb->errorno = 0;
      break;
    case CMOERROR:
      cb->errorno = 1;
      break;
    case CMOERROR2:
      cb->errorno = -1;
      break;
    }
    return(0);
  }
}


/* ------------------------------------- */
/* This function is called after reading the tag CMO_INT32COEFF */
struct coeff * cmoGetInt32Coeff(struct cmoBuffer *cb)
{
  struct coeff *c;
  c = intToCoeff(cmoGetIntFromBuf(CMOGET,cb),CurrentRingp);
  return(c);
}
void *cmoGetString(struct cmoBuffer *cb, int size)
{
  char *d;
  int i;
  if (size > 0) {
    d = (char *)sGC_malloc(size);
    if (d == NULL) {
      errorCmo("No more memory in cmoGetString().\n");
    }
  }else{ 
    d = (char *)sGC_malloc(1); d[0] = '\0';
    if (d == NULL) {
      errorCmo("No more memory in cmoGetString().\n");
    }
  }
  for (i=0; i<size; i++) {
    d[i] = cmoGetIntFromBuf(CMOGETBYTE, cb);
  }
  return(d);
}
    
POLY cmoGetMonomial32(struct cmoBuffer *cb)
{
  int nn,i,tt;
  struct coeff *c;
  struct monomial *m;
  MP_INT *mi;
  MP_INT *mi2;
  int skip;
  int nn0,nn1;
  extern int ReverseOutputOrder;
  skip = 0;
  nn = cmoGetIntFromBuf(CMOGET,cb);
  if (nn > (CurrentRingp->n)*2 ) {
    CurrentRingp = KopRingp(KdefaultPolyRing(KpoInteger(nn/2+(nn%2))));
    warningCmo("cmoGetMonomials32(): Changed the current ring, because your peer sent a DMS that does not fit to the current ring.");

    /* original code.
       skip = nn - (CurrentRingp->n)*2;
       nn1 = nn0 = CurrentRingp->n;
       if (! (cb->errorno) ) {
       warningCmo("cmoGetMonomial32(): serialized polynomial \\not\\in CurrentRing.");
       }
       cmoGetIntFromBuf(CMOERROR,cb);
    */
  }
  if (nn == (CurrentRingp->n)*2 ) {
    nn1 = nn0 = CurrentRingp->n;
    skip = 0;
  }else {
    nn0 = nn/2;
    nn1 = nn - nn0;
    skip = 0;
  }

  m = newMonomial(CurrentRingp);
  if (ReverseOutputOrder) {
    for (i=0; i<nn0; i++) {
      m->e[nn0-i-1].x = cmoGetIntFromBuf(CMOGET,cb);
    }
    for (i=0; i<nn1; i++) {
      m->e[nn1-i-1].D = cmoGetIntFromBuf(CMOGET,cb);
    }
  }else{
    for (i=0; i<nn0; i++) {
      m->e[i].x = cmoGetIntFromBuf(CMOGET,cb);
    }
    for (i=0; i<nn1; i++) {
      m->e[i].D = cmoGetIntFromBuf(CMOGET,cb);
    }
  }

  /* Throw a way extra data. */
  for (i=0; i<skip; i++) {
    cmoGetIntFromBuf(CMOGET,cb);
  }
  
  tt = cmoGetIntFromBuf(CMOGET,cb);
  switch(tt) {
  case CMO_INT32:
    c = cmoGetInt32Coeff(cb);
    break;
  case CMO_ZZ_OLD:
  case CMO_ZZ:
    if (OxVersion >= 199907170 && tt == CMO_ZZ_OLD)  {
      errorCmo("CMO_ZZ_OLD is not implemented.");
    }else if (OxVersion < 199907170 && tt == CMO_ZZ)  {
      errorCmo("CMO_ZZ is not implemented.");
    }
    mi = newMP_INT();
    cmoGetGMPCoeff(mi, cb);
    c = mpintToCoeff(mi,CurrentRingp);
    /* errorCmo("Not implemented."); */
    break;
  case CMO_QQ:
    if (! (cb->errorno) ) {
      warningCmo("cmoGetMonomial32(): coefficient CMO_QQ is not supported. Denominators are ignored.");
    }
    cmoGetIntFromBuf(CMOERROR,cb);
    mi = newMP_INT();
    cmoGetGMPCoeff(mi,cb);
    c = mpintToCoeff(mi,CurrentRingp);  /* we take only numerator */
    /* Throw a way denominators. */
    mi2 = newMP_INT();
    cmoGetGMPCoeff(mi2,cb);
    break;
  default:
    cmoGetIntFromBuf(CMOERROR2,cb);
    errorCmo("cmoGetMonomial32(): coeff type that is not implemented.");
    break;
  }
  return(newCell(c,m));
}



/* ------------------------------------- */
void cmoObjectToCmo00(struct object ob)
{
  struct object rob = OINIT;
  cmoint tmp[16];
  char tmpc[16];
  int i,size;
  struct object vlist = OINIT;
  struct object vlist0 = OINIT;
  int m;
  struct object ob2 = OINIT;

  /* NO initialization */
  switch(ob.tag) {
  case Snull:
    cmoOutCmoNull();
    break;
  case Sinteger:
    /* fprintf(stderr,"For test.\n"); */
    cmoOutInt32(ob.lc.ival); 
    break;
  case Sdollar:
    cmoOutString(ob.lc.str,strlen(ob.lc.str));
    break;
  case Spoly:
    /* cmoOutPolynomial(KopPOLY(ob)); */
    cmoOutPolynomial2(KopPOLY(ob));
    break;
  case Sarray:
    tmp[0] = htonl(CMO_LIST);
    tmp[1] = htonl(size=getoaSize(ob));
    cmoOutputToBuf(CMOPUT,tmp,sizeof(cmoint)*2);
    for (i=0; i<size; i++) {
      cmoObjectToCmo00(getoa(ob,i));
    }
    break;
  case SuniversalNumber:
    cmoOutGMPCoeff(ob.lc.universalNumber->val.bigp);
    break;
  case SrationalFunction:
    tmp[0] = htonl(CMO_RATIONAL);
    cmoOutputToBuf(CMOPUT,tmp,sizeof(cmoint));
	cmoObjectToCmo00(*(Knumerator(ob)));
	cmoObjectToCmo00(*(Kdenominator(ob)));
    break;
  case Sdouble:
    if (sizeof(double) != 8) errorCmo("double is assumed to be 8 bytes.");
    cmoOutRawInt(CMO_64BIT_MACHINE_DOUBLE);
    *((double *) tmpc) = (double) *(ob.lc.dbl);
    cmoOutputToBuf(CMOPUT,tmpc,8);
    break;
  case Sclass:
    switch(ectag(ob)) {
    case CLASSNAME_ERROR_PACKET:
      /* fprintf(stderr,"ectag=%d\n",ectag(*KopErrorPacket(ob)));  **kxx:CMO_ERROR*/
      if (ectag(*KopErrorPacket(ob)) == CLASSNAME_ERROR_PACKET) {
        tmp[0] = htonl(CMO_ERROR);
        cmoOutputToBuf(CMOPUT,tmp,sizeof(cmoint));
      }else{
        tmp[0] = htonl(CMO_ERROR2);
        cmoOutputToBuf(CMOPUT,tmp,sizeof(cmoint));
        /* Send without OX_DATA header !! */
        cmoObjectToCmo00(*(KopErrorPacket(ob)));
      }
      break;
    case CLASSNAME_mathcap:
      tmp[0] = htonl(CMO_MATHCAP);
      cmoOutputToBuf(CMOPUT,tmp,sizeof(cmoint));
      /* Send without OX_DATA header !! */
      cmoObjectToCmo00(*(KopMathCap(ob)));
      break;
    case CLASSNAME_indeterminate:
      tmp[0] = htonl(CMO_INDETERMINATE);
      cmoOutputToBuf(CMOPUT,tmp,sizeof(cmoint));
      /* cmoObjectToCmo00(KopIndeterminate(ob)); Old code. */
      /* If you need to translate the name, do it here. */
      if (CmoClientMode) {
        ob = KopIndeterminate(ob);
      }else{
        ob = cmoTranslateVariable_outGoing(KopIndeterminate(ob));
      }
      cmoObjectToCmo00(ob);
      break;
    case CLASSNAME_recursivePolynomial:
      /* We assume that the format of the recursive polynomial
         is OK. */
      tmp[0] = htonl(CMO_RECURSIVE_POLYNOMIAL);
      cmoOutputToBuf(CMOPUT,tmp,sizeof(cmoint));
      ob = KopRecursivePolynomial(ob);
      vlist = getoa(ob,0);
      vlist0 = newObjectArray(getoaSize(vlist));
      for (i=0; i<getoaSize(vlist); i++) {
        if (getoa(vlist,i).tag == Sdollar) {
          if (CmoClientMode) {
            putoa(vlist0,i,
                  KpoIndeterminate(getoa(vlist,i)));
          }else{
            putoa(vlist0,i,
                  KpoIndeterminate(cmoTranslateVariable_outGoing(getoa(vlist,i))));
          }
        }else{
          putoa(vlist0,i,getoa(vlist,i));
        }
      }
      cmoObjectToCmo00(vlist0); /* output the list of variables. */
      cmoObjectToCmo00(getoa(ob,1)); /* output the body of the recursive poly
                                        polynomial in one variable or any object*/
      break;
    case CLASSNAME_polynomialInOneVariable:
      tmp[0] = htonl(CMO_POLYNOMIAL_IN_ONE_VARIABLE);
      cmoOutputToBuf(CMOPUT,tmp,sizeof(cmoint));
      ob = KopPolynomialInOneVariable(ob);
      if (ob.tag != Sarray) {
        cmoObjectToCmo00(ob);
      }else{
        /* We do not check the format. */
        m = (getoaSize(ob)-1)/2; /* the number of monomials */
        cmoOutRawInt(m);
        ob2 = getoa(ob,0);       /* the variable name by integer. */
        if (ob2.tag != Sinteger) {
          warningCmo("cmoObjectToCmo00(): polynomial in one variable: this field should be integer. Output 0");
          /* cmoOutInt32(0); */
          cmoOutRawInt(0);
        }else{
          /* cmoObjectToCmo00(ob2); */
          cmoOutRawInt(KopInteger(ob2));
        }
        for (i=1; i<getoaSize(ob); i = i+2) {
          cmoOutRawInt(KopInteger(getoa(ob,i)));  /* exponent */
          cmoObjectToCmo00(getoa(ob,i+1));          /* coefficient */
        }
      }
      break;
    case CLASSNAME_tree:
      cmoOutRawInt(CMO_TREE);
      ob = KopTree(ob);
      cmoObjectToCmo00(getoa(ob,0));
      cmoObjectToCmo00(getoa(ob,1));
      cmoObjectToCmo00(getoa(ob,2));
      break;
    default:
      warningCmo("cmoObjectToCmo(): unknown etag for Sclass. Output CMO_NULL");
      cmoOutCmoNull(); /* otherwise core dump. */
      break;
    }
    break; 
  default:
    warningCmo("cmoObjectToCmo(): unknown tag. Output CMO_NULL");
    cmoOutCmoNull(); /* otherwise core dump. */
    break;
  }
  /* no flush */
}

struct object cmoObjectToCmo(struct object ob)
{
  struct object rob = OINIT;
  if (DebugCMO) {
    fprintf(stderr,"cmoObjectToCmo: ");
    printObject(ob,1,stderr);
  }
  cmoOutputToBuf(CMOINIT,NULL,0);
  cmoObjectToCmo00(ob);
  rob.tag = Sclass;
  rob.lc.ival = CMO;
  rob.rc.voidp = cmoOutputToBuf(CMOFLUSH,NULL,0);
  return(rob);
}
void cmoDumpCmo(struct object ob)
{
  if (ob.tag == Sclass && ob.lc.ival == CMO) {
    dumpCmoBuf((struct cmoBuffer *) ob.rc.voidp);
  }else {
    errorCmo("cmoDumpCmo(): Object is not CMO.");
  }
}

int Lisplike = 0;  
/* It is for debugging. Internal use only. */
/*  [(cmoLispLike) 1] extension can be used to turn this option on. */
/*  debug/cmotest.sm1 test11, test12 */

struct object cmoCmoToObject00(struct cmoBuffer *cb)
{
  struct object rob = OINIT;
  struct object ob1 = OINIT;
  struct object ob2 = OINIT;
  int tt,ival;
  int i,size;
  MP_INT *mi;
  MP_INT *mi2;
  struct ring *oldringp;
  char tmpc[16];
  struct object vlist = OINIT;
  struct object vlist0 = OINIT;
  int k;
  int m;
  struct object ob = OINIT;


  tt = cmoGetIntFromBuf(CMOGET,cb); /* read the tag */
  /* fprintf(stderr,"CmoToObject00: tag=%d\n",tt); */
  if (Lisplike) {
    printf("(%s[%x],", cmotagToName(tt),tt);
  }
  switch (tt) {
  case CMO_ERROR2:
    rob = KnewErrorPacketObj(cmoCmoToObject00(cb));
    if (Lisplike) { printObject(rob,0,stdout); }
    break;
  case CMO_ERROR:
    rob = KnewErrorPacketObj(KnewErrorPacket(SerialCurrent,-1,"CMO_ERROR"));
    break;
  case CMO_NULL:
    rob.tag = Snull; 
    break;
  case CMO_INT32:
    /* For test. */
    ival = cmoGetIntFromBuf(CMOGET,cb);
    rob = KpoInteger(ival);
    if (Lisplike) { printObject(rob,0,stdout); }
    break;
  case CMO_STRING:
    size = cmoGetIntFromBuf(CMOGET,cb);
    if (Lisplike) { printf("[size=%d],",size); }
    rob = KpoString((char *)cmoGetString(cb,size));
    if (Lisplike) { printObject(rob,0,stdout); }
    break;
  case CMO_MATHCAP:
    rob = newObjectArray(2);
    putoa(rob,0,KpoString("mathcap-object"));  
    /* We should create Class.mathcap in a future by KpoMathCap */
    ob1= cmoCmoToObject00(cb);
    putoa(rob,1,ob1);
    if (Lisplike) { printObject(rob,0,stdout); }
    break;
  case CMO_LIST:
  case CMO_ARRAY:
    size = cmoGetIntFromBuf(CMOGET,cb);
    if (size < 0) errorCmo("cmoCmoToObject00(): size of array is negative.");
    rob = newObjectArray(size);
    if (Lisplike) { printf("[size=%d],",size); }
    /* printf("size=%d\n",size); */
    for (i=0; i<size; i++) {
      putoa(rob,i,cmoCmoToObject00(cb));
      /* printObject(getoa(rob,i),0,stdout); */
      if (i==0) {
        ob1 = getoa(rob,0);
        if (ob1.tag == CMO+CMO_DMS) {
          goto dmscase ;
        }
      }
    }
    break;
  case CMO_DMS:
    rob.tag = CMO+CMO_DMS;  /* OK?? */
    break;
  case CMO_DISTRIBUTED_POLYNOMIAL:
    size = cmoGetIntFromBuf(CMOGET,cb);
    if (Lisplike) { printf("[size=]%d,",size); }
    if (size < 0) errorCmo("cmoCmoToObject00(): CMO_DISTRIBUTED_POLYNOMIAL : negative size field.");
    rob = newObjectArray(size);
    /* Case for DMS. */
    oldringp = CurrentRingp;
    ob1 = cmoCmoToObject00(cb);
    if (ob1.tag == Sdollar) {
      /* Change the CurrentRingp by looking up the name. */
      ob1 = KfindUserDictionary(KopString(ob1));
      if (ob1.tag != Sring) {
        errorCmo("cmoCmoToObject00(): your ring is not defined in the name space.");
      }
      CurrentRingp = KopRingp(ob1);
    }
    i = 0;
    while (i<size) {
      putoa(rob,i,cmoCmoToObject00(cb));
      i++;
    }
    CurrentRingp = oldringp;
    rob = cmoListToPoly2(rob);
    break;

  case CMO_DMS_GENERIC:
    rob.tag = Snull;
    break;
  case CMO_DMS_OF_N_VARIABLES:
    rob = cmoCmoToObject00(cb); /* list structure will come. */
    break;
  case CMO_RING_BY_NAME:
    rob = cmoCmoToObject00(cb); /* read the string. */
    break;
  case CMO_MONOMIAL32:
    rob = KpoPOLY(cmoGetMonomial32(cb));
    if (Lisplike) { printObject(rob,0,stdout); }
    break;
  case CMO_ZERO:
    rob = KpoPOLY(POLYNULL);
    break;
  case CMO_ZZ_OLD:
  case CMO_ZZ:
    if (OxVersion >= 199907170 && tt == CMO_ZZ_OLD)  {
      errorCmo("CMO_ZZ_OLD is not implemented.");
    }else if (OxVersion < 199907170 && tt == CMO_ZZ)  {
      errorCmo("CMO_ZZ is not implemented.");
    }
    mi = newMP_INT();
    cmoGetGMPCoeff(mi, cb);
    rob.tag = SuniversalNumber;
    rob.lc.universalNumber = mpintToCoeff(mi,SmallRingp);
    if (Lisplike) { printObject(rob,0,stdout); }
    break;
  case CMO_QQ:
    mi = newMP_INT();
    cmoGetGMPCoeff(mi, cb);
    mi2 = newMP_INT();
    cmoGetGMPCoeff(mi2, cb);
    ob1.tag = SuniversalNumber;
    ob1.lc.universalNumber = mpintToCoeff(mi,SmallRingp);
    ob2.tag = SuniversalNumber;
    ob2.lc.universalNumber = mpintToCoeff(mi2,SmallRingp);
    rob = KooDiv2(ob1,ob2);
    if (Lisplike) { printObject(rob,0,stdout); }
    break;
  case CMO_64BIT_MACHINE_DOUBLE:
    for (i=0; i<8; i++) {
      tmpc[i] = cmoGetIntFromBuf(CMOGETBYTE, cb);
    }
    rob = KpoDouble(*((double *)tmpc));
    if (Lisplike) { printObject(rob,0,stdout); }
    break;
  case CMO_INDETERMINATE:
    /* Old code. rob = KpoIndeterminate(cmoCmoToObject00(cb)); */
    /* If you need to change the names of indeterminates,
       do it here. */
    if (CmoClientMode) {
      rob = KpoIndeterminate(cmoCmoToObject00(cb));
    }else{
      rob = KpoIndeterminate(cmoTranslateVariable_inComming(cmoCmoToObject00(cb)));
    }
    break;
  case CMO_RECURSIVE_POLYNOMIAL:
    vlist0 = cmoCmoToObject00(cb); /* list of variables by indeterminates. */
    vlist = newObjectArray(getoaSize(vlist0));
    for (i=0; i<getoaSize(vlist0); i++) {
      ob1 = getoa(vlist0,i);
      if (ectag(ob1) == CLASSNAME_indeterminate) {
        ob1 = KopIndeterminate(ob1);
      }else if (ectag(ob1) == CLASSNAME_tree) {
        /* do nothing. */
      }
      putoa(vlist,i,ob1);
    }
    /* vlist is a list of variables by strings. */
    
    rob = newObjectArray(2);
    putoa(rob,0,vlist);
    putoa(rob,1,cmoCmoToObject00(cb));
    rob = KpoRecursivePolynomial(rob);
    if (!isRecursivePolynomial2(rob)) {
      errorCmo("cmoCmoToObject00(): invalid format of recursive polynomial. Return null.");
      rob.tag = Snull;
    }
    break;
  case CMO_POLYNOMIAL_IN_ONE_VARIABLE:
    m = cmoGetIntFromBuf(CMOGET,cb);
    if (Lisplike) { printf("[numberOfMonomials=%d],",m); }
    if (m < 0) {
      errorCmo("cmoCmoToObject00(): invalid size of polynomial in one variable.");
      rob.tag = Snull;
      break;
    }
    rob = newObjectArray(2*m+1);
    /* ob1 = cmoCmoToObject00(cb); 
       putoa(rob,0,ob1);  name of the variable */
    i = cmoGetIntFromBuf(CMOGET,cb);
    putoa(rob,0,KpoInteger(i));
    if (Lisplike) { printf("[the main variable=]%d,",i); }
    for (i=1; i< 2*m+1; i = i+2) {
      k = cmoGetIntFromBuf(CMOGET,cb); /* exponent */
      if (Lisplike) { printf("[exponent=]%d,",k); }
      putoa(rob,i,KpoInteger(k));
      ob2 = cmoCmoToObject00(cb); putoa(rob,i+1,ob2);
    }
    rob = KpoPolynomialInOneVariable(rob);
    break;
  case CMO_TREE:
    ob1 = cmoCmoToObject00(cb);
    ob2 = cmoCmoToObject00(cb);
    rob = newObjectArray(3);
    putoa(rob,0,ob1);
    putoa(rob,1,ob2);
    putoa(rob,2,cmoCmoToObject00(cb));
    if (getoaSize(rob) != 3) {
      warningCmo("CMO_TREE : the object is not an array of the length 3.");
    }else{
      ob1 = getoa(rob,0);
      if (ob1.tag != Sdollar) warningCmo("CMO_TREE : the first arg must be the node name by a string.");
      ob2 = getoa(rob,1); /* Attribute List */
      if (ob2.tag != Sarray) warningCmo("CMO_TREE : the second arg must be a list of attributes.");
      rob = KpoTree(rob);
    }
    break;
  case CMO_RATIONAL:
    ob1 = cmoCmoToObject00(cb);
    ob2 = cmoCmoToObject00(cb);
    rob = KooDiv2(ob1,ob2);
    if (Lisplike) { printObject(rob,0,stdout); }
    break;
  defaut:
    fprintf(stderr,"tag=%d (%x) ",tt,tt);
    errorCmo("cmoCmoToObject00(): unknown CMO tag. returns null object.");
    rob.tag = Snull;
    break;
  }
  if (Lisplike) { printf("),");  fflush(stdout); }
  return(rob);

 dmscase: ;
  /* Case for DMS. */
  oldringp = CurrentRingp;
  i = 1;
  if (i >= size) errorCmo("cmoCmoToObject00(): DMS, ring-def, ...");
  putoa(rob,i,cmoCmoToObject00(cb));
  ob1 = getoa(rob,1);
  if (ob1.tag == Sdollar) {
    /* Change the CurrentRingp by looking up the name. */
    ob1 = KfindUserDictionary(KopString(ob1));
    if (ob1.tag != Sring) {
      errorCmo("cmoCmoToObject00(): your ring is not defined in the name space.");
    }
    CurrentRingp = KopRingp(ob1);
  }
  i = 2;
  while (i<size) {
    putoa(rob,i,cmoCmoToObject00(cb));
    i++;
  }
  CurrentRingp = oldringp;
  if (Lisplike) { printf("),"); fflush(stdout); }
  return(rob);
}
    
struct object cmoCmoToObject(struct object ob)
{
  struct object rob = OINIT;
  struct object ob0 = OINIT;
  struct cmoBuffer *cb;
  if (!(ob.tag == Sclass && ob.lc.ival == CMO)) {
    rob.tag = Snull;
    errorCmo("cmoCmoToObject(): the argument is not CMO.");
    return(rob);
  }
  cb = (struct cmoBuffer *) ob.rc.voidp;
  cmoGetIntFromBuf(CMOINIT,cb);
  if (cb->pos == 0) {
    /* null */
    rob.tag = Snull;
    return(rob);
  }
  rob = cmoCmoToObject00(cb);
  rob = cmoListToPoly(rob);
  if (DebugCMO) {
    fprintf(stderr,"cmoCmoToObject: ");
    printObject(rob,1,stderr);
  }
  return(rob);
}

struct object cmoListToPoly(struct object ob) {
  struct object ob0 = OINIT;
  struct object rob = OINIT;
  int i,n;
  if (ob.tag == Sarray) {
    n = getoaSize(ob);
    if (n >= 1) {
      ob0 = getoa(ob,0);
      if (ob0.tag == CMO+CMO_DMS) {
        rob = KpoPOLY(cmoListToPOLY(ob)); /* not ToPoly, ToPOLY */
      }else{
        rob = newObjectArray(n);
        for (i=0; i<n; i++) {
          putoa(rob,i,cmoListToPoly(getoa(ob,i)));
        }
      }
    }else{
      rob = ob;
    }
  }else{
    rob = ob;
  }
  return(rob);
}

struct object cmoListToPoly2(struct object ob) 
{
  int size,i;
  struct object ob0 = OINIT;
  struct object ob1 = OINIT;
  POLY f;
  /*
    printf("<<");printObject(ob,0,stdout); printf(">>\n"); fflush(stdout);
  */
  if (ob.tag != Sarray) {
    errorCmo("cmoListToPoly2(): the argument must be array.");
  }
  size = getoaSize(ob);
  f = POLYNULL;
  for (i=size-1; i>=0; i--) {
    ob1 = getoa(ob,i);
    if (ob1.tag == Spoly) {
      f = ppAdd(f,KopPOLY(ob1));
    }else{
      errorCmo("cmoListToPoly2(): format error.");
    }
  }
  return(KpoPOLY(f));
}

/*
main() {
 int i;
 struct cmoBuffer *cb;
 printf("%d\n",sizeof(long int));
 for (i=0; i<300; i++) {
   cmoOutInt32(i);
 }
 dumpCmoBuf(cb=cmoOutputToBuf(CMOFLUSH,NULL,0));
 cmoGetIntFromBuf(CMOINIT,cb);
 for (i=0; i<300; i++) {
   printf("%5d",cmoGetIntFromBuf(CMOGET,cb));
 }
 putchar('\n');
}
*/

POLY cmoListToPOLY(struct object ob) 
{
  int size,i;
  struct object ob0 = OINIT;
  struct object ob1 = OINIT;
  POLY f;
  /*
    printf("<<");printObject(ob,0,stdout); printf(">>\n"); fflush(stdout);
  */
  if (ob.tag != Sarray) {
    errorCmo("cmoListToPOLY(): the argument must be array.");
  }
  size = getoaSize(ob);
  if (size < 2) {
    errorCmo("cmoListToPOLY(): the first element of the array must be CMO-tag.");
    errorCmo("cmoListToPOLY(): the second element must be the ring definition.");
  }
  ob0 = getoa(ob,0);
  ob1 = getoa(ob,1);  /* ring defintion. It is not used for now. */
  /* printObject(ob1,0,stdout); */
  switch(ob0.tag) {
  case (CMO+CMO_DMS):
    f = POLYNULL;
    for (i=size-1; i>=2; i--) {
      ob1 = getoa(ob,i);
      if (ob1.tag == Spoly) {
        f = ppAdd(f,KopPOLY(ob1));
      }else{
        f = ppAdd(f,cmoListToPOLY(ob1));
      }
    }
    return(f);
    break;
  default:
    errorCmo("cmoListToPoly(): unknown tag.");
    break;
  }
}


int Kan_PushBinary(int size,void *data)
{
  struct cmoBuffer cb;
  struct object ob = OINIT;
  cb.pos = size;
  cb.rpos = 0;
  cb.buf = data;
  cb.size = size;
  ob.tag = Sclass; ob.lc.ival = CMO;
  ob.rc.voidp = &cb;
  KSpush(cmoCmoToObject(ob));
  return(0);
}

void *Kan_PopBinary(int *sizep)
{
  struct object ob = OINIT;
  struct cmoBuffer *cb;
  ob = KSpop();
  ob = cmoObjectToCmo(ob);
  if (ob.tag == Sclass && ob.lc.ival == CMO) {
    cb = (struct cmoBuffer *) (ob.rc.voidp);
    *sizep = cb->pos;
    return(cb->buf);
  }
  return(NULL);
}





struct object cmoObjectFromStream(struct object obStream)
{
  struct cmoBuffer cb;
  struct object rob = OINIT;
  extern int DebugCMO;
  if (obStream.tag != Sfile) {
    errorCmo("cmoObjectFromStream: Argument must be of type file.");
  }
  if (strcmp(obStream.lc.str,MAGIC2) != 0) {
    errorCmo("cmoObjectFromStream: Argument must be of type plugin/file2 buffered IO.");
  }
  rob = cmoObjectFromStream2((FILE2 *)obStream.rc.voidp);
  if (DebugCMO >= 2) {
    fprintf(stderr,"cmoObjectFromStream: ");
    printObject(rob,1,stderr);
  }
  return(rob);
}
struct object cmoObjectFromStream2(FILE2 *fp2)
{
  struct cmoBuffer cb;
  struct object rob = OINIT;
  cb.isStream=1; cb.fp = fp2;
  cmoGetIntFromBuf(CMOINITSTREAM,&cb);
  rob = cmoCmoToObject00(&cb);
  rob = cmoListToPoly(rob);
  if (cb.errorno) errorCmo("at cmoObjectFromStream2");
  if (DebugCMO) {
    fprintf(stderr,"cmoObjectFromStream2: ");
    printObject(rob,1,stderr);
  }
  return(rob);
}

struct object cmoObjectToStream(struct object ob, struct object obStream)
{
  struct object rob = OINIT;
  extern int DebugCMO;
  if (obStream.tag != Sfile) {
    errorCmo("cmoObjectToStream: Argument must be of type file.");
  }
  if (strcmp(obStream.lc.str,MAGIC2) != 0) {
    errorCmo("cmoObjectToStream: Argument must be of type plugin/file2 buffered IO.");
  }
  if (DebugCMO >= 2) {
    fprintf(stderr,"cmoObjectToStream: ");
    printObject(ob,1,stderr);
  }
  return(cmoObjectToStream2(ob,(FILE2 *)obStream.rc.voidp));
}

struct object cmoObjectToStream2(struct object ob, FILE2 *fp2)
{
  struct object rob = OINIT;
  cmoOutputToBuf(CMOINITSTREAM,(void *)fp2,0);
  if (DebugCMO) {
    fprintf(stderr,"cmoObjectToStream2: ");
    printObject(ob,1,stderr);
  }
  cmoObjectToCmo00(ob);
  fp2fflush(fp2);
  rob = KpoInteger(0);
  return(rob);
}

int Kan_pushCMOFromStream(FILE2 *fp)
{
  struct object ob = OINIT;
  struct object rob = OINIT;
  ob.tag = Sfile; ob.rc.voidp = (void *)fp; ob.lc.str = MAGIC2;
  rob = cmoObjectFromStream(ob);
  KSpush(rob);
  return(0);
}

int Kan_popCMOToStream(FILE2 *fp,int serial)
{
  struct object ob = OINIT;
  struct object sob = OINIT;
  sob.tag = Sfile; sob.rc.file = (void *)fp; sob.lc.str = MAGIC2;
  ob = Kpop();
  /*outfp2(fp);*/ /* outfp2 is for debugging. see develop/97feb.. 1999, 1/19*/
  if (!cmoCheckMathCap(ob, (struct object *)(fp->mathcapList))) {
    fprintf(stderr,"%s\n","Protection by mathcap. You cannot send this object to your peer.");
    ob = KnewErrorPacket(serial,-1,"cmoObjectToStream: protection by mathcap");
    cmoObjectToStream(ob,sob);
    return(-1);
  }
  cmoObjectToStream(ob,sob);
  return(0);
}

int Kan_setMathCapToStream(FILE2 *fp,struct object ob) {
  struct object *obp;
  obp = (struct object *)sGC_malloc(sizeof(struct object));
  *obp = ob;
  fp->mathcapList = (void *)obp;
}

/* It is declared in oxmisc2.h, too. */
struct object newMathCap(struct mathCap *mathcap){
  struct object rob = OINIT;
  struct object ob1 = OINIT;
  struct object ob2 = OINIT;
  struct object ob3 = OINIT;
  struct object obOx = OINIT;
  struct object obSm = OINIT;
  struct object ob3tmp = OINIT;
  struct object *obp;
  int i,j;
  struct object mathinfo = OINIT;

  rob = newObjectArray(3);

  mathinfo = *((struct object *) (mathcap->infop));
  ob1 = newObjectArray(getoaSize(mathinfo));
  for (i=0; i<getoaSize(mathinfo); i++) {
    putoa(ob1,i,getoa(mathinfo,i));
  }
  ob3 = newObjectArray(mathcap->oxSize);
  for (i=0; i<mathcap->oxSize; i++) {
    ob3tmp = newObjectArray(2);
    putoa(ob3tmp,0,KpoInteger((mathcap->ox)[i]));
    ob2 = newObjectArray(mathcap->n);
    for (j=0; j<mathcap->n; j++) {
      putoa(ob2,j,KpoInteger((mathcap->cmo)[j]));
    }
    putoa(ob3tmp,1,ob2);
    putoa(ob3,i,ob3tmp);
  }

  obSm = newObjectArray(mathcap->smSize);
  for (i=0; i<mathcap->smSize; i++) {
    putoa(obSm,i,KpoInteger((mathcap->sm)[i]));
  }
  putoa(rob,0,ob1); /* Version , name etc */
  putoa(rob,1,obSm); /* SM tags */
  putoa(rob,2,ob3);  /* OX_DATA format, cmo types. */
  obp = (struct object *)sGC_malloc(sizeof(struct object));
  *obp = rob;
  return( KpoMathCap(obp) );
}

struct object KSmathCap(void)
{
  struct mathCap *mathcap;
  mathcap = KSmathCapByStruct();
  return(newMathCap(mathcap));
}

void *KSmathCapByStruct(void) 
     /* Return the math cap of kan/sm1 with cmo.c as a mathcap classObject*/
{
  struct mathCap *mathcap;
  struct object ob = OINIT;
  char *s1,*s2;
  struct object *mathinfo;
  char *sys;
  char *sysVersion;
  extern char *OxSystem;         /* Example :  ox_sm1.plain */
  extern char *OxSystemVersion;  /* Example :  0.1 */
  mathcap = (struct mathCap *)sGC_malloc(sizeof(struct mathCap));
  mathinfo = sGC_malloc(sizeof(struct object));

  sys = (char *) sGC_malloc(sizeof(char)*(strlen(OxSystem)+strlen("Ox_system=")+2));
  strcpy(sys,"Ox_system="); strcat(sys,OxSystem);
  sysVersion =
    (char *) sGC_malloc(sizeof(char)*(strlen(OxSystemVersion)+strlen("Version=")+2));
  strcpy(sysVersion,"Version="); strcat(sysVersion,OxSystemVersion);

  ob = newObjectArray(4);
  putoa(ob,0,KpoInteger(OxVersion)); /* Open XM protocol Version */
  /* The rest entries must be strings. See oxmisc2.c oxSendMathcap */
  putoa(ob,1,KpoString(sys));
  putoa(ob,2,KpoString(sysVersion));
  s1 = getenv("HOSTTYPE");
  if (s1 == NULL) s1="unknown";
  s2 = (char *)sGC_malloc(strlen(s1)+2+strlen("HOSTTYPE="));
  strcpy(s2,"HOSTTYPE=");
  strcat(s2,s1);
  putoa(ob,3,KpoString(s2));

  *mathinfo = ob;
  mathcap->infop = (void *) mathinfo;

  mathcap->cmo[0] = CMO_ERROR2;
  mathcap->cmo[1] = CMO_NULL;
  mathcap->cmo[2] = CMO_INT32;
  mathcap->cmo[3] = CMO_STRING;
  mathcap->cmo[4] = CMO_MATHCAP;
  mathcap->cmo[5] = CMO_LIST;
  mathcap->cmo[6] = CMO_MONOMIAL32;
  if (OxVersion >= 199907170) {
    mathcap->cmo[7] = CMO_ZZ;
  }else{
    mathcap->cmo[7] = CMO_ZZ_OLD;
  }
  mathcap->cmo[8] = CMO_ZERO;
  mathcap->cmo[9] = CMO_DMS;
  mathcap->cmo[10] = CMO_DMS_GENERIC;
  mathcap->cmo[11]= CMO_DMS_OF_N_VARIABLES;
  mathcap->cmo[12]= CMO_RING_BY_NAME;
  mathcap->cmo[13]= CMO_INT32COEFF;
  mathcap->cmo[14]= CMO_DISTRIBUTED_POLYNOMIAL;
  mathcap->cmo[15]= CMO_INDETERMINATE;
  mathcap->cmo[16]= CMO_TREE;
  mathcap->cmo[17]= CMO_RECURSIVE_POLYNOMIAL;
  mathcap->cmo[18]= CMO_POLYNOMIAL_IN_ONE_VARIABLE;
  mathcap->cmo[19]= CMO_64BIT_MACHINE_DOUBLE;
  mathcap->cmo[20]= CMO_ARRAY;
  mathcap->cmo[21]= CMO_RATIONAL;
  mathcap->cmo[22]= CMO_QQ;

  mathcap->n = 23 ;   /* This is the number of cmo object. You can use
                         cmo upto 1023. see mathcap.h */

  mathcap->ox[0] = OX_DATA;
  mathcap->oxSize = 1 ;   /* This is the number of OX object. You can use
                             OX upto 1023. see mathcap.h */

  mathcap->sm[0] = SM_popCMO;
  mathcap->sm[1] = SM_popString;
  mathcap->sm[2] = SM_mathcap;
  mathcap->sm[3] = SM_pops;
  mathcap->sm[4] = SM_setName;
  mathcap->sm[5] = SM_executeStringByLocalParser;
  mathcap->sm[6] = SM_executeFunction;
  mathcap->sm[7] = SM_shutdown;
  mathcap->sm[8] = SM_setMathCap;
  mathcap->sm[9] = SM_getsp;
  mathcap->sm[10] = SM_dupErrors;
  mathcap->sm[11] = SM_pushCMOtag;
  mathcap->sm[12] = SM_executeFunctionWithOptionalArgument;
  mathcap->sm[13] = SM_nop;
  mathcap->smSize = 14;  

  return((void *)mathcap);
}

int cmoOutRawInt(int k)
{
  cmoint tmp[1];
  tmp[0] = htonl((cmoint ) k);
  cmoOutputToBuf(CMOPUT,tmp,sizeof(cmoint));
}  

void warningCmo(char *s) {
  fprintf(stderr,"Warning: plugin/cmo.c : %s\n",s);
}

void errorCmo(char *s) {
  fprintf(stderr,"plugin/cmo.c : %s\n",s);
  errorKan1("%s\n","cmo fatal error. ox servers need SM_control_reset_connection.");
  /* ErrorPacket is automatically push on the ErrorStack.
     cf. var.sm1, [(ErrorStack)] system_variable */
  /*   KSexecuteString(" error "); */
}

/* for dubugging. Should be comment out later. */
int outfp2(FILE2 *fp2) {
  int i;
  printf("---------  outfp2 ---------\n"); fflush(stdout);
  /*  if (checkfp2(fp2," f2pdumpBuf ") == -1) {
      return(-1);
      }*/
  printf("fd=%d\n",fp2->fd);
  printf("initialied=%d\n",fp2->initialized);
  printf("readpos=%d\n",fp2->readpos);
  printf("readsize=%d\n",fp2->readsize);
  printf("writepos=%d\n",fp2->writepos);
  printf("limit=%d\n",fp2->limit);
  for (i=0; i<fp2->readsize; i++) {
    printf("readBuf[%d]=%2x ",i,fp2->readBuf[i]);
  }
  for (i=0; i<fp2->writepos; i++) {
    printf("writeBuf[%d]=%2x ",i,fp2->writeBuf[i]);
  }
  printf("\n");
  printObject(*( (struct object *)fp2->mathcapList),0,stdout);
  printf("\n");
  return(0);
}

static char *translateReservedName(char *s) {
  /* We do not translate h and q */
  if (strcmp(s,"e_") == 0) { /* Should read the @@@E.symbol value */
    return("#65_");
  }else if (strcmp(s,"E") == 0) {
    return("#45");
  }else if (strcmp(s,"H") == 0) {
    return("#48");
  }else if (strcmp(s,"PAD")==0) {
    return("#4FAD");
  }else {
    return(NULL);
  }
}
      
struct object cmoTranslateVariable_inComming(struct object ob) {
  /* ob must be Sdollar, return value must be Sdollar. */
  /* Read a variable name from an other system,
     and translate the variable name into
     a suitable sm1 variable name. */
  char *s;
  char *t;
  int n, i, count;
  if (ob.tag != Sdollar) errorCmo("cmoTranslateVariable_inComming: the argument must be a string.");
  s = KopString(ob);
  t = translateReservedName(s);
  if (t != NULL) {
    if (Lisplike) printf("[%s==>%s]",s,t);
    return(KpoString(t));
  }

  n = strlen(s);
  for (i=count=0; i<n; i++,count++) {
    if (s[i] <= ' ' || s[i] == '#') {
      count += 2;
    }
  }
  if (n != count) {
    t = (char *) sGC_malloc(sizeof(char)*(count+2));
    if (t == NULL) errorCmo("No more memory.");
    for (i=count=0; i<n; i++) {
      if (s[i] <= ' ' || s[i] == '#') {
        t[count++] = '#';
        t[count++] = (s[i]/16 < 10? s[i]/16+'0': (s[i]/16-10)+'A');
        t[count++] = (s[i]%16 < 10? s[i]%16+'0': (s[i]%16-10)+'A');
      }else{
        t[count++] = s[i];
      }
    }
    t[count] = '\0';
  }else{
    t = s;
  }
  if (Lisplike) {
    printf("[%s==>%s]",s,t);
  }
  return(KpoString(t));
}

#define isHex_cmo(a) ((a >= '0' && a <='9') || (a>='A' && a <='F')?1:0)
#define hexnum_cmo(a) (a>='A' && a <='F'? a-'A'+10 : a-'0')
struct object cmoTranslateVariable_outGoing(struct object ob) {
  /* ob must be Sdollar, return value must be Sdollar. */
  char *s, *t;
  int i,j,n;
  int p;
  if (ob.tag != Sdollar) errorCmo("cmoTranslateVariable_inComming: the argument must be a string.");
  s = KopString(ob);
  n = strlen(s);
  for (i=0; i<n; i++) {
    if (i < n-2 && s[i] == '#' && isHex_cmo(s[i+1]) && isHex_cmo(s[i+2])) {
      t = (char *) sGC_malloc(sizeof(char)*(n+2));
      if (t == NULL) errorCmo("No more memory.");
      break;
    }
    if (i== n-1) {
      return(KpoString(s));
    }
  }
  for (i=j=0; i<n; i++) {
    if (i < n-2 && s[i] == '#' && isHex_cmo(s[i+1]) && isHex_cmo(s[i+2])) {
      p = (hexnum_cmo(s[i+1]))*16+hexnum_cmo(s[i+2]);
      t[j++] = p; i += 2;
    }else{
      t[j++] = s[i];
    }
  }
  t[j] = '\0';
  if (Lisplike) {
    printf("[%s-->%s]",s,t);
  }
  return(KpoString(t));
}

