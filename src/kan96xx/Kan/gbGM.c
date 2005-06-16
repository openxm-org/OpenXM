/* $OpenXM: OpenXM/src/kan96xx/Kan/gbGM.c,v 1.4 2005/06/09 04:09:22 takayama Exp $ */
/* gbGM.c  GM=Gebauer and Moller
 */

#include <stdio.h>
#include "datatype.h"
#include "extern2.h"
#include "matrix.h"
#include "gradedset.h"


/*#define DEBUG*/
/* Take statistics of mmLarger_matrix in orderT.c */
/* #define STATISTICS */

struct polySet_gm {
  POLY *g;
  int lim;
  int size;
  int *del;
};

struct pair_gm {
  POLY lcm;
  int i;
  int j;
};

struct pairSet {
  struct pair_gm *p;
  int lim;
  int size;
  int *del;
};

/* prototypes */
struct polySet_gm newPolySet_gm(int n);
struct pairSet newPairSet(int n);
int pairSetSize(struct pairSet d);
struct pairSet pairSetJoin(struct pairSet a,struct pairSet b);
struct polySet_gm enlargePolySet_gm(struct polySet_gm g);
struct pairSet deletePair_gm(struct pairSet d,int index);
int minPair_gm(struct pairSet d);
struct pairSet updatePair_gm(struct pairSet d,int t,
                             struct polySet_gm g,POLY gt);
struct polySet_gm markRedundant_gm(struct polySet_gm g,int j);
/*
  struct gradedPolySet *groebner_gm(struct arrayOfPOLY f,
  int needBack,
  int needSyz, struct pair **grP);
*/
void outputPairSet(struct pairSet d);
void outputPolySet_gm(struct polySet_gm g);
void errorGBGM(char *s);
POLY mReductionBySet(POLY f, POLY *s,int size);
POLY mReductionCdrBySet(POLY f, POLY *s,int size);
int criterion1(POLY f,POLY g,POLY lc);

extern int UseCriterion1;
extern int Verbose;

#define GLIMIT 100
int Criterion1,Criterion2; /* for taking a statistics. */
int Spairs;
extern int Criterion2B, Criterion2F, Criterion2M;
extern int Statistics;

#ifdef STATISTICS
int CountE;
int CountI[2*N0];
#endif

struct polySet_gm newPolySet_gm(n)
     int n;
{
  struct polySet_gm g;
  int i;
  g.g = (POLY *) sGC_malloc(sizeof(POLY)*(n+1));
  g.del = (int *) sGC_malloc(sizeof(int)*(n+1));
  if ((g.g == (POLY *)NULL) ||
      (g.del == (int *)NULL))
    errorGBGM("No more memory.");
  g.lim = n;
  g.size = 0;
  for (i=0; i<n; i++) g.del[i] = 0;
  return(g);
}

struct pairSet newPairSet(n)
     int n;
{
  struct pairSet g;
  int i;
  g.p = (struct pair_gm *) sGC_malloc(sizeof(struct pair_gm)*(n+1));
  g.del = (int *) sGC_malloc(sizeof(int)*(n+1));
  if ((g.p == (struct pair_gm *)NULL) ||
      (g.del == (int *)NULL))
    errorGBGM("No more memory.");
  g.lim = n;
  g.size = 0;
  for (i=0; i<n; i++) g.del[i] = 0;
  return(g);
}

int pairSetSize(d)
     struct pairSet d;
{
  int s,i;
  s = 0;
  for (i=0; i< d.size; i++) {
    if (d.del[i] == 0) s++;
  }
  return(s);
}

struct pairSet pairSetJoin(a,b)
     struct pairSet a,b;
{
  int m,n,k,i;
  struct pairSet ans;
  m = pairSetSize(a);
  n = pairSetSize(b);
  ans = newPairSet(m+n);
  k = 0;
  for (i=0 ; i<a.size; i++) {
    if (a.del[i] == 0) {
      ans.p[k] = a.p[i]; ans.del[k++] = 0;
    }
  }
  for (i=0; i<b.size; i++) {
    if (b.del[i] == 0) {
      ans.p[k] = b.p[i]; ans.del[k++] = 0;
    }
  }
  if (k != (m+n)) errorGBGM("Internal error in pairSetJoin().");
  ans.size = m+n;
  return(ans);
}

