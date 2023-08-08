/* $OpenXM: OpenXM/src/kan96xx/Kan/kanExport0.c,v 1.49 2015/10/08 11:49:37 takayama Exp $  */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "datatype.h"
#include "stackm.h"
#include "extern.h"
#include "extern2.h"
#include "lookup.h"
#include "matrix.h"
#include "gradedset.h"
#include "kclass.h"

#define universalToPoly(un,rp) (isZero(un)?ZERO:coeffToPoly(un,rp))

static void checkDuplicateName(char *xvars[],char *dvars[],int n);

static void yet() { fprintf(stderr,"Not implemented."); }

int SerialCurrent = -1;  /* Current Serial number of the recieved packet as server. */

int ReverseOutputOrder = 1;  
int WarningNoVectorVariable = 1;
extern int QuoteMode;

/** :arithmetic **/
struct object KooAdd(ob1,ob2)
     struct object ob1,ob2;
{
  extern struct ring *CurrentRingp;
  struct object rob = NullObject;
  POLY r;
  int s,i;
  objectp f1,f2,g1,g2;
  struct object nn = OINIT;
  struct object dd = OINIT;
  
  switch (Lookup[ob1.tag][ob2.tag]) {
  case SintegerSinteger:
    return(KpoInteger(ob1.lc.ival + ob2.lc.ival));
    break;
  case SpolySpoly:
    r = ppAdd(ob1.lc.poly,ob2.lc.poly);
    rob.tag = Spoly; rob.lc.poly = r;
    return(rob);
    break;
  case SarraySarray:
    s = getoaSize(ob1);
    if (s != getoaSize(ob2)) {
      errorKan1("%s\n","Two arrays must have a same size.");
    }
    rob = newObjectArray(s);
    for (i=0; i<s; i++) {
      putoa(rob,i,KooAdd(getoa(ob1,i),getoa(ob2,i)));
    }
    return(rob);
    break;
  case SuniversalNumberSuniversalNumber:
    rob.tag = SuniversalNumber;
    rob.lc.universalNumber = newUniversalNumber(0);
    Cadd(rob.lc.universalNumber,ob1.lc.universalNumber,ob2.lc.universalNumber);
    return(rob);
    break;
  case SuniversalNumberSpoly:
    rob.tag = Spoly;
    r = ob2.lc.poly;
    if (r ISZERO) {
      /*warningKan("KooAdd(universalNumber,0 polynomial) cannot determine the ring for the result. Assume the current ring.");
        rob.lc.poly = universalToPoly(ob1.lc.universalNumber,CurrentRingp);*/
      rob = ob1;  
      return(rob); /* returns universal number. */
    }
    rob.lc.poly = ppAdd(universalToPoly(ob1.lc.universalNumber,r->m->ringp),r);
    return(rob);
    break;
  case SpolySuniversalNumber:
    return(KooAdd(ob2,ob1));
    break;
  case SuniversalNumberSinteger:
    rob.tag = SuniversalNumber;
    rob.lc.universalNumber = newUniversalNumber(0);
    nn.tag = SuniversalNumber;
    nn.lc.universalNumber = newUniversalNumber(KopInteger(ob2));
    Cadd(rob.lc.universalNumber,ob1.lc.universalNumber,nn.lc.universalNumber);
    return(rob);
    break;
  case SintegerSuniversalNumber:
    rob.tag = SuniversalNumber;
    rob.lc.universalNumber = newUniversalNumber(0);
    nn.tag = SuniversalNumber;
    nn.lc.universalNumber = newUniversalNumber(KopInteger(ob1));
    Cadd(rob.lc.universalNumber,nn.lc.universalNumber,ob2.lc.universalNumber);
    return(rob);
    break;

  case SrationalFunctionSrationalFunction:
    f1 = Knumerator(ob1);
    f2 = Kdenominator(ob1);
    g1 = Knumerator(ob2);
    g2 = Kdenominator(ob2);
    nn = KooAdd(KooMult(*g2,*f1),KooMult(*f2,*g1));
    dd = KooMult(*f2,*g2);
    rob = KnewRationalFunction0(copyObjectp(&nn),copyObjectp(&dd));
    KisInvalidRational(&rob);
    return(rob);
    break;
  case SpolySrationalFunction:  /* f1 + g1/g2 = (g2 f1 + g1)/g2 */
  case SuniversalNumberSrationalFunction:
    g1 = Knumerator(ob2);
    g2 = Kdenominator(ob2);
    nn = KooAdd(KooMult(*g2,ob1),*g1);
    rob = KnewRationalFunction0(copyObjectp(&nn),g2);
    KisInvalidRational(&rob);
    return(rob);
    break;
  case SrationalFunctionSpoly:
  case SrationalFunctionSuniversalNumber:
    return(KooAdd(ob2,ob1));
    break;
  case SdoubleSdouble:
    return(KpoDouble( KopDouble(ob1) + KopDouble(ob2) ));
    break;
  case SdoubleSinteger:
  case SdoubleSuniversalNumber:
  case SdoubleSrationalFunction:
    return(KpoDouble( KopDouble(ob1) + toDouble0(ob2) ) );
    break;
  case SintegerSdouble:
  case SuniversalNumberSdouble:
  case SrationalFunctionSdouble:
    return(KpoDouble( toDouble0(ob1) + KopDouble(ob2) ) );
    break;
  case SclassSclass:
  case SclassSinteger:
  case SclassSpoly:
  case SclassSuniversalNumber:
  case SclassSrationalFunction:
  case SclassSdouble:
  case SpolySclass:
  case SintegerSclass:
  case SuniversalNumberSclass:
  case SrationalFunctionSclass:
  case SdoubleSclass:
    return(Kclass_ooAdd(ob1,ob2));
    break;
    

  default:
    if (QuoteMode) {
      rob = addTree(ob1,ob2);
    }else{
      warningKan("KooAdd() has not supported yet these objects.\n");
    }
    break;
  }
  return(rob);
}

struct object KooSub(ob1,ob2)
     struct object ob1,ob2;
{
  struct object rob = NullObject;
  POLY r;
  int s,i;
  objectp f1,f2,g1,g2;
  extern struct coeff *UniversalZero;
  struct object nn = OINIT;
  struct object dd = OINIT;
  
  switch (Lookup[ob1.tag][ob2.tag]) {
  case SintegerSinteger:
    return(KpoInteger(ob1.lc.ival - ob2.lc.ival));
    break;
  case SpolySpoly:
    r = ppSub(ob1.lc.poly,ob2.lc.poly);
    rob.tag = Spoly; rob.lc.poly = r;
    return(rob);
    break;
  case SarraySarray:
    s = getoaSize(ob1);
    if (s != getoaSize(ob2)) {
      errorKan1("%s\n","Two arrays must have a same size.");
    }
    rob = newObjectArray(s);
    for (i=0; i<s; i++) {
      putoa(rob,i,KooSub(getoa(ob1,i),getoa(ob2,i)));
    }
    return(rob);
    break;
  case SuniversalNumberSuniversalNumber:
    rob.tag = SuniversalNumber;
    rob.lc.universalNumber = newUniversalNumber(0);
    Csub(rob.lc.universalNumber,ob1.lc.universalNumber,ob2.lc.universalNumber);
    return(rob);
    break;

  case SuniversalNumberSpoly:
    rob.tag = Spoly;
    r = ob2.lc.poly;
    if (r ISZERO) {
      rob = ob1;  
      return(rob); /* returns universal number. */
    }
    rob.lc.poly = ppSub(universalToPoly(ob1.lc.universalNumber,r->m->ringp),r);
    return(rob);
    break;
  case SpolySuniversalNumber:
    rob.tag = Spoly;
    r = ob1.lc.poly;
    if (r ISZERO) {
      rob.tag = SuniversalNumber;
      rob.lc.universalNumber = newUniversalNumber(0);
      Csub(rob.lc.universalNumber,UniversalZero,ob2.lc.universalNumber);
      return(rob); /* returns universal number. */
    }
    rob.lc.poly = ppSub(r,universalToPoly(ob2.lc.universalNumber,r->m->ringp));
    return(rob);
    break;

  case SuniversalNumberSinteger:
    rob.tag = SuniversalNumber;
    rob.lc.universalNumber = newUniversalNumber(0);
    nn.tag = SuniversalNumber;
    nn.lc.universalNumber = newUniversalNumber(KopInteger(ob2));
    Csub(rob.lc.universalNumber,ob1.lc.universalNumber,nn.lc.universalNumber);
    return(rob);
    break;
  case SintegerSuniversalNumber:
    rob.tag = SuniversalNumber;
    rob.lc.universalNumber = newUniversalNumber(0);
    nn.tag = SuniversalNumber;
    nn.lc.universalNumber = newUniversalNumber(KopInteger(ob1));
    Csub(rob.lc.universalNumber,nn.lc.universalNumber,ob2.lc.universalNumber);
    return(rob);
    break;

  case SrationalFunctionSrationalFunction:
    f1 = Knumerator(ob1);
    f2 = Kdenominator(ob1);
    g1 = Knumerator(ob2);
    g2 = Kdenominator(ob2);
    nn = KooSub(KooMult(*g2,*f1),KooMult(*f2,*g1));
    dd = KooMult(*f2,*g2);
    rob = KnewRationalFunction0(copyObjectp(&nn),copyObjectp(&dd));
    KisInvalidRational(&rob);
    return(rob);
    break;
  case SpolySrationalFunction:  /* f1 - g1/g2 = (g2 f1 - g1)/g2 */
  case SuniversalNumberSrationalFunction:
    g1 = Knumerator(ob2);
    g2 = Kdenominator(ob2);
    nn = KooSub(KooMult(*g2,ob1),*g1);
    rob = KnewRationalFunction0(copyObjectp(&nn),g2);
    KisInvalidRational(&rob);
    return(rob);
    break;
  case SrationalFunctionSpoly:
  case SrationalFunctionSuniversalNumber: /* f1/f2 - ob2= (f1 - f2*ob2)/f2 */
    f1 = Knumerator(ob1);
    f2 = Kdenominator(ob1);
    nn = KooSub(*f1,KooMult(*f2,ob2));
    rob = KnewRationalFunction0(copyObjectp(&nn),f2);
    KisInvalidRational(&rob);
    return(rob);
    break;

  case SdoubleSdouble:
    return(KpoDouble( KopDouble(ob1) - KopDouble(ob2) ));
    break;
  case SdoubleSinteger:
  case SdoubleSuniversalNumber:
  case SdoubleSrationalFunction:
    return(KpoDouble( KopDouble(ob1) - toDouble0(ob2) ) );
    break;
  case SintegerSdouble:
  case SuniversalNumberSdouble:
  case SrationalFunctionSdouble:
    return(KpoDouble( toDouble0(ob1) - KopDouble(ob2) ) );
    break;

  default:
    if (QuoteMode) {
      rob = minusTree(ob1,ob2);
    }else{
      warningKan("KooSub() has not supported yet these objects.\n");
    }
    break;
  }
  return(rob);
}

struct object KooMult(ob1,ob2)
     struct object ob1,ob2;
{
  struct object rob = NullObject;
  POLY r;
  int i,s;
  objectp f1,f2,g1,g2;
  struct object dd = OINIT;
  struct object nn = OINIT;

  
  switch (Lookup[ob1.tag][ob2.tag]) {
  case SintegerSinteger:
    return(KpoInteger(ob1.lc.ival * ob2.lc.ival));
    break;
  case SpolySpoly:
    r = ppMult(ob1.lc.poly,ob2.lc.poly);
    rob.tag = Spoly; rob.lc.poly = r;
    return(rob);
    break;
  case SarraySarray:
    return(KaoMult(ob1,ob2));
    break;
  case SpolySarray:
  case SuniversalNumberSarray:
  case SrationalFunctionSarray:
  case SintegerSarray:
    s = getoaSize(ob2);
    rob = newObjectArray(s);
    for (i=0; i<s; i++) {
      putoa(rob,i,KooMult(ob1,getoa(ob2,i)));
    }
    return(rob);
    break;

  case SarraySpoly:
  case SarraySuniversalNumber:
  case SarraySrationalFunction:
  case SarraySinteger:
    s = getoaSize(ob1);
    rob = newObjectArray(s);
    for (i=0; i<s; i++) {
      putoa(rob,i,KooMult(getoa(ob1,i),ob2));
    }
    return(rob);
    break;
    

  case SuniversalNumberSuniversalNumber:
    rob.tag = SuniversalNumber;
    rob.lc.universalNumber = newUniversalNumber(0);
    Cmult(rob.lc.universalNumber,ob1.lc.universalNumber,ob2.lc.universalNumber);
    return(rob);
    break;

  case SuniversalNumberSpoly:
    r = ob2.lc.poly;
    if (r ISZERO) {
      rob.tag = SuniversalNumber;  
      rob.lc.universalNumber = newUniversalNumber(0);
      return(rob); /* returns universal number. */
    }
    if (isZero(ob1.lc.universalNumber)) {
      rob.tag = Spoly;
      rob.lc.poly = ZERO;
      return(rob);
    }
    rob.tag = Spoly;
    rob.lc.poly = ppMult(universalToPoly(ob1.lc.universalNumber,r->m->ringp),r);
    return(rob);
    break;
  case SpolySuniversalNumber:
    return(KooMult(ob2,ob1));
    break;
    
  case SuniversalNumberSinteger:
    rob.tag = SuniversalNumber;
    rob.lc.universalNumber = newUniversalNumber(0);
    nn.tag = SuniversalNumber;
    nn.lc.universalNumber = newUniversalNumber(KopInteger(ob2));
    Cmult(rob.lc.universalNumber,ob1.lc.universalNumber,nn.lc.universalNumber);
    return(rob);
    break;
  case SintegerSuniversalNumber:
    rob.tag = SuniversalNumber;
    rob.lc.universalNumber = newUniversalNumber(0);
    nn.tag = SuniversalNumber;
    nn.lc.universalNumber = newUniversalNumber(KopInteger(ob1));
    Cmult(rob.lc.universalNumber,nn.lc.universalNumber,ob2.lc.universalNumber);
    return(rob);
    break;

  case SrationalFunctionSrationalFunction:
    f1 = Knumerator(ob1);
    f2 = Kdenominator(ob1);
    g1 = Knumerator(ob2);
    g2 = Kdenominator(ob2);
    nn = KooMult(*f1,*g1);
    dd = KooMult(*f2,*g2);
    rob = KnewRationalFunction0(copyObjectp(&nn),copyObjectp(&dd));
    KisInvalidRational(&rob);
    return(rob);
    break;
  case SpolySrationalFunction:  /* ob1 g1/g2 */
  case SuniversalNumberSrationalFunction:
    g1 = Knumerator(ob2);
    g2 = Kdenominator(ob2);
    nn = KooMult(ob1,*g1);
    rob = KnewRationalFunction0(copyObjectp(&nn),g2);
    KisInvalidRational(&rob);
    return(rob);
    break;
  case SrationalFunctionSpoly:
  case SrationalFunctionSuniversalNumber: /* f1*ob2/f2 */
    f1 = Knumerator(ob1);
    f2 = Kdenominator(ob1);
    nn = KooMult(*f1,ob2);
    rob = KnewRationalFunction0(copyObjectp(&nn),f2);
    KisInvalidRational(&rob);
    return(rob);
    break;

  case SdoubleSdouble:
    return(KpoDouble( KopDouble(ob1) * KopDouble(ob2) ));
    break;
  case SdoubleSinteger:
  case SdoubleSuniversalNumber:
  case SdoubleSrationalFunction:
    return(KpoDouble( KopDouble(ob1) * toDouble0(ob2) ) );
    break;
  case SintegerSdouble:
  case SuniversalNumberSdouble:
  case SrationalFunctionSdouble:
    return(KpoDouble( toDouble0(ob1) * KopDouble(ob2) ) );
    break;

  default:
    if (QuoteMode) {
      rob = timesTree(ob1,ob2);
    }else{
      warningKan("KooMult() has not supported yet these objects.\n");
    }
    break;
  }
  return(rob);
}



