/* $OpenXM: OpenXM/src/kan96xx/Kan/ecart.c,v 1.16 2003/08/27 03:11:12 takayama Exp $ */
#include <stdio.h>
#include "datatype.h"
#include "extern2.h"
#include "gradedset.h"

#define outofmemory(a) if (a == NULL) {fprintf(stderr,"ecart.c: out of memory.\n"); exit(10);}
/* Local structures */
struct ecartReducer {
  int ell; /* s^\ell */
  int first; /* first =1 ==> gset else gg */
  int grade;  int gseti;  /* if first==1,  gradedPolySet */
  int ggi;                /* if first==0   ecartPoly [] */
};
struct ecartPolyArray {
  int size;
  int limit;
  POLY *pa;
  POLY *cf;
  POLY *syz;
};

static struct ecartReducer ecartFindReducer(POLY r,struct gradedPolySet *gset,struct ecartPolyArray *epa);
static struct ecartReducer ecartFindReducer_mod(POLY r,struct gradedPolySet *gset,struct ecartPolyArray *epa);
static int ecartCheckPoly(POLY f);  /* check if it does not contain s-variables */
static int ecartCheckEnv();         /* check if the environment is OK for ecart div*/
static struct ecartPolyArray *ecartPutPolyInG(POLY g,struct ecartPolyArray *eparray,POLY cf, POLY syz);
static int ecartGetEll(POLY r,POLY g);
static POLY ecartDivideSv(POLY r,int *d);
/* No automatic homogenization and s is used as a standart var. */
static POLY reduction_ecart0(POLY r,struct gradedPolySet *gset,
                             int needSyz, struct syz0 *syzp);
/* Automatic homogenization and s->1 */
static POLY reduction_ecart1(POLY r,struct gradedPolySet *gset,
                             int needSyz, struct syz0 *syzp);
static POLY reduction_ecart1_mod(POLY r,struct gradedPolySet *gset);
static POLY  ecartCheckSyz0(POLY cf,POLY r_0,POLY syz,
                            struct gradedPolySet *gg,POLY r);
static void  ecartCheckSyz0_printinfo(POLY cf,POLY r_0,POLY syz,
                            struct gradedPolySet *gg,POLY r);

extern int DebugReductionRed;
extern int TraceLift;
struct ring *TraceLift_ringmod;
extern DoCancel;
int DebugReductionEcart = 0;
extern DebugContentReduction;

/* This is used for goHomogenization */
extern int DegreeShifto_size;
extern int *DegreeShifto_vec;

/* It is used reduction_ecart() and ecartFindReducer()
   to determine if we homogenize in this function */
extern int EcartAutomaticHomogenization;

#define LARGE 0x7fffffff


static POLY ecartDivideSv(POLY r,int *d) {
  POLY f;
  int k;
  *d = 0;
  if (r == POLYNULL) return r;
  f = r; k = LARGE;
  while (r != POLYNULL) {
    if (r->m->e[0].x < k) {
      k = r->m->e[0].x;
    }
    r = r->next;
  }

  if (k > 0) {
    *d = k;
	return ppMult(cxx(1,0,-k,f->m->ringp),f);
  }else{
	return f;
  }

  /* Do not do the below. It caused a bug. cf. misc-2003/07/ecart/b4.sm1 test2.
     Note. 2003.8.26
   */
  /*
  if (k > 0) {
    *d = k;
    f = r;
    while (r != POLYNULL) {
      r->m->e[0].x -= k;
      r = r->next;
    }
  }
  return f;
  */
}

static int ecartGetEll(POLY f,POLY g) {
  int n,i,p;
  MONOMIAL tf;
  MONOMIAL tg;
  
  if (f ISZERO) return(-1);
  if (g ISZERO) return(-1);

  checkRingIsR(f,g);

  if (!isSameComponent_x(f,g)) return(-1);
  tf = f->m; tg = g->m; n = tf->ringp->n;
  for (i=1; i<n; i++) {
    if (tf->e[i].x < tg->e[i].x) return(-1);
    if (tf->e[i].D < tg->e[i].D) return(-1);
  }
  if (tf->e[0].D < tg->e[0].D) return(-1);  /*  h  */
  p = tf->e[0].x - tg->e[0].x;  /* H,  s */
  if (p >=0 ) return 0;
  else return(-p);
}


