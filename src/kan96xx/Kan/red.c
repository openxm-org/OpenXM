/* $OpenXM: OpenXM/src/kan96xx/Kan/red.c,v 1.10 2005/06/09 04:09:22 takayama Exp $ */
#include <stdio.h>
#include "datatype.h"
#include "extern2.h"
#include "gradedset.h"

#define mymax(p,q) (p>q?p:q)

int DebugReductionRed = 0;
int DebugContentReduction = 0;
extern int Sugar;

struct spValue spZero(void) {
  struct spValue r;
  r.a = ZERO;
  r.b = ZERO;
  return r;
}
struct spValue sp_gen(f,g)
     POLY f;
     POLY g;
     /* the results may be rewritten. */
{
  struct spValue r;
  POLY a;
  POLY b;
  MONOMIAL tf,tg;
  MONOMIAL ta,tb;
  int n,i;
  struct coeff *ac;
  struct coeff *bc;
  int amodify,bmodify,c,ell;
  
  if ((f ISZERO) || (g ISZERO)) {
    warningGradedSet("sp_gen(): f and g must not be zero. Returns zero.");
    r.a = ZERO;
    r.b = ZERO;
    return(r);
  }
  
  checkRingSp(f,g,r);
  tf = f->m; tg = g->m;
  n = tf->ringp->n;
  ta = newMonomial(tf->ringp);
  tb = newMonomial(tf->ringp);
  for (i=0; i<n; i++) {
    ta->e[i].x = mymax(tf->e[i].x,tg->e[i].x) - tf->e[i].x;
    ta->e[i].D = mymax(tf->e[i].D,tg->e[i].D) - tf->e[i].D;
    tb->e[i].x = mymax(tf->e[i].x,tg->e[i].x) - tg->e[i].x;
    tb->e[i].D = mymax(tf->e[i].D,tg->e[i].D) - tg->e[i].D;
  }
  
  switch (f->coeffp->tag) {
  case INTEGER:
    a = newCell(coeffCopy(g->coeffp),ta);
    b = newCell(coeffCopy(f->coeffp),tb);
    b->coeffp->val.i = -(b->coeffp->val.i);
    break;
  case MP_INTEGER:
    {
      MP_INT *gcd,*ac;
      gcd = newMP_INT();
      mpz_gcd(gcd,f->coeffp->val.bigp,g->coeffp->val.bigp);
      ac = newMP_INT();
      mpz_mdiv(ac,g->coeffp->val.bigp,gcd);
      mpz_mdiv(gcd,f->coeffp->val.bigp,gcd);
      mpz_neg(gcd,gcd);
      
      a = newCell(mpintToCoeff(ac,tf->ringp),ta);
      b = newCell(mpintToCoeff(gcd,tf->ringp),tb);
    }
    break;
  case POLY_COEFF:
    c = f->m->ringp->c; ell = f->m->ringp->l;
    if (ell-c > 0) { /* q-case */
      amodify = 0;
      for (i=c; i<ell; i++) {
        amodify += (tb->e[i].D)*(tg->e[i].x);
      }
      bmodify = 0;
      for (i=c; i<ell; i++) {
        bmodify += (ta->e[i].D)*(tf->e[i].x);
      }
      if (amodify > bmodify) {
        amodify = amodify-bmodify;
        bmodify = 0;
      }else{
        bmodify = bmodify - amodify;
        amodify = 0;
      }
    }
    if (amodify) {
      /* a ---> q^amodify a,  b ---> - b */
      ac = polyToCoeff(cxx(1,0,amodify,f->m->ringp->next),f->m->ringp);
      Cmult(ac,ac,g->coeffp);
      a = newCell(ac,ta);
      bc = coeffNeg(f->coeffp,f->m->ringp);
      b = newCell(bc,tb);
    }else{
      /* a ---> a,  b ---> -q^bmodify b */
      a = newCell(coeffCopy(g->coeffp),ta);
      bc = coeffNeg(f->coeffp,f->m->ringp);
      Cmult(bc,polyToCoeff(cxx(1,0,bmodify,f->m->ringp->next),f->m->ringp),bc);
      b = newCell(bc,tb);
    }
    break;
  default:
    errorGradedSet("sp_gen(): Unsupported tag.");
    break;
  }
  /* r.sp = ppAddv(ppMult(a,f),ppMult(b,g)); */
  r.a = a;
  r.b = b;
  return(r);
}
    

