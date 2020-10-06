/* $OpenXM: OpenXM/src/kan96xx/Kan/sugar.c,v 1.7 2003/09/20 09:57:29 takayama Exp $ */
#include <stdio.h>
#include "datatype.h"
#include "extern2.h"
#include "gradedset.h"

#define mymax(p,q) (p>q?p:q)

/* static int DebugReduction = 0; 
*/
extern int DebugReductionRed;
extern int DebugContentReduction;
extern int Ecart;
extern int Ecart_sugarGrade;
static int ecart_grade_sugar(POLY f);

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
  extern int DoCancel;

  if (Ecart) {
	Ecart_sugarGrade = sugarGrade;
	return reduction_ecart(f,gset,needSyz,syzp);
  }
  if (needSyz) {
    if (f ISZERO) { rp = CurrentRingp; } else { rp = f->m->ringp; }
    cf = cxx(1,0,0,rp);
    syz = ZERO;
  }

  if (needSyz && DoCancel) {
	warningGradedSet("needSyz is not supported when DoCancel is turned on. DoCancel is set to 0.\n");
	DoCancel = 0;
  }
  if (DoCancel && (f != POLYNULL)) shouldReduceContent(f,1);

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

              if (DoCancel && (f != POLYNULL)) {
                if (shouldReduceContent(f,0)) {
                  struct coeff *cont;
                  f = reduceContentOfPoly(f,&cont);
                  shouldReduceContent(f,1);
                  if (DebugContentReduction) printf("CONT=%s ",coeffToString(cont));
                }
              }

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

  if (DoCancel && (f != POLYNULL)) {
    if (f->m->ringp->p == 0) {
	  struct coeff *cont;
	  f = reduceContentOfPoly(f,&cont);
	  if (DebugContentReduction) printf("cont=%s ",coeffToString(cont));     
    }
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
  if (DebugReductionRed) {
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
      if (DebugReductionRed) {
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

  /* if (Ecart) return ecart_grade_sugar(f); */
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

static int ecart_grade_sugar(POLY f)  /* experimental */
{
  int r;
  int i,ans;
  MONOMIAL tf;
  static int nn,mm,ll,cc,n,m,l,c;
  static struct ring *cr = (struct ring *)NULL;
  int soffset;
  int sdegree;

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

  soffset = 0x7fffffff; /* large number */
  while (f != NULL) {
    r = 0;
    tf = f->m;
    for (i=m; i<nn; i++) {
      r -= tf->e[i].x;
      r += tf->e[i].D;
    }
    f = f->next;
    soffset = (r < soffset? r: soffset);
  }

  ans = 0;
  while (f != NULL) {
    r = 0;  sdegree = 0;
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
      sdegree -= tf->e[i].x;
      sdegree += tf->e[i].D;

    }
	r += sdegree-soffset;
    f = f->next;
    ans = (ans>r?ans:r);
  }
  return(ans);
}