#define EP_SIZE 10
static struct ecartPolyArray *ecartPutPolyInG(POLY g,struct ecartPolyArray *eparray,POLY cf,POLY syz)
{
  struct ecartPolyArray *a;
  POLY *ep;
  int i;
  if (eparray == (struct ecartPolyArray *)NULL) {
    a = (struct ecartPolyArray *) sGC_malloc(sizeof(struct ecartPolyArray));
    outofmemory(a);
    ep = (POLY *) sGC_malloc(sizeof(POLY)*EP_SIZE);
    outofmemory(ep);
    a->cf = (POLY *) sGC_malloc(sizeof(POLY)*EP_SIZE);
    outofmemory(a->cf);
    a->syz = (POLY *) sGC_malloc(sizeof(POLY)*EP_SIZE);
    outofmemory(a->syz);
    a->limit = EP_SIZE;
    a->size = 0;
    a->pa = ep;
    eparray = a;
  }
  if (eparray->size >= eparray->limit) {
    a = (struct ecartPolyArray *) sGC_malloc(sizeof(struct ecartPolyArray));
    outofmemory(a);
    ep = (POLY *) sGC_malloc(sizeof(POLY)*((eparray->limit)*2));
    outofmemory(ep);
    a->cf = (POLY *) sGC_malloc(sizeof(POLY)*((eparray->limit)*2));
    outofmemory(a->cf);
    a->syz = (POLY *) sGC_malloc(sizeof(POLY)*((eparray->limit)*2));
    outofmemory(a->syz);
    a->limit = (eparray->limit)*2;
    a->size = eparray->size;
    a->pa = ep;
    for (i=0; i<eparray->size; i++) {
      (a->pa)[i] = (eparray->pa)[i];
      (a->cf)[i] = (eparray->cf)[i];
      (a->syz)[i] = (eparray->syz)[i];
    }
    eparray  = a;
  }
  i = eparray->size;
  (eparray->pa)[i] = g;
  (eparray->cf)[i] = cf;
  (eparray->syz)[i] = syz;
  (eparray->size)++;
  return eparray;
}

static struct ecartReducer ecartFindReducer(POLY r,struct gradedPolySet *gset,
                                            struct ecartPolyArray *epa)
{
  int grd;
  struct polySet *set;
  int minGrade = 0;
  int minGseti = 0;
  int minGgi   = 0;
  int ell1 = LARGE;
  int ell2 = LARGE;
  int ell;
  int i;
  struct ecartReducer er;
  /* Try to find a reducer in gset; */
  grd = 0;
  while (grd < gset->maxGrade) {
    set = gset->polys[grd];
    for (i=0; i<set->size; i++) {
      if (set->gh[i] == POLYNULL) {
        /* goHomogenize set->gh[i] */
          if (EcartAutomaticHomogenization) {
              set->gh[i] = goHomogenize11(set->g[i],DegreeShifto_vec,DegreeShifto_size,-1,1);
          }else{
              set->gh[i] = set->g[i];
          }
      }
      ell = ecartGetEll(r,set->gh[i]);
      if ((ell>=0) && (ell < ell1)) {
        ell1 = ell;
        minGrade = grd; minGseti=i;
      }
    }
    grd++;
  }
  if (epa != NULL) {
    /* Try to find in the second group. */
    for (i=0; i< epa->size; i++) {
      ell = ecartGetEll(r,(epa->pa)[i]);
      if ((ell>=0) && (ell < ell2)) {
        ell2 = ell;
        minGgi = i;
      }
    }
  }

