/* $OpenXM: OpenXM/src/kan96xx/Kan/poly4.c,v 1.15 2005/06/16 05:07:23 takayama Exp $ */
#include <stdio.h>
#include <string.h>
#include "datatype.h"
#include "stackm.h"
#include "extern.h"
#include "extern2.h"
#include "matrix.h"
static void shell(int v[],int n);
static int degreeOfPrincipalPart(POLY f);
static int degreeOfInitW(POLY f,int w[]);
static int degreeOfInitWS(POLY f,int w[],int s[]);
static int dDegree(POLY f);
static POLY dHomogenize(POLY f);

static void shell(v,n)
     int v[];
     int n;
{
  int gap,i,j,temp;
  
  for (gap = n/2; gap > 0; gap /= 2) {
    for (i = gap; i<n; i++) {
      for (j=i-gap ; j>=0 && v[j]<v[j+gap]; j -= gap) {
        temp = v[j];
        v[j] = v[j+gap];
        v[j+gap] = temp;
      }
    }
  }
}


struct matrixOfPOLY *parts(f,v)
     POLY f;
     POLY v;  /* v must be a single variable, e.g. x */
{
  struct matrixOfPOLY *evPoly;
  int vi = 0;  /* index of v */
  int vx = 1;  /* x --> 1, D--> 0 */
  int n,evSize,i,k,e;
  int *ev;
  struct object *evList;
  struct object *list;
  struct object ob = OINIT;
  POLY ans;
  POLY h;
  extern struct ring *CurrentRingp;
  POLY ft;

  
  if (f ISZERO || v ISZERO) {
    evPoly = newMatrixOfPOLY(2,1);
    getMatrixOfPOLY(evPoly,0,0) = ZERO;
    getMatrixOfPOLY(evPoly,1,0) = ZERO;
    return(evPoly);
  }
  n = v->m->ringp->n;
  /* get the index of the variable v */
  for (i=0; i<n; i++) {
    if (v->m->e[i].x) {
      vx = 1; vi = i; break;
    }else if (v->m->e[i].D) {
      vx = 0; vi = i; break;
    }
  }
  ft = f;
  /* get the vector of exponents */
  evList = NULLLIST;
  while (ft != POLYNULL) {
    if (vx) {
      e = ft->m->e[vi].x;
    }else{
      e = ft->m->e[vi].D;
    }
    ft = ft->next;
    ob = KpoInteger(e);
    if (!memberQ(evList,ob)) {
      list = newList(&ob);
      evList = vJoin(evList,list);
    }
  }
  /*printf("evList = "); printObjectList(evList);*/
  evSize = klength(evList);
  ev = (int *)sGC_malloc(sizeof(int)*(evSize+1));
  if (ev == (int *)NULL) errorPoly("No more memory.");
  for (i=0; i<evSize; i++) {
    ev[i] = KopInteger(car(evList));
    evList = cdr(evList);
  }
  /* sort ev */
  shell(ev,evSize);

  /* get coefficients */
  evPoly = newMatrixOfPOLY(2,evSize);
  for (i=0; i<evSize; i++) {
    ans = ZERO;
    getMatrixOfPOLY(evPoly,0,i) = cxx(ev[i],0,0,CurrentRingp);
    ft = f;
    while (ft != POLYNULL) {
      if (vx) {
        if (ft->m->e[vi].x == ev[i]) {
          h = newCell(ft->coeffp,monomialCopy(ft->m));
          xset0(h,vi); /* touch monomial part, so you need to copy it above. */
          ans = ppAdd(ans,h);
        }
      }else{
        if (ft->m->e[vi].D == ev[i]) {
          h = newCell(ft->coeffp,monomialCopy(ft->m));
          dset0(h,vi);
          ans = ppAdd(ans,h);
        }
      }
      ft = ft->next;
    }
    getMatrixOfPOLY(evPoly,1,i) = ans;
  }
  return(evPoly);
}
      