struct object KoNegate(obj)
     struct object obj;
{
  struct object rob = NullObject;
  extern struct ring SmallRing;
  struct object tob = OINIT;
  switch(obj.tag) {
  case Sinteger:
    rob = obj;
    rob.lc.ival = -rob.lc.ival;
    break;
  case Spoly:
    rob.tag = Spoly;
    rob.lc.poly = ppSub(ZERO,obj.lc.poly);
    break;
  case SuniversalNumber:
    rob.tag = SuniversalNumber;
    rob.lc.universalNumber = coeffNeg(obj.lc.universalNumber,&SmallRing);
    break;
  case SrationalFunction:
    rob.tag = SrationalFunction;
    tob = KoNegate(*(Knumerator(obj)));
    Knumerator(rob) = copyObjectp( &tob);
    Kdenominator(rob) = Kdenominator(obj);
    break;

  case Sdouble:
    rob = KpoDouble( - toDouble0(obj) );
    break;

  default:
    if (QuoteMode) {
      rob = unaryminusTree(obj);
    }else{
      warningKan("KoNegate() has not supported yet these objects.\n");
    }
    break;
  }
  return(rob);
}
    
struct object KoInverse(obj)
     struct object obj;
{
  struct object rob = NullObject;
  extern struct coeff *UniversalOne;
  objectp onep;
  struct object tob = OINIT;
  switch(obj.tag) {
  case Spoly:
    tob.tag = SuniversalNumber;
    tob.lc.universalNumber = UniversalOne;
    onep = copyObjectp(& tob);
    rob = KnewRationalFunction0(onep,copyObjectp(&obj));
    KisInvalidRational(&rob);
    break;
  case SuniversalNumber:
    tob.tag = SuniversalNumber;
    tob.lc.universalNumber = UniversalOne;
    onep = copyObjectp(& tob);
    rob = KnewRationalFunction0(onep,copyObjectp(&obj));
    KisInvalidRational(&rob);
    break;
  case SrationalFunction:
    rob = obj;
    Knumerator(rob) = Kdenominator(obj);
    Kdenominator(rob) = Knumerator(obj);
    KisInvalidRational(&rob);
    break;
  default:
    warningKan("KoInverse() has not supported yet these objects.\n");
    break;
  }
  return(rob);
}
    

static int isVector(ob)
     struct object ob;
{
  int i,n;
  n = getoaSize(ob);
  for (i=0; i<n; i++) {
    if (getoa(ob,i).tag == Sarray) return(0);
  }
  return(1);
}

static int isMatrix(ob,m,n)
     struct object ob;
     int m,n;
{
  int i,j;
  for (i=0; i<m; i++) {
    if (getoa(ob,i).tag != Sarray) return(0);
    if (getoaSize(getoa(ob,i)) != n) return(0);
    for (j=0; j<n; j++) {
      if (getoa(getoa(ob,i),j).tag != Spoly) return(-1);
    }
  }
  return(1);
}


struct object KaoMult(aa,bb)
     struct object aa,bb;
     /* aa and bb is assumed to be array. */
{
  int m,n,m2,n2; 
  int i,j,k;
  POLY tmp;
  POLY fik;
  POLY gkj;
  struct object rob = OINIT;
  int r1,r2;
  int rsize;
  struct object tob = OINIT;
  struct object ob1 = OINIT;
  extern struct ring SmallRing;

  m = getoaSize(aa); m2 = getoaSize(bb);
  if (m == 0 || m2 == 0) errorKan1("%s\n","KaoMult(). Invalid matrix size.");

  /*  new code for vector x vector,... etc */
  r1 = isVector(aa); r2 = isVector(bb);
  if (r1 && r2 ) { /* vector X vector ---> scalar.*/
    rsize = getoaSize(aa);
    if (rsize != getoaSize(bb)) {
      errorKan1("%s\n","KaoMult(vector,vector). The size of the vectors must be the same.");
    }
    if (r1 != 0) {
      ob1 = getoa(aa,0);
      if (ob1.tag == Spoly) {
        rob.tag = Spoly; rob.lc.poly = ZERO;
      }else if (ob1.tag == Sinteger) {
        rob.tag = Sinteger; rob.lc.ival = 0;
      }else {
        rob.tag = SuniversalNumber;
        rob.lc.universalNumber = intToCoeff(0,&SmallRing);
      }
    }else{
      rob.tag = Spoly; rob.lc.poly = ZERO;
    }
    for (i=0; i<rsize; i++) {
      rob = KooAdd(rob,KooMult(getoa(aa,i),getoa(bb,i)));
    }
    return(rob);
  } else if (r1 == 0 && r2 ) { /* matrix X vector ---> vector */
    /* (m n) (m2=n) */
    n = getoaSize(getoa(aa,0));
    if (isMatrix(aa,m,n) == 0) {
      errorKan1("%s\n","KaoMult(matrix,vector). The left object is not matrix.");
    }else if (n != m2) {
      errorKan1("%s\n","KaoMult(). Invalid matrix and vector sizes for mult.");
    } else ;
    rob = newObjectArray(m);
    for (i=0; i<m; i++) {
      getoa(rob,i) = KooMult(getoa(aa,i),bb);
    }
    return(rob);
  }else if (r1 && r2 == 0) { /* vector X matrix ---> vector */
    tob = newObjectArray(1);
    getoa(tob,0) = aa;  /* [aa] * bb and strip [ ] */
    tob = KooMult(tob,bb);
    return(getoa(tob,0));
  } else ; /* continue: matrix X matrix case. */
  /* end of new code */

  if (getoa(aa,0).tag != Sarray || getoa(bb,0).tag != Sarray) {
    errorKan1("%s\n","KaoMult(). Matrix must be given.");
  }
  n = getoaSize(getoa(aa,0)); 
  n2 = getoaSize(getoa(bb,0));
  if (n != m2) errorKan1("%s\n","KaoMult(). Invalid matrix size for mult. ((p,q)X(q,r)");
  r1 = isMatrix(aa,m,n); r2 = isMatrix(bb,m2,n2);
  if (r1 == -1 || r2 == -1) {
    /* Object multiplication. Elements are not polynomials. */
    struct object ofik = OINIT;
	struct object ogkj = OINIT;
	struct object otmp = OINIT;
    rob = newObjectArray(m);
    for (i=0; i<m; i++) {
      getoa(rob,i) = newObjectArray(n2);
    }
    for (i=0; i<m; i++) {
      for (j=0; j<n2; j++) {
        ofik = getoa(getoa(aa,i),0);
        ogkj = getoa(getoa(bb,0),j);
        otmp = KooMult( ofik, ogkj);
        for (k=1; k<n; k++) {
          ofik = getoa(getoa(aa,i),k);
          ogkj = getoa(getoa(bb,k),j);
          otmp = KooAdd(otmp, KooMult( ofik, ogkj));
        }
        getoa(getoa(rob,i),j) = otmp;
      }
    }
    return(rob);
    /*errorKan1("%s\n","KaoMult().Elements of the matrix must be polynomials.");*/
  }
  if (r1 == 0 || r2 == 0)
    errorKan1("%s\n","KaoMult(). Invalid matrix form for mult.");

  rob = newObjectArray(m);
  for (i=0; i<m; i++) {
    getoa(rob,i) = newObjectArray(n2);
  }
  for (i=0; i<m; i++) {
    for (j=0; j<n2; j++) {
      tmp = ZERO;
      for (k=0; k<n; k++) {
        fik = KopPOLY(getoa(getoa(aa,i),k));
        gkj = KopPOLY(getoa(getoa(bb,k),j));
        tmp = ppAdd(tmp, ppMult( fik, gkj));
      }
      getoa(getoa(rob,i),j) = KpoPOLY(tmp);
    }
  }
  return(rob);
}

struct object KooDiv(ob1,ob2)
     struct object ob1,ob2;
{
  struct object rob = NullObject;
  switch (Lookup[ob1.tag][ob2.tag]) {
  case SintegerSinteger:
    return(KpoInteger((ob1.lc.ival) / (ob2.lc.ival)));
    break;
  case SuniversalNumberSuniversalNumber:
    rob.tag = SuniversalNumber;
    rob.lc.universalNumber = newUniversalNumber(0);
    universalNumberDiv(rob.lc.universalNumber,ob1.lc.universalNumber,
                       ob2.lc.universalNumber);
    return(rob);
    break;


  default:
    if (QuoteMode) {
      rob = divideTree(ob1,ob2);
    }else{
      warningKan("KooDiv() has not supported yet these objects.\n");
    }
    break;
  }
  return(rob);
}

/* :relation */
int KooEqualQ(obj1,obj2)
     struct object obj1;
     struct object obj2;
{
  struct object ob = OINIT;
  int i;
  extern int Verbose;
  if (obj1.tag != obj2.tag) {
    warningKan("KooEqualQ(ob1,ob2): the datatypes of ob1 and ob2  are not same. Returns false (0).\n");
	if (Verbose & 0x10) {
	  fprintf(stderr,"obj1(tag:%d)=",obj1.tag);
	  printObject(obj1,0,stderr);
	  fprintf(stderr,", obj2(tag:%d)=",obj2.tag);
	  printObject(obj2,0,stderr);
	  fprintf(stderr,"\n"); fflush(stderr);
	}
    return(0);
  }
  switch(obj1.tag) {
  case 0:
    return(1); /* case of NullObject */
    break;
  case Sinteger:
    if (obj1.lc.ival == obj2.lc.ival) return(1);
    else return(0);
    break;
  case Sstring:
  case Sdollar:
    if (strcmp(obj1.lc.str, obj2.lc.str)==0) return(1);
    else return(0);
    break;
  case Spoly:
    ob = KooSub(obj1,obj2);
    if (KopPOLY(ob) == ZERO) return(1);
    else return(0);
  case Sarray:
    if (getoaSize(obj1) != getoaSize(obj2)) return(0);
    for (i=0; i< getoaSize(obj1); i++) {
      if (KooEqualQ(getoa(obj1,i),getoa(obj2,i))) { ; }
      else { return(0); }
    }
    return(1);
  case Slist:
    if (KooEqualQ(*(obj1.lc.op),*(obj2.lc.op))) {
      if (isNullList(obj1.rc.op)) {
        if (isNullList(obj2.rc.op)) return(1);
        else return(0);
      }else{
        if (isNullList(obj2.rc.op)) return(0);
        return(KooEqualQ(*(obj1.rc.op),*(obj2.rc.op)));
      }
    }else{
      return(0);
    }
    break;
  case SuniversalNumber:
    return(coeffEqual(obj1.lc.universalNumber,obj2.lc.universalNumber));
    break;
  case Sring:
    return(KopRingp(obj1) == KopRingp(obj2));
    break;
  case Sclass:
    return(KclassEqualQ(obj1,obj2));
    break;
  case Sdouble:
    return(KopDouble(obj1) == KopDouble(obj2));
    break;
  default:
    errorKan1("%s\n","KooEqualQ() has not supported these objects yet.");
    break;
  }
}
      

struct object KoIsPositive(ob1)
     struct object ob1;
{
  struct object rob = NullObject;
  switch (ob1.tag) {
  case Sinteger:
    return(KpoInteger(ob1.lc.ival > 0));
    break;
  default:
    warningKan("KoIsPositive() has not supported yet these objects.\n");
    break;
  }
  return(rob);
}

struct object KooGreater(obj1,obj2)
     struct object obj1;
     struct object obj2;
{
  struct object ob = OINIT;
  int tt;
  if (obj1.tag != obj2.tag) {
    errorKan1("%s\n","You cannot compare different kinds of objects.");
  }
  switch(obj1.tag) {
  case 0:
    return(KpoInteger(1)); /* case of NullObject */
    break;
  case Sinteger:
    if (obj1.lc.ival > obj2.lc.ival) return(KpoInteger(1));
    else return(KpoInteger(0));
    break;
  case Sstring:
  case Sdollar:
    if (strcmp(obj1.lc.str, obj2.lc.str)>0) return(KpoInteger(1));
    else return(KpoInteger(0));
    break;
  case Spoly:
    if ((*mmLarger)(obj1.lc.poly,obj2.lc.poly) == 1) return(KpoInteger(1));
    else return(KpoInteger(0));
    break;
  case SuniversalNumber:
    tt = coeffGreater(obj1.lc.universalNumber,obj2.lc.universalNumber);
    if (tt > 0) return(KpoInteger(1));
    else return(KpoInteger(0));
    break;
  case Sdouble:
    if ( KopDouble(obj1) > KopDouble(obj2) ) return(KpoInteger(1));
    else return(KpoInteger(0));
    break;
  case Sarray:
  {
    int i,m1,m2;
    struct object rr = OINIT;
    m1 = getoaSize(obj1); m2 = getoaSize(obj2);
    for (i=0; i< (m1>m2?m2:m1); i++) {
      rr=KooGreater(getoa(obj1,i),getoa(obj2,i));
      if (KopInteger(rr) == 1) return rr;
      rr=KooGreater(getoa(obj2,i),getoa(obj1,i));
      if (KopInteger(rr) == 1) return KpoInteger(0);
    }
    if (m1 > m2) return KpoInteger(1);
    else return KpoInteger(0);
  }
  break;
  default:
    errorKan1("%s\n","KooGreater() has not supported these objects yet.");
    break;
  }
}
      
struct object KooLess(obj1,obj2)
     struct object obj1;
     struct object obj2;
{
  struct object ob;
  int tt;
  if (obj1.tag != obj2.tag) {
    errorKan1("%s\n","You cannot compare different kinds of objects.");
  }
  switch(obj1.tag) {
  case 0:
    return(KpoInteger(1)); /* case of NullObject */
    break;
  case Sinteger:
    if (obj1.lc.ival < obj2.lc.ival) return(KpoInteger(1));
    else return(KpoInteger(0));
    break;
  case Sstring:
  case Sdollar:
    if (strcmp(obj1.lc.str, obj2.lc.str)<0) return(KpoInteger(1));
    else return(KpoInteger(0));
    break;
  case Spoly:
    if ((*mmLarger)(obj2.lc.poly,obj1.lc.poly) == 1) return(KpoInteger(1));
    else return(KpoInteger(0));
    break;
  case SuniversalNumber:
    tt = coeffGreater(obj1.lc.universalNumber,obj2.lc.universalNumber);
    if (tt < 0) return(KpoInteger(1));
    else return(KpoInteger(0));
    break;
  case Sdouble:
    if ( KopDouble(obj1) < KopDouble(obj2) ) return(KpoInteger(1));
    else return(KpoInteger(0));
    break;
  case Sarray:
  {
    int i,m1,m2;
    struct object rr = OINIT;
    m1 = getoaSize(obj1); m2 = getoaSize(obj2);
    for (i=0; i< (m1>m2?m2:m1); i++) {
      rr=KooLess(getoa(obj1,i),getoa(obj2,i));
      if (KopInteger(rr) == 1) return rr;
      rr=KooLess(getoa(obj2,i),getoa(obj1,i));
      if (KopInteger(rr) == 1) return KpoInteger(0);
    }
    if (m1 < m2) return KpoInteger(1);
    else return KpoInteger(0);
  }
  break;
  default:
    errorKan1("%s\n","KooLess() has not supported these objects yet.");
    break;
  }
}
      
/* :conversion */