  if (DebugReductionRed || (DebugReductionEcart&1)) {
    printf("ecartFindReducer(): ell1=%d, ell2=%d, minGrade=%d, minGseti=%d, minGgi=%d\n",ell1,ell2,minGrade,minGseti,minGgi);
  }
  if (ell1 <= ell2) {
    if (ell1 == LARGE) {
      er.ell = -1;
      return er;
    }else{
      er.ell = ell1;
      er.first = 1;
      er.grade = minGrade;
      er.gseti = minGseti;
      return er;
    }
  }else{
      er.ell = ell2;
      er.first = 0;
      er.ggi = minGgi;
      return er;
  }
}

static POLY  ecartCheckSyz0(POLY cf,POLY r_0,POLY syz,
                           struct gradedPolySet *gg,POLY r)
{
  POLY f;
  int grd,i;
  POLY q;
  struct coeff *c;
  f = ppMult(cf,r_0);
  while (syz != POLYNULL) {
    grd = syz->m->e[0].x;
    i = syz->m->e[0].D;
    c = syz->coeffp;
    if (c->tag == POLY_COEFF) {
      q = c->val.f;
    }else{
      q = POLYNULL;
    }
    f = ppAdd(f,ppMult(q,((gg->polys[grd])->g)[i]));
                                /* not gh, works only for _ecart0 */
    syz = syz->next;
  }
  f = ppSub(f,r);
  return f;
}

static void  ecartCheckSyz0_printinfo(POLY cf,POLY r_0,POLY syz,
                                      struct gradedPolySet *gg,POLY r)
{
  POLY f;
  int grd,i;
  POLY q;
  struct coeff *c;
  fprintf(stderr,"cf=%s\n",POLYToString(cf,'*',1));
  fprintf(stderr,"r_0=%s\n",POLYToString(r_0,'*',1));
  fprintf(stderr,"syz=%s\n",POLYToString(syz,'*',1));
  fprintf(stderr,"r=%s\n",POLYToString(r,'*',1));
  f = ppMult(cf,r_0);
  while (syz != POLYNULL) {
    grd = syz->m->e[0].x;
    i = syz->m->e[0].D;
    c = syz->coeffp;
    if (c->tag == POLY_COEFF) {
      q = c->val.f;
    }else{
      q = POLYNULL;
    }
	fprintf(stderr,"[grd,idx]=[%d,%d], %s\n",grd,i,
			POLYToString(((gg->polys[grd])->g)[i],'*',1));
    /* f = ppAdd(f,ppMult(q,((gg->polys[grd])->g)[i])); */
    syz = syz->next;
  }
  /* f = ppSub(f,r); */
}


POLY reduction_ecart(r,gset,needSyz,syzp)
     POLY r;
     struct gradedPolySet *gset;
     int needSyz;
     struct syz0 *syzp; /* set */
{
  POLY rn;
  if (TraceLift && needSyz) {
    warningGradedSet("TraceLift cannot be used to get syzygy. TraceLift is turned off.\n");
    TraceLift = 0;
  }
  if (TraceLift) {
	if (EcartAutomaticHomogenization) {
	  if (TraceLift_ringmod == NULL) {
		warningPoly("reduction_ecart: TraceLift_ringmod is not set.\n");
		return reduction_ecart1(r,gset,needSyz,syzp);
	  }
	  rn = reduction_ecart1_mod(r,gset);
	  if (rn == POLYNULL) return rn;
	  else return reduction_ecart1(r,gset,needSyz,syzp);
	}else{
	  return reduction_ecart0(r,gset,needSyz,syzp);
	}
  }else{
	if (EcartAutomaticHomogenization) {
	  return reduction_ecart1(r,gset,needSyz,syzp);
	}else{
	  return reduction_ecart0(r,gset,needSyz,syzp);
	}
  }
}

/*
  r and gset are assumed to be (0,1)-homogenized (h-homogenized)
  Polynomials r and gset are assumed
  to be double homogenized (h-homogenized and s(H)-homogenized)
 */  