struct object parts2(f,v)
     POLY f;
     POLY v;  /* v must be a single variable, e.g. x */
{
  struct matrixOfPOLY *evPoly;
  int vi = 0;  /* index of v */
  int vx = 1;  /* x --> 1, D--> 0 */
  int n,evSize,i,k,e;
  int *ev;
  struct object *evList;
  struct object *list;
  struct object ob = OINIT;
  POLY ans;
  POLY h;
  POLY ft;
  struct object ob1 = OINIT;
  struct object ob2 = OINIT;
  struct object rob = OINIT;

  
  if (f ISZERO || v ISZERO) {
    evPoly = newMatrixOfPOLY(2,1);
    getMatrixOfPOLY(evPoly,0,0) = ZERO;
    getMatrixOfPOLY(evPoly,1,0) = ZERO;
    rob = newObjectArray(2);
    ob1 = newObjectArray(1);
    ob2 = newObjectArray(1);
    putoa(ob1,0,KpoInteger(0));
    putoa(ob2,0,KpoPOLY(POLYNULL));
    putoa(rob,0,ob1); putoa(rob,1,ob2);
    return(rob);
  }
  n = v->m->ringp->n;
  /* get the index of the variable v */
  for (i=0; i<n; i++) {
    if (v->m->e[i].x) {
      vx = 1; vi = i; break;
    }else if (v->m->e[i].D) {
      vx = 0; vi = i; break;
    }
  }
  ft = f;
  /* get the vector of exponents */
  evList = NULLLIST;
  while (ft != POLYNULL) {
    if (vx) {
      e = ft->m->e[vi].x;
    }else{
      e = ft->m->e[vi].D;
    }
    ft = ft->next;
    ob = KpoInteger(e);
    if (!memberQ(evList,ob)) {
      list = newList(&ob);
      evList = vJoin(evList,list);
    }
  }
  /*printf("evList = "); printObjectList(evList);*/
  evSize = klength(evList);
  ev = (int *)sGC_malloc(sizeof(int)*(evSize+1));
  if (ev == (int *)NULL) errorPoly("No more memory.");
  for (i=0; i<evSize; i++) {
    ev[i] = KopInteger(car(evList));
    evList = cdr(evList);
  }
  /* sort ev */
  shell(ev,evSize);

  /* get coefficients */
  evPoly = newMatrixOfPOLY(2,evSize);
  for (i=0; i<evSize; i++) {
    ans = ZERO;
    /* getMatrixOfPOLY(evPoly,0,i) = cxx(ev[i],0,0,CurrentRingp); */
    getMatrixOfPOLY(evPoly,0,i) = POLYNULL;
    ft = f;
    while (ft != POLYNULL) {
      if (vx) {
        if (ft->m->e[vi].x == ev[i]) {
          h = newCell(ft->coeffp,monomialCopy(ft->m));
          xset0(h,vi); /* touch monomial part, so you need to copy it above. */
          ans = ppAdd(ans,h);
        }
      }else{
        if (ft->m->e[vi].D == ev[i]) {
          h = newCell(ft->coeffp,monomialCopy(ft->m));
          dset0(h,vi);
          ans = ppAdd(ans,h);
        }
      }
      ft = ft->next;
    }
    getMatrixOfPOLY(evPoly,1,i) = ans;
  }
  rob = newObjectArray(2);
  ob1 = newObjectArray(evSize);
  ob2 = newObjectArray(evSize);
  for (i=0; i<evSize; i++) {
    putoa(ob2,i,KpoPOLY(getMatrixOfPOLY(evPoly,1,i)));
    putoa(ob1,i,KpoInteger(ev[i]));
  }
  putoa(rob,0,ob1); putoa(rob,1,ob2);
  return(rob);
}
      
int pDegreeWrtV(f,v)
     POLY f;
     POLY v;
{
  int vx = 1;
  int vi = 0;
  int i,n;
  int ans;
  if (f ISZERO || v ISZERO) return(0);
  n = f->m->ringp->n;
  for (i=0; i<n; i++) {
    if (v->m->e[i].x) {
      vx = 1; vi = i;
      break;
    }else if (v->m->e[i].D) {
      vx = 0; vi = i;
      break;
    }
  }
  if (vx) {
    ans = f->m->e[vi].x;
  }else{
    ans = f->m->e[vi].D;
  }
  f = f->next;
  while (f != POLYNULL) {
    if (vx) {
      if (f->m->e[vi].x > ans) ans = f->m->e[vi].x;
    }else{
      if (f->m->e[vi].D > ans) ans = f->m->e[vi].D;
    }
    f = f->next;
  }
  return(ans);
}