struct object KdataConversion(obj,key)
     struct object obj;
     char *key;
{
  char tmps[128]; /* Assume that double is not more than 128 digits */
  char intstr[100]; /* Assume that int is not more than 100 digits */
  struct object rob = OINIT;
  extern struct ring *CurrentRingp;
  extern struct ring SmallRing;
  int flag;
  struct object rob1 = OINIT;
  struct object rob2 = OINIT;
  char *s;
  int i;
  double f;
  double f2;
  /* reports the data type */
  if (key[0] == 't' || key[0] =='e') {
    if (strcmp(key,"type?")==0) {
      rob = KpoInteger(obj.tag);
      return(rob);
    }else if (strcmp(key,"type??")==0) {
      if (obj.tag != Sclass) {
        rob = KpoInteger(obj.tag);
      }else {
        rob = KpoInteger(ectag(obj));
      }
      return(rob);
    }else if (strcmp(key,"error")==0) {
      rob = KnewErrorPacketObj(obj);
      return(rob);
    }
  }
  switch(obj.tag) {
  case Snull:
    if (strcmp(key,"integer") == 0) {
      rob = KpoInteger(0);
      return(rob);
    }else if (strcmp(key,"universalNumber") == 0) {
      rob.tag = SuniversalNumber;
      rob.lc.universalNumber = intToCoeff(obj.lc.ival,&SmallRing);
      return(rob);
    }else if (strcmp(key,"poly") == 0) {
      rob = KpoPOLY(ZERO);
      return rob;
    }else if (strcmp(key,"array") == 0) {
      rob = newObjectArray(0);
      return rob;
    }else{
      /* fprintf(stderr,"key=%s\n",key); */
      warningKan("Sorry. The data conversion from null to this data type has not supported yet.\n");
    }
    break;
  case Sinteger:
    if (strcmp(key,"string") == 0) { /* ascii code */
      rob.tag = Sdollar;
      rob.lc.str = (char *)sGC_malloc(2);
      if (rob.lc.str == (char *)NULL) errorKan1("%s","No more memory.\n");
      (rob.lc.str)[0] = obj.lc.ival; (rob.lc.str)[1] = '\0';
      return(rob);
    }else if (strcmp(key,"integer")==0) {
      return(obj);
    }else if (strcmp(key,"poly") == 0) {
      rob.tag = Spoly;
      rob.lc.poly = cxx(obj.lc.ival,0,0,CurrentRingp);
      return(rob);
    }else if (strcmp(key,"dollar") == 0) {
      rob.tag = Sdollar;
      sprintf(intstr,"%d",obj.lc.ival);
      rob.lc.str = (char *)sGC_malloc(strlen(intstr)+2);
      if (rob.lc.str == (char *)NULL) errorKan1("%s","No more memory.\n");
      strcpy(rob.lc.str,intstr);
      return(rob);
    }else if (strcmp(key,"universalNumber")==0) {
      rob = KintToUniversalNumber(obj.lc.ival);
      return(rob);
    }else if (strcmp(key,"double") == 0) {
      rob = KpoDouble((double) (obj.lc.ival));
      return(rob);
    }else if (strcmp(key,"null") == 0) {
      rob = NullObject;
      return(rob);
    }else{
      warningKan("Sorry. This type of data conversion has not supported yet.\n");
    }
    break;
  case Sdollar:
    if (strcmp(key,"dollar") == 0 || strcmp(key,"string")==0) {
      rob = obj;
      return(rob);
    }else if (strcmp(key,"literal") == 0) {
      rob.tag = Sstring;
      s = (char *) sGC_malloc(sizeof(char)*(strlen(obj.lc.str)+3));
      if (s == (char *) NULL)   {
        errorKan1("%s\n","No memory.");
      }
      s[0] = '/';
      strcpy(&(s[1]),obj.lc.str);
      rob.lc.str = &(s[1]);
      /* set the hashing value. */
      rob2 = lookupLiteralString(s);
      rob.rc.op = rob2.lc.op;
      return(rob);
    }else if (strcmp(key,"poly")==0) {
      rob.tag = Spoly;
      rob.lc.poly = stringToPOLY(obj.lc.str,CurrentRingp);
      return(rob);
    }else if (strcmp(key,"array")==0) {
      rob = newObjectArray(strlen(obj.lc.str));
      for (i=0; i<strlen(obj.lc.str); i++) {
        putoa(rob,i,KpoInteger((obj.lc.str)[i]));
      }
      return(rob);
    }else if (strcmp(key,"universalNumber") == 0) {
      rob.tag = SuniversalNumber;
      rob.lc.universalNumber = stringToUniversalNumber(obj.lc.str,&flag);
      if (flag == -1) errorKan1("KdataConversion(): %s",
                                "It's not number.\n");
      return(rob);
    }else if (strcmp(key,"double") == 0) {
      /* Check the format.  2.3432 e2 is not allowed. It should be 2.3232e2.*/
      flag = 0;
      for (i=0; (obj.lc.str)[i] != '\0'; i++) {
        if ((obj.lc.str)[i] > ' ' && flag == 0) flag=1;
        else if ((obj.lc.str)[i] <= ' ' && flag == 1) flag = 2;
        else if ((obj.lc.str)[i] > ' ' && flag == 2) flag=3;
      }
      if (flag == 3) errorKan1("KdataConversion(): %s","The data for the double contains blanck(s)");
      /* Read the double. */
      if (sscanf(obj.lc.str,"%lf",&f) <= 0) {
        errorKan1("KdataConversion(): %s","It cannot be translated to double.");
      }
      rob = KpoDouble(f);
      return(rob);
    }else if (strcmp(key,"null") == 0) {
      rob = NullObject;
      return(rob);
    }else{
      warningKan("Sorry. This type of data conversion has not supported yet.\n");
    }
    break;
  case Sarray:
    if (strcmp(key,"array") == 0) {
      return(rob);
    }else if (strcmp(key,"list") == 0) {
      rob = KarrayToList(obj);
      return(rob);
    }else if (strcmp(key,"arrayOfPOLY")==0) {
      rob = KpoArrayOfPOLY(arrayToArrayOfPOLY(obj));
      return(rob);
    }else if (strcmp(key,"matrixOfPOLY")==0) {
      rob = KpoMatrixOfPOLY(arrayToMatrixOfPOLY(obj));
      return(rob);
    }else if (strcmp(key,"gradedPolySet")==0) {
      rob = KpoGradedPolySet(arrayToGradedPolySet(obj));
      return(rob);
    }else if (strcmp(key,"null") == 0) {
      rob = NullObject;
      return(rob);
    }else if (strcmp(key,"byteArray") == 0) {
      rob = newByteArray(getoaSize(obj),obj);
      return(rob);
    }else {
	  { /* Automatically maps the elements. */
		int n,i;
		n = getoaSize(obj);
		rob = newObjectArray(n);
		for (i=0; i<n; i++) {
		  putoa(rob,i,KdataConversion(getoa(obj,i),key));
		}
		return(rob);
	  }
    }
    break;
  case Spoly:
    if ((strcmp(key,"poly")==0) || (strcmp(key,"numerator")==0)) {
      rob = obj;
      return(rob);
    }else if (strcmp(key,"integer")==0) {
      if (obj.lc.poly == ZERO) return(KpoInteger(0));
      else {
        return(KpoInteger(coeffToInt(obj.lc.poly->coeffp)));
      }
    }else if (strcmp(key,"string")==0 || strcmp(key,"dollar")==0) {
      rob.tag = Sdollar;
      rob.lc.str = KPOLYToString(KopPOLY(obj));
      return(rob);
    }else if (strcmp(key,"array") == 0) {
      return( POLYToArray(KopPOLY(obj)));
    }else if (strcmp(key,"map")==0) {
      return(KringMap(obj));
    }else if (strcmp(key,"universalNumber")==0) {
      if (obj.lc.poly == ZERO) {
        rob.tag = SuniversalNumber;
        rob.lc.universalNumber = newUniversalNumber(0);
      } else {
        if (obj.lc.poly->coeffp->tag == MP_INTEGER) {
          rob.tag = SuniversalNumber;
          rob.lc.universalNumber = newUniversalNumber2(obj.lc.poly->coeffp->val.bigp);
        }else {
          rob = NullObject;
          warningKan("Coefficient is not MP_INT.");
        }
      }
      return(rob);
    }else if (strcmp(key,"ring")==0) {
      if (obj.lc.poly ISZERO) {
        warningKan("Zero polynomial does not have the ring structure field.\n");
      }else{
        rob.tag = Sring;
        rob.lc.ringp = (obj.lc.poly)->m->ringp;
        return(rob);
      }
    }else if (strcmp(key,"null") == 0) {
      rob = NullObject;
      return(rob);
    }else{
      warningKan("Sorry. This type of data conversion has not supported yet.\n");
    }
    break;
  case SarrayOfPOLY:
    if (strcmp(key,"array")==0) {
      rob = arrayOfPOLYToArray(KopArrayOfPOLYp(obj));
      return(rob);
    }else{
      warningKan("Sorry. This type of data conversion has not supported yet.\n");
    }
    break;
  case SmatrixOfPOLY:
    if (strcmp(key,"array")==0) {
      rob = matrixOfPOLYToArray(KopMatrixOfPOLYp(obj));
      return(rob);
    }else if (strcmp(key,"null") == 0) {
      rob = NullObject;
      return(rob);
    }else{
      warningKan("Sorry. This type of data conversion has not supported yet.\n");
    }
    break;
  case Slist:
    if (strcmp(key,"array") == 0) {
      rob = KlistToArray(obj);
      return(rob);
    }
    break;
  case SuniversalNumber:
    if ((strcmp(key,"universalNumber")==0) || (strcmp(key,"numerator")==0)) {
      rob = obj;
      return(rob);
    }else if (strcmp(key,"integer")==0) {
      rob = KpoInteger(coeffToInt(obj.lc.universalNumber));
      return(rob);
    }else if (strcmp(key,"poly")==0) {
      rob = KpoPOLY(universalToPoly(obj.lc.universalNumber,CurrentRingp));
      return(rob);
    }else if (strcmp(key,"string")==0 || strcmp(key,"dollar")==0) {
      rob.tag = Sdollar;
      rob.lc.str = coeffToString(obj.lc.universalNumber);
      return(rob);
    }else if (strcmp(key,"null") == 0) {
      rob = NullObject;
      return(rob);
    }else if (strcmp(key,"double") == 0) {
      rob = KpoDouble( toDouble0(obj) );
      return(rob);
    }else if (strcmp(key,"denominator") == 0) {
      rob = KintToUniversalNumber(1);
      return(rob);
    }else{
      warningKan("Sorry. This type of data conversion of universalNumber has not supported yet.\n");
    }
    break;
  case SrationalFunction:
    if (strcmp(key,"rationalFunction")==0) {
      return(rob);
    } if (strcmp(key,"numerator")==0) {
      rob = *(Knumerator(obj));
      return(rob);
    }else if  (strcmp(key,"denominator")==0) {
      rob = *(Kdenominator(obj));
      return(rob);
    }else if  (strcmp(key,"string")==0 || strcmp(key,"dollar")==0) {
      rob1 = KdataConversion(*(Knumerator(obj)),"string");
      rob2 = KdataConversion(*(Kdenominator(obj)),"string");
      s = sGC_malloc(sizeof(char)*( strlen(rob1.lc.str) + strlen(rob2.lc.str) + 10));
      if (s == (char *)NULL) errorKan1("%s\n","KdataConversion(): No memory");
      sprintf(s,"(%s)/(%s)",rob1.lc.str,rob2.lc.str);
      rob.tag = Sdollar;
      rob.lc.str = s;
      return(rob);
    }else if  (strcmp(key,"cancel")==0) {
      warningKan("Sorry. Data conversion <<cancel>> of rationalFunction has not supported yet.\n");
      return(obj);
    }else if (strcmp(key,"null") == 0) {
      rob = NullObject;
      return(rob);
    }else if (strcmp(key,"double") == 0) {
      rob = KpoDouble( toDouble0(obj) );
      return(rob);
    }else{
      warningKan("Sorry. This type of data conversion of rationalFunction has not supported yet.\n");
    }
    break;
  case Sdouble:
    if (strcmp(key,"integer") == 0) {
      rob = KpoInteger( (int) KopDouble(obj));
      return(rob);
    } else if (strcmp(key,"universalNumber") == 0) {
      rob.tag = SuniversalNumber;
      rob.lc.universalNumber = intToCoeff((int) KopDouble(obj),&SmallRing);
      return(rob);
    }else if ((strcmp(key,"string") == 0) || (strcmp(key,"dollar") == 0)) {
      sprintf(tmps,"%f",KopDouble(obj));
      s = sGC_malloc(strlen(tmps)+2);
      if (s == (char *)NULL) errorKan1("%s\n","KdataConversion(): No memory");
      strcpy(s,tmps);
      rob.tag = Sdollar;
      rob.lc.str = s;
      return(rob);
    }else if (strcmp(key,"double")==0) {
      return(obj);
    }else if (strcmp(key,"null") == 0) {
      rob = NullObject;
      return(rob);
    }else {
      warningKan("Sorry. This type of data conversion of rationalFunction has not supported yet.\n");
    }
    break;
  case Sring:
    if (strcmp(key,"orderMatrix")==0) {
      rob = oGetOrderMatrix(KopRingp(obj));
      return(rob);
    }else if (strcmp(key,"oxRingStructure")==0) {
      rob = oRingToOXringStructure(KopRingp(obj));
      return(rob);
    }else{
      warningKan("Sorryl This type of data conversion of ringp has not supported yet.\n");
    }
    break;
  case SbyteArray:
    if (strcmp(key,"array") == 0) {
      rob = byteArrayToArray(obj);
      return(rob);
    } else {
      warningKan("Sorryl This type of data conversion of ringp has not supported yet.\n");
    }
    break;
  default:
    warningKan("Sorry. This type of data conversion has not supported yet.\n");
  }
  return(NullObject);
}

/* cf. macro to_int32 */
struct object Kto_int32(struct object ob) {
  int n,i;
  struct object otmp = OINIT;
  struct object rob = OINIT;
  if (ob.tag == SuniversalNumber) return KdataConversion(ob,"integer");
  if (ob.tag == Sarray) {
	n = getoaSize(ob);
	rob = newObjectArray(n);
	for (i=0; i<n; i++) {
	  otmp = Kto_int32(getoa(ob,i));
	  putoa(rob,i,otmp);
	}
	return rob;
  }
  return ob;
}    
/* conversion functions between primitive data and objects.
   If it's not time critical, it is recommended to use these functions */
struct object KpoInteger(k)
     int k;
{
  struct object obj = OINIT;
  obj.tag = Sinteger;
  obj.lc.ival = k; obj.rc.ival = 0;
  return(obj);
}
struct object KpoString(s)
     char *s;
{
  struct object obj = OINIT;
  obj.tag = Sdollar;
  obj.lc.str = s; obj.rc.ival = 0;
  return(obj);
}
struct object KpoPOLY(f)
     POLY f;
{
  struct object obj = OINIT;
  obj.tag = Spoly;
  obj.lc.poly = f; obj.rc.ival = 0;
  return(obj);
}
struct object KpoArrayOfPOLY(ap)
     struct arrayOfPOLY *ap ;
{
  struct object obj = OINIT;
  obj.tag = SarrayOfPOLY;
  obj.lc.arrayp = ap; obj.rc.ival = 0;
  return(obj);
}

struct object KpoMatrixOfPOLY(mp)
     struct matrixOfPOLY *mp ;
{
  struct object obj = OINIT;
  obj.tag = SmatrixOfPOLY;
  obj.lc.matrixp = mp; obj.rc.ival = 0;
  return(obj);
}

struct object KpoRingp(ringp)
     struct ring *ringp;
{
  struct object obj = OINIT;
  obj.tag = Sring;
  obj.lc.ringp = ringp;
  return(obj);
}

struct object KpoUniversalNumber(u)
     struct coeff *u;
{
  struct object obj = OINIT;
  obj.tag = SuniversalNumber;
  obj.lc.universalNumber = u;
  return(obj);
}
struct object KintToUniversalNumber(n)
	 int n;
{
  struct object rob = OINIT;
  extern struct ring SmallRing;
  rob.tag = SuniversalNumber;
  rob.lc.universalNumber = intToCoeff(n,&SmallRing);
  return(rob);
}

/*** conversion 2. Data conversions on arrays and matrices. ****/
struct object arrayOfPOLYToArray(aa)
     struct arrayOfPOLY *aa;
{
  POLY *a;
  int size;
  struct object r = OINIT;
  int j;
  struct object tmp = OINIT;

  size = aa->n; a = aa->array;
  r = newObjectArray(size);
  for (j=0; j<size; j++) {
    tmp.tag = Spoly;
    tmp.lc.poly= a[j];
    putoa(r,j,tmp);
  }
  return( r );
}

struct object matrixOfPOLYToArray(pmat)
     struct matrixOfPOLY *pmat;
{
  struct object r = OINIT;
  struct object tmp = OINIT;
  int i,j;
  int m,n;
  POLY *mat;
  struct arrayOfPOLY ap;
  
