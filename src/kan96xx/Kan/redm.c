/* $OpenXM: OpenXM/src/kan96xx/Kan/redm.c,v 1.6 2003/08/24 05:19:43 takayama Exp $ */
#include <stdio.h>
#include <stdlib.h>
#include "datatype.h"
#include "extern2.h"
#include "gradedset.h"

#define mymax(p,q) (p>q?p:q)

static int DebugReduction = 0;


int isReducible_module(f,g)
     POLY f;
     POLY g;
{
  int n,i;
  MONOMIAL tf;
  MONOMIAL tg;
  
  if (f ISZERO) return(0);
  if (g ISZERO) return(0);

  checkRingIsR(f,g);

  if (!(*isSameComponent)(f,g)) return(0);
  tf = f->m; tg = g->m; n = tf->ringp->n;
  for (i=0; i<n; i++) {
    if (tf->e[i].x < tg->e[i].x) return(0);
    if (tf->e[i].D < tg->e[i].D) return(0);
  }
  return(1);
}


int isSameComponent_x(f,g)
     POLY f;
     POLY g;
{
  static int nn,mm,ll,cc,n,m,l,c;
  static struct ring *cr = (struct ring *)NULL;
  MONOMIAL tf;
  MONOMIAL tg;
  int i;

  //printf("%s, %s\n",POLYToString(f,'*',1),POLYToString(g,'*',1));
  if (f ISZERO) return(1);
  if (g ISZERO) return(1);

  tf = f->m; tg = g->m;
  if (tf->ringp != cr) {
    n = tf->ringp->n;
    m = tf->ringp->m;
    l = tf->ringp->l;
    c = tf->ringp->c;
    nn = tf->ringp->nn;
    mm = tf->ringp->mm;
    ll = tf->ringp->ll;
    cc = tf->ringp->cc;
    cr = tf->ringp;
  }
  for (i=cc; i<c; i++) {
    if (tf->e[i].x != tg->e[i].x) return(0);
    if (tf->e[i].D != tg->e[i].D) return(0);
  }
  for (i=ll; i<l; i++) {
    if (tf->e[i].x != tg->e[i].x) return(0);
    /*if (tf->e[i].D != tg->e[i].D) return(0);*/
  }
  for (i=mm; i<m; i++) {
    if (tf->e[i].x != tg->e[i].x) return(0);
    /*if (tf->e[i].D != tg->e[i].D) return(0);*/
  }
  for (i=nn; i<n; i++) {
    if (tf->e[i].x != tg->e[i].x) return(0);
    /*if (tf->e[i].D != tg->e[i].D) return(0);*/
  }
  return(1);
}

int isSameComponent_xd(f,g)
     POLY f;
     POLY g;
{
  static int nn,mm,ll,cc,n,m,l,c;
  static struct ring *cr = (struct ring *)NULL;
  MONOMIAL tf;
  MONOMIAL tg;
  int i;

  if (f ISZERO) return(1);
  if (g ISZERO) return(1);

  tf = f->m; tg = g->m;
  if (tf->ringp != cr) {
    n = tf->ringp->n;
    m = tf->ringp->m;
    l = tf->ringp->l;
    c = tf->ringp->c;
    nn = tf->ringp->nn;
    mm = tf->ringp->mm;
    ll = tf->ringp->ll;
    cc = tf->ringp->cc;
    cr = tf->ringp;
  }
  for (i=cc; i<c; i++) {
    if (tf->e[i].x != tg->e[i].x) return(0);
    if (tf->e[i].D != tg->e[i].D) return(0);
  }
  for (i=ll; i<l; i++) {
    if (tf->e[i].x != tg->e[i].x) return(0);
    if (tf->e[i].D != tg->e[i].D) return(0);
  }
  for (i=mm; i<m; i++) {
    if (tf->e[i].x != tg->e[i].x) return(0);
    if (tf->e[i].D != tg->e[i].D) return(0);
  }
  for (i=nn; i<n; i++) {
    if (tf->e[i].x != tg->e[i].x) return(0);
    if (tf->e[i].D != tg->e[i].D) return(0);
  }
  return(1);
}


POLY lcm_module(f,g)
     POLY f;
     POLY g;
{
  MONOMIAL tf,tg;
  MONOMIAL lcm;
  int n;
  int i;
  
  tf = f->m; tg = g->m;
  if (!(*isSameComponent)(f,g)) return(ZERO);
  n = tf->ringp->n;
  lcm = newMonomial(tf->ringp);
  for (i=0; i<n; i++) {
    lcm->e[i].x = mymax(tf->e[i].x,tg->e[i].x);
    lcm->e[i].D = mymax(tf->e[i].D,tg->e[i].D);
  }
  return(newCell(intToCoeff(1,tf->ringp),lcm));
}