static POLY reduction_ecart0(r,gset,needSyz,syzp)
     POLY r;
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
  struct ecartReducer ells;
  struct ecartPolyArray *gg;
  POLY pp;
  int ell;
  POLY cf_o;
  POLY syz_o;
  POLY r_0;
  int se;
  struct coeff *cont;

  extern struct ring *CurrentRingp;
  struct ring *rp;

  r_0 = r;
  gg = NULL;
  if (needSyz) {
    if (r ISZERO) { rp = CurrentRingp; } else { rp = r->m->ringp; }
    cf = cxx(1,0,0,rp);
    syz = ZERO;
  }

  if (r != POLYNULL) {
    rp = r->m->ringp;
    if (! rp->weightedHomogenization) {
      errorKan1("%s\n","ecart.c: the given ring must be declared with [(weightedHomogenization) 1]");
    }
  }

  if (DoCancel && (r != POLYNULL)) shouldReduceContent(r,1);

  if (DebugReductionEcart&1) printf("--------------------------------------\n");
  do {
    if (DebugReductionRed) printf("r=%s\n",POLYToString(r,'*',1));
    if (DebugReductionEcart & 1) printf("r=%s+...\n",POLYToString(head(r),'*',1));
    ells = ecartFindReducer(r,gset,gg);
    ell = ells.ell;
    if (ell > 0) {
      if (DebugReductionEcart & 2) printf("*");
      if (needSyz) {
        gg = ecartPutPolyInG(r,gg,cf,syz);
      }else{
        gg = ecartPutPolyInG(r,gg,POLYNULL,POLYNULL);
      }
    }
    if (ell >= 0) {
      if (ells.first) {
        pp = ((gset->polys[ells.grade])->gh)[ells.gseti];
      }else{
        if (DebugReductionEcart & 4) printf("#");
        pp = (gg->pa)[ells.ggi];
      }
      if (ell > 0) r = ppMult(cxx(1,0,ell,rp),r); /* r = s^ell r */
      r = (*reduction1)(r,pp,needSyz,&cc,&cg);

      if (DoCancel && (r != POLYNULL)) { 
        if (shouldReduceContent(r,0)) {
          r = reduceContentOfPoly(r,&cont);
          shouldReduceContent(r,1);
          if (DebugReductionEcart || DebugReductionRed || DebugContentReduction) printf("CONT=%s ",coeffToString(cont));   
        }
      }


      if (needSyz) {
        if (ells.first) {
          if (ell >0) cc = ppMult(cc,cxx(1,0,ell,rp));
          cf = ppMult(cc,cf);
          syz = cpMult(toSyzCoeff(cc),syz);
          syz = ppAdd(syz,toSyzPoly(cg,ells.grade,ells.gseti));
        }else{
          if (ell >0) cc = ppMult(cc,cxx(1,0,ell,rp));
          cf_o = (gg->cf)[ells.ggi];
          syz_o = (gg->syz)[ells.ggi];
          cf = ppMult(cc,cf);
          cf = ppAdd(cf,ppMult(cg,cf_o));
          syz = cpMult(toSyzCoeff(cc),syz);
          syz = ppAdd(syz,cpMult(toSyzCoeff(cg),syz_o));
          /* Note. 2003.07.19 */
        }
        if (DebugReductionRed) {
          POLY tp;
          tp = ecartCheckSyz0(cf,r_0,syz,gset,r);
          if (tp != POLYNULL) {
            fprintf(stderr,"reduction_ecart0(): sygyzy is broken. Return the Current values.\n");
            fprintf(stderr,"tp=%s\n",POLYToString(tp,'*',1));
			ecartCheckSyz0_printinfo(cf,r_0,syz,gset,r);
            syzp->cf = cf;
            syzp->syz = syz;
            return r;
          }
        }
      }
      if (r ISZERO) goto ss;

      /* r = r/s^? Don't do this?? */
      r = ecartDivideSv(r,&se); 
	  if (needSyz && (se > 0)) {
		POLY tt;
		tt = cxx(1,0,-se,rp);
		cf = ppMult(tt,cf);
		syz = cpMult(toSyzCoeff(tt),syz);
	  }

    }
  }while (ell >= 0);

 ss: ;
  if (needSyz) {
    syzp->cf = cf;   /* cf is in the ring of r */
    syzp->syz = syz; /* syz is in the syzRing of r */
  }

  if (DoCancel && (r != POLYNULL)) { 
    if (r->m->ringp->p == 0) {
	  r = reduceContentOfPoly(r,&cont);
	  if (DebugReductionEcart || DebugReductionRed || DebugContentReduction) printf("cont=%s ",coeffToString(cont));     
    }
  }

  return(r);
}

