/* $OpenXM: OpenXM/src/kan96xx/Kan/ecart.c,v 1.2 2003/07/17 12:11:09 takayama Exp $ */
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
};

static struct ecartReducer ecartFindReducer(POLY r,struct gradedPolySet *gset,struct ecartPolyArray *epa);
static int ecartCheckPoly(POLY f);  /* check if it does not contain s-variables */
static int ecartCheckEnv();         /* check if the environment is OK for ecart div*/
static struct ecartPolyArray *ecartPutPolyInG(POLY g,struct ecartPolyArray *eparray);
static int ecartGetEll(POLY r,POLY g);
static POLY ecartDivideSv(POLY r);

extern int DebugReductionRed; 

/* This is used for goHomogenization */
extern int DegreeShifto_size;
extern int *DegreeShifto_vec;

/* It is used reduction_ecart() and ecartFindReducer()
   to determine if we homogenize in this function */
extern int EcartAutomaticHomogenization;

#define LARGE 0x7fffffff


static POLY ecartDivideSv(POLY r) {
  POLY f;
  int k;
  if (r == POLYNULL) return r;
  f = r; k = LARGE;
  while (r != POLYNULL) {
    if (r->m->e[0].x < k) {
      k = r->m->e[0].x;
    }
    r = r->next;
  }
  if (k > 0) {
    f = r;
    while (r != POLYNULL) {
      r->m->e[0].x -= k;
      r = r->next;
    }
  }
  return f;
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
static struct ecartPolyArray *ecartPutPolyInG(POLY g,struct ecartPolyArray *eparray)
{
  struct ecartPolyArray *a;
  POLY *ep;
  int i;
  if (eparray == (struct ecartPolyArray *)NULL) {
    a = (struct ecartPolyArray *) sGC_malloc(sizeof(struct ecartPolyArray));
    outofmemory(a);
    ep = (POLY *) sGC_malloc(sizeof(POLY)*EP_SIZE);
    outofmemory(ep);
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
    a->limit = (eparray->limit)*2;
    a->size = eparray->size;
    a->pa = ep;
    for (i=0; i<eparray->size; i++) {
      (a->pa)[i] = (eparray->pa)[i];
    }
    eparray  = a;
  }
  i = eparray->size;
  (eparray->pa)[i] = g;
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

  if (DebugReductionRed) {
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

/*
  r and gset are assumed to be (0,1)-homogenized (h-homogenized)
  If EcartAutomaticHomogenization == 0, then r and gset are assumed
  to be double homogenized (h-homogenized and s(H)-homogenized)
 */  
POLY reduction_ecart(r,gset,needSyz,syzp)
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

  extern struct ring *CurrentRingp;
  struct ring *rp;

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

  if (EcartAutomaticHomogenization) {
	  r = goHomogenize11(r,DegreeShifto_vec,DegreeShifto_size,-1,1);
  }
  /* 1 means homogenize only s */

  do {
    if (DebugReductionRed) printf("r=%s\n",POLYToString(r,'*',1));
    ells = ecartFindReducer(r,gset,gg);
    ell = ells.ell;
    if (ell > 0) {
      gg = ecartPutPolyInG(r,gg);
    }
    if (ell >= 0) {
      if (ells.first) {
        pp = ((gset->polys[ells.grade])->gh)[ells.gseti];
      }else{
        pp = (gg->pa)[ells.ggi];
      }
      if (ell > 0) r = mpMult(cxx(1,0,ell,rp),r); /* r = s^ell r */
      r = (*reduction1)(r,pp,needSyz,&cc,&cg);
      if (needSyz) {
        if (ells.first) {
          cf = ppMult(cc,cf);
          syz = cpMult(toSyzCoeff(cc),syz);
          syz = ppAddv(syz,toSyzPoly(cg,ells.grade,ells.gseti));
        }else{
          /* BUG: not yet */
        }
      }
      if (r ISZERO) goto ss;
      r = ecartDivideSv(r); /* r = r/s^? */
    }
  }while (ell >= 0);

 ss: ;
  if (needSyz) {
    syzp->cf = cf;   /* cf is in the CurrentRingp */
    syzp->syz = syz; /* syz is in the SyzRingp */
    /* BUG: dehomogenize the syzygy */
  }
  /* 
  r = goDeHomogenizeS(r);
  */
  return(r);
}
