/* $OpenXM$ */
#include <stdio.h>
#include "datatype.h"
#include "extern2.h"

static int badLRule(POLY set[],int num);

POLY mReplace(mm,lSideX,rSideX,sizex,lSideD,rSideD,sized,commutative)
POLY mm;
int lSideX[];
POLY rSideX[];  /* Rule: a=lSideX[i], x_a ---> rSideX[i] */
int sizex;
int lSideD[];   /* Rule: b=lSideD[i], D_b ---> rSideD[i] */
POLY rSideD[];
int sized;
int commutative;
{
/* The function should be tuned by using a table. */
  POLY rp;
  POLY fac;
  int i;
  int j;
  int flag;
  MONOMIAL mp;
  int n;

  mp = mm->m;
  rp = newCell(coeffCopy(mm->coeffp),newMonomial(mp->ringp));
  n = mp->ringp->n;

  /* Multiply backwards. It's faster. */
  for (i=n-1; i>=0; i--) {
    if (mp->e[i].D != 0) {
      flag = 1;
      for (j=0; j<sized; j++) {
	if (lSideD[j] == i) {
	  if (commutative) fac = pPower_poly(rSideD[j],mp->e[i].D);
	  else fac = pPower(rSideD[j],mp->e[i].D);
	  /* if negative, this does not work  well!! */
	  flag = 0;
	  break;
	}
      }
      if (flag) fac = cdd(1,i,mp->e[i].D,mp->ringp);
      if (commutative) rp = ppMult_poly(fac,rp);
      else rp = ppMult(fac,rp);
    }
  }

  for (i=n-1; i>=0; i--) {
    if (mp->e[i].x != 0) {
      flag = 1;
      for (j=0; j<sizex; j++) {
	if (lSideX[j] == i) {
	  if (commutative) fac = pPower_poly(rSideX[j],mp->e[i].x);
	  else fac = pPower(rSideX[j],mp->e[i].x);
	  /* if negative, this does not work well!! */
	  flag = 0;
	  break;
	}
      }
      if (flag) fac = cxx(1,i,mp->e[i].x,mp->ringp);
      if (commutative) rp = ppMult_poly(fac,rp);
      else rp = ppMult(fac,rp);
    }
  }

  return(rp);
}

/*
  lRule[i] ---> rRule[i]
*/
POLY replace(f,lRule,rRule,num)
POLY f;
POLY lRule[];  /* lRule[i] must be x0 or ... or D{N-1} */
POLY rRule[];
int num;
{
  POLY rSideX[N0];
  POLY rSideD[N0];
  int  lSideX[N0];
  int  lSideD[N0];
  int i,j,size;
  int sizex,sized;
  POLY rp;
  int n;

  /***printf("f=%s\n",POLYToString(f,'*',0));
  for (i=0; i<num; i++) {
    printf("%s --> %s\n",POLYToString(lRule[i],'*',0),
	   POLYToString(rRule[i],'*',0));
  }
  printf("\n"); ***/
  
  if (f ISZERO) return(ZERO);
  sizex = sized = 0;
  if (badLRule(lRule,num)) {
    warningPoly("replace(): lRule must be a variable.");
    return(ZERO);
  }
  n = f->m->ringp->n;
  /* x-part */
  for (i=0; i<n; i++) {
    for (j=0; j<num; j++) {
      if (lRule[j]->m->e[i].x == 1) {
	lSideX[sizex] = i;
	rSideX[sizex] = rRule[j];
	sizex++;
	if (sizex >= n) {
	  warningPoly(" replace(): too many x-rules . ");
	  sizex--;
	}
	break;
      }
    }
  }
  /* D-part */
  for (i=0; i<n; i++) {
    for (j=0; j<num; j++) {
      if (lRule[j]->m->e[i].D == 1) {
	lSideD[sized] = i;
	rSideD[sized] = rRule[j];
	sized++;
	if (sized >= n) {
	  warningPoly(" replacen(): too many D-rules . ");
	  sized--;
	}
	break;
      }
    }
  }

  rp = ZERO;
  if (f ISZERO) return(ZERO);
  while(f != POLYNULL) {
    rp = ppAddv(rp,mReplace(f,lSideX,rSideX,sizex,lSideD,rSideD,sized,0));
    f = f->next;
  }
  return(rp);
}
      

/* For the dirty trick of mpMult_difference */  
POLY replace_poly(f,lRule,rRule,num)  
POLY f;
POLY lRule[];  /* lRule[i] must be x0 or ... or D{N-1} */
POLY rRule[];
int num;
{
  POLY rSideX[N0];
  POLY rSideD[N0];
  int  lSideX[N0];
  int  lSideD[N0];
  int i,j,size;
  int sizex,sized;
  POLY rp;
  int n;

  /***printf("f=%s\n",POLYToString(f,'*',0));
  for (i=0; i<num; i++) {
    printf("%s --> %s\n",POLYToString(lRule[i],'*',0),
	   POLYToString(rRule[i],'*',0));
  }
  printf("\n"); ***/
  
  if (f ISZERO) return(ZERO);
  sizex = sized = 0;
  if (badLRule(lRule,num)) {
    warningPoly("replace(): lRule must be a variable.");
    return(ZERO);
  }
  n = f->m->ringp->n;
  /* x-part */
  for (i=0; i<n; i++) {
    for (j=0; j<num; j++) {
      if (lRule[j]->m->e[i].x == 1) {
	lSideX[sizex] = i;
	rSideX[sizex] = rRule[j];
	sizex++;
	if (sizex >= n) {
	  warningPoly(" replace(): too many x-rules . ");
	  sizex--;
	}
	break;
      }
    }
  }
  /* D-part */
  for (i=0; i<n; i++) {
    for (j=0; j<num; j++) {
      if (lRule[j]->m->e[i].D == 1) {
	lSideD[sized] = i;
	rSideD[sized] = rRule[j];
	sized++;
	if (sized >= n) {
	  warningPoly(" replacen(): too many D-rules . ");
	  sized--;
	}
	break;
      }
    }
  }

  rp = ZERO;
  if (f ISZERO) return(ZERO);
  while(f != POLYNULL) {
    rp = ppAddv(rp,mReplace(f,lSideX,rSideX,sizex,lSideD,rSideD,sized,1));
    f = f->next;
  }
  return(rp);
}
      
  
static int badLRule(set,num)
POLY set[];
int num;
{ int i;
  for (i=0; i<num; i++) {
    if (set[0] ISZERO) {
      return(1);
    }
  }
  return(0);
}