  m = pmat->m; n = pmat->n; mat = pmat->mat;
  r = newObjectArray(m);
  for (i=0; i<m; i++) {
    ap.n = n; ap.array = &(mat[ind(i,0)]);
    tmp = arrayOfPOLYToArray(&ap);
    /* ind() is the macro defined in matrix.h. */
    putoa(r,i,tmp);
  }
  return(r);
}

struct arrayOfPOLY *arrayToArrayOfPOLY(oa)
     struct object oa;
{
  POLY *a;
  int size;
  int i;
  struct object tmp = OINIT;
  struct arrayOfPOLY *ap;
  
  if (oa.tag != Sarray) errorKan1("KarrayToArrayOfPOLY(): %s",
                                  "Argument is not array\n");
  size = getoaSize(oa);
  a = (POLY *)sGC_malloc(sizeof(POLY)*size);
  for (i=0; i<size; i++) {
    tmp = getoa(oa,i);
    if (tmp.tag != Spoly) errorKan1("KarrayToArrayOfPOLY():%s ",
                                    "element must be polynomial.\n");
    a[i] = tmp.lc.poly;
  }
  ap = (struct arrayOfPOLY *)sGC_malloc(sizeof(struct arrayOfPOLY));
  ap->n = size;
  ap->array = a;
  return(ap);
}

struct matrixOfPOLY *arrayToMatrixOfPOLY(oa)
     struct object oa;
{
  POLY *a;
  int m;
  int n;
  int i,j;
  struct matrixOfPOLY *ma;

  struct object tmp = OINIT;
  struct object tmp2 = OINIT;
  if (oa.tag != Sarray) errorKan1("KarrayToMatrixOfPOLY(): %s",
                                  "Argument is not array\n");
  m = getoaSize(oa);
  tmp = getoa(oa,0);
  if (tmp.tag != Sarray) errorKan1("arrayToMatrixOfPOLY():%s ",
                                   "Argument is not array\n");
  n = getoaSize(tmp);
  a = (POLY *)sGC_malloc(sizeof(POLY)*(m*n));
  for (i=0; i<m; i++) {
    tmp = getoa(oa,i);
    if (tmp.tag != Sarray) errorKan1("arrayToMatrixOfPOLY(): %s",
                                     "element must be array.\n");
    for (j=0; j<n; j++) {
      tmp2 = getoa(tmp,j);
      if (tmp2.tag != Spoly) errorKan1("arrayToMatrixOfPOLY(): %s",
                                       "element must be a polynomial.\n");
      a[ind(i,j)] = tmp2.lc.poly;
      /* we use the macro ind here.  Be careful of using m and n. */
    }
  }
  ma = (struct matrixOfPOLY *)sGC_malloc(sizeof(struct matrixOfPOLY));
  ma->m = m; ma->n = n;
  ma->mat = a;
  return(ma);
}

/* :misc */

/* :ring    :kan */
int objArrayToOrderMatrix(oA,order,n,oasize)
     struct object oA;
     int order[];
     int n;
     int oasize;
{
  int size;
  int k,j;
  struct object tmpOa = OINIT;
  struct object obj = OINIT;
  if (oA.tag != Sarray) {
    warningKan("The argument should be of the form [ [...] [...] ... [...]].");
    return(-1);
  }
  size = getoaSize(oA);
  if (size != oasize) {
    warningKan("The row size of the array is wrong.");
    return(-1);
  }  
  for (k=0; k<size; k++) {
    tmpOa = getoa(oA,k);
    if (tmpOa.tag != Sarray) {
      warningKan("The argument should be of the form [ [...] [...] ... [...]].");
      return(-1);
    }
    if (getoaSize(tmpOa) != 2*n) {
      warningKan("The column size of the array is wrong.");
      return(-1);
    }
    for (j=0; j<2*n; j++) {
      obj = getoa(tmpOa,j);
      order[k*2*n+j] = obj.lc.ival;
    }
  }
  return(0);
}

int KsetOrderByObjArray(oA)
     struct object oA;
{
  int *order;
  int n,c,l, oasize;
  extern struct ring *CurrentRingp;
  extern int AvoidTheSameRing;
  /* n,c,l must be set in the CurrentRing */
  if (AvoidTheSameRing) {
    errorKan1("%s\n","KsetOrderByObjArray(): You cannot change the order matrix when AvoidTheSameRing == 1.");
  }
  n = CurrentRingp->n;
  c = CurrentRingp->c;
  l = CurrentRingp->l;
  if (oA.tag != Sarray) {
    warningKan("The argument should be of the form [ [...] [...] ... [...]].");
    return(-1);
  }
  oasize = getoaSize(oA);
  order = (int *)sGC_malloc(sizeof(int)*((2*n)*oasize+1));
  if (order == (int *)NULL) errorKan1("%s\n","KsetOrderByObjArray(): No memory.");
  if (objArrayToOrderMatrix(oA,order,n,oasize) == -1) {
    return(-1);
  }
  setOrderByMatrix(order,n,c,l,oasize); /* Set order to the current ring. */
  return(0);
}

static int checkRelations(c,l,m,n,cc,ll,mm,nn)
     int c,l,m,n,cc,ll,mm,nn;
{
  if (!(1<=c && c<=l && l<=m && m<=n)) return(1);
  if (!(cc<=ll && ll<=mm && mm<=nn && nn <= n)) return(1);
  if (!(cc<c || ll < l || mm < m || nn < n)) {
    if (WarningNoVectorVariable) {
      warningKanNoStrictMode("Ring definition: there is no variable to represent vectors.\n");
    }
  }
  if (!(cc<=c && ll <= l && mm <= m && nn <= n)) return(1);
  return(0);
}

struct object KgetOrderMatrixOfCurrentRing() 
{
  extern struct ring *CurrentRingp;
  return(oGetOrderMatrix(CurrentRingp));
}

  
int KsetUpRing(ob1,ob2,ob3,ob4,ob5)
     struct object ob1,ob2,ob3,ob4,ob5;
     /* ob1 = [x(0), ..., x(n-1)];
        ob2 = [D(0), ..., D(n-1)];
        ob3 = [p,c,l,m,n,cc,ll,mm,nn,next];
        ob4 = Order matrix
        ob5 = [(keyword) value (keyword) value ....]
     */
#define RP_LIMIT 5000
{
  int i;
  struct object ob = OINIT;
  int c,l,m,n;
  int cc,ll,mm,nn;
  int p;
  char **xvars;
  char **dvars;
  int *outputVars;
  int *order;
  static int rp = 0;
  static struct ring *rstack[RP_LIMIT];
  
  extern struct ring *CurrentRingp;
  struct ring *newRingp;
  int ob3Size;
  struct ring *nextRing;
  int oasize;
  static int ringSerial = 0;
  char *ringName = NULL;
  int aa;
  extern int AvoidTheSameRing;
  extern char *F_mpMult;
  char *fmp_mult_saved;
  char *mpMultName = NULL;
  struct object rob = OINIT;
  struct ring *savedCurrentRingp;

  /* To get the ring structure. */
  if (ob1.tag == Snull) {
    rob = newObjectArray(rp);
    for (i=0; i<rp; i++) {
      putoa(rob,i,KpoRingp(rstack[i]));
    }
    KSpush(rob);
    return(0);
  }

  if (ob3.tag != Sarray) errorKan1("%s\n","Error in the 3rd argument. You need to give 4 arguments.");
  ob3Size = getoaSize(ob3);
  if (ob3Size != 9 && ob3Size != 10)
    errorKan1("%s\n","Error in the 3rd argument.");
  for (i=0; i<9; i++) {
    ob = getoa(ob3,i);
    if (ob.tag != Sinteger) errorKan1("%s\n","The 3rd argument should be a list of integers.");
  }
  if (ob3Size == 10) {
    ob = getoa(ob3,9);
    if (ob.tag != Sring)
      errorKan1("%s\n","The last arguments of the 3rd argument must be a pointer to a ring.");
    nextRing = KopRingp(ob);
  } else {
    nextRing = (struct ring *)NULL;
  }
  
  p = getoa(ob3,0).lc.ival;
  c = getoa(ob3,1).lc.ival;  l = getoa(ob3,2).lc.ival;
  m = getoa(ob3,3).lc.ival;  n = getoa(ob3,4).lc.ival;
  cc = getoa(ob3,5).lc.ival;  ll = getoa(ob3,6).lc.ival;
  mm = getoa(ob3,7).lc.ival;  nn = getoa(ob3,8).lc.ival;
  if (checkRelations(c,l,m,n,cc,ll,mm,nn,n)) {
    errorKan1("%s\n","1<=c<=l<=m<=n and cc<=c<=ll<=l<=mm<=m<=nn<=n \nand (cc<c or ll < l or mm < m or nn < n)  must be satisfied.");
  }
  if (getoaSize(ob2) != n || getoaSize(ob1) != n) {
    errorKan1("%s\n","Error in the 1st or 2nd arguments.");
  }
  for (i=0; i<n; i++) {
    if (getoa(ob1,i).tag != Sdollar || getoa(ob2,i).tag != Sdollar) {
      errorKan1("%s\n","Error in the 1st or 2nd arguments.");
    }
  }
  xvars = (char **) sGC_malloc(sizeof(char *)*n);
  dvars = (char **) sGC_malloc(sizeof(char *)*n);
  if (xvars == (char **)NULL || dvars == (char **)NULL) {
    fprintf(stderr,"No more memory.\n");
    exit(15);
  }
  for (i=0; i<n; i++) {
    xvars[i] = getoa(ob1,i).lc.str;
    dvars[i] = getoa(ob2,i).lc.str;
  }
  checkDuplicateName(xvars,dvars,n);

  outputVars = (int *)sGC_malloc(sizeof(int)*n*2);
  if (outputVars == NULL) {
    fprintf(stderr,"No more memory.\n");
    exit(15);
  }
  if (ReverseOutputOrder) {
    for (i=0; i<n; i++) outputVars[i] = n-i-1;
    for (i=0; i<n; i++) outputVars[n+i] = 2*n-i-1;
  }else{
    for (i=0; i<2*n; i++) {
      outputVars[i] = i;
    }
  }

  ob4 = Kto_int32(ob4); /* order matrix */
  oasize = getoaSize(ob4);
  order = (int *)sGC_malloc(sizeof(int)*((2*n)*oasize+1));
  if (order == (int *)NULL) errorKan1("%s\n","No memory.");
  if (objArrayToOrderMatrix(ob4,order,n,oasize) == -1) {
    errorKan1("%s\n","Errors in the 4th matrix (order matrix).");
  }
  /* It's better to check the consistency of the order matrix here. */
  savedCurrentRingp = CurrentRingp;

  newRingp = (struct ring *)sGC_malloc(sizeof(struct ring));
  if (newRingp == NULL) errorKan1("%s\n","No more memory.");
  /* Generate the new ring before calling setOrder...(). */
  *newRingp = *CurrentRingp;
  CurrentRingp = newRingp;  /* Push the current ring. */
  setOrderByMatrix(order,n,c,l,oasize); /* set order to the CurrentRing. */
  CurrentRingp = savedCurrentRingp; /* recover it. */


  /* Set the default name of the ring */
  ringName = (char *)sGC_malloc(16);
  sprintf(ringName,"ring%05d",ringSerial);
  ringSerial++;
  
  /* Set the current ring */
  newRingp->n = n; newRingp->m = m; newRingp->l = l; newRingp->c = c;
  newRingp->nn = nn; newRingp->mm = mm; newRingp->ll = ll;
  newRingp->cc = cc;
  newRingp->x = xvars;
  newRingp->D = dvars;
  newRingp->Dsmall = makeDsmall(dvars,n);
  /* You don't need to set order and orderMatrixSize here.
     It was set by setOrder(). */
  setFromTo(newRingp);

  newRingp->p = p;
  newRingp->next = nextRing;
  newRingp->multiplication = mpMult;
  /* These values  should will be reset if the optional value is given. */
  newRingp->schreyer = 0;
  newRingp->gbListTower = NULL;
  newRingp->outputOrder = outputVars;
  newRingp->weightedHomogenization = 0;
  newRingp->degreeShiftSize = 0;
  newRingp->degreeShiftN = 0;
  newRingp->degreeShift = NULL;
  newRingp->partialEcart = 0;
  newRingp->partialEcartGlobalVarX = NULL;

  if (ob5.tag != Sarray || (getoaSize(ob5) % 2) != 0) {
    errorKan1("%s\n","[(keyword) value (keyword) value ....] should be given.");
  }
  for (i=0; i < getoaSize(ob5); i += 2) {
    if (getoa(ob5,i).tag == Sdollar) {
      if (strcmp(KopString(getoa(ob5,i)),"mpMult") == 0) {
        if (getoa(ob5,i+1).tag != Sdollar) {
          errorKan1("%s\n","A keyword should be given. (mpMult)");
        }
        fmp_mult_saved = F_mpMult;
        mpMultName = KopString(getoa(ob5,i+1));
        switch_function("mpMult",mpMultName);
        /* Note that this cause a global effect. It will be done again. */
        newRingp->multiplication = mpMult;
        switch_function("mpMult",fmp_mult_saved);
      } else if (strcmp(KopString(getoa(ob5,i)),"coefficient ring") == 0) {
        if (getoa(ob5,i+1).tag != Sring) {
          errorKan1("%s\n","The pointer to a ring should be given. (coefficient ring)");
        }
        nextRing = KopRingp(getoa(ob5,i+1));
        newRingp->next = nextRing;
      } else if (strcmp(KopString(getoa(ob5,i)),"valuation") == 0) {
        errorKan1("%s\n","Not implemented. (valuation)");
      } else if (strcmp(KopString(getoa(ob5,i)),"characteristic") == 0) {
        if (getoa(ob5,i+1).tag != Sinteger) {
          errorKan1("%s\n","A integer should be given. (characteristic)");
        }
        p = KopInteger(getoa(ob5,i+1));
        newRingp->p = p;
      } else if (strcmp(KopString(getoa(ob5,i)),"schreyer") == 0) {
        if (getoa(ob5,i+1).tag != Sinteger) {
          errorKan1("%s\n","A integer should be given. (schreyer)");
        }
        newRingp->schreyer = KopInteger(getoa(ob5,i+1));
      } else if (strcmp(KopString(getoa(ob5,i)),"gbListTower") == 0) {
        if (getoa(ob5,i+1).tag != Slist) {
          errorKan1("%s\n","A list should be given (gbListTower).");
        }
        newRingp->gbListTower = newObject();
        *((struct object *)(newRingp->gbListTower)) = getoa(ob5,i+1);
      } else if (strcmp(KopString(getoa(ob5,i)),"ringName") == 0) {
        if (getoa(ob5,i+1).tag != Sdollar) {
          errorKan1("%s\n","A name should be given. (ringName)");
        }
        ringName = KopString(getoa(ob5,i+1));
      } else if (strcmp(KopString(getoa(ob5,i)),"weightedHomogenization") == 0) {
        if (getoa(ob5,i+1).tag != Sinteger) {
          errorKan1("%s\n","A integer should be given. (weightedHomogenization)");
        }
        newRingp->weightedHomogenization = KopInteger(getoa(ob5,i+1));
      } else if (strcmp(KopString(getoa(ob5,i)),"degreeShift") == 0) {
        if (getoa(ob5,i+1).tag != Sarray) {
          errorKan1("%s\n","An array of array should be given. (degreeShift)");
        }
        {
          struct object ods = OINIT;
          struct object ods2 = OINIT;
          int dssize,k,j,nn;
          ods=getoa(ob5,i+1);
          if ((getoaSize(ods) < 1) || (getoa(ods,0).tag != Sarray)) {
            errorKan1("%s\n", "An array of array should be given. (degreeShift)");
          }
          nn = getoaSize(ods);
          dssize = getoaSize(getoa(ods,0));
          newRingp->degreeShiftSize = dssize;
          newRingp->degreeShiftN = nn;
          newRingp->degreeShift = (int *) sGC_malloc(sizeof(int)*(dssize*nn+1));
          if (newRingp->degreeShift == NULL) errorKan1("%s\n","No more memory.");
          for (j=0; j<nn; j++) {
            ods2 = getoa(ods,j);
            for (k=0; k<dssize; k++) {
              if (getoa(ods2,k).tag == SuniversalNumber) {
                (newRingp->degreeShift)[j*dssize+k] = coeffToInt(getoa(ods2,k).lc.universalNumber);
              }else{
                (newRingp->degreeShift)[j*dssize+k] = KopInteger(getoa(ods2,k));
              }
            }
          }
        }
      } else if (strcmp(KopString(getoa(ob5,i)),"partialEcartGlobalVarX") == 0) {
        if (getoa(ob5,i+1).tag != Sarray) {
          errorKan1("%s\n","An array of array should be given. (partialEcart)");
        }
        {
          struct object odv = OINIT;
          struct object ovv = OINIT;
          int k,j,nn;
          char *vname;
          odv=getoa(ob5,i+1);
          nn = getoaSize(odv);
          newRingp->partialEcart = nn;
          newRingp->partialEcartGlobalVarX = (int *) sGC_malloc(sizeof(int)*nn+1);
          if (newRingp->partialEcartGlobalVarX == NULL) errorKan1("%s\n","No more memory.");
          for (j=0; j<nn; j++)
            (newRingp->partialEcartGlobalVarX)[j] = -1;
          for (j=0; j<nn; j++) {
            ovv = getoa(odv,j);
            if (ovv.tag != Sdollar) errorKan1("%s\n","partialEcartGlobalVarX: string is expected.");
            vname = KopString(ovv);
            for (k=0; k<n; k++) {
              if (strcmp(vname,xvars[k]) == 0) {
                (newRingp->partialEcartGlobalVarX)[j] = k; break;
              }else{
                if (k == n-1) errorKan1("%s\n","partialEcartGlobalVarX: no such variable.");
              }
            }
          }
        }

        switch_function("grade","module1v");
        /* Warning: grading is changed to module1v!! */
      } else {
        errorKan1("%s\n","Unknown keyword to set_up_ring@");
      }
    }else{
      errorKan1("%s\n","A keyword enclosed by braces have to be given.");
    }
  }

  newRingp->name = ringName;


  if (AvoidTheSameRing) {
    aa = isTheSameRing(rstack,rp,newRingp);
    if (aa < 0) {
      /* This ring has never been defined. */
      CurrentRingp = newRingp;
      /* Install it to the RingStack */
      if (rp <RP_LIMIT) {
        rstack[rp] = CurrentRingp; rp++; /* Save the previous ringp */
      }else{
        rp = 0;
        errorKan1("%s\n","You have defined too many rings. Check the value of RP_LIMIT.");
      }
    }else{
      /* This ring has been defined. */
      /* Discard the newRingp */
      CurrentRingp = rstack[aa];
      ringSerial--;
    }
  }else{
    CurrentRingp = newRingp;
    /* Install it to the RingStack */
    if (rp <RP_LIMIT) {
      rstack[rp] = CurrentRingp; rp++; /* Save the previous ringp */
    }else{
      rp = 0;
      errorKan1("%s\n","You have defined too many rings. Check the value of RP_LIMIT.");
    }
  }
  if (mpMultName != NULL) {
    switch_function("mpMult",mpMultName);
  }

  initSyzRingp();
  
  return(0);
}
      