/*
  r and gset are assumed to be (0,1)-homogenized (h-homogenized)
  r and gset are not assumed
  to be double homogenized (h-homogenized and s(H)-homogenized)
  They are automatically s(H)-homogenized, and s is set to 1 
  when this function returns.
 */  
static POLY reduction_ecart1(r,gset,needSyz,syzp)
     POLY r;
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
  struct ecartReducer ells;
  struct ecartPolyArray *gg;
  POLY pp;
  int ell;
  POLY cf_o;
  POLY syz_o;
  POLY r_0;
  int se;
  struct coeff *cont;

  extern struct ring *CurrentRingp;
  struct ring *rp;
  extern struct ring *SmallRingp;

  r_0 = r;
  gg = NULL;
  if (needSyz) {
    if (r ISZERO) { rp = CurrentRingp; } else { rp = r->m->ringp; }
    cf = cxx(1,0,0,rp);
    syz = ZERO;
  }

  if (r != POLYNULL) {
    rp = r->m->ringp;
    if (! rp->weightedHomogenization) {
      errorKan1("%s\n","ecart.c: the given ring must be declared with [(weightedHomogenization) 1]");
    }
  }

  r = goHomogenize11(r,DegreeShifto_vec,DegreeShifto_size,-1,1);
  /* 1 means homogenize only s */
  if (DoCancel && (r != POLYNULL)) shouldReduceContent(r,1);

  if (DebugReductionEcart&1) printf("=======================================\n");
  do {
    if (DebugReductionRed) printf("(ecart1(d)) r=%s\n",POLYToString(r,'*',1));
    if (DebugReductionEcart & 1) printf("r=%s+,,,\n",POLYToString(head(r),'*',1));

    ells = ecartFindReducer(r,gset,gg);
    ell = ells.ell;
    if (ell > 0) {
      if (DebugReductionEcart & 2) printf("%");
      if (needSyz) {
        gg = ecartPutPolyInG(r,gg,cf,syz);
      }else{
        gg = ecartPutPolyInG(r,gg,POLYNULL,POLYNULL);
      }
    }
    if (ell >= 0) {
      if (ells.first) {
        pp = ((gset->polys[ells.grade])->gh)[ells.gseti];
      }else{
        if (DebugReductionEcart & 4) {printf("+"); fflush(NULL);}
        pp = (gg->pa)[ells.ggi];
      }
      if (ell > 0) r = ppMult(cxx(1,0,ell,rp),r); /* r = s^ell r */
      r = (*reduction1)(r,pp,needSyz,&cc,&cg);

      if (DoCancel && (r != POLYNULL)) { 
        if (shouldReduceContent(r,0)) {
          r = reduceContentOfPoly(r,&cont);
          shouldReduceContent(r,1);
          if (DebugReductionEcart || DebugReductionRed || DebugContentReduction) printf("CONT=%s ",coeffToString(cont));   
        }
      }

      if (needSyz) {
        if (ells.first) {
          if (ell > 0) cc = ppMult(cc,cxx(1,0,ell,rp));
          cf = ppMult(cc,cf);
          syz = cpMult(toSyzCoeff(cc),syz);
          syz = ppAdd(syz,toSyzPoly(cg,ells.grade,ells.gseti));
        }else{
          if (ell >0) cc = ppMult(cc,cxx(1,0,ell,rp));
          cf_o = (gg->cf)[ells.ggi];
          syz_o = (gg->syz)[ells.ggi];
          cf = ppMult(cc,cf);
          cf = ppAdd(cf,ppMult(cg,cf_o));
          syz = cpMult(toSyzCoeff(cc),syz);
          syz = ppAdd(syz,cpMult(toSyzCoeff(cg),syz_o));
          /* Note. 2003.07.19 */
        }
      }

      if (DebugReductionRed) {
        POLY tp;
        tp = ecartCheckSyz0(cf,r_0,syz,gset,r);
        tp = goDeHomogenizeS(tp);
        if (tp != POLYNULL) {
          fprintf(stderr,"Error: reduction_ecart1(): sygyzy is broken. Return the Current values.\n");
          fprintf(stderr,"%s\n",POLYToString(tp,'*',1));
          syzp->cf = cf;
          syzp->syz = syz;
          return r;
        }
      }

      if (r ISZERO) goto ss1;
      r = ecartDivideSv(r,&se); /* r = r/s^? */

      if (needSyz && (se > 0)) { /* It may not necessary because of dehomo*/
        POLY tt;
        /*printf("!1/H!"); fflush(NULL);*/ /* misc-2003/ecart/t1.sm1 foo4 */
        tt = cxx(1,0,-se,rp);
        cf = ppMult(tt,cf);
        syz = cpMult(toSyzCoeff(tt),syz);
      }

      /* For debug */
      if (DebugReductionRed && needSyz) {
        POLY tp;
        tp = ecartCheckSyz0(cf,r_0,syz,gset,r);
        tp = goDeHomogenizeS(tp);
        if (tp != POLYNULL) {
          fprintf(stderr,"Error: reduction_ecart1() after divide: sygyzy is broken. Return the Current values.\n");
          fprintf(stderr,"%s\n",POLYToString(tp,'*',1));
          syzp->cf = cf;
          syzp->syz = syz;
          return r;
        }
      }

    }
  }while (ell >= 0);

 ss1: ;
  if (needSyz) {
    /* dehomogenize the syzygy. BUG, this may be inefficient.  */
    cf = goDeHomogenizeS(cf);
    syz = goDeHomogenizeS(syz);
    /*printf("cf=%s\n",POLYToString(cf,'*',1));
      printf("syz=%s\n",POLYToString(syz,'*',1));*/
    syzp->cf = cf;   /* cf is in the CurrentRingp */
    syzp->syz = syz; /* syz is in the SyzRingp */
  }

  r = goDeHomogenizeS(r);
  if (DoCancel && (r != POLYNULL)) { 
    if (r->m->ringp->p == 0) {
      r = reduceContentOfPoly(r,&cont);
      if (DebugReductionEcart || DebugReductionRed || DebugContentReduction) printf("cont=%s ",coeffToString(cont));     
    }
  }

  /* For debug */
  if (DebugReductionRed && needSyz) {
    POLY tp;
    tp = ecartCheckSyz0(cf,r_0,syz,gset,r);
    tp = goDeHomogenizeS(tp);
    if (tp != POLYNULL) {
      fprintf(stderr,"Error: reduction_ecart1() last step: sygyzy is broken. Return the Current values.\n");
      fprintf(stderr,"%s\n",POLYToString(tp,'*',1));
      syzp->cf = cf;
      syzp->syz = syz;
      return r;
    }
  }

  return(r);
}