int containVectorVariable(POLY f)
{
  MONOMIAL tf;
  static int nn,mm,ll,cc,n,m,l,c;
  static struct ring *cr = (struct ring *)NULL;
  int i;

  if (f ISZERO) return(0);
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

  while (f != POLYNULL) {
    tf = f->m;
    for (i=cc; i<c; i++) {
      if ( tf->e[i].x ) return(1);
      if ( tf->e[i].D ) return(1);
    }
    for (i=ll; i<l; i++) {
      if (tf->e[i].x) return(1);
      if (tf->e[i].D) return(1);
    }
    for (i=mm; i<m; i++) {
      if (tf->e[i].x) return(1);
      if (tf->e[i].D) return(1);
    }
    for (i=nn; i<n; i++) {
      if (tf->e[i].x) return(1);
      if (tf->e[i].D) return(1);
    }
    f = f->next;
  }
  return(0);

}

POLY homogenize(f)
     POLY f;
     /* homogenize by using (*grade)(f) */
{
  POLY t;
  int maxg;
  int flag,d;
  extern int Homogenize;

  if (f == ZERO) return(f);
  if (Homogenize == 3) { /* double homogenization Dx x = x Dx + h H */
    return dHomogenize(f); 
  }
  t = f; maxg = (*grade)(f); flag = 0;
  while (t != POLYNULL) {
    d = (*grade)(t);
    if (d != maxg) flag = 1;
    if (d > maxg) {
      maxg = d;
    }
    t = t->next;
  }
  if (flag == 0) return(f);

  f = pmCopy(f); /* You can rewrite the monomial parts */
  t = f;
  while (t != POLYNULL) {
    d = (*grade)(t);
    if (d != maxg) {
      t->m->e[0].D += maxg-d; /* Multiply h^(maxg-d) */
    }
    t = t->next;
  }
  return(f);
}

int isHomogenized(f)
     POLY f;
{
  POLY t;
  extern int Homogenize_vec;
  int maxg;
  if (!Homogenize_vec) return(isHomogenized_vec(f));
  if (f == ZERO) return(1);
  if (f->m->ringp->weightedHomogenization) {
	return 1; /* BUG: do not chech in case of one-zero homogenization */
  }
  maxg = (*grade)(f);
  t = f;
  while (t != POLYNULL) {
    if (maxg != (*grade)(t)) return(0);
    t = t->next;
  }
  return(1);
}

int isHomogenized_vec(f)
     POLY f;
{
  /* This is not efficient version. *grade should be grade_module1v(). */
  POLY t;
  int ggg;
  if (f == ZERO) return(1);
  if (f->m->ringp->weightedHomogenization) {
	return 1; /* BUG: do not chech in case of one-zero homogenization */
  }
  while (f != POLYNULL) {
    t = f;
    ggg = (*grade)(f);
    while (t != POLYNULL) {
      if ((*isSameComponent)(f,t)) {
        if (ggg != (*grade)(t)) return(0);
      }
      t = t->next;
    }
    f = f->next;
  }
  return(1);
}

static POLY dHomogenize(f)
POLY f;
{
  POLY t;
  int maxg, maxdg;
  int flag,d,dd,neg;

  if (f == ZERO) return(f);

  t = f;
  maxg = (*grade)(f); 
  while (t != POLYNULL) {
	dd = (*grade)(t);
	if (maxg < dd) maxg = dd;
	t = t->next;
  }
  /* fprintf(stderr,"maxg=%d\n",maxg); */

  t = f;
  maxdg = dDegree(f); 
  while (t != POLYNULL) {
	dd = dDegree(t);
	if (maxdg < dd) maxdg = dd;
	t = t->next;
  }
  /* fprintf(stderr,"maxdg=%d\n",maxdg); */

  t = f;
  flag = 0;
  while (t != POLYNULL) {
    d = (*grade)(t);
    if (d != maxg) flag = 1;
    if (d > maxg) {
      maxg = d;
    }
    d = dDegree(f);
    if (d > maxdg) {
      maxdg = d;
    }
    t = t->next;
  }
  if (flag == 0) return(f);

  t = f; neg = 0;
  while (t != POLYNULL) {
    d = (*grade)(t);
    dd = dDegree(t);
    if (maxg-d-(maxdg-dd) < neg) {
      neg = maxg-d-(maxdg-dd);
    }
    t = t->next;
  }
  neg = -neg;

  f = pmCopy(f); /* You can rewrite the monomial parts */
  t = f;
  while (t != POLYNULL) {
    d = (*grade)(t);
    dd = dDegree(t);
    t->m->e[0].D += maxdg-dd; /* h */
    t->m->e[0].x += maxg-d-(maxdg-dd)+neg; /* Multiply H */
    /* example Dx^2+Dx+x */
    t = t->next;
  }
  return(f);
}