POLY reduction1_gen_debug(f,g,needSyz,c,h)
     POLY f;
     POLY g;
     int needSyz;
     POLY *c; /* set */
     POLY *h; /* set */
     /* f must be reducible by g.  r = c*f + h*g */
{
  extern struct ring *CurrentRingp;
  struct ring *rp;
  struct spValue sv;
  POLY f2;
  int showLength = 0;

  /*DebugReductionRed = 1;*/
  if (needSyz) {
    if (f ISZERO) { rp = CurrentRingp; } else {rp = f->m->ringp; }
    *c = cxx(1,0,0,rp);
    *h = ZERO;
  }

  sv = (*sp)(f,g);
  f2 = ppAddv(cpMult((sv.a)->coeffp,f),ppMult(sv.b,g));
  if (showLength) {printf(" [%d] ",pLength(f)); fflush(stdout);}
  if (!isOrdered(f2) || !isOrdered(f)) {
    if (!isOrdered(f)) {
      printf("\nf is not ordered polynomial.");
    }else if (!isOrdered(f)) {
      printf("\nf2 is not ordered polynomial.");
    }
    printf("f=%s,\nf2=%s\n",POLYToString(f,'*',1),POLYToString(f2,'*',1)); 
    getchar();
    getchar();
  }
  
  if ((*mmLarger)(f2,f) >= 1) {
    printf("error in reduction1.");
    printf("f=%s --> f2=%s\n",POLYToString(f,'*',1),POLYToString(f2,'*',1));
    printf("f2 = (%s)*f+(%s)*(%s)\n",POLYToString(sv.a,'*',1),
           POLYToString(sv.b,'*',1),POLYToString(g,'*',1));
    getchar();
    getchar();
  }
  if (DebugReductionRed & 1) {
    printf("%s --> %s\n",POLYToString(f,'*',1),POLYToString(f2,'*',1));
  }else if (DebugReductionRed & 2) {
    printf("(head) %s --> %s\n",POLYToString(head(f),'*',1),POLYToString(head(f2),'*',1));
  }

  f = f2;
  if (needSyz) {
    *c = ppMult(sv.a,*c);
    *h = ppAdd(ppMult(sv.a,*h),sv.b);
  }

  while ((*isReducible)(f,g)) {
    sv = (*sp)(f,g);
    f2 = ppAddv(cpMult((sv.a)->coeffp,f),ppMult(sv.b,g));
    if (DebugReductionRed & 1) {
      printf("%s --> %s\n",POLYToString(f,'*',1),POLYToString(f2,'*',1));
	}else if (DebugReductionRed & 2) {
      printf("(head) %s --> %s\n",POLYToString(head(f),'*',1),POLYToString(head(f2),'*',1));
    }
    if (showLength) {printf(" [%d] ",pLength(f)); fflush(stdout);}
    if (!isOrdered(f2) || !isOrdered(f)) {
      if (!isOrdered(f)) {
        printf("\nf is not ordered polynomial.");
      }else if (!isOrdered(f)) {
        printf("\nf2 is not ordered polynomial.");
      }
      printf("f=%s,\nf2=%s\n",POLYToString(f,'*',1),POLYToString(f2,'*',1));
      getchar();
      getchar();
    }

    if ((*mmLarger)(f2,f) >= 1) {
      printf("error in reduction1.");
      printf("f=%s --> f2=%s\n",POLYToString(f,'*',1),POLYToString(f2,'*',1));
      printf("f2 = (%s)*f+(%s)*(%s)\n",POLYToString(sv.a,'*',1),
             POLYToString(sv.b,'*',1),
             POLYToString(g,'*',1));
      getchar();
      getchar();
    }
    f = f2;
    if (needSyz) {
      *c = ppMult(sv.a,*c);
      *h = ppAdd(ppMult(sv.a,*h),sv.b);
    }
  }
  if (DebugReductionRed & 2) printf("-----------  end of reduction_gen_debug\n");
  return(f);
}

