/* $OpenXM$ */
#include <stdio.h>
#include "datatype.h"
#include "extern2.h"
#include "gradedset.h"

#define mymax(p,q) (p>q?p:q)

static int DebugReduction = 0;

POLY reduction_sugar(POLY f,struct gradedPolySet *gset,int needSyz,
		     struct syz0 *syzp,int sugarGrade)
{
  int reduced,reduced1,reduced2;
  int grd;
  struct polySet *set;
  POLY cf,syz;
  int i;
  POLY cc,cg;
  int gradelimit;
  int tdegm;

  extern struct ring *CurrentRingp;
  struct ring *rp;
  
  if (needSyz) {
    if (f ISZERO) { rp = CurrentRingp; } else { rp = f->m->ringp; }
    cf = cxx(1,0,0,rp);
    syz = ZERO;
  }

  reduced = 0; /* no */
  /* Take minimum */
  gradelimit = (gset->maxGrade < sugarGrade+1 ?gset->maxGrade: sugarGrade+1);
  do {
    reduced1 = 0; /* no */
    grd = 0;
    while (grd < gset->maxGrade) {
      set = gset->polys[grd];
      do {
	reduced2 = 0; /* no */
	for (i=0; i<set->size; i++) {
	  if (f ISZERO) goto ss;
	  if ((*isReducible)(f,set->g[i])) {
	    tdegm = grade_gen(f) - grade_gen(set->g[i]);
	    /* Reduce if and only if sugarGrade does not increase. */
	    if (tdegm+grd <= sugarGrade) {
	      f = reduction1_sugar(f,set->g[i],needSyz,&cc,&cg,sugarGrade);
	      if (needSyz) {
		cf = ppMult(cc,cf);
		syz = cpMult(toSyzCoeff(cc),syz);
		syz = ppAddv(syz,toSyzPoly(cg,grd,i));
	      }
	      reduced = reduced1 = reduced2 = 1; /* yes */
	    }
	  }
	}
      } while (reduced2 != 0);
      grd++;
    }
  }while (reduced1 != 0);

  ss: ;
  if (needSyz) {
    syzp->cf = cf;   /* cf is in the CurrentRingp */
    syzp->syz = syz; /* syz is in the SyzRingp */
  }
  return(f);
}

POLY reduction1_sugar(f,g,needSyz,c,h,sugarGrade)
POLY f;
POLY g;
int needSyz;
POLY *c; /* set */
POLY *h; /* set */
int sugarGrade;
/* f must be reducible by g.  r = c*f + h*g */
{
  extern struct ring *CurrentRingp;
  struct ring *rp;
  struct spValue sv;
  POLY f2;
  int grd,ggrd,tdegm;

  
  if (needSyz) {
    if (f ISZERO) { rp = CurrentRingp; } else {rp = f->m->ringp; }
    *c = cxx(1,0,0,rp);
    *h = ZERO;
  }

  sv = (*sp)(f,g);
  f2 = ppAddv(cpMult((sv.a)->coeffp,f),ppMult(sv.b,g));
  if (DebugReduction) {
    printf("c=%s, d=%s, g=%s:  f --> c*f + d*g.\n",
	   POLYToString(sv.a,'*',1),POLYToString(sv.b,'*',1),POLYToString(g,'*',1));	   
    printf("%s --> %s\n",POLYToString(f,'*',1),POLYToString(f2,'*',1));
  }
  f = f2;
  if (needSyz) {
    *c = ppMult(sv.a,*c);
    *h = ppAdd(ppMult(sv.a,*h),sv.b);
  }

  grd = grade_sugar(g); ggrd = grade_gen(g);
  while ((*isReducible)(f,g)) {
    tdegm = grade_gen(f) - ggrd;
    /* Reduce if and only if sugarGrade does not increase. */
    if (tdegm+grd <= sugarGrade) {
      sv = (*sp)(f,g);
      f2 = ppAddv(cpMult((sv.a)->coeffp,f),ppMult(sv.b,g));
      if (DebugReduction) {
	printf("! c=%s, d=%s, g=%s:  f --> c*f + d*g.\n",
	       POLYToString(sv.a,'*',1),POLYToString(sv.b,'*',1),POLYToString(g,'*',1));	   
	printf("%s --> %s\n",POLYToString(f,'*',1),POLYToString(f2,'*',1));
      }
      f = f2;
      if (needSyz) {
	*c = ppMult(sv.a,*c);
	*h = ppAdd(ppMult(sv.a,*h),sv.b);
      }
    }else{
      break;
    }
  }
  return(f);
}

int grade_sugar(f)
POLY f;
{
  int r;
  int i,ans;
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

  ans = 0;
  while (f != NULL) {
    r = 0;
    tf = f->m;
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
    f = f->next;
    ans = (ans>r?ans:r);
  }
  return(ans);
}