static int degreeOfPrincipalPart(f)
     POLY f;
{
  int n,i,dd;
  if (f ISZERO) return(0);
  n = f->m->ringp->n; dd = 0;
  /* D[0] is homogenization var */
  for (i=1; i<n; i++) {
    dd += f->m->e[i].D;
  }
  return(dd);
}

static int dDegree(f)
     POLY f;
{
  int nn,i,dd,m;
  if (f ISZERO) return(0);
  nn = f->m->ringp->nn; dd = 0;
  m = f->m->ringp->m;
  for (i=m; i<nn; i++) {
    dd += f->m->e[i].D;
  }
  return(dd);
}
  
POLY POLYToPrincipalPart(f)
     POLY f;
{
  POLY node;
  struct listPoly nod;
  POLY h;
  POLY g;
  int maxd = -20000; /* very big negative number */
  int dd;
  node = &nod; node->next = POLYNULL; h = node;
  
  g = pCopy(f); /* shallow copy */
  while (!(f ISZERO)) {
    dd = degreeOfPrincipalPart(f);
    if (dd > maxd) maxd = dd;
    f = f->next;
  }
  while (!(g ISZERO)) {
    dd = degreeOfPrincipalPart(g);
    if (dd == maxd) {
      h->next = g;
      h = h->next;
    }
    g = g->next;
  }
  h->next = POLYNULL;
  return(node->next);
}

static int degreeOfInitW(f,w)
     POLY f;
     int w[];
{
  int n,i,dd;
  if (f ISZERO) {
    errorPoly("degreeOfInitW(0,w) ");
  }
  n = f->m->ringp->n; dd = 0;
  for (i=0; i<n; i++) {
    dd += (f->m->e[i].D)*w[n+i];
    dd += (f->m->e[i].x)*w[i];
  }
  return(dd);
}

POLY POLYToInitW(f,w)
     POLY f;
     int w[]; /* weight vector */
{
  POLY h;
  POLY g;
  int maxd;
  int dd;
  h = POLYNULL;

  /*printf("1:%s\n",POLYToString(f,'*',1));*/
  if (f ISZERO) return(f);
  maxd = degreeOfInitW(f,w);
  g = f; 
  while (!(f ISZERO)) {
    dd = degreeOfInitW(f,w);
    if (dd > maxd) maxd = dd;
    f = f->next;
  }
  while (!(g ISZERO)) {
    dd = degreeOfInitW(g,w);
    if (dd == maxd) {
      h = ppAdd(h,newCell(g->coeffp,g->m)); /* it might be slow. */
    }
    g = g->next;
  }
  /*printf("2:%s\n",POLYToString(h,'*',1));*/
  return(h);
}

static int degreeOfInitWS(f,w,s)
     POLY f;
     int w[];
	 int s[];
{
  int n,i,dd;
  if (f ISZERO) {
    errorPoly("degreeOfInitWS(0,w) ");
  }
  if (s == (int *) NULL) return degreeOfInitW(f,w);
  n = f->m->ringp->n; dd = 0;
  for (i=0; i<n-1; i++) {
    dd += (f->m->e[i].D)*w[n+i];
    dd += (f->m->e[i].x)*w[i];
  }
  dd += s[(f->m->e[n-1].x)];
  return(dd);
}

POLY POLYToInitWS(f,w,s)
     POLY f;
     int w[]; /* weight vector */
	 int s[]; /* shift vector */
{
  POLY h;
  POLY g;
  int maxd;
  int dd;
  h = POLYNULL;

  /*printf("1s:%s\n",POLYToString(f,'*',1));*/
  if (f ISZERO) return(f);
  maxd = degreeOfInitWS(f,w,s);
  g = f;
  while (!(f ISZERO)) {
    dd = degreeOfInitWS(f,w,s);
    if (dd > maxd) maxd = dd;
    f = f->next;
  }
  while (!(g ISZERO)) {
    dd = degreeOfInitWS(g,w,s);
    if (dd == maxd) {
      h = ppAdd(h,newCell(g->coeffp,g->m)); /* it might be slow. */
    }
    g = g->next;
  }
  /*printf("2s:%s\n",POLYToString(h,'*',1));*/
  return(h);
}

int ordWsAll(f,w,s)
     POLY f;
     int w[]; /* weight vector */
	 int s[]; /* shift vector */
{
  int maxd;
  int dd;

  if (f ISZERO)  errorPoly("ordWsAll(0,w,s) ");
  maxd = degreeOfInitWS(f,w,s);
  while (!(f ISZERO)) {
    dd = degreeOfInitWS(f,w,s);
    if (dd > maxd) maxd = dd;
    f = f->next;
  }
  return maxd;
}