struct object KsetVariableNames(struct object ob,struct ring *rp)
{
  int n,i;
  struct object ox = OINIT;
  struct object otmp = OINIT;
  char **xvars;
  char **dvars;
  if (ob.tag  != Sarray) {
    errorKan1("%s\n","KsetVariableNames(): the argument must be of the form [(x) (y) (z) ...]");
  }
  n = rp->n;
  ox = ob;
  if (getoaSize(ox) != 2*n) {
    errorKan1("%s\n","KsetVariableNames(): the argument must be of the form [(x) (y) (z) ...] and the length of [(x) (y) (z) ...] must be equal to the number of x and D variables.");
  }
  xvars = (char **)sGC_malloc(sizeof(char *)*n);
  dvars = (char **)sGC_malloc(sizeof(char *)*n);
  if (xvars == NULL || dvars == NULL) {
    errorKan1("%s\n","KsetVariableNames(): no more memory.");
  }
  for (i=0; i<2*n; i++) {
    otmp = getoa(ox,i);
    if(otmp.tag != Sdollar) {
      errorKan1("%s\n","KsetVariableNames(): elements must be strings.");
    }
    if (i < n) {
      xvars[i] = KopString(otmp);
    }else{
      dvars[i-n] = KopString(otmp);
    }
  }
  checkDuplicateName(xvars,dvars,n);
  rp->x = xvars;
  rp->D = dvars;
  return(ob);
}


  
void KshowRing(ringp)
     struct ring *ringp;
{
  showRing(1,ringp);
}

struct object KswitchFunction(ob1,ob2)
     struct object ob1,ob2;
{
  char *ans ;
  struct object rob = OINIT;
  int needWarningForAvoidTheSameRing = 0;
  extern int AvoidTheSameRing;
  if ((ob1.tag != Sdollar) || (ob2.tag != Sdollar)) {
    errorKan1("%s\n","$function$ $name$ switch_function\n");
  }
  if (AvoidTheSameRing && needWarningForAvoidTheSameRing) {
    if (strcmp(KopString(ob1),"mmLarger") == 0 ||
        strcmp(KopString(ob1),"mpMult") == 0 ||
        strcmp(KopString(ob1),"monomialAdd") == 0 ||
        strcmp(KopString(ob1),"isSameComponent") == 0) {
      fprintf(stderr,",switch_function ==> %s ",KopString(ob1));
      warningKan("switch_function might cause a trouble under AvoidTheSameRing == 1.\n");
    }
  }
  if (AvoidTheSameRing) {
    if (strcmp(KopString(ob1),"mmLarger") == 0 &&
        ((strcmp(KopString(ob2),"matrix") != 0) && (strcmp(KopString(ob2),"module_matrix") != 0))) {
      fprintf(stderr,"mmLarger = %s",KopString(ob2));
      errorKan1("%s\n","mmLarger can set only to matrix/module_matrix under AvoidTheSameRing == 1.");
    }
  }
  
  ans = switch_function(ob1.lc.str,ob2.lc.str);
  if (ans == NULL) {
    rob = NullObject;
  }else{
    rob = KpoString(ans);
  }
  return(rob);
  
}

void KprintSwitchStatus(void)
{
  print_switch_status();
}

struct object KoReplace(of,rule)
     struct object of;
     struct object rule;
{
  struct object rob = OINIT;
  POLY f;
  POLY lRule[N0*2];
  POLY rRule[N0*2];
  POLY r;
  int i;
  int n;
  struct object trule = OINIT;
  

  if (rule.tag != Sarray) {
    errorKan1("%s\n"," KoReplace(): The second argument must be array.");
  }
  n = getoaSize(rule);

  if (of.tag == Spoly) {
  }else if (of.tag ==Sclass && ectag(of) == CLASSNAME_recursivePolynomial) {
    return(KreplaceRecursivePolynomial(of,rule));
  }else{
    errorKan1("%s\n"," KoReplace(): The first argument must be a polynomial.");
  }
  f = KopPOLY(of);

  if (f ISZERO) {
  }else{
    if (n >= 2*(f->m->ringp->n)) {
      errorKan1("%s\n"," KoReplace(): too many rules for replacement. ");
    }
  }

  for (i=0; i<n; i++) {
    trule = getoa(rule,i);
    if (trule.tag != Sarray) {
      errorKan1("%s\n"," KoReplace(): The second argument must be of the form [[a b] [c d] ....].");
    }
    if (getoaSize(trule) != 2) {
      errorKan1("%s\n"," KoReplace(): The second argument must be of the form [[a b] [c d] ....].");
    }

    if (getoa(trule,0).tag != Spoly) {
      errorKan1("%s\n"," KoReplace(): The second argument must be of the form [[a b] [c d] ....] where a,b,c,d,... are polynomials.");
    }
    if (getoa(trule,1).tag != Spoly) {
      errorKan1("%s\n"," KoReplace(): The second argument must be of the form [[a b] [c d] ....] where a,b,c,d,... are polynomials.");
    }

    lRule[i] = KopPOLY(getoa(trule,0));
    rRule[i] = KopPOLY(getoa(trule,1));
  }
  
  r = replace(f,lRule,rRule,n);
  rob.tag = Spoly; rob.lc.poly = r;

  return(rob);
}


struct object Kparts(f,v)
     struct object f;
     struct object v;
{
  POLY ff;
  POLY vv;
  struct object obj = OINIT;
  struct matrixOfPOLY *co;
  /* check the data type */
  if (f.tag != Spoly || v.tag != Spoly)
    errorKan1("%s\n","arguments of Kparts() must have polynomial as arguments.");

  co = parts(KopPOLY(f),KopPOLY(v));
  obj = matrixOfPOLYToArray(co);
  return(obj);
}

struct object Kparts2(f,v)
     struct object f;
     struct object v;
{
  POLY ff;
  POLY vv;
  struct object obj = OINIT;
  struct matrixOfPOLY *co;
  /* check the data type */
  if (f.tag != Spoly || v.tag != Spoly)
    errorKan1("%s\n","arguments of Kparts2() must have polynomial as arguments.");

  obj = parts2(KopPOLY(f),KopPOLY(v));
  return(obj);
}


struct object Kdegree(ob1,ob2)
     struct object ob1,ob2;
{
  if (ob1.tag != Spoly || ob2.tag != Spoly)
    errorKan1("%s\n","The arguments must be polynomials.");

  return(KpoInteger(pDegreeWrtV(KopPOLY(ob1),KopPOLY(ob2))));
}

struct object KringMap(obj)
     struct object obj;
{
  extern struct ring *CurrentRingp;
  extern struct ring *SyzRingp;
  POLY f;
  POLY r;
  if (obj.tag != Spoly)
    errorKan1("%s\n","The argments must be polynomial.");
  f = KopPOLY(obj);
  if (f ISZERO) return(obj);
  if (f->m->ringp == CurrentRingp) return(obj);
  if (f->m->ringp == CurrentRingp->next) {
    r = newCell(newCoeff(),newMonomial(CurrentRingp));
    r->coeffp->tag = POLY_COEFF;
    r->coeffp->val.f = f;
    return(KpoPOLY(r));
  }else if (f->m->ringp == SyzRingp) {
    return(KpoPOLY(f->coeffp->val.f));
  }
  errorKan1("%s\n","The ring map is not defined in this case.");
}


struct object Ksp(ob1,ob2)
     struct object ob1,ob2;
{
  struct spValue sv;
  struct object rob = OINIT;
  struct object cob = OINIT;
  POLY f;
  if (ob1.tag != Spoly || ob2.tag != Spoly)
    errorKan1("%s\n","Ksp(): The arguments must be polynomials.");
  sv = (*sp)(ob1.lc.poly,ob2.lc.poly);
  f = ppAddv(ppMult(sv.a,KopPOLY(ob1)),
             ppMult(sv.b,KopPOLY(ob2)));
  rob = newObjectArray(2);
  cob = newObjectArray(2);
  putoa(rob,1,KpoPOLY(f));
  putoa(cob,0,KpoPOLY(sv.a));
  putoa(cob,1,KpoPOLY(sv.b));
  putoa(rob,0,cob);
  return(rob);
}
  
struct object Khead(ob)
     struct object ob;
{
  if (ob.tag != Spoly) errorKan1("%s\n","Khead(): The argument should be a polynomial.");
  return(KpoPOLY(head( KopPOLY(ob))));
}
  

/* :eval */
struct object Keval(obj)
     struct object obj;
{
  char *key;
  int size;
  struct object rob = OINIT;
  rob = NullObject;
  
  if (obj.tag != Sarray)
    errorKan1("%s\n","[$key$ arguments] eval");
  if (getoaSize(obj) < 1) 
    errorKan1("%s\n","[$key$ arguments] eval");
  if (getoa(obj,0).tag != Sdollar)
    errorKan1("%s\n","[$key$ arguments] eval");
  key = getoa(obj,0).lc.str;
  size = getoaSize(obj);


  return(rob);
}
  
/* :Utilities */
char *KremoveSpace(str)
     char str[];
{
  int size;
  int start;
  int end;
  char *s;
  int i;

  size = strlen(str);
  for (start = 0; start <= size; start++) {
    if (str[start] > ' ') break;
  }
  for (end = size-1; end >= 0; end--) {
    if (str[end] > ' ') break;
  }
  if (start > end) return((char *) NULL);
  s = (char *) sGC_malloc(sizeof(char)*(end-start+2));
  if (s == (char *)NULL) errorKan1("%s\n","removeSpace(): No more memory.");
  for (i=0; i< end-start+1; i++)
    s[i] = str[i+start];
  s[end-start+1] = '\0';
  return(s);
}

struct object KtoRecords(ob)
     struct object ob;
{
  struct object obj = OINIT;
  struct object tmp = OINIT;
  int i;
  int size;
  char **argv;
  
  obj = NullObject;
  switch(ob.tag) {
  case Sdollar: break;
  default:
    errorKan1("%s","Argument of KtoRecords() must be a string enclosed by dollars.\n");
    break;
  }
  size = strlen(ob.lc.str)+3;
  argv = (char **) sGC_malloc((size+1)*sizeof(char *));
  if (argv == (char **)NULL)
    errorKan1("%s","No more memory.\n");
  size = KtoArgvbyCurryBrace(ob.lc.str,argv,size);
  if (size < 0)
    errorKan1("%s"," KtoRecords(): You have an error in the argument.\n");
    
  obj = newObjectArray(size);
  for (i=0; i<size; i++) {
    tmp.tag = Sdollar;
    tmp.lc.str = argv[i];
    (obj.rc.op)[i] = tmp;
  }
  return(obj);
}

int KtoArgvbyCurryBrace(str,argv,limit)
     char *str;
     char *argv[];
     int limit;
     /* This function returns argc */
     /* decompose into tokens by the separators
   { }, [ ], and characters of which code is less than SPACE.
   Example.   { }  ---> nothing            (argc=0)
              {x}----> x                   (argc=1)
              {x,y} --> x   y              (argc=2)
          {ab, y, z } --> ab   y   z   (argc=3)
              [[ab],c,d]  --> [ab] c   d
*/
{
  int argc;
  int n;
  int i;
  int k;
  char *a;
  char *ident;
  int level = 0;
  int comma;

  if (str == (char *)NULL) {
    fprintf(stderr,"You use NULL string to toArgvbyCurryBrace()\n");
    return(0);
  }
  
  n = strlen(str);
  a = (char *) sGC_malloc(sizeof(char)*(n+3));
  a[0]=' '; 
  strcpy(&(a[1]),str);
  n = strlen(a); a[0] = '\0';
  comma = -1; 
  for (i=1; i<n; i++) {
    if (a[i] == '{' || a[i] == '[') level++;
    if (level <= 1 && ( a[i] == ',')) {a[i] = '\0'; ++comma;}
    if (level <= 1 && (a[i]=='{' || a[i]=='}' || a[i]=='[' || a[i]==']'))
      a[i] = '\0';
    if (a[i] == '}' || a[i] == ']') level--;
    if ((level <= 1) && (comma == -1) && ( a[i] > ' ')) comma = 0;
  }

  if (comma == -1) return(0);

  argc=0;
  for (i=0; i<n; i++) {
    if ((a[i] == '\0') && (a[i+1] != '\0')) ++argc;
  }
  if (argc > limit) return(-argc);

  k = 0;
  for (i=0; i<n; i++) {
    if ((a[i] == '\0') && (a[i+1] != '\0')) {
      ident = (char *) sGC_malloc(sizeof(char)*( strlen(&(a[i+1])) + 3));
      strcpy(ident,&(a[i+1]));
      argv[k] = KremoveSpace(ident);
      if (argv[k] != (char *)NULL) k++;
      if (k >= limit) errorKan1("%s\n","KtoArgvbyCurryBraces(): k>=limit.");
    }
  }
  argc = k;
  /*for (i=0; i<argc; i++) fprintf(stderr,"%d %s\n",i,argv[i]);*/
  return(argc);
}