POLY reduction1_gen(f,g,needSyz,c,h)
     POLY f;
     POLY g;
     int needSyz;
     POLY *c; /* set */
     POLY *h; /* set */
     /* f must be reducible by g.  r = c*f + h*g */
{
  extern struct ring *CurrentRingp;
  struct ring *rp;
  struct spValue sv;
  POLY f2;
  extern int DoCancel;
  static int crcount=0;
  
  if (needSyz) {
    if (f ISZERO) { rp = CurrentRingp; } else {rp = f->m->ringp; }
    *c = cxx(1,0,0,rp);
    *h = ZERO;
  }
  if ((DoCancel&4) && (f != POLYNULL)) shouldReduceContent(f,1);

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

  

  while ((*isReducible)(f,g)) {
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

	if ((DoCancel&4) && (f != POLYNULL)) {
	  if (shouldReduceContent(f,0)) {
		struct coeff *cont;
		f = reduceContentOfPoly(f,&cont);
		shouldReduceContent(f,1);
		if (DebugContentReduction) {
		  printf("CoNT=%s ",coeffToString(cont));
		  if (crcount % 10 == 0) fflush(NULL);
		  crcount++;
		}
	  }
	}

  }
  return(f);
}

POLY reduction1Cdr_gen(f,fs,g,needSyz,c,h)
     POLY f;
     POLY fs;
     POLY g;
     int needSyz;
     POLY *c; /* set */
     POLY *h; /* set */
     /* f must be reducible by g.  r = c*f + h*g */
{
  extern struct ring *CurrentRingp;
  struct ring *rp;
  struct spValue sv;
  POLY f2;

  
  if (needSyz) {
    if (f ISZERO) { rp = CurrentRingp; } else {rp = f->m->ringp; }
    *c = cxx(1,0,0,rp);
    *h = ZERO;
  }

  sv = (*sp)(fs,g);
  f2 = ppAddv(cpMult((sv.a)->coeffp,f),ppMult(sv.b,g));
  if (DebugReductionRed) {
    printf("%s --> %s\n",POLYToString(f,'*',1),POLYToString(f2,'*',1));
  }
  f = f2;
  if (needSyz) {
    *c = ppMult(sv.a,*c);
    *h = ppAdd(ppMult(sv.a,*h),sv.b);
  }


  while ((fs = (*isCdrReducible)(f,g)) != ZERO) {
    sv = (*sp)(fs,g);
    f2 = ppAddv(cpMult((sv.a)->coeffp,f),ppMult(sv.b,g));
    if (DebugReductionRed) {
      printf("%s --> %s\n",POLYToString(f,'*',1),POLYToString(f2,'*',1));
    }
    f = f2;
    if (needSyz) {
      *c = ppMult(sv.a,*c);
      *h = ppAdd(ppMult(sv.a,*h),sv.b);
    }
  }
  return(f);
}


/* for debug */
int isOrdered(f)
     POLY f;
{ POLY g;
 if (f ISZERO) return(1);
 g = f->next;
 while (g != POLYNULL) {
   if ((*mmLarger)(g,f)>=1) return(0);
   f = g;
   g = f->next;
 }
 return(1);
}


POLY reduction_gen(f,gset,needSyz,syzp)
     POLY f;
     struct gradedPolySet *gset;
     int needSyz;
     struct syz0 *syzp; /* set */
{
  int reduced,reduced1,reduced2;
  int grd;
  struct polySet *set;
  POLY cf,syz;
  int i;
  POLY cc,cg;