/*
1.The substitution  "ringp->multiplication = ...." is allowed only in
  KsetUpRing(), so the check in KswitchFunction is not necessary.
2.mmLarger != matrix and AvoidTheSameRing==1, then error
3.If Schreyer = 1, then the system always generates a new ring.
4.The execution of set_order_by_matrix is not allowed when Avoid... == 1.
5.When mmLarger == tower  (in tower.sm1, tower-sugar.sm1), we do
  as follows with our own risk.
[(AvoidTheSameRing)] pushEnv [ [(AvoidTheSameRing) 0] system_variable (mmLarger) (tower) switch_function ] pop popEnv
*/
int isTheSameRing(struct ring *rstack[],int rp, struct ring *newRingp)
{
  struct ring *rrr;
  int i,j,k;
  int a=0;
  for (k=0; k<rp; k++) {
    rrr = rstack[k];
    if (rrr->p != newRingp->p) { a=1; goto bbb ; }
    if (rrr->n != newRingp->n) { a=2; goto bbb ; }
    if (rrr->nn != newRingp->nn) { a=3; goto bbb ; }
    if (rrr->m != newRingp->m) { a=4; goto bbb ; }
    if (rrr->mm != newRingp->mm) { a=5; goto bbb ; }
    if (rrr->l != newRingp->l) { a=6; goto bbb ; }
    if (rrr->ll != newRingp->ll) { a=7; goto bbb ; }
    if (rrr->c != newRingp->c) { a=8; goto bbb ; }
    if (rrr->cc != newRingp->cc) { a=9; goto bbb ; }
    for (i=0; i<rrr->n; i++) {
      if (strcmp(rrr->x[i],newRingp->x[i])!=0) { a=10; goto bbb ; }
      if (strcmp(rrr->D[i],newRingp->D[i])!=0) { a=11; goto bbb ; }
    }
    if (rrr->orderMatrixSize != newRingp->orderMatrixSize) { a=12; goto bbb ; }
    for (i=0; i<rrr->orderMatrixSize; i++) {
      for (j=0; j<2*(rrr->n); j++) {
        if (rrr->order[i*2*(rrr->n)+j] != newRingp->order[i*2*(rrr->n)+j])
          { a=13; goto bbb ; }
      }
    }
    if (rrr->next != newRingp->next) { a=14; goto bbb ; }
    if (rrr->multiplication != newRingp->multiplication) { a=15; goto bbb ; }
    /* if (rrr->schreyer != newRingp->schreyer) { a=16; goto bbb ; }*/
    if (newRingp->schreyer == 1) { a=16; goto bbb; }
    if (rrr->weightedHomogenization != newRingp->weightedHomogenization) { a=16; goto bbb; }
    if (rrr->degreeShiftSize != newRingp->degreeShiftSize) {
      a = 17; goto bbb;
    }
    if (rrr->degreeShiftN != newRingp->degreeShiftN) {
      a = 17; goto bbb;
    }
    for (i=0; i < rrr->degreeShiftSize; i++) {
      for (j=0; j< rrr->degreeShiftN; j++) {
        if (rrr->degreeShift[i*(rrr->degreeShiftN)+j] !=
            newRingp->degreeShift[i*(rrr->degreeShiftN)+j]) {
          a = 17; goto bbb;
        }
      }
    }

    /* The following fields are ignored. 
       void *gbListTower;
       int *outputOrder;
       char *name;
    */
    /* All tests are passed. */
    return(k);
  bbb: ;
    /* for debugging. */
    /* fprintf(stderr," reason=%d, ",a); */
  }
  return(-1);
}

/* s->1 */  
POLY goDeHomogenizeS(POLY f) {
  POLY lRule[1];
  POLY rRule[1];
  struct ring *rp;
  POLY ans;
  /* printf("1:[%s]\n",POLYToString(f,'*',1)); */
  if (f == POLYNULL) return f;
  rp = f->m->ringp;
  if (rp->next == NULL) {
	lRule[0] = cxx(1,0,1,rp);
	rRule[0] = cxx(1,0,0,rp);
	ans=replace(f,lRule,rRule,1);
  }else{
	struct coeff *cp;
	POLY t;
	POLY nc;
    ans = POLYNULL;
	while (f != POLYNULL) {
	  cp = f->coeffp;
	  if (cp->tag == POLY_COEFF) {
		t = goDeHomogenizeS((cp->val).f);
		nc = newCell(polyToCoeff(t,f->m->ringp),monomialCopy(f->m));
		ans = ppAddv(ans,nc);
		f = f->next;
	  }else{
		ans = f; break;
	  }
	}
  }
  /* printf("2:[%s]\n",POLYToString(ans,'*',1)); */
  return ans;
}