struct object KstringToArgv(struct object ob) {
  struct object rob = OINIT;
  char *s;
  int n,wc,i,inblank;
  char **argv;
  if (ob.tag != Sdollar)
    errorKan1("%s\n","KstringToArgv(): the argument must be a string.");
  n = strlen(KopString(ob));
  s = (char *) sGC_malloc(sizeof(char)*(n+2));
  if (s == NULL) errorKan1("%s\n","KstringToArgv(): No memory.");
  strcpy(s,KopString(ob));
  inblank = 1;  wc = 0; 
  for (i=0; i<n; i++) {
    if (inblank && (s[i] > ' ')) {
      wc++; inblank = 0;
    }else if ((!inblank) && (s[i] <= ' ')) {
      inblank = 1;
    }
  }
  argv = (char **) sGC_malloc(sizeof(char *)*(wc+2));
  argv[0] = NULL;
  inblank = 1;  wc = 0; 
  for (i=0; i<n; i++) {
    if (inblank && (s[i] > ' ')) {
      argv[wc] = &(s[i]); argv[wc+1]=NULL;
      wc++; inblank = 0;
    }else if ((inblank == 0) && (s[i] <= ' ')) {
      inblank = 1; s[i] = 0;
    }else if (inblank && (s[i] <= ' ')) {
      s[i] = 0;
    }
  }

  rob = newObjectArray(wc);
  for (i=0; i<wc; i++) {
    putoa(rob,i,KpoString(argv[i]));
    /* printf("%s\n",argv[i]); */
  }
  return(rob);
}

struct object KstringToArgv2(struct object ob,struct object oseparator) {
  struct object rob = OINIT;
  char *s;
  int n,wc,i,inblank;
  char **argv;
  int separator;
  if (ob.tag != Sdollar)
    errorKan1("%s\n","KstringToArgv2(): the argument must be a string.");
  if (oseparator.tag == Sinteger) {
	separator = KopInteger(oseparator);
  }else if (oseparator.tag == Sdollar) {
	s = KopString(oseparator);
	separator=s[0];
  }else {
    errorKan1("%s\n","KstringToArgv2(ob,separator):the argument must be strings.");
  }
  n = strlen(KopString(ob));
  s = (char *) sGC_malloc(sizeof(char)*(n+2));
  if (s == NULL) errorKan1("%s\n","KstringToArgv(): No memory.");
  strcpy(s,KopString(ob));
  inblank = 1;  wc = 0; 
  for (i=0; i<n; i++) {
    if (inblank && (s[i] != separator)) {
      wc++; inblank = 0;
    }else if ((!inblank) && (s[i] == separator)) {
      inblank = 1;
    }
  }
  argv = (char **) sGC_malloc(sizeof(char *)*(wc+2));
  argv[0] = NULL;
  inblank = 1;  wc = 0; 
  for (i=0; i<n; i++) {
    if (inblank && (s[i] != separator)) {
      argv[wc] = &(s[i]); argv[wc+1]=NULL;
      wc++; inblank = 0;
    }else if ((inblank == 0) && (s[i] == separator)) {
      inblank = 1; s[i] = 0;
    }else if (inblank && (s[i] == separator)) {
      s[i] = 0;
    }
  }

  rob = newObjectArray(wc);
  for (i=0; i<wc; i++) {
    putoa(rob,i,KpoString(argv[i]));
    /* printf("%s\n",argv[i]); */
  }
  return(rob);
}

static void checkDuplicateName(xvars,dvars,n)
     char *xvars[];
     char *dvars[];
     int n;
{
  int i,j;
  char *names[N0*2];
  for (i=0; i<n; i++) {
    names[i] = xvars[i]; names[i+n] = dvars[i];
  }
  n = 2*n;
  for (i=0; i<n; i++) {
    for (j=i+1; j<n; j++) {
      if (strcmp(names[i],names[j]) == 0) {
        fprintf(stderr,"\n%d=%s, %d=%s\n",i,names[i],j,names[j]);
        errorKan1("%s\n","Duplicate definition of the name above in SetUpRing().");
      }
    }
  }
}

struct object KooPower(struct object ob1,struct object ob2) {
  struct object rob = OINIT;
  /* Bug. It has not yet been implemented. */
  if (QuoteMode) {
    rob = powerTree(ob1,ob2);
  }else{
    warningKan("KooDiv2() has not supported yet these objects.\n");
  }
  return(rob);
}



struct object KooDiv2(ob1,ob2)
     struct object ob1,ob2;
{
  struct object rob = NullObject;
  POLY f;
  extern struct ring *CurrentRingp;
  int s,i;
  double d;
  
  switch (Lookup[ob1.tag][ob2.tag]) {
  case SpolySpoly:
  case SuniversalNumberSuniversalNumber:
  case SuniversalNumberSpoly:
  case SpolySuniversalNumber:
    rob = KnewRationalFunction0(copyObjectp(&ob1),copyObjectp(&ob2));
    KisInvalidRational(&rob);
    return(rob);
    break;
  case SarraySpoly:
  case SarraySuniversalNumber:
  case SarraySrationalFunction:
    s = getoaSize(ob1);
    rob = newObjectArray(s);
    for (i=0; i<s; i++) {
      putoa(rob,i,KooDiv2(getoa(ob1,i),ob2));
    }
    return(rob);
    break;
  case SpolySrationalFunction:
  case SrationalFunctionSpoly:
  case SrationalFunctionSrationalFunction:
  case SuniversalNumberSrationalFunction:
  case SrationalFunctionSuniversalNumber:
    rob = KoInverse(ob2);
    rob = KooMult(ob1,rob);
    return(rob);
    break;

  case SdoubleSdouble:
    d = KopDouble(ob2);
    if (d == 0.0) errorKan1("%s\n","KooDiv2, Division by zero.");
    return(KpoDouble( KopDouble(ob1) / d ));
    break;
  case SdoubleSinteger:
  case SdoubleSuniversalNumber:
  case SdoubleSrationalFunction:
    d = toDouble0(ob2);
    if (d == 0.0) errorKan1("%s\n","KooDiv2, Division by zero.");
    return(KpoDouble( KopDouble(ob1) / d) );
    break;
  case SintegerSdouble:
  case SuniversalNumberSdouble:
  case SrationalFunctionSdouble:
    d = KopDouble(ob2);
    if (d == 0.0) errorKan1("%s\n","KooDiv2, Division by zero.");
    return(KpoDouble( toDouble0(ob1) / d ) );
    break;

  default:
    if (QuoteMode) {
      rob = divideTree(ob1,ob2);
    }else{
      warningKan("KooDiv2() has not supported yet these objects.\n");
    }
    break;
  }
  return(rob);
}
/* Template 
  case SrationalFunctionSrationalFunction:
    warningKan("Koo() has not supported yet these objects.\n");
    return(rob);
    break;
  case SpolySrationalFunction:
    warningKan("Koo() has not supported yet these objects.\n");
    return(rob);
    break;
  case SrationalFunctionSpoly:
    warningKan("Koo() has not supported yet these objects.\n");
    return(rob);
    break;
  case SuniversalNumberSrationalFunction:
    warningKan("Koo() has not supported yet these objects.\n");
    return(rob);
    break;
  case SrationalFunctionSuniversalNumber:
    warningKan("Koo() has not supported yet these objects.\n");
    return(rob);
    break;
*/

int KisInvalidRational(op)
     objectp op;
{
  extern struct coeff *UniversalZero;
  if (op->tag != SrationalFunction) return(0);
  if (KisZeroObject(Kdenominator(*op))) {
    errorKan1("%s\n","KisInvalidRational(): zero division. You have f/0.");
  }
  if (KisZeroObject(Knumerator(*op))) {
    op->tag = SuniversalNumber;
    op->lc.universalNumber = UniversalZero;
  }
  return(0);
}  

struct object KgbExtension(struct object obj)
{
  char *key;
  int size;
  struct object keyo = OINIT;
  struct object rob = NullObject;
  struct object obj1 = OINIT;
  struct object obj2 = OINIT;
  struct object obj3 = OINIT;
  POLY f1;
  POLY f2;
  POLY f3;
  POLY f;
  int m,i;
  struct pairOfPOLY pf;
  struct coeff *cont;

  if (obj.tag != Sarray) errorKan1("%s\n","KgbExtension(): The argument must be an array.");
  size = getoaSize(obj);
  if (size < 1) errorKan1("%s\n","KgbExtension(): Empty array.");
  keyo = getoa(obj,0);
  if (keyo.tag != Sdollar) errorKan1("%s\n","KgbExtension(): No key word.");
  key = KopString(keyo);

  /* branch by the key word. */
  if (strcmp(key,"isReducible")==0) {
    if (size != 3) errorKan1("%s\n","[(isReducible)  poly1 poly2] gbext.");
    obj1 = getoa(obj,1);
    obj2 = getoa(obj,2);
    if (obj1.tag != Spoly || obj2.tag != Spoly)
      errorKan1("%s\n","[(isReducible)  poly1 poly2] gb.");
    f1 = KopPOLY(obj1);
    f2 = KopPOLY(obj2);
    rob = KpoInteger((*isReducible)(f1,f2));
  }else if (strcmp(key,"lcm") == 0) {
    if (size != 3) errorKan1("%s\n","[(lcm)  poly1 poly2] gb.");
    obj1 = getoa(obj,1);
    obj2 = getoa(obj,2);
    if (obj1.tag != Spoly || obj2.tag != Spoly)
      errorKan1("%s\n","[(lcm)  poly1 poly2] gbext.");
    f1 = KopPOLY(obj1);
    f2 = KopPOLY(obj2);
    rob = KpoPOLY((*lcm)(f1,f2));
  }else if (strcmp(key,"grade")==0) {
    if (size != 2) errorKan1("%s\n","[(grade)  poly1 ] gbext.");
    obj1 = getoa(obj,1);
    if (obj1.tag != Spoly)
      errorKan1("%s\n","[(grade)  poly1 ] gbext.");
    f1 = KopPOLY(obj1);
    rob = KpoInteger((*grade)(f1));
  }else if (strcmp(key,"mod")==0) {
    if (size != 3) errorKan1("%s\n","[(mod) poly num] gbext");
    obj1 = getoa(obj,1);
    obj2 = getoa(obj,2);
    if (obj1.tag != Spoly || obj2.tag != SuniversalNumber) {
      errorKan1("%s\n","The datatype of the argument mismatch: [(mod) polynomial  universalNumber] gbext");
    }
    rob = KpoPOLY( modulopZ(KopPOLY(obj1),KopUniversalNumber(obj2)) );
  }else if (strcmp(key,"tomodp")==0) {
    /* The ring must be a ring of characteristic p. */
    if (size != 3) errorKan1("%s\n","[(tomod) poly ring] gbext");
    obj1 = getoa(obj,1);
    obj2 = getoa(obj,2);
    if (obj1.tag != Spoly || obj2.tag != Sring) {
      errorKan1("%s\n","The datatype of the argument mismatch: [(tomod) polynomial  ring] gbext");
    }
    rob = KpoPOLY( modulop(KopPOLY(obj1),KopRingp(obj2)) );
  }else if (strcmp(key,"tomod0")==0) {
    /* Ring must be a ring of characteristic 0. */
    if (size != 3) errorKan1("%s\n","[(tomod0) poly ring] gbext");
    obj1 = getoa(obj,1);
    obj2 = getoa(obj,2);
    if (obj1.tag != Spoly || obj2.tag != Sring) {
      errorKan1("%s\n","The datatype of the argument mismatch: [(tomod0) polynomial  ring] gbext");
    }
    errorKan1("%s\n","It has not been implemented.");
    rob = KpoPOLY( POLYNULL );
  }else if (strcmp(key,"divByN")==0) {
    if (size != 3) errorKan1("%s\n","[(divByN) poly num] gbext");
    obj1 = getoa(obj,1);
    obj2 = getoa(obj,2);
    if (obj1.tag != Spoly || obj2.tag != SuniversalNumber) {
      errorKan1("%s\n","The datatype of the argument mismatch: [(divByN) polynomial  universalNumber] gbext");
    }
    pf =  quotientByNumber(KopPOLY(obj1),KopUniversalNumber(obj2));
    rob  = newObjectArray(2);
    putoa(rob,0,KpoPOLY(pf.first));
    putoa(rob,1,KpoPOLY(pf.second));
  }else if (strcmp(key,"isConstant")==0) {
    if (size != 2) errorKan1("%s\n","[(isConstant) poly ] gbext bool");
    obj1 = getoa(obj,1);
    if (obj1.tag != Spoly) {
      errorKan1("%s\n","The datatype of the argument mismatch: [(isConstant) polynomial] gbext");
    }
    return(KpoInteger(isConstant(KopPOLY(obj1))));
  }else if (strcmp(key,"isConstantAll")==0) {
    if (size != 2) errorKan1("%s\n","[(isConstantAll) poly ] gbext bool");
    obj1 = getoa(obj,1);
    if (obj1.tag != Spoly) {
      errorKan1("%s\n","The datatype of the argument mismatch: [(isConstantAll) polynomial] gbext");
    }
    return(KpoInteger(isConstantAll(KopPOLY(obj1))));
  }else if (strcmp(key,"schreyerSkelton") == 0) {
    if (size != 2) errorKan1("%s\n","[(schreyerSkelton) array_of_poly ] gbext array");
    obj1 = getoa(obj,1);
    return(KschreyerSkelton(obj1));
  }else if (strcmp(key,"lcoeff") == 0) {
    if (size != 2) errorKan1("%s\n","[(lcoeff) poly] gbext poly");
    obj1 = getoa(obj,1);
    if (obj1.tag != Spoly) errorKan1("%s\n","[(lcoeff) poly] gbext poly");
    f = KopPOLY(obj1);
    if (f == POLYNULL) return(KpoPOLY(f));
    return(KpoPOLY( newCell(coeffCopy(f->coeffp),newMonomial(f->m->ringp))));
  }else if (strcmp(key,"lmonom") == 0) {
    if (size != 2) errorKan1("%s\n","[(lmonom) poly] gbext poly");
    obj1 = getoa(obj,1);
    if (obj1.tag != Spoly) errorKan1("%s\n","[(lmonom) poly] gbext poly");
    f = KopPOLY(obj1);
    if (f == POLYNULL) return(KpoPOLY(f));
    return(KpoPOLY( newCell(intToCoeff(1,f->m->ringp),monomialCopy(f->m))));
  }else if (strcmp(key,"toes") == 0) {
    if (size != 2) errorKan1("%s\n","[(toes) array] gbext poly");
    obj1 = getoa(obj,1);
    if (obj1.tag != Sarray) errorKan1("%s\n","[(toes) array] gbext poly");
    return(KvectorToSchreyer_es(obj1));
  }else if (strcmp(key,"toe_") == 0) {
    if (size != 2) errorKan1("%s\n","[(toe_) array] gbext poly");
    obj1 = getoa(obj,1);
    if (obj1.tag == Spoly) return(obj1);
    if (obj1.tag != Sarray) errorKan1("%s\n","[(toe_) array] gbext poly");
    return(KpoPOLY(arrayToPOLY(obj1)));
  }else if (strcmp(key,"isOrdered") == 0) {
    if (size != 2) errorKan1("%s\n","[(isOrdered) poly] gbext poly");
    obj1 = getoa(obj,1);
    if (obj1.tag != Spoly) errorKan1("%s\n","[(isOrdered) poly] gbext poly");
    return(KisOrdered(obj1));
  }else if (strcmp(key,"reduceContent")==0) {
    if (size != 2) errorKan1("%s\n","[(reduceContent)  poly1 ] gbext.");
    obj1 = getoa(obj,1);
    if (obj1.tag != Spoly)
      errorKan1("%s\n","[(reduceContent)  poly1 ] gbext.");
    f1 = KopPOLY(obj1);
    rob = newObjectArray(2);
    f1 = reduceContentOfPoly(f1,&cont);
    putoa(rob,0,KpoPOLY(f1));
    if (f1 == POLYNULL) {
      putoa(rob,1,KpoPOLY(f1));
    }else{
      putoa(rob,1,KpoPOLY(newCell(cont,newMonomial(f1->m->ringp))));
    }
  }else if (strcmp(key,"ord_ws_all")==0) {
    if (size != 3) errorKan1("%s\n","[(ord_ws_all) fv wv] gbext");
    obj1 = getoa(obj,1);
    obj2 = getoa(obj,2);
    rob  = KordWsAll(obj1,obj2);
  }else if (strcmp(key,"exponents")==0) {
    if (size == 3) {
      obj1 = getoa(obj,1);
      obj2 = getoa(obj,2);
      rob  = KgetExponents(obj1,obj2);
    }else if (size == 2) {
      obj1 = getoa(obj,1);
      obj2 = KpoInteger(2);
      rob  = KgetExponents(obj1,obj2);
    }else{
      errorKan1("%s\n","[(exponents) f type] gbext");
    }
  }else {
    errorKan1("%s\n","gbext : unknown tag.");
  }
  return(rob);
}