struct polySet_gm enlargePolySet_gm(g)
     struct polySet_gm g;
{
  int i;
  struct polySet_gm ans;
  if (g.lim < g.size+3) {
    ans = newPolySet_gm((g.lim)*2);
    for (i=0; i<g.size; i++) {
      ans.g[i] = g.g[i];
      ans.del[i] = g.del[i];
    }
    ans.size = g.size;
  }else{ ans = g; }
  return(ans);
}

struct pairSet deletePair_gm(d,index)
     struct pairSet d;
     int index;
     /* delete d[index] */
{
  int i;
  d.del[index] = 0;
  for (i= index; i<d.size-1; i++) {
    d.p[i] = d.p[i+1];
  }
  d.size--;
  return(d);
}

int minPair_gm(d)
     struct pairSet d;
{
  POLY min;
  int index,i;
  min = d.p[0].lcm;
  index = 0;
  for (i=0; i < d.size; i++) {
    if ( (*mmLarger)(min, d.p[i].lcm)) {
      index = i;
      min = d.p[i].lcm;
    }
  }
  return(index);
}

struct pairSet updatePair_gm(d,t,g,gt)
     struct pairSet d;
     int t;
     struct polySet_gm g;
     POLY gt;
{
  int i,j,k;
  struct pairSet new;
  POLY it;
  POLY jt;

  if (Verbose >= 2) {
    printf("\nupdatepair_gm(): ");
    printf("g.size=%d ",g.size);
    printf("d.size=%d; ",d.size);
  }

  /* make list (1,t), (2,t), .... ====> new ( D1 ) */
  new = newPairSet(g.size); new.size = 0;
  for (i=0; i<g.size; i++) {
    if (g.del[i] == 0) {
      new.p[i].i = i;
      new.p[i].j = t;
      new.p[i].lcm = (*lcm)(g.g[i],gt);
    }else{
      new.p[i].i = i;  
      new.p[i].j = t;
      new.p[i].lcm = ZERO; /* Compute it when it is necessary. */
    }
    new.del[i] = g.del[i];
    new.size++;
  }
#ifdef DEBUG
  /*printf("\nnew is ...");
    outputPairSet(new);*/
#endif
  
  /* Candel in D=d all (i,j) such that B_t(i,j) */

  for (k=0; k<d.size; k++) {
    if ((d.del[k] == 0) && ((*isReducible)(d.p[k].lcm,gt)) ) {
      /* check T(it) != T(i,j) != T(j,t) */
      i = d.p[k].i; j = d.p[k].j;
      if ((new.del[i] == 1) || (new.del[j] == 1)) {
        /* fprintf(stderr,"Warning in updatepair_gm(): i=%d, j=%d; rewriting new.\n",i,j); */
        if (new.del[i] == 1) {
          new.p[i].lcm = (*lcm)(g.g[i],gt);
        }
        if (new.del[j] == 1) {
          new.p[j].lcm = (*lcm)(g.g[j],gt);
        }
      }
      if (((*mmLarger)((new.p[i].lcm),(new.p[j].lcm)) != 2) &&
          ((*mmLarger)((new.p[i].lcm),(d.p[k].lcm)) != 2) &&
          ((*mmLarger)((new.p[j].lcm),(d.p[k].lcm)) != 2)) {
        /* delete T(i,j) in d */
        d.del[k] = 1;
        if (Verbose >= 2) printf("B%d(%d,%d) ",t,i,j);
        Criterion2B++;
      }
    }
  }

  /* Then, d is D' */
  /* Next, check M(i,t) and F(i,t) */
  for (i=0; i<g.size; i++) {
    for (j=i+1; j<g.size; j++) {
      if ((g.del[i] == 0) && (g.del[j] == 0)) {
        it = new.p[i].lcm;
        jt = new.p[j].lcm;
        switch ( (*mmLarger)(it,jt)) {
        case 2:
          /* F(j,t), i<j */
          new.del[j] = 1;
          if (Verbose >= 2) printf("F%d(%d,%d) ",i,j,t);
          Criterion2F++;
          break;
        case 1:
          /* g[i] > g[j],  M(i,t) */
          if ((*isReducible)(it,jt)) {
            new.del[i] = 1;
            if (Verbose >=2) printf("M%d(%d,%d) ",j,i,t);
            Criterion2M++;
          }
          break;
        case 0: /* M(j,t) */
          if ((*isReducible)(jt,it)) {
            new.del[j] = 1;
            if (Verbose >=2) printf("M%d(%d,%d) ",i,j,t);
            Criterion2M++;
          }
          break;

        }
      }
    }
  }

  /* Finally, check T(i) T(t) = T(i,t) */
  if (UseCriterion1) {
    for (i=0; i<g.size; i++) {
      if (new.del[i] == 0) {
        if (criterion1(g.g[i],gt,new.p[i].lcm)) {
          new.del[i] = 1;
          if (Verbose >=2) printf("1(%d,%d) ",i,t);
          Criterion1++;
        }
      }
    }
  }

  /* d <-- D' \cup new */
  d = pairSetJoin(d,new);
  if (Verbose >=2) printf("new d.size = %d.\n",d.size);
#ifdef DEBUG
  outputPairSet(d);
#endif  
  return(d);
}