POLY goDeHomogenizeS_buggy(POLY f) {
  POLY node;
  POLY lastf;
  struct listPoly nod;
  POLY h;
  POLY tf;
  int gt,first;

  printf("1:[%s]\n",POLYToString(f,'*',1));
  if (f == POLYNULL) return(POLYNULL);
  node = &nod;
  node->next = POLYNULL;
  lastf = POLYNULL;
  first = 1;
  while (f != POLYNULL) {
    tf = newCell(f->coeffp,monomialCopy(f->m));
    tf->m->e[0].x = 0;  /* H, s variable in the G-O paper. */
    if (first) {
      node->next = tf;
      lastf = tf;
      first = 0;
    }else{
      gt = (*mmLarger)(lastf,tf);
      if (gt == 1) {
        lastf->next = tf;
        lastf = tf;
      }else{
        h = node->next;
        h = ppAddv(h,tf);
        node->next = h;
        lastf = h;
        while (lastf->next != POLYNULL) {
          lastf = lastf->next;
        }
      }
    }
    f = f->next;
  }
  printf("2:[%s]\n",POLYToString(node->next,'*',1));
  return (node->next);
}  

/* Granger-Oaku's homogenization for the ecart tangent cone.
   Note: 2003.07.10.
   ds[] is the degree shift.
   ei ( element index ). If it is < 0, then e[n-1]->x will be used,
                         else ei is used.
   if onlyS is set to 1, then input is assumed to be (u,v)-h-homogeneous.
*/
POLY goHomogenize(POLY f,int u[],int v[],int ds[],int dssize,int ei,int onlyS)
{
  POLY node;
  POLY lastf;
  struct listPoly nod;
  POLY h;
  POLY tf;
  int gt,first,m,mp,t,tp,dsIdx,message;
  struct ring *rp;

  message = 1;
  if (f == POLYNULL) return(POLYNULL);
  rp = f->m->ringp;
  /*
  if ((rp->degreeShiftSize == 0) && (dssize > 0)) {
	warningPoly("You are trying to homogenize a polynomial with degree shift. However, the polynomial belongs to the ring without degreeShift option. It may cause a trouble in comparison in free module.\n");
  }
  */
  node = &nod;
  node->next = POLYNULL;
  lastf = POLYNULL;
  first = 1;
  while (f != POLYNULL) {
    if (first) {
      t = m = dGrade1(f);
      tp = mp = uvGrade1(f,u,v,ds,dssize,ei);
    }else{
      t =  dGrade1(f);
      tp = uvGrade1(f,u,v,ds,dssize,ei);
      if (t > m) m = t;
      if (tp < mp) mp = tp;
    }

    tf = newCell(f->coeffp,monomialCopy(f->m));
	/* Automatic dehomogenize. Not +=  */
	if (message && ((tf->m->e[0].D != 0) || (tf->m->e[0].x != 0))) {
      /*go-debug fprintf(stderr,"Automatic dehomogenize and homogenize.\n"); */
	  message = 0;
	}
	if (!onlyS) {
	  tf->m->e[0].D = -t;  /* h */
	}
    tf->m->e[0].x = tp;  /* H, s variable in the G-O paper. */
	/*go-debug printf("t(h)=%d, tp(uv+ds)=%d\n",t,tp); */
    if (first) {
      node->next = tf;
      lastf = tf;
      first = 0;
    }else{
      gt = (*mmLarger)(lastf,tf);
      if (gt == 1) {
        lastf->next = tf;
        lastf = tf;
      }else{
        /*go-debug printf("?\n"); */
        h = node->next;
        h = ppAddv(h,tf);
        node->next = h;
        lastf = h;
        while (lastf->next != POLYNULL) {
          lastf = lastf->next;
        }
      }
	}
	f = f->next;
  }
  h = node->next;
  /*go-debug printf("m=%d, mp=%d\n",m,mp); */
  while (h != POLYNULL) {
    /*go-debug printf("Old: h=%d, s=%d\n",h->m->e[0].D,h->m->e[0].x);  */
    if (!onlyS) h->m->e[0].D += m;   /* h */
    h->m->e[0].x += -mp; /* H, s*/
    /*go-debug printf("New: h=%d, s=%d\n",h->m->e[0].D,h->m->e[0].x); */
    h = h->next;
  }
  return (node->next);
}  