int grade_module1v(f)
     POLY f;
{
  int r;
  int i;
  MONOMIAL tf;
  static int nn,mm,ll,cc,n,m,l,c;
  static struct ring *cr = (struct ring *)NULL;

  if (f ISZERO) return(-1);
  tf = f->m;
  if (tf->ringp != cr) {
    n = tf->ringp->n;
    m = tf->ringp->m;
    l = tf->ringp->l;
    c = tf->ringp->c;
    nn = tf->ringp->nn;
    mm = tf->ringp->mm;
    ll = tf->ringp->ll;
    cc = tf->ringp->cc;
    cr = tf->ringp;
  }

  r = 0;
  for (i=0; i<cc; i++) {
    r += tf->e[i].x;
    r += tf->e[i].D;
  }
  for (i=c; i<ll; i++) {
    r += tf->e[i].x;
    r += tf->e[i].D;
  }
  for (i=l; i<mm; i++) {
    r += tf->e[i].x;
    r += tf->e[i].D;
  }
  for (i=m; i<nn; i++) {
    r += tf->e[i].x;
    r += tf->e[i].D;
  }
  /*printf("%s --> %d\n",POLYToString(f,'*',1),r);*/
  return(r);
}


int grade_module1(f)
     POLY f;
{
  int r;
  int i;
  MONOMIAL tf;
  static int nn,mm,ll,cc,n,m,l,c;
  static struct ring *cr = (struct ring *)NULL;

  if (f ISZERO) return(-1);
  tf = f->m;
  if (tf->ringp != cr) {
    n = tf->ringp->n;
    m = tf->ringp->m;
    l = tf->ringp->l;
    c = tf->ringp->c;
    nn = tf->ringp->nn;
    mm = tf->ringp->mm;
    ll = tf->ringp->ll;
    cc = tf->ringp->cc;
    cr = tf->ringp;
  }

  r = 0;
  for (i=0; i<n; i++) {
    r += tf->e[i].x;
    r += tf->e[i].D;
  }
  /*printf("%s --> %d\n",POLYToString(f,'*',1),r);*/
  return(r);
}



int grade_firstvec(f)   /* grading by the first vector and h */
     POLY f;
{
  int r;
  int i,k;
  int exp[2*N0];
  MONOMIAL tf;
  static int n;
  static int *order,*from,*to;
  static struct ring *cr = (struct ring *)NULL;

  if (f ISZERO) return(-1);
  tf = f->m;
  if (tf->ringp != cr) {
    n = tf->ringp->n;
    order = tf->ringp->order;
    from = tf->ringp->from;
    to = tf->ringp->to;
  }

  for (i=n-1,k=0; i>=0; i--,k++) {
    exp[k] = tf->e[i].x;
    exp[k+n] = tf->e[i].D;
  }
  r = exp[2*n-1]; /* degree of h */
  for (i=from[0]; i<to[0]; i++) {
    r += exp[i]*order[i];
  }
  /*printf("%s --> %d\n",POLYToString(f,'*',1),r);*/
  return(r);
}

int eliminated(ff)
     POLY ff;
{
#define RULEMAX 10
  int r;
  int i; int k;
  MONOMIAL tf;
  static int nn,mm,ll,cc,n,m,l,c;
  static struct ring *cr = (struct ring *)NULL;
  POLY f;
  POLY lRule[RULEMAX];
  POLY rRule[RULEMAX];

  if (ff ISZERO) return(-1);
  tf = ff->m;
  if (tf->ringp != cr) {
    n = tf->ringp->n;
    m = tf->ringp->m;
    l = tf->ringp->l;
    c = tf->ringp->c;
    nn = tf->ringp->nn;
    mm = tf->ringp->mm;
    ll = tf->ringp->ll;
    cc = tf->ringp->cc;
    cr = tf->ringp;
  }
  
  lRule[0] = cdd(1,0,1,ff->m->ringp);  /* h */
  rRule[0] = cxx(1,0,0,ff->m->ringp);  /* 1 */
  k = 1;
  if ( c-cc + l-ll + m-mm + n -nn >= RULEMAX-1){
    fprintf(stderr,"redm.c: RULEMAX is small.\n");
    exit(1);
  }
  for (i=cc; i<c; i++,k++) {
    lRule[k] = cdd(1,i,1,ff->m->ringp);
    rRule[k] = ZERO;
  }
  for (i=ll; i<l; i++,k++) {
    lRule[k] = cdd(1,i,1,ff->m->ringp);
    rRule[k] = cxx(1,0,0,ff->m->ringp); /* Qe = 1???? */
  }
  for (i=mm; i<m; i++,k++) {
    lRule[k] = cdd(1,i,1,ff->m->ringp);
    rRule[k] = cxx(1,0,0,ff->m->ringp); /* Ee = 1 */
  }
  for (i=nn; i<n; i++,k++) {
    lRule[k] = cdd(1,i,1,ff->m->ringp);
    rRule[k] = ZERO;   /* De = 0 */
  }
  f = replace(ff,lRule,rRule,k);


  if (f ISZERO) return(-1);
  for (i=cc; i<c; i++) {
    if (tf->e[i].x) return 0;
  }
  for (i=ll; i<l; i++) {
    if (tf->e[i].x) return 0;
  }
  for (i=mm; i<m; i++) {
    if (tf->e[i].x) return 0;
  }
  for (i=nn; i<n; i++) {
    if (tf->e[i].x) return 0;
  }
  return(1);
}