/* Functions for trace lift  */
static struct ecartReducer ecartFindReducer_mod(POLY r,
                                                struct gradedPolySet *gset,
                                                struct ecartPolyArray *epa)
{
  int grd;
  struct polySet *set;
  int minGrade = 0;
  int minGseti = 0;
  int minGgi   = 0;
  int ell1 = LARGE;
  int ell2 = LARGE;
  int ell;
  int i;
  struct ecartReducer er;
  /* Try to find a reducer in gset; */
  grd = 0;
  while (grd < gset->maxGrade) {
    set = gset->polys[grd];
    for (i=0; i<set->size; i++) {
      if (set->gh[i] == POLYNULL) {
        /* goHomogenize set->gh[i] */
          if (EcartAutomaticHomogenization) {
              set->gh[i] = goHomogenize11(set->g[i],DegreeShifto_vec,DegreeShifto_size,-1,1);
          }else{
              set->gh[i] = set->g[i];
          }
      }
	  if (TraceLift && (set->gmod[i] == POLYNULL)) {
		set->gmod[i] = modulop(set->gh[i],TraceLift_ringmod);
	  }
	  if (TraceLift) {
		ell = ecartGetEll(r,set->gmod[i]);
	  }else{
		ell = ecartGetEll(r,set->gh[i]);
	  }
      if ((ell>=0) && (ell < ell1)) {
        ell1 = ell;
        minGrade = grd; minGseti=i;
      }
    }
    grd++;
  }
  if (epa != NULL) {
    /* Try to find in the second group. */
    for (i=0; i< epa->size; i++) {
      ell = ecartGetEll(r,(epa->pa)[i]);
      if ((ell>=0) && (ell < ell2)) {
        ell2 = ell;
        minGgi = i;
      }
    }
  }