struct object KmpzExtension(struct object obj)
{
  char *key;
  int size;
  struct object keyo = OINIT;
  struct object rob = NullObject;
  struct object obj0 = OINIT;
  struct object obj1 = OINIT;
  struct object obj2 = OINIT;
  struct object obj3 = OINIT;
  MP_INT *f;
  MP_INT *g;
  MP_INT *h;
  MP_INT *r0;
  MP_INT *r1;
  MP_INT *r2;
  int gi;
  extern struct ring *SmallRingp;


  if (obj.tag != Sarray) errorKan1("%s\n","KmpzExtension(): The argument must be an array.");
  size = getoaSize(obj);
  if (size < 1) errorKan1("%s\n","KmpzExtension(): Empty array.");
  keyo = getoa(obj,0);
  if (keyo.tag != Sdollar) errorKan1("%s\n","KmpzExtension(): No key word.");
  key = KopString(keyo);

  /* branch by the key word. */
  if (strcmp(key,"gcd")==0) {
    if (size != 3) errorKan1("%s\n","[(gcd)  universalNumber universalNumber] mpzext.");
    obj1 = getoa(obj,1);
    obj2 = getoa(obj,2);
    if (obj1.tag != SuniversalNumber) {
      obj1 = KdataConversion(obj1,"universalNumber");
	}
    if (obj2.tag != SuniversalNumber) {
      obj2 = KdataConversion(obj2,"universalNumber");
	}
    if (obj1.tag != SuniversalNumber || obj2.tag != SuniversalNumber)
      errorKan1("%s\n","[(gcd)  universalNumber universalNumber] mpzext.");
    if (! is_this_coeff_MP_INT(obj1.lc.universalNumber) ||
        ! is_this_coeff_MP_INT(obj2.lc.universalNumber)) {
      errorKan1("%s\n","[(gcd)  universalNumber universalNumber] mpzext.");
    }
    f = coeff_to_MP_INT(obj1.lc.universalNumber);
    g = coeff_to_MP_INT(obj2.lc.universalNumber);
    r1 = newMP_INT();
    mpz_gcd(r1,f,g);
    rob.tag = SuniversalNumber;
    rob.lc.universalNumber = mpintToCoeff(r1,SmallRingp); 
  }else if (strcmp(key,"tdiv_qr")==0) {
    if (size != 3) errorKan1("%s\n","[(tdiv_qr)  universalNumber universalNumber] mpzext.");
    obj1 = getoa(obj,1);
    obj2 = getoa(obj,2);
    if (obj1.tag != SuniversalNumber) {
      obj1 = KdataConversion(obj1,"universalNumber");
	}
    if (obj2.tag != SuniversalNumber) {
      obj2 = KdataConversion(obj2,"universalNumber");
	}
    if (obj1.tag != SuniversalNumber || obj2.tag != SuniversalNumber)
      errorKan1("%s\n","[(tdiv_qr)  universalNumber universalNumber] mpzext.");
    if (! is_this_coeff_MP_INT(obj1.lc.universalNumber) ||
        ! is_this_coeff_MP_INT(obj2.lc.universalNumber)) {
      errorKan1("%s\n","[(tdiv_qr)  universalNumber universalNumber] mpzext.");
    }
    f = coeff_to_MP_INT(obj1.lc.universalNumber);
    g = coeff_to_MP_INT(obj2.lc.universalNumber);
    r1 = newMP_INT();
    r2 = newMP_INT();
    mpz_tdiv_qr(r1,r2,f,g);
    obj1.tag = SuniversalNumber;
    obj1.lc.universalNumber = mpintToCoeff(r1,SmallRingp); 
    obj2.tag = SuniversalNumber;
    obj2.lc.universalNumber = mpintToCoeff(r2,SmallRingp); 
    rob = newObjectArray(2);
    putoa(rob,0,obj1); putoa(rob,1,obj2);
  } else if (strcmp(key,"cancel")==0) {
    if (size != 2) {
      errorKan1("%s\n","[(cancel)  universalNumber/universalNumber] mpzext.");
    }
    obj0 = getoa(obj,1);
    if (obj0.tag == SuniversalNumber) return(obj0);
    if (obj0.tag != SrationalFunction) {
      errorKan1("%s\n","[(cancel)  universalNumber/universalNumber] mpzext.");
      return(obj0);
    }
    obj1 = *(Knumerator(obj0));
    obj2 = *(Kdenominator(obj0));
    if (obj1.tag != SuniversalNumber || obj2.tag != SuniversalNumber) {
      errorKan1("%s\n","[(cancel)  universalNumber/universalNumber] mpzext.");
      return(obj0);
    }
    if (! is_this_coeff_MP_INT(obj1.lc.universalNumber) ||
        ! is_this_coeff_MP_INT(obj2.lc.universalNumber)) {
      errorKan1("%s\n","[(cancel)  universalNumber/universalNumber] mpzext.");
    }
    f = coeff_to_MP_INT(obj1.lc.universalNumber);
    g = coeff_to_MP_INT(obj2.lc.universalNumber);

    r0 = newMP_INT();
    r1 = newMP_INT();
    r2 = newMP_INT();
    mpz_gcd(r0,f,g);
    mpz_divexact(r1,f,r0);
    mpz_divexact(r2,g,r0);
    obj1.tag = SuniversalNumber;
    obj1.lc.universalNumber = mpintToCoeff(r1,SmallRingp); 
    obj2.tag = SuniversalNumber;
    obj2.lc.universalNumber = mpintToCoeff(r2,SmallRingp); 

    rob = KnewRationalFunction0(copyObjectp(&obj1),copyObjectp(&obj2));
    KisInvalidRational(&rob);
  }else if (strcmp(key,"sqrt")==0 ||
            strcmp(key,"com")==0) {
    /*  One arg functions  */
    if (size != 2) errorKan1("%s\n","[key num] mpzext");
    obj1 = getoa(obj,1);
    if (obj1.tag != SuniversalNumber) {
      obj1 = KdataConversion(obj1,"universalNumber");
	}
    if (obj1.tag != SuniversalNumber)
      errorKan1("%s\n","[key num] mpzext : num must be a universalNumber.");
    if (! is_this_coeff_MP_INT(obj1.lc.universalNumber))
      errorKan1("%s\n","[key num] mpzext : num must be a universalNumber.");
    f = coeff_to_MP_INT(obj1.lc.universalNumber);
    if (strcmp(key,"sqrt")==0) {
      r1 = newMP_INT();
      mpz_sqrt(r1,f);
    }else if (strcmp(key,"com")==0) {
      r1 = newMP_INT();
      mpz_com(r1,f);
    }
    rob.tag = SuniversalNumber;
    rob.lc.universalNumber = mpintToCoeff(r1,SmallRingp);
  }else if (strcmp(key,"probab_prime_p")==0 ||
            strcmp(key,"and") == 0 ||
            strcmp(key,"ior")==0) {
    /* Two args functions */
    if (size != 3) errorKan1("%s\n","[key  num1 num2] mpzext.");
    obj1 = getoa(obj,1);
    obj2 = getoa(obj,2);
    if (obj1.tag != SuniversalNumber) {
      obj1 = KdataConversion(obj1,"universalNumber");
	}
    if (obj2.tag != SuniversalNumber) {
      obj2 = KdataConversion(obj2,"universalNumber");
	}
    if (obj1.tag != SuniversalNumber || obj2.tag != SuniversalNumber)
      errorKan1("%s\n","[key num1 num2] mpzext.");
    if (! is_this_coeff_MP_INT(obj1.lc.universalNumber) ||
        ! is_this_coeff_MP_INT(obj2.lc.universalNumber)) {
      errorKan1("%s\n","[key  num1 num2] mpzext.");
    }
    f = coeff_to_MP_INT(obj1.lc.universalNumber);
    g = coeff_to_MP_INT(obj2.lc.universalNumber);
    if (strcmp(key,"probab_prime_p")==0) {
      gi = (int) mpz_get_si(g);
      if (mpz_probab_prime_p(f,gi)) {
        rob = KpoInteger(1);
      }else {
        rob = KpoInteger(0);
      }
    }else if (strcmp(key,"and")==0) {
      r1 = newMP_INT();
      mpz_and(r1,f,g);
      rob.tag = SuniversalNumber;
      rob.lc.universalNumber = mpintToCoeff(r1,SmallRingp);
    }else if (strcmp(key,"ior")==0) {
      r1 = newMP_INT();
      mpz_ior(r1,f,g);
      rob.tag = SuniversalNumber;
      rob.lc.universalNumber = mpintToCoeff(r1,SmallRingp);
    }
      
  }else if (strcmp(key,"powm")==0) {  
    /* three args */
    if (size != 4) errorKan1("%s\n","[key num1 num2 num3] mpzext");
    obj1 = getoa(obj,1); obj2 = getoa(obj,2); obj3 = getoa(obj,3);
    if (obj1.tag != SuniversalNumber) {
      obj1 = KdataConversion(obj1,"universalNumber");
	}
    if (obj2.tag != SuniversalNumber) {
      obj2 = KdataConversion(obj2,"universalNumber");
	}
    if (obj3.tag != SuniversalNumber) {
      obj3 = KdataConversion(obj3,"universalNumber");
	}
    if (obj1.tag != SuniversalNumber ||
        obj2.tag != SuniversalNumber ||
        obj3.tag != SuniversalNumber ) {
      errorKan1("%s\n","[key num1 num2 num3] mpzext : num1, num2 and num3 must be universalNumbers.");
    }
    if (! is_this_coeff_MP_INT(obj1.lc.universalNumber) ||
        ! is_this_coeff_MP_INT(obj2.lc.universalNumber) ||
        ! is_this_coeff_MP_INT(obj3.lc.universalNumber)) {
      errorKan1("%s\n","[key num1 num2 num3] mpzext : num1, num2 and num3 must be universalNumbers.");
    }
    f = coeff_to_MP_INT(obj1.lc.universalNumber);
    g = coeff_to_MP_INT(obj2.lc.universalNumber);
    h = coeff_to_MP_INT(obj3.lc.universalNumber);
    if (mpz_sgn(g) < 0) errorKan1("%s\n","[(powm) base exp mod] mpzext : exp must not be negative.");
    r1 = newMP_INT();
    mpz_powm(r1,f,g,h);
    rob.tag = SuniversalNumber;
    rob.lc.universalNumber = mpintToCoeff(r1,SmallRingp);
  } else if (strcmp(key,"lcm")==0) {
    if (size != 3) errorKan1("%s\n","[(lcm)  universalNumber universalNumber] mpzext.");
    obj1 = getoa(obj,1);
    obj2 = getoa(obj,2);
    if (obj1.tag != SuniversalNumber) {
      obj1 = KdataConversion(obj1,"universalNumber");
	}
    if (obj2.tag != SuniversalNumber) {
      obj2 = KdataConversion(obj2,"universalNumber");
	}
    if (obj1.tag != SuniversalNumber || obj2.tag != SuniversalNumber)
      errorKan1("%s\n","[lcm num1 num2] mpzext.");
    if (! is_this_coeff_MP_INT(obj1.lc.universalNumber) ||
        ! is_this_coeff_MP_INT(obj2.lc.universalNumber)) {
      errorKan1("%s\n","[(lcm)  universalNumber universalNumber] mpzext.");
    }
    f = coeff_to_MP_INT(obj1.lc.universalNumber);
    g = coeff_to_MP_INT(obj2.lc.universalNumber);
    r1 = newMP_INT();
    mpz_lcm(r1,f,g);
    rob.tag = SuniversalNumber;
    rob.lc.universalNumber = mpintToCoeff(r1,SmallRingp); 
  }else {
    errorKan1("%s\n","mpzExtension(): Unknown tag.");
  }
  return(rob);
}


/** : context   */
struct object KnewContext(struct object superObj,char *name) {
  struct context *cp;
  struct object ob = OINIT;
  if (superObj.tag != Sclass) {
    errorKan1("%s\n","The argument of KnewContext must be a Class.Context");
  }
  if (superObj.lc.ival != CLASSNAME_CONTEXT) {
    errorKan1("%s\n","The argument of KnewContext must be a Class.Context");
  }
  cp = newContext0((struct context *)(superObj.rc.voidp),name);
  ob.tag = Sclass;
  ob.lc.ival = CLASSNAME_CONTEXT;
  ob.rc.voidp = cp;
  return(ob);
}

struct object KcreateClassIncetance(struct object ob1,
                                    struct object ob2,
                                    struct object ob3)
{
  /* [class-tag super-obj] size [class-tag]  cclass */
  struct object ob4 = OINIT;
  int size,size2,i;
  struct object ob5 = OINIT;
  struct object rob = OINIT;
  
  if (ob1.tag != Sarray)
    errorKan1("%s\n","cclass: The first argument must be an array.");
  if (getoaSize(ob1) < 1) 
    errorKan1("%s\n","cclass: The first argument must be [class-tag ....].");
  ob4 = getoa(ob1,0);
  if (ectag(ob4) != CLASSNAME_CONTEXT)
    errorKan1("%s\n","cclass: The first argument must be [class-tag ....].");

  if (ob2.tag != Sinteger)
    errorKan1("%s\n","cclass: The second argument must be an integer.");
  size = KopInteger(ob2);
  if (size < 1)
    errorKan1("%s\n","cclass: The size must be > 0.");

  if (ob3.tag != Sarray)
    errorKan1("%s\n","cclass: The third argument must be an array.");
  if (getoaSize(ob3) < 1) 
    errorKan1("%s\n","cclass: The third argument must be [class-tag].");
  ob5 = getoa(ob3,0);
  if (ectag(ob5) != CLASSNAME_CONTEXT)
    errorKan1("%s\n","cclass: The third argument must be [class-tag].");
                    
  rob = newObjectArray(size);
  putoa(rob,0,ob5);
  if (getoaSize(ob1) < size) size2 = getoaSize(ob1);
  else size2 = size;
  for (i=1; i<size2; i++) {
    putoa(rob,i,getoa(ob1,i));
  }
  for (i=size2; i<size; i++) {
    putoa(rob,i,NullObject);
  }
  return(rob);
}


struct object KpoDouble(double a) {
  struct object rob;
  rob.tag = Sdouble;
  /* rob.lc.dbl = (double *)sGC_malloc_atomic(sizeof(double)); */
  rob.lc.dbl = (double *)sGC_malloc(sizeof(double));
  if (rob.lc.dbl == (double *)NULL) {
    fprintf(stderr,"No memory.\n"); exit(10);
  }
  *(rob.lc.dbl) = a;
  return(rob);
}

double toDouble0(struct object ob) {
  double r;
  int r3;
  struct object ob2 = OINIT;
  struct object ob3 = OINIT;
  switch(ob.tag) {
  case Sinteger:
    return( (double) (KopInteger(ob)) );
  case SuniversalNumber:
    return((double) coeffToInt(ob.lc.universalNumber));
  case SrationalFunction:
    /* The argument is assumed to be a rational number. */
    ob2 = newObjectArray(2);  ob3 = KpoString("cancel");
    putoa(ob2,0,ob3); putoa(ob2,1,ob);
    ob = KmpzExtension(ob2);
    ob2 = *Knumerator(ob);  ob3 = *Kdenominator(ob);
    r3 =  coeffToInt(ob3.lc.universalNumber);
    if (r3  == 0) {
      errorKan1("%s\n","toDouble0(): Division by zero.");
      break;
    }
    r = ((double) coeffToInt(ob2.lc.universalNumber)) / ((double)r3);
    return(r);
  case Sdouble:
    return( KopDouble(ob) );
  default:
    errorKan1("%s\n","toDouble0(): This type of conversion is not supported.");
    break;
  }
  return(0.0);
}

