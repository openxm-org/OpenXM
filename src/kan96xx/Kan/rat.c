/* $OpenXM$ */

/* rat.c: rational functions */
#include <stdio.h>
#include "datatype.h"
#include "stackm.h"
#include "extern.h"
#include "extern2.h"

static void errorRationalFunction(char *str);
static void warningRationalFunction(char *str);

struct object KnewRationalFunction0(op1,op2)
objectp op1;
objectp op2;
{
  struct object rob;
  rob.tag = SrationalFunction;
  Knumerator(rob) = op1;
  Kdenominator(rob) = op2;
  KisInvalidRational(&rob);
  return(rob);
}

  
int KisZeroObject(op)
objectp op;
{
  switch(op->tag) {
  case Spoly:
    if ((op->lc.poly) ISZERO) return(1);
    else return(0);
    break;
  case SuniversalNumber:
    if (isZero( op->lc.universalNumber)) return(1);
    else return(0);
    break;
  default:
    warningRationalFunction("KisZeroObject() does not support this object.");
    return(0);
  }
}

objectp copyObjectp(op)
objectp op;
{
  objectp rop;
  rop = newObject();
  *rop = *op;
  return(rop);
}

void errorRationalFunction(str)
char *str;
{
  fprintf(stderr,"Error(rat.c): %s\n",str);
  exit(20);
}

void warningRationalFunction(str)
char *str;
{
  fprintf(stderr,"Warning(rat.c): %s\n",str);
}