/* u[] = -1, v[] = 1 */
POLY goHomogenize11(POLY f,int ds[],int dssize,int ei,int onlyS)
{
  int r;
  int i,t,n,m,nn;
  MONOMIAL tf;
  static int *u;
  static int *v;
  static struct ring *cr = (struct ring *)NULL;

  if (f == POLYNULL) return POLYNULL;

  tf = f->m;
  if (tf->ringp != cr) {
    n = tf->ringp->n;
    m = tf->ringp->m;
    nn = tf->ringp->nn;
    cr = tf->ringp;
	u = (int *)sGC_malloc(sizeof(int)*n);
	v = (int *)sGC_malloc(sizeof(int)*n);
	for (i=0; i<n; i++) u[i]=v[i]=0;
	for (i=m; i<nn; i++) {
	  u[i] = -1; v[i] = 1;
	}
  }
  return(goHomogenize(f,u,v,ds,dssize,ei,onlyS));
}

POLY goHomogenize_dsIdx(POLY f,int u[],int v[],int dsIdx,int ei,int onlyS)
{
  if (f == POLYNULL) return POLYNULL;
}
POLY goHomogenize11_dsIdx(POLY f,int ds[],int dsIdx,int ei,int onlyS)
{
  if (f == POLYNULL) return POLYNULL;
}

/* cf. KsetUpRing() in kanExport0.c */
struct ring *newRingOverFp(struct ring *rp,int p) {
  struct ring *newRingp;
  char *ringName = NULL;
  char pstr[64];
  sprintf(pstr,"_%d",p);
  ringName = (char *)sGC_malloc(128);
  newRingp = (struct ring *)sGC_malloc(sizeof(struct ring));
  if (newRingp == NULL) errorPoly("No more memory.\n");
  strcpy(ringName,rp->name);
  strcat(ringName,pstr);
  *newRingp = *rp;
  newRingp->p = p;
  newRingp->name = ringName;
  return newRingp;
}

