#include <stdio.h>
#include "datatype.h"
#include "stackm.h"
#include "extern.h"
#include "extern2.h"
#include "matrix.h"
static void shell(int v[],int n);
static int degreeOfPrincipalPart(POLY f);
static int degreeOfInitW(POLY f,int w[]);


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
  struct object ob;
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
  struct object ob;
  POLY ans;
  POLY h;
  POLY ft;
  struct object ob1,ob2,rob;

  
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

  if (f == ZERO) return(f);
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
  POLY node;
  struct listPoly nod;
  POLY h;
  POLY g;
  int maxd;
  int dd;
  node = &nod; node->next = POLYNULL; h = node;

  if (f ISZERO) return(f);
  maxd = degreeOfInitW(f,w);
  g = pCopy(f); /* shallow copy */
  while (!(f ISZERO)) {
    dd = degreeOfInitW(f,w);
    if (dd > maxd) maxd = dd;
    f = f->next;
  }
  while (!(g ISZERO)) {
    dd = degreeOfInitW(g,w);
    if (dd == maxd) {
      h->next = g;
      h = h->next;
    }
    g = g->next;
  }
  h->next = POLYNULL;
  return(node->next);
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
  
  