  extern struct ring *CurrentRingp;
  struct ring *rp;
  extern int DoCancel;
  
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
  do {
    reduced1 = 0; /* no */
    grd = 0;
    while (grd < gset->maxGrade) {
      if (!Sugar) {
        if (grd > (*grade)(f)) break;
      }
      set = gset->polys[grd];
      do {
        reduced2 = 0; /* no */
        for (i=0; i<set->size; i++) {
          if (f ISZERO) goto ss;
          if ((*isReducible)(f,set->g[i])) {
            f = (*reduction1)(f,set->g[i],needSyz,&cc,&cg);

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

POLY reduction_gen_rev(f,gset,needSyz,syzp)
     POLY f;
     struct gradedPolySet *gset;
     int needSyz;
     struct syz0 *syzp; /* set */
{
  int reduced,reduced1,reduced2;
  int grd;
  struct polySet *set;
  POLY cf,syz;
  int i;
  POLY cc,cg;

  extern struct ring *CurrentRingp;
  struct ring *rp;
  
  if (needSyz) {
    if (f ISZERO) { rp = CurrentRingp; } else { rp = f->m->ringp; }
    cf = cxx(1,0,0,rp);
    syz = ZERO;
  }

  reduced = 0; /* no */
  do {
    reduced1 = 0; /* no */
    grd = ((*grade)(f) < gset->maxGrade-1?(*grade)(f): gset->maxGrade-1);
    while (grd >= 0) {  /* reverse order for reduction */
      set = gset->polys[grd];
      do {
        reduced2 = 0; /* no */
        for (i=0; i<set->size; i++) {
          if (f ISZERO) goto ss;
          if ((*isReducible)(f,set->g[i])) {
            f = (*reduction1)(f,set->g[i],needSyz,&cc,&cg);
            if (needSyz) {
              cf = ppMult(cc,cf);
              syz = cpMult(toSyzCoeff(cc),syz);
              syz = ppAddv(syz,toSyzPoly(cg,grd,i));
            }
            reduced = reduced1 = reduced2 = 1; /* yes */
          }
        }
      } while (reduced2 != 0);
      grd--;
    }
  }while (reduced1 != 0);

 ss: ;
  if (needSyz) {
    syzp->cf = cf;   /* cf is in the CurrentRingp */
    syzp->syz = syz; /* syz is in the SyzRingp */
  }
  return(f);
}

POLY reductionCdr_gen(f,gset,needSyz,syzp)
     POLY f;
     struct gradedPolySet *gset;
     int needSyz;
     struct syz0 *syzp; /* set */
{
  int reduced,reduced1,reduced2;
  int grd;
  struct polySet *set;
  POLY cf,syz;
  int i;
  POLY cc,cg;
  POLY fs;

  extern struct ring *CurrentRingp;
  struct ring *rp;
  
  if (needSyz) {
    if (f ISZERO) { rp = CurrentRingp; } else {rp = f->m->ringp; }
    cf = cxx(1,0,0,rp);
    syz = ZERO;
  }

  reduced = 0; /* no */
  do {
    reduced1 = 0; /* no */
    grd = 0;
    while (grd < gset->maxGrade) {
      if (!Sugar) {
        if (grd > (*grade)(f)) break;
      }
      set = gset->polys[grd];
      do {
        reduced2 = 0; /* no */
        for (i=0; i<set->size; i++) {
          if (f ISZERO) goto ss;
          if ((fs =(*isCdrReducible)(f,set->g[i])) != ZERO) {
            f = (*reduction1Cdr)(f,fs,set->g[i],needSyz,&cc,&cg);
            if (needSyz) {
              cf = ppMult(cc,cf);
              syz = cpMult(toSyzCoeff(cc),syz);
              syz = ppAddv(syz,toSyzPoly(cg,grd,i));
            }
            reduced = reduced1 = reduced2 = 1; /* yes */
          }
        }
      } while (reduced2 != 0);
      grd++;
    }
  }while (reduced1 != 0);

 ss: ;
  if (needSyz) {
    syzp->cf = cf;
    syzp->syz = syz;
  }
  return(f);
}

int isReducible_gen(f,g)
     POLY f;
     POLY g;
{
  int n,i;
  MONOMIAL tf;
  MONOMIAL tg;
  
  if (f ISZERO) return(0);
  if (g ISZERO) return(0);

  checkRingIsR(f,g);

  tf = f->m; tg = g->m;
  n = tf->ringp->n;
  for (i=0; i<n; i++) {
    if (tf->e[i].x < tg->e[i].x) return(0);
    if (tf->e[i].D < tg->e[i].D) return(0);
  }
  return(1);
}

POLY isCdrReducible_gen(f,g)
     POLY f;
     POLY g;
{
  while (f != POLYNULL) {
    if ((*isReducible)(f,g)) {
      return(f);
    }
    f = f->next;
  }
  return(ZERO);
}

POLY lcm_gen(f,g)
     POLY f;
     POLY g;
{
  MONOMIAL tf,tg;
  MONOMIAL lcm;
  int n;
  int i;
  
  tf = f->m; tg = g->m;
  n = tf->ringp->n;
  lcm = newMonomial(tf->ringp);
  for (i=0; i<n; i++) {
    lcm->e[i].x = mymax(tf->e[i].x,tg->e[i].x);
    lcm->e[i].D = mymax(tf->e[i].D,tg->e[i].D);
  }
  return(newCell(intToCoeff(1,tf->ringp),lcm));
}

int grade_gen(f)
     POLY f;
{
  int r;
  int i,n;
  MONOMIAL tf;
  if (f ISZERO) return(-1);
  tf = f->m;
  n = tf->ringp->n;
  r = 0;
  for (i=0; i<n; i++) {
    r += tf->e[i].x;
    r += tf->e[i].D;
  }
  return(r);
}

/* constructors */
POLY toSyzPoly(cg,grd,index)
     POLY cg;
     int grd;
     int index;
     /* the result is read only. */
{
  extern struct ring *SyzRingp;
  POLY r;
  r = newCell(toSyzCoeff(cg),newMonomial(SyzRingp));
  r->m->e[0].x = grd;
  r->m->e[0].D = index;
  return(r);
}

struct coeff *toSyzCoeff(f)
     POLY f;
{
  extern struct ring *SyzRingp;
  struct coeff *c;
  c = newCoeff();
  c->tag = POLY_COEFF;
  c->val.f = f;
  c->p = SyzRingp->p;
  return(c);
}

void initSyzRingp() {
  extern struct ring *SyzRingp;
  extern struct ring *CurrentRingp;
  static char *x[]={"grade"};
  static char *d[]={"index"};
  static int order[]={1,0,
                      0,1};
  static int outputOrderForSyzRing[] = {0,1};
  static int ringSerial = 0;
  char *ringName = NULL;
  SyzRingp = (struct ring *)sGC_malloc(sizeof(struct ring));
  if (SyzRingp == (struct ring *)NULL)
    errorGradedSet("initSyzRingp(); No more memory");
  SyzRingp->p = CurrentRingp->p;
  SyzRingp->n = 1; SyzRingp->m = 1; SyzRingp->l = 1; SyzRingp->c = 1;
  SyzRingp->nn = 1; SyzRingp->mm = 1; SyzRingp->ll = 1;
  SyzRingp->cc = 1;
  SyzRingp->x = x;
  SyzRingp->D = d;
  SyzRingp->order = order;
  SyzRingp->orderMatrixSize = 2;
  setFromTo(SyzRingp);
  SyzRingp->next = CurrentRingp;
  SyzRingp->multiplication = mpMult_poly;  /* Multiplication is not used.*/
  SyzRingp->schreyer = 0;
  SyzRingp->gbListTower = NULL;
  SyzRingp->outputOrder = outputOrderForSyzRing;
  ringName = (char *)sGC_malloc(20);
  if (ringName == NULL) errorGradedSet("No more memory.");
  sprintf(ringName,"syzring%05d",ringSerial);
  SyzRingp->name = ringName;
  SyzRingp->partialEcart = 0;
}

POLY reductionCdr_except_grd_i(POLY f,struct gradedPolySet *gset,
                               int needSyz,struct syz0 *syzp,
                               int skipGrd,int skipi, int *reducedp)
{
  int reduced,reduced1,reduced2;
  int grd;
  struct polySet *set;
  POLY cf,syz;
  int i;
  POLY cc,cg;
  POLY fs;

  extern struct ring *CurrentRingp;
  struct ring *rp;

  *reducedp = 0;
  if (needSyz) {
    if (f ISZERO) { rp = CurrentRingp; } else {rp = f->m->ringp; }
    cf = cxx(1,0,0,rp);
    syz = ZERO;
  }

  reduced = 0; /* no */
  do {
    reduced1 = 0; /* no */
    grd = 0;
    while (grd < gset->maxGrade) {
      /*
        if (!Sugar) {
        if (grd > (*grade)(f)) break;
        }
      */
      set = gset->polys[grd];
      do {
        reduced2 = 0; /* no */
        for (i=0; i<set->size; i++) {
          if (f ISZERO) goto ss;
          if ((!((grd == skipGrd) && (i == skipi))) && (set->del[i]==0)) {
            /*  Do not use deleted element.*/
            if ((fs =(*isCdrReducible)(f,set->g[i])) != ZERO) { 
              f = (*reduction1Cdr)(f,fs,set->g[i],needSyz,&cc,&cg);
              /* What is cg? */
              if (needSyz) {
                cf = ppMult(cc,cf);
                syz = cpMult(toSyzCoeff(cc),syz);
                syz = ppAddv(syz,toSyzPoly(cg,grd,i));
              }
              *reducedp = reduced = reduced1 = reduced2 = 1; /* yes */
            }
          }
        }
      } while (reduced2 != 0);
      grd++;
    }
  }while (reduced1 != 0);

 ss: ;
  if (needSyz) {
    syzp->cf = cf;
    syzp->syz = syz;
  }
  return(f);
}