struct object KpoGradedPolySet(struct gradedPolySet *grD) {
  struct object rob = OINIT;
  rob.tag = Sclass;
  rob.lc.ival = CLASSNAME_GradedPolySet;
  rob.rc.voidp = (void *) grD;
  return(rob);
}

static char *getspace0(int a) {
  char *s;
  a = (a > 0? a:-a);
  s = (char *) sGC_malloc(a+1);
  if (s == (char *)NULL) {
    errorKan1("%s\n","no more memory.");
  }
  return(s);
}
struct object KdefaultPolyRing(struct object ob) {
  struct object rob = OINIT;
  int i,j,k,n;
  struct object ob1 = OINIT;
  struct object ob2 = OINIT;
  struct object ob3 = OINIT;
  struct object ob4 = OINIT;
  struct object ob5 = OINIT;
  struct object t1 = OINIT;
  char *s1;
  extern struct ring *CurrentRingp;
  static struct ring *a[N0];
  
  rob = NullObject;
  if (ob.tag != Sinteger) {
    errorKan1("%s\n","KdefaultPolyRing(): the argument must be integer.");
  }
  n = KopInteger(ob);
  if (n <= 0) {
    /* initializing */
    for (i=0; i<N0; i++) {
      a[i] = (struct ring*) NULL;
    }
    return(rob);
  }
  
  if ( a[n] != (struct ring*)NULL) return(KpoRingp(a[n]));

  /* Let's construct ring of polynomials of 2n variables  */
  /* x variables */
  ob1 = newObjectArray(n);
  for (i=0; i<n; i++) {
    s1 = getspace0(1+ ((n-i)/10) + 1);
    sprintf(s1,"x%d",n-i);
    putoa(ob1,i,KpoString(s1));
  }
  ob2 = newObjectArray(n);
  s1 = getspace0(1);
  sprintf(s1,"h");
  putoa(ob2,0,KpoString(s1));
  for (i=1; i<n; i++) {
    s1 = getspace0(1+((n+n-i)/10)+1);
    sprintf(s1,"x%d",n+n-i);
    putoa(ob2,i,KpoString(s1));
  }

  ob3 = newObjectArray(9);
  putoa(ob3,0,KpoInteger(0));
  for (i=1; i<9; i++) {
    putoa(ob3,i,KpoInteger(n));
  }

  ob4 = newObjectArray(2*n);
  t1 = newObjectArray(2*n);
  for (i=0; i<2*n; i++) {
    putoa(t1,i,KpoInteger(1));
  }
  putoa(ob4,0,t1);
  for (i=1; i<2*n; i++) {
    t1 = newObjectArray(2*n);
    for (j=0; j<2*n; j++) {
      putoa(t1,j,KpoInteger(0));
      if (j == (2*n-i)) {
        putoa(t1,j,KpoInteger(-1));
      }
    }
    putoa(ob4,i,t1);
  }

  ob5 = newObjectArray(2);
  putoa(ob5,0,KpoString("mpMult"));
  putoa(ob5,1,KpoString("poly"));

  KsetUpRing(ob1,ob2,ob3,ob4,ob5);
  a[n] = CurrentRingp;
  return(KpoRingp(a[n]));
}
  
  
struct object Krest(struct object ob) {
  struct object rob;
  struct object *op;
  int n,i;
  if (ob.tag == Sarray) {
    n = getoaSize(ob);
    if (n == 0) return ob;
    rob = newObjectArray(n-1);
    for (i=1; i<n; i++) {
      putoa(rob,i-1,getoa(ob,i));
    }
    return rob;
  }else if ((ob.tag == Slist) || (ob.tag == Snull)) {
    return Kcdr(ob);
  }else{
    errorKan1("%s\n","Krest(ob): ob must be an array or a list.");
  }
}
struct object Kjoin(struct object ob1, struct object ob2) {
  struct object rob = OINIT;
  int n1,n2,i;
  if ((ob1.tag == Sarray) &&  (ob2.tag == Sarray)) {
    n1 = getoaSize(ob1); n2 = getoaSize(ob2);
    rob = newObjectArray(n1+n2);
    for (i=0; i<n1; i++) {
      putoa(rob,i,getoa(ob1,i));
    }
    for (i=n1; i<n1+n2; i++) {
      putoa(rob,i,getoa(ob2,i-n1));
    }
    return rob;
  }else if ((ob1.tag == Slist) || (ob1.tag == Snull)) {
	if ((ob2.tag == Slist) || (ob2.tag == Snull)) {
	  return KvJoin(ob1,ob2);
	}else{
	  errorKan1("%s\n","Kjoin: both argument must be a list.");
	}
  }else{
    errorKan1("%s\n","Kjoin: arguments must be arrays.");
  }
}
  
struct object Kget(struct object ob1, struct object ob2) {
  struct object rob = OINIT;
  struct object tob = OINIT;
  int i,j,size,n;
  if (ob2.tag == Sinteger) {
    i =ob2.lc.ival;
  }else if (ob2.tag == SuniversalNumber) {
    i = KopInteger(KdataConversion(ob2,"integer"));
  }else if (ob2.tag == Sarray) {
    n = getoaSize(ob2);
    if (n == 0) return ob1;
    rob = ob1;
    for (i=0; i<n; i++) {
      rob=Kget(rob,getoa(ob2,i));
    }
    return rob;
  }
  if (ob1.tag == Sarray) {
    size = getoaSize(ob1);
    if ((0 <= i) && (i<size)) {
      return(getoa(ob1,i));
    }else{
      errorKan1("%s\n","Kget: Index is out of bound. (get)\n");
    }
  }else if (ob1.tag == Slist) {
    rob = NullObject;
    if (i < 0) errorKan1("%s\n","Kget: Index is negative. (get)"); 
    for (j=0; j<i; j++) {
      rob = Kcdr(ob1);
      if ((ob1.tag == Snull) && (rob.tag == Snull)) {
        errorKan1("%s\n","Kget: Index is out of bound. (get) cdr of null list.\n");
      }
      ob1 = rob;
    }
    return Kcar(ob1);
  } else if (ob1.tag == SbyteArray) {
    size = getByteArraySize(ob1);
    if ((0 <= i) && (i<size)) {
      return(KpoInteger(KopByteArray(ob1)[i]));
    }else{
      errorKan1("%s\n","Kget: Index is out of bound. (get)\n");
    }
  } else if (ob1.tag == Sdollar) {
    unsigned char *sss;
    sss = (unsigned char *) KopString(ob1);
    size = strlen(sss);
    if ((0 <= i) && (i<size)) {
      return(KpoInteger(sss[i]));
    }else{
      errorKan1("%s\n","Kget: Index is out of bound. (get)\n");
    }

  }else errorKan1("%s\n","Kget: argument must be an array or a list.");
}

/* Constructor of byteArray */
struct object newByteArray(int size,struct object obj) {
  unsigned char *ba;
  unsigned char *ba2;
  struct object rob = OINIT;
  struct object tob = OINIT;
  int i,n;
  ba = NULL;
  if (size > 0) {
    ba = (unsigned char *) sGC_malloc(size);
    if (ba == NULL) errorKan1("%s\n","No more memory.");
  }
  rob.tag = SbyteArray; rob.lc.bytes = ba; rob.rc.ival = size;
  if (obj.tag == SbyteArray) {
    n = getByteArraySize(obj);
    ba2 = KopByteArray(obj);
    for (i=0; i<(n<size?n:size); i++) {
      ba[i] = ba2[i];
    }
    for (i=n; i<size; i++) ba[i] = 0;
    return rob;
  }else if (obj.tag == Sarray) {
    n = getoaSize(obj);
    for (i=0; i<n; i++) {
      tob = getoa(obj,i);
      tob = Kto_int32(tob);
      if (tob.tag != Sinteger) errorKan1("%s\n","newByteArray: array is not an array of integer or universalNumber.");
      ba[i] = (unsigned char) KopInteger(tob);
    }
    for (i=n; i<size; i++) ba[i] = 0;
    return rob;
  }else{
    for (i=0; i<size; i++) ba[i] = 0;
    return rob;
  }
}
struct object newByteArrayFromStr(char *s,int size) {
  unsigned char *ba;
  struct object rob = OINIT;
  int i;
  ba = NULL;
  if (size > 0) {
    ba = (unsigned char *) sGC_malloc(size);
    if (ba == NULL) errorKan1("%s\n","No more memory.");
  }
  rob.tag = SbyteArray; rob.lc.bytes = ba; rob.rc.ival = size;
  for (i=0; i<size; i++) {
	ba[i] = (char) s[i];
  }
  return(rob);
}

struct object byteArrayToArray(struct object obj) {
  int n,i; unsigned char *ba;
  struct object rob = OINIT;
  if (obj.tag != SbyteArray) errorKan1("%s\n","byteArrayToArray: argument is not an byteArray.");
  n = getByteArraySize(obj);
  rob = newObjectArray(n);
  ba = KopByteArray(obj);
  for (i=0; i<n; i++) putoa(rob,i,KpoInteger((int) ba[i]));
  return rob;
}

struct object KgetAttributeList(struct object ob){
  struct object rob = OINIT;
  if (ob.attr != NULL) rob = *(ob.attr);
  else rob = NullObject;
  return rob;
}
struct object  KsetAttributeList(struct object ob,struct object attr) {
  ob.attr = newObject();
  *(ob.attr) = attr;
  return ob;
}
struct object KgetAttribute(struct object ob,struct object key) {
  struct object rob = OINIT;
  struct object alist = OINIT;
  int n,i;
  struct object tob = OINIT;
  char *s;
  rob = NullObject;
  if (ob.attr == NULL) return rob;
  alist = *(ob.attr);
  if (alist.tag != Sarray) return rob;
  if (key.tag != Sdollar) return rob;
  s = KopString(key);
  n = getoaSize(alist);
  for (i = 0; i < n; i += 2) {
    tob = getoa(alist,i);
    if (tob.tag == Sdollar) {
      if (strcmp(KopString(tob),s) == 0) {
        if (i+1 < n) rob = getoa(alist,i+1);
        return rob;
      }
    }
  }
  return rob;
}
/*  ob (key) (value) setAttribute /ob set. They are not destructive. */
struct object KsetAttribute(struct object ob,struct object key,struct object value) {
  struct object rob = OINIT;
  struct object alist = OINIT;
  int n,i;
  char *s = "";
  struct object tob = OINIT;
  rob = ob;
  if (ob.attr == NULL) {
    rob.attr = newObject();
    *(rob.attr) = newObjectArray(2);
    putoa((*(rob.attr)),0,key);
    putoa((*(rob.attr)),1,value);
    return rob;
  }
  alist = *(ob.attr);
  if (alist.tag != Sarray) return rob;
  if (key.tag != Sdollar) {
    s = KopString(key);
  }
  n = getoaSize(alist);
  for (i = 0; i < n; i += 2) {
    tob = getoa(alist,i);
    if (tob.tag == Sdollar) {
      if (strcmp(KopString(tob),s) == 0) {
        if (i+1 < n) putoa(alist,i+1,value);
        return rob;
      }
    }
  }

  rob.attr = newObject();
  *(rob.attr) = newObjectArray(n+2);
  for (i=0; i<n; i++) {
    putoa((*(rob.attr)),i,getoa((*(ob.attr)),i));
  }
  putoa((*(rob.attr)),n,key);
  putoa((*(rob.attr)),n+1,value);
  return rob;
}

struct object KaddModuleOrder(struct object ob) {
  struct ring *rp;
  int *order;
  int n;
  int *ord_orig;
  int rank_of_module;
  int array_size;
  int *pos_array;
  int **weight_array;
  int i,j;
  struct object weight;
  extern struct ring *CurrentRingp;
  struct object rob;

  rob = ob;
  rp = CurrentRingp;
  if (rp->module_rank < 1) {
    errorKan1("%s\n","KaddModuleOrder(): (mmLarger) (module_matrix) switch_function has not been executed.");
  }
  n = rp->n;
  ord_orig=rp->order_orig;

  rank_of_module = KopInteger(getoa(ob,0));
  array_size = (getoaSize(ob)-1)/2;
  if (array_size==0) errorKan1("%s\n","Format error in KaddModuleOrder(): [rank_of_module pos1 weight1 pos2 wegith2 ...]");
  pos_array=(int *)sGC_malloc(sizeof(int)*array_size);
  weight_array=(int **) sGC_malloc(sizeof(int)*array_size);
  for (i=0; i<array_size; i++) weight_array[i]=NULL;
  for (i=0; i<array_size; i++) {
    pos_array[i] = KopInteger(getoa(ob,1+2*i));
    weight=getoa(ob,1+2*i+1);
    if (isObjectArray(weight)) {
      if (getoaSize(weight) != 2*n+rank_of_module) errorKan1("%s\n","Format error in KaddModuleOrder(): length of weight");
      weight_array[i] = (int *) sGC_malloc(sizeof(int)*(2*n+rank_of_module));
      for (j=0; j<2*n+rank_of_module; j++) {
	weight_array[i][j] = KopInteger(getoa(weight,j));
      }
    }
  }

  order = add_module_order(n,ord_orig,rank_of_module,array_size,pos_array,weight_array);
  rp->order = order;
  rp->order_row_size = 2*n+rank_of_module;
  rp->order_col_size = 2*n+array_size;
  rp->module_rank = rank_of_module;
  return rob;
}

/******************************************************************
     Error handler
******************************************************************/

int errorKan1(str,message)
     char *str;
     char *message;
{
  extern char *GotoLabel;
  extern int GotoP;
  extern int ErrorMessageMode;
  extern int RestrictedMode, RestrictedMode_saved;
  char tmpc[1024];
  RestrictedMode = RestrictedMode_saved;
  cancelAlarm();
  if (ErrorMessageMode == 1 || ErrorMessageMode == 2) {
    sprintf(tmpc,"\nERROR(kanExport[0|1].c): ");
    if (strlen(message) < 900) {
      strcat(tmpc,message);
    }
    pushErrorStack(KnewErrorPacket(SerialCurrent,-1,tmpc));
  }
  if (ErrorMessageMode != 1) {
    fprintf(stderr,"\nERROR(kanExport[0|1].c): ");
    fprintf(stderr,str,message);
    (void) traceShowStack(); traceClearStack();
  }
  /* fprintf(stderr,"Hello "); */
  if (GotoP) {
    /* fprintf(stderr,"Hello. GOTO "); */
    fprintf(Fstack,"The interpreter was looking for the label <<%s>>. It is also aborted.\n",GotoLabel);
    GotoP = 0;
  }
  stdOperandStack(); contextControl(CCRESTORE);
  /* fprintf(stderr,"Now. Long jump!\n"); */
#if defined(__CYGWIN__)
  MYSIGLONGJMP(EnvOfStackMachine,1);
#else
  MYLONGJMP(EnvOfStackMachine,1);
#endif
}


int warningKan(str)
     char *str;
{
  extern int WarningMessageMode;
  extern int Strict;
  char tmpc[1024];
  if (WarningMessageMode == 1 || WarningMessageMode == 2) {
    sprintf(tmpc,"\nWARNING(kanExport[0|1].c): ");
    if (strlen(str) < 900) {
      strcat(tmpc,str);
    }
    pushErrorStack(KnewErrorPacket(SerialCurrent,-1,tmpc));
  }
  if (WarningMessageMode != 1) {
    fprintf(stderr,"\nWARNING(kanExport[0|1].c): ");
    fprintf(stderr,"%s",str);
    fprintf(stderr,"\n");
  }
  /* if (Strict) errorKan1("%s\n"," "); */
  if (Strict) errorKan1("%s\n",str);
  return(0);
}

int warningKanNoStrictMode(str)
     char *str; 
{
  extern int Strict;
  int t;
  t = Strict;
  Strict = 0;
  warningKan(str);
  Strict = t;
  return(0);
}