struct polySet_gm markRedundant_gm(g,j)
     struct polySet_gm g;
     int j;
     /* compare only with g[j] */
{
  int i;
  for (i=0; i<g.size; i++) {
    if ((g.del[i] == 0) && (i != j)) {
      if (g.del[j] == 0) {
        switch((*mmLarger)((g.g[i]),(g.g[j]))) {
        case 2:
          g.del[j] = 1;
          break;
        case 1:
          if ((*isReducible)((g.g[i]),(g.g[j]))) {
            g.del[i] = 1;
          }
          break;
        case 0:
          if ((*isReducible)((g.g[j]),(g.g[i]))) {
            g.del[j] = 1;
          }
          break;
        }
      }
    }
  }
#ifdef DEBUG
  /*printf("\nend of markRedundant_gm...");
    outputPolySet_gm(g);*/
#endif  
  return(enlargePolySet_gm(g));
}



struct gradedPolySet *groebner_gm(f,needBack,needSyz,grP,countDown,forceReduction,reduceOnly,gbCheck)
     struct arrayOfPOLY *f;
     int needBack;
     int needSyz;
     struct pair **grP;
     int countDown;
     int forceReduction;
     int reduceOnly;
     int gbCheck;
{
  int r;
  struct pair_gm top;
  int tindex;
  POLY h;
  int i,j;
  int ss;
  struct polySet_gm g;
  struct pairSet d;
  int t;
  struct spValue sv;
  struct gradedPolySet *ans;
  int grade,indx;
  extern int ReduceLowerTerms;

  if (needBack || needSyz) {
    fprintf(stderr,"Warning: groebner_gm() does not compute the backward transformation and syzygies.\n");
  }
  if (reduceOnly || gbCheck) {
    fprintf(stderr,"Warning: groebner_gm() does not implement reduceOnly/gbCheck.\n");
  }
  
#ifdef STATISTICS
  CountE = 0;
  for (i=0; i<2*N; i++) CountI[i]=0;
#endif
  /* taking the statistics. */
  Criterion1 = Criterion2B = Criterion2M = Criterion2F = 0;
  Spairs = 0;

  r = f->n;
  g = newPolySet_gm(r + GLIMIT);
  d = newPairSet(1);

  g.size = 0; 
  g.g[g.size] = getArrayOfPOLY(f,g.size);
  g.del[g.size] = 0; g.size ++;

  for (t=1; t<r; t++) {
    d = updatePair_gm(d,t,g,getArrayOfPOLY(f,t));
    g.g[t] = getArrayOfPOLY(f,t); g.del[t] = 0; g.size ++ ;
    g = markRedundant_gm(g,t);
  }

#ifdef DEBUG
  outputPolySet_gm(g);
#endif

  while (d.size > 0) {
    tindex = minPair_gm(d);
    top = d.p[tindex];
    if (Verbose) {
      printf("\nRemaining pairs = %d : ",d.size);
    }
    i = top.i;
    j = top.j;
    if (Verbose >=2) printf("sp(%d,%d) ",i,j);
    Spairs++;
    sv = (*sp)(g.g[i],g.g[j]);
    h = ppAddv(ppMult(sv.a,g.g[i]),ppMult(sv.b,g.g[j]));
    h = mReductionBySet(h,g.g,g.size);
    if (Verbose >=2) printf("->%s ",POLYToString(h,' ',0));

    if (!Verbose) {
      printf(" <%d>",d.size);
      fflush(stdout);
    }
    
    if (!(h ISZERO)) {
      if (ReduceLowerTerms) {
        h = mReductionCdrBySet(h,g.g,g.size);
      }
      d = updatePair_gm(d,r,g,h);
      g.g[r] = h; g.del[r] = 0;
      r++; g.size++;
      g = markRedundant_gm(g,r-1);
    }
    d = deletePair_gm(d,tindex);
#ifdef DEBUG
    outputPolySet_gm(g);
#endif    
  }


  if (Verbose || Statistics) {
    printf("\n\nThe number of s-pairs is %d.\n",Spairs);
    printf("Criterion1 is applied %d times.\n",Criterion1);
    printf("Criterions M,F and B are applied M=%d, F=%d, B=%d times.\n",Criterion2M,Criterion2F,Criterion2B);
    Criterion1 = Criterion2M = Criterion2F = Criterion2B = 0;
    Spairs = 0;
  }
#ifdef STATISTICS
  printf("\n\nEqual : %d\n",CountE);
  for (i=0; i<2*N; i++) {
    printf("%d times of loop : %d\n",i,CountI[i]);
  }
#endif

  ans = newGradedPolySet(1);
  ans->gb = 1;
  for (i=0; i<ans->lim; i++) {
    ans->polys[i] = newPolySet(1);
  }

  for (i=0; i<g.size; i++) {
    if (g.del[i] == 0) {
      grade=-1;whereInG(ans,g.g[i],&grade,&indx,0); /* we do not use sugar. */
      ans = putPolyInG(ans,g.g[i],grade,indx,(struct syz0 *)NULL,0,-1);
    }
  }

  printf("\n");
  return(ans);
}