/*  
   P = 3001;
   L = [ ];
   while (P<10000) {
     L=cons(P,L);
     P = pari(nextprime,P+1);
	} 
   print(L);
*/
#define N799  799
static int nextPrime(void) {
  static int pt = 0;
  static int tb[N799] = 
{3001,3011,3019,3023,3037,3041,3049,3061,3067,3079,3083,3089,3109,3119,3121,3137,3163,3167,3169,3181,3187,3191,3203,3209,3217,3221,3229,3251,3253,3257,3259,3271,3299,3301,3307,3313,3319,3323,3329,3331,3343,3347,3359,3361,3371,3373,3389,3391,3407,3413,3433,3449,3457,3461,3463,3467,3469,3491,3499,3511,3517,3527,3529,3533,3539,3541,3547,3557,3559,3571,3581,3583,3593,3607,3613,3617,3623,3631,3637,3643,3659,3671,3673,3677,3691,3697,3701,3709,3719,3727,3733,3739,3761,3767,3769,3779,3793,3797,3803,3821,3823,3833,3847,3851,3853,3863,3877,3881,3889,3907,3911,3917,3919,3923,3929,3931,3943,3947,3967,3989,
  4001,4003,4007,4013,4019,4021,4027,4049,4051,4057,4073,4079,4091,4093,4099,4111,4127,4129,4133,4139,4153,4157,4159,4177,4201,4211,4217,4219,4229,4231,4241,4243,4253,4259,4261,4271,4273,4283,4289,4297,4327,4337,4339,4349,4357,4363,4373,4391,4397,4409,4421,4423,4441,4447,4451,4457,4463,4481,4483,4493,4507,4513,4517,4519,4523,4547,4549,4561,4567,4583,4591,4597,4603,4621,4637,4639,4643,4649,4651,4657,4663,4673,4679,4691,4703,4721,4723,4729,4733,4751,4759,4783,4787,4789,4793,4799,4801,4813,4817,4831,4861,4871,4877,4889,4903,4909,4919,4931,4933,4937,4943,4951,4957,4967,4969,4973,4987,4993,4999,
  5003,5009,5011,5021,5023,5039,5051,5059,5077,5081,5087,5099,5101,5107,5113,5119,5147,5153,5167,5171,5179,5189,5197,5209,5227,5231,5233,5237,5261,5273,5279,5281,5297,5303,5309,5323,5333,5347,5351,5381,5387,5393,5399,5407,5413,5417,5419,5431,5437,5441,5443,5449,5471,5477,5479,5483,5501,5503,5507,5519,5521,5527,5531,5557,5563,5569,5573,5581,5591,5623,5639,5641,5647,5651,5653,5657,5659,5669,5683,5689,5693,5701,5711,5717,5737,5741,5743,5749,5779,5783,5791,5801,5807,5813,5821,5827,5839,5843,5849,5851,5857,5861,5867,5869,5879,5881,5897,5903,5923,5927,5939,5953,5981,5987,
  6007,6011,6029,6037,6043,6047,6053,6067,6073,6079,6089,6091,6101,6113,6121,6131,6133,6143,6151,6163,6173,6197,6199,6203,6211,6217,6221,6229,6247,6257,6263,6269,6271,6277,6287,6299,6301,6311,6317,6323,6329,6337,6343,6353,6359,6361,6367,6373,6379,6389,6397,6421,6427,6449,6451,6469,6473,6481,6491,6521,6529,6547,6551,6553,6563,6569,6571,6577,6581,6599,6607,6619,6637,6653,6659,6661,6673,6679,6689,6691,6701,6703,6709,6719,6733,6737,6761,6763,6779,6781,6791,6793,6803,6823,6827,6829,6833,6841,6857,6863,6869,6871,6883,6899,6907,6911,6917,6947,6949,6959,6961,6967,6971,6977,6983,6991,6997,
  7001,7013,7019,7027,7039,7043,7057,7069,7079,7103,7109,7121,7127,7129,7151,7159,7177,7187,7193,7207,7211,7213,7219,7229,7237,7243,7247,7253,7283,7297,7307,7309,7321,7331,7333,7349,7351,7369,7393,7411,7417,7433,7451,7457,7459,7477,7481,7487,7489,7499,7507,7517,7523,7529,7537,7541,7547,7549,7559,7561,7573,7577,7583,7589,7591,7603,7607,7621,7639,7643,7649,7669,7673,7681,7687,7691,7699,7703,7717,7723,7727,7741,7753,7757,7759,7789,7793,7817,7823,7829,7841,7853,7867,7873,7877,7879,7883,7901,7907,7919,7927,7933,7937,7949,7951,7963,7993,
8009,8011,8017,8039,8053,8059,8069,8081,8087,8089,8093,8101,8111,8117,8123,8147,8161,8167,8171,8179,8191,8209,8219,8221,8231,8233,8237,8243,8263,8269,8273,8287,8291,8293,8297,8311,8317,8329,8353,8363,8369,8377,8387,8389,8419,8423,8429,8431,8443,8447,8461,8467,8501,8513,8521,8527,8537,8539,8543,8563,8573,8581,8597,8599,8609,8623,8627,8629,8641,8647,8663,8669,8677,8681,8689,8693,8699,8707,8713,8719,8731,8737,8741,8747,8753,8761,8779,8783,8803,8807,8819,8821,8831,8837,8839,8849,8861,8863,8867,8887,8893,8923,8929,8933,8941,8951,8963,8969,8971,8999,
  9001,9007,9011,9013,9029,9041,9043,9049,9059,9067,9091,9103,9109,9127,9133,9137,9151,9157,9161,9173,9181,9187,9199,9203,9209,9221,9227,9239,9241,9257,9277,9281,9283,9293,9311,9319,9323,9337,9341,9343,9349,9371,9377,9391,9397,9403,9413,9419,9421,9431,9433,9437,9439,9461,9463,9467,9473,9479,9491,9497,9511,9521,9533,9539,9547,9551,9587,9601,9613,9619,9623,9629,9631,9643,9649,9661,9677,9679,9689,9697,9719,9721,9733,9739,9743,9749,9767,9769,9781,9787,9791,9803,9811,9817,9829,9833,9839,9851,9857,9859,9871,9883,9887,9901,9907,9923,9929,9931,9941,9949,9967,9973};
  
  if (pt <N799) {
	return tb[pt++];
  }else{
	pt = 0;
	return tb[pt++];
  }
}

int getPrime(int p) {
  int i;
  if (p <= 2) return nextPrime();
  for (i=2; i<p; i++) {
	if (p % i == 0) {
	  return nextPrime();
	}
  }
  return p;
}