  if (DebugReductionRed || (DebugReductionEcart&1)) {
    printf("ecartFindReducer_mod(): ell1=%d, ell2=%d, minGrade=%d, minGseti=%d, minGgi=%d, p=%d\n",ell1,ell2,minGrade,minGseti,minGgi,TraceLift_ringmod->p);
  }
  if (ell1 <= ell2) {
    if (ell1 == LARGE) {
      er.ell = -1;
      return er;
    }else{
      er.ell = ell1;
      er.first = 1;
      er.grade = minGrade;
      er.gseti = minGseti;
      return er;
    }
  }else{
      er.ell = ell2;
      er.first = 0;
      er.ggi = minGgi;
      return er;
  }
}

static POLY reduction_ecart1_mod(r,gset)
     POLY r;
     struct gradedPolySet *gset;
{
  int reduced,reduced1,reduced2;
  int grd;
  struct polySet *set;
  int i;
  POLY cc,cg;
  struct ecartReducer ells;
  struct ecartPolyArray *gg;
  POLY pp;
  int ell;
  int se;

  extern struct ring *CurrentRingp;
  struct ring *rp;

  gg = NULL;

  if (r != POLYNULL) {
    rp = r->m->ringp;
    if (! rp->weightedHomogenization) {
      errorKan1("%s\n","ecart.c: the given ring must be declared with [(weightedHomogenization) 1]");
    }
  }

  r = goHomogenize11(r,DegreeShifto_vec,DegreeShifto_size,-1,1);
  /* 1 means homogenize only s */
  /*printf("r=%s (mod 0)\n",POLYToString(head(r),'*',1)); 
	KshowRing(TraceLift_ringmod); **/

  r = modulop(r,TraceLift_ringmod);
  if (r != POLYNULL) rp = r->m->ringp; /* reset rp */

  /* printf("r=%s (mod p)\n",POLYToString(head(r),'*',1)); **/

  if (DebugReductionEcart&1) printf("=====================================mod\n");
  do {
    if (DebugReductionRed) printf("(ecart1_mod(d)) r=%s\n",POLYToString(r,'*',1));
    if (DebugReductionEcart & 1) printf("r=%s+,,,\n",POLYToString(head(r),'*',1));

    ells = ecartFindReducer_mod(r,gset,gg);
    ell = ells.ell;
    if (ell > 0) {
      if (DebugReductionEcart & 2) printf("%");
      gg = ecartPutPolyInG(r,gg,POLYNULL,POLYNULL);
    }
    if (ell >= 0) {
      if (ells.first) {
        pp = ((gset->polys[ells.grade])->gmod)[ells.gseti];
      }else{
        if (DebugReductionEcart & 4) {printf("+"); fflush(NULL);}
        pp = (gg->pa)[ells.ggi];
      }
      if (ell > 0) r = ppMult(cxx(1,0,ell,rp),r); /* r = s^ell r */
      r = (*reduction1)(r,pp,0,&cc,&cg);
      if (r ISZERO) goto ss1;
      r = ecartDivideSv(r,&se); /* r = r/s^? */
    }
  }while (ell >= 0);

 ss1: ;

  r = goDeHomogenizeS(r);

  return(r);
}