void outputPairSet(d)
     struct pairSet d;
{ int i;
 printf("\nOutput struct pairSet. ");
 printf(".size=%d, .lim=%d :\n",d.size,d.lim);
 for (i=0; i<d.size; i++) {
   printf("%d[%d] i=%d,j=%d, lcm=%s  ",i,d.del[i],d.p[i].i,d.p[i].j,
          POLYToString(d.p[i].lcm,' ',0));
 }
 printf("\n"); fflush(stdout);
}
       
void outputPolySet_gm(g)
     struct polySet_gm g;
{ int i;
 printf("\nOutput struct polySet_gm. ");
 printf(".size=%d, .lim=%d :\n",g.size,g.lim);
 for (i=0; i<g.size; i++) {
   printf("%d[%d] %s\n",i,g.del[i],POLYToString(g.g[i],' ',0));
 }
 printf("\n"); fflush(stdout);
}
       
int criterion1(f0,g0,lc0)
     POLY f0;
     POLY g0;
     POLY lc0;
{
  /* This is used only for commutative case. */
  register int i;
  MONOMIAL f,g,lc;
  int n;
  f = f0->m; g = g0->m; lc = lc0->m;
  if (f->ringp != g->ringp) return(0);
  if (f->ringp != lc->ringp) return(0);
  n = f->ringp->n;
  for (i = 0; i<n ; i++) {
    if (( f->e[i].x + g->e[i].x) != (lc->e[i].x)) return(0);
  }
  for (i = 0; i<n ; i++) {
    if (( f->e[i].D + g->e[i].D) != (lc->e[i].D)) return(0);
  }
  return(1);
}


POLY mReductionBySet(f,s,size)
     POLY f;
     POLY *s;
     int size;
{
  int reduced1;
  int i;
  POLY cc,cg;
  do {
    reduced1 = 0;
    for (i=0; i<size; i++) {
      if (f ISZERO) goto ss;
      if ((*isReducible)(f,s[i])) {
        f = (*reduction1)(f,s[i],0,&cc,&cg);
        reduced1 = 1;
      }
    }
  } while (reduced1 != 0);
 ss: ;
  return(f);
}

POLY mReductionCdrBySet(f,s,size)
     POLY f;
     POLY *s;
     int size;
{
  int reduced1;
  int i;
  POLY cc,cg;
  POLY fs;
  do {
    reduced1 = 0;
    for (i=0; i<size; i++) {
      if (f ISZERO) goto ss;
      if ((fs=(*isCdrReducible)(f,s[i])) != ZERO) {
        f = (*reduction1Cdr)(f,fs,s[i],0,&cc,&cg);
        reduced1 = 1;
      }
    }
  } while (reduced1 != 0);
 ss: ;
  return(f);
}


void errorGBGM(s)
     char *s;
{
  fprintf(stderr,"Fatal error in yaGbasis.c: %s \n",s);
  exit(10);
}



  
 


  