/* Grading by the weight vector (0,1). Note 2003.07.10 
   dGrade1 is the grade of the in(f).
*/
int dGrade1(f)
     POLY f;
{
  int r;
  int i;
  MONOMIAL tf;
  static int nn,mm,ll,cc,n,m,l,c;
  static struct ring *cr = (struct ring *)NULL;
  extern int *DegreeShiftD_vec;
  extern int DegreeShiftD_size;

  if (f ISZERO) return(-1);
  tf = f->m;
  if (tf->ringp != cr) {
    n = tf->ringp->n;
    m = tf->ringp->m;
    l = tf->ringp->l;
    c = tf->ringp->c;
    nn = tf->ringp->nn;
    mm = tf->ringp->mm;
    ll = tf->ringp->ll;
    cc = tf->ringp->cc;
    cr = tf->ringp;
  }

  r = 0;
  for (i=m; i<nn; i++) {
    r += tf->e[i].D;
  }
  if (DegreeShiftD_size > 0) {
	if ((tf->e[n-1].x <DegreeShiftD_size) && (tf->e[n-1].x >= 0)) {
	  r += DegreeShiftD_vec[tf->e[n-1].x];
	}else{
	  /* warning. out of range. */ 
	}
  }
  return(r);
}

/* Grading by the weight vector (0,1). Note 2003.07.10 
*/
int dGrade(f)
     POLY f;
{
  int r, first, t;
  if (f ISZERO) return(-1);
  first = 1;
  while (f != POLYNULL) {
    if (first) {
	  r = dGrade1(f);
	  first = 0;
	}else{
	  t = dGrade1(f);
	  if (t > r) r = t;
	}
  }
  return r;
}

/* Grading by the weight vector (u,v)
   and degree shift ds.  Note 2003.07.10 
   uvGrade1 is the grade of the in(f).
   ei ( element index ). If it is < 0, then e[n-1]->x will be used,
                         else ei is used.
*/
int uvGrade1(POLY f,int u[],int v[],int ds[],int dssize,int ei)
{
  int r;
  int i,t;
  MONOMIAL tf;
  static int nn,mm,ll,cc,n,m,l,c;
  static struct ring *cr = (struct ring *)NULL;

  if (f ISZERO) return(-1);
  tf = f->m;
  if (tf->ringp != cr) {
    n = tf->ringp->n;
    m = tf->ringp->m;
    l = tf->ringp->l;
    c = tf->ringp->c;
    nn = tf->ringp->nn;
    mm = tf->ringp->mm;
    ll = tf->ringp->ll;
    cc = tf->ringp->cc;
    cr = tf->ringp;
  }

  r = 0;
  for (i=0; i<n; i++) {
    r += (tf->e[i].x)*u[i];
    r += (tf->e[i].D)*v[i];
  }
  /* Degree shift */
  if (ei <0) {
	t = tf->e[n-1].x;
	if ((t <dssize) && (t >= 0)) {
	  r += ds[t];
	}else{
	  /* Warning! ds[t] is assumed to be zero. */
	}
  }else{
	t = ei;
	if ((t <dssize) && (t >= 0)) {
	  r += ds[t];
	}else{
	  /* Warning! ds[t] is assumed to be zero. */
	}
  }
  return(r);
}

/* Grading by the weight vector (u,v)
   and degree shift ds.  Note 2003.07.10 
*/
int uvGrade(POLY f,int u[],int v[],int ds[],int dssize,int ei)
{
  int r, first, t;
  if (f ISZERO) return(LARGE_NEGATIVE_NUMBER);
  first = 1;
  while (f != POLYNULL) {
    if (first) {
	  r = uvGrade1(f,u,v,ds,dssize,ei);
	  first = 0;
	}else{
	  t = uvGrade1(f,u,v,ds,dssize,ei);
	  if (t > r) r = t;
	}
  }
  return r;
}

