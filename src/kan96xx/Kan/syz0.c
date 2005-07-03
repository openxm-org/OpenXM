/* $OpenXM: OpenXM/src/kan96xx/Kan/syz0.c,v 1.4 2003/08/27 03:11:12 takayama Exp $ */
#include <stdio.h>
#include <stdlib.h>
#include "datatype.h"
#include "extern2.h"
#include "matrix.h"
#include "gradedset.h"

extern int KanGBmessage;
static int Debugsyz0 = 0;
  
static POLY getFactor0(struct gradedPolySet *grG,int grd,int index);
static void clearMark(struct gradedPolySet *grG);
static void printMatrixOfPOLY(struct matrixOfPOLY *mat);
static void printArrayOfPOLY(struct arrayOfPOLY *mat);
static int isZeroRow(struct matrixOfPOLY *mat,int i);

static int getSize00OfGradedSet(struct gradedPolySet *g);
static int positionInPairs(struct pair *top,struct pair *pairs);
/*  if the top is not in the list pairs, it returns -1. */
static struct pair *oldPairsToNewPairs(struct pair *opairs,
                                       int *table,int size);
/* In the function getSyzygy(), reduced Grobner basis *grBases is
   constructed from grG by deleting unnecessary elements.
   The array <<table>> gives the correspondence between the index
   ig (i-grade), ii (i-index) of *grBases and grG.
   When <<opairs>> keeps syzygies for grG, it returns syzygies for
   *grBases.
   Note that the result syzygies are not necessarily COMPLETE.
   Note that the <<prev>> fields are not set properly.   
*/
struct matrixOfPOLY *getSyzygy01(struct gradedPolySet *reducedBasis,
                                 struct pair *excludedPairs);
/*
   When <<excludedPairs>> is NULL,
   this function computes all syzygies for the <<reducedBasis>>.
   If not, this function computes all syzygies except those in excludedPairs.
   The <<serial>> field of the <<reducedBasis>> is used to change
   syzygy polynomials into arrays.  March 15, 1997
 */



/* if (mark[j]), then the simplification is done. */
void getBackwardTransformation(grG)
     struct gradedPolySet *grG;
     /* grG->polys[i]->mark[j],syz[j] are modified. */
{
  int i,j;
  struct polySet *ps;


  for (i=0; i<grG->maxGrade; i++) {
    ps = grG->polys[i];
    for (j=0; j<ps->size; j++) {
      if (ps->mark[j] == 0 && ps->del[j] == 0) {
        simplifyBT(i,j,grG);
      }
    }
  }
}

void simplifyBT(grd,index,grG)
     int grd;
     int index;
     struct gradedPolySet *grG;
{
  POLY s,r;
  int g0,i0;

  s = grG->polys[grd]->syz[index]->syz;
  r = ZERO;

  if (KanGBmessage) {printf("."); fflush(stdout); }
  
  while (s != ZERO) {
    g0 = srGrade(s);
    i0 = srIndex(s);
    if (grG->polys[g0]->mark[i0]) {
      r = ppAdd(r,cpMult(s->coeffp,grG->polys[g0]->syz[i0]->syz));
    }else{
      simplifyBT(g0,i0,grG);
      r = ppAdd(r,cpMult(s->coeffp,grG->polys[g0]->syz[i0]->syz));
    }
    s = s->next;
  }
  grG->polys[grd]->syz[index]->syz = r;
  grG->polys[grd]->mark[index] = 1;
}


static POLY getFactor0(grG,grd,index)
     struct gradedPolySet *grG;
     int grd;
     int index;
{
  POLY ans;
  int i,j;
  struct polySet *ps;

  ans = cxx(1,0,0,grG->polys[grd]->g[index]->m->ringp);
  for (i=0; i<grG->maxGrade; i++) {
    ps = grG->polys[i];
    for (j=0; j<ps->size; j++) {
      if (ps->mark[j] && !(i == grd && j == index)) {
        ans = ppMult(ps->syz[j]->cf,ans);
      }
    }
  }
  return(ans);
}

struct arrayOfPOLY *getSyzygy0(grG,zeroPairs)
     struct gradedPolySet *grG;
     struct pair *zeroPairs;
{
  struct pair *tmp;
  int size,k,i;
  POLY s;
  struct arrayOfPOLY *r;
  struct arrayOfPOLY *ans;

  /* outputGradedPolySet(grG); */
  /* outputNode(zeroPairs);   */
  /* outputNode() workds as outputPairs() */
  tmp = zeroPairs; size = 0;
  while (tmp != (struct pair *)NULL) {
    size++;
    tmp = tmp->next;
  }
  ans = newArrayOfPOLY(size+1);

  k = 0;

  while (zeroPairs != (struct pair *)NULL) {
    s = getSyzPolyFromSp(zeroPairs,grG);
    if (s ISZERO) {

    }else {
      getArrayOfPOLY(ans,k) = s;
      k++;
    }
    zeroPairs = zeroPairs->next;
  }

  r = newArrayOfPOLY(k);
  for (i=0; i<k; i++) {
    getArrayOfPOLY(r,i) = getArrayOfPOLY(ans,i);
  }
  return(r);
}

struct matrixOfPOLY *getSyzygy(grG,zeroPairs,grBasesp,backwardMatp)
     struct gradedPolySet *grG;
     struct pair *zeroPairs;
     struct gradedPolySet **grBasesp;
     struct matrixOfPOLY **backwardMatp;
{
  int serial;
  int i,j,kk;
  struct polySet *ps;
  POLY fi;
  int grd,indx;
  struct gradedPolySet *newG;
  POLY r;
  struct syz0 syz;
  struct arrayOfPOLY *ap,*vec;
  struct matrixOfPOLY *mp;
  struct matrixOfPOLY *b,*nc,*m2,*m1,*ans,*ans2;
  struct arrayOfPOLY *dc;
  int size;
  int *table;
  struct pair *excludePairs;
  struct matrixOfPOLY *mp0;
  struct matrixOfPOLY *m0;
  extern int Sugar;

  size = getSize00OfGradedSet(grG);
  table = (int *)sGC_malloc(sizeof(int)*4*(size+1));
  if (table == (int *)NULL) errorSyz0("Out of memory.");

  if (Debugsyz0) { printf("grG=");outputGradedPolySet(grG,1);}

  /* set grBases */
  *grBasesp = newGradedPolySet(0);
  serial = 0;
  for (i=0; i<grG->maxGrade; i++) {
    ps = grG->polys[i];
    for (j=0; j<ps->size; j++) {
      if (ps->del[j] == 0) {
        fi = ps->g[j];
        grd = -1;whereInG(*grBasesp,fi,&grd,&indx,Sugar);
        *grBasesp =
          putPolyInG(*grBasesp,fi,grd,indx,newSyz0(),0,serial);
        table[4*serial] = i; table[4*serial+1] = j;
        table[4*serial+2] = grd; table[4*serial+3] = indx;
        serial++;
      }
    }
  }
  if (Debugsyz0) {printf("*grBasesp=");outputGradedPolySet(*grBasesp,1);}

  if (size != serial) {
    fprintf(stderr," size(%d) != serial(%d) ",size,serial);
    errorSyz0("Internal error size != serial");
  }

  /* set newG. Compute the forward transformations.*/
  newG = gradedPolySetCopy(grG);
  for (i=0; i<grG->maxGrade;i++) {
    ps = newG->polys[i];
    for (j=0; j<ps->size; j++) {
      fi = ps->g[j];
      r = (*reduction)(fi,*grBasesp,1,&syz);
      if (KanGBmessage) {printf("."); fflush(stdout);}
      if (! (r ISZERO)) errorSyz0("Internal error; getSyzygy(): groebner basis must be given.");
      ps->syz[j] = newSyz0();
      /* printf("%s : ",POLYToString(syz.cf,'*',1));
         printf("%s\n",POLYToString(syz.syz,'*',1)); */
      ps->syz[j]->cf = syz.cf;
      ps->syz[j]->syz = syz.syz;
    }
  }

  ap = getSyzygy0(newG,zeroPairs);

  /*    */
  excludePairs = oldPairsToNewPairs(zeroPairs,table,size);
  if (Debugsyz0) {
    printf("zeroPairs = "); outputNode(zeroPairs);
    for (i=0; i<size; i++) {
      printf("old gi=%d, old ii=%d, new gi=%d, new ii=%d\n",
             table[4*i],table[4*i+1],table[4*i+2],table[4*i+3]);
    }
    printf("excludePairs = "); outputNode(excludePairs);
    printf("\n");
  }

  if (KanGBmessage) {printf("#"); fflush(stdout); }
  mp0 = getSyzygy01(*grBasesp,excludePairs);
  if (mp0 == NULL) return NULL;
  if (KanGBmessage) {printf("#"); fflush(stdout); }
  
  /* We compute E-CB. The number of G (G-basis) is serial. */
  /* F = - C G,  G = B F , then  F + CB F = 0*/
  b = getBackwardMatrixOfPOLY(grG);
  nc = getNC(newG,serial,*grBasesp);
  dc = getDC(newG);
  /*{
    printf("\nb=\n"); printMatrixOfPOLY(b);
    printf("\n\nnc=\n"); printMatrixOfPOLY(nc);
    printf("\n\ndc=\n"); printArrayOfPOLY(dc);
    }*/
    
  if (KanGBmessage) {printf(":"); fflush(stdout);}
  m2 = getSyzygy1(b,nc,dc);
  *backwardMatp = b;
  
  /* mp is the syzygy of the G-basis. */
  mp = newMatrixOfPOLY(ap->n,serial);
  for (i=0; i < ap->n; i++) {
    vec = syzPolyToArrayOfPOLY(serial,getArrayOfPOLY(ap,i),*grBasesp);
    for (j=0; j < serial; j++) {
      getMatrixOfPOLY(mp,i,j) = getArrayOfPOLY(vec,j);
    }
  }
  /*printf(" ap->n = %d, serial=%d \n",ap->n, serial);
    printf("\nmp = \n"); printMatrixOfPOLY(mp); */

  if (KanGBmessage) {printf(";"); fflush(stdout);}
  m1 = aaMult(mp,b);
  m0 = aaMult(mp0,b);
  /* printf("\nm0 = \n"); printMatrixOfPOLY(m0); */

  ans = newMatrixOfPOLY((m0->m)+(m1->m)+(m2->m),m2->n);
  kk = 0;
  for (i=0; i<m0->m; i++) {
    if (!isZeroRow(m0,i)) {
      for (j=0; j<m0->n; j++) {
        getMatrixOfPOLY(ans,kk,j) = getMatrixOfPOLY(m0,i,j);
      }
      kk++;
    }
  }
  for (i=0; i<m1->m; i++) {
    if (!isZeroRow(m1,i)) {
      for (j=0; j<m1->n; j++) {
        getMatrixOfPOLY(ans,kk,j) = getMatrixOfPOLY(m1,i,j);
      }
      kk++;
    }
  }
  for (i=0; i<m2->m; i++) {
    if (!isZeroRow(m2,i)) {
      for (j=0; j<m2->n; j++) {
        getMatrixOfPOLY(ans,kk,j) = getMatrixOfPOLY(m2,i,j);
      }
      kk++; if (KanGBmessage) printf("*");
    }
  }
  if (kk != ans->m) {
    ans2 = newMatrixOfPOLY(kk,ans->n);
    for (i=0; i<kk; i++) {
      for (j=0; j<ans->n; j++) {
        getMatrixOfPOLY(ans2,i,j) = getMatrixOfPOLY(ans,i,j);
      }
    }
    return(ans2);
  }else{
    return(ans);
  }

}

POLY getSyzPolyFromSp(spij,grG)
     struct pair *spij;
     struct gradedPolySet *grG;
{
  int ig,ii,jg,ji;
  POLY dk;
  POLY f;
  POLY t,ans;
  int grd,indx;

  ig = spij->ig; ii = spij->ii;
  jg = spij->jg; ji = spij->ji;
  if (grG->polys[ig]->del[ii] != 0 || grG->polys[jg]->del[ji] != 0)
    return(ZERO);
  dk = spij->syz; /* in SyzRing */
  clearMark(grG);
  f = dk;
  while (f != POLYNULL) {
    grd = srGrade(f);
    indx = srIndex(f);
    grG->polys[grd]->mark[indx] = 1;
    f=f->next;
  }

  f = dk; ans = ZERO;
  while (f != POLYNULL) {
    grd = srGrade(f);
    indx = srIndex(f);
    t = grG->polys[grd]->syz[indx]->syz;
    t = cpMult(f->coeffp,t);
    t = cpMult(toSyzCoeff(getFactor0(grG,grd,indx)),t);
    ans = ppAdd(ans,t);
    f = f->next;
  }
  return(ans);
}

static void clearMark(grG)
     struct gradedPolySet *grG;
{
  int i,j;
  struct polySet *ps;
  for (i=0; i<grG->maxGrade; i++) {
    ps = grG->polys[i];
    for (j=0; j<ps->size; j++) {
      ps->mark[j] = 0;
    }
  }
}


struct arrayOfPOLY *syzPolyToArrayOfPOLY(size,f,grG)
     int size;
     POLY f; /* f is in the SyzRingp */
     struct gradedPolySet *grG;
{
  struct arrayOfPOLY *ap;
  int i,g0,i0,serial;

  ap = newArrayOfPOLY(size);
  for (i=0; i<size; i++) {
    getArrayOfPOLY(ap,i) = ZERO;
  }

  while (f != POLYNULL) {
    g0 = srGrade(f);
    i0 = srIndex(f);
    serial = grG->polys[g0]->serial[i0];
    if (serial < 0) {
      errorSyz0("syzPolyToArrayOfPOLY(): invalid serial[i] of grG.");
    }
    if (getArrayOfPOLY(ap,serial) != ZERO) {
      errorSyz0("syzPolyToArrayOfPOLY(): syzygy polynomial is broken.");
    }
    getArrayOfPOLY(ap,serial) = srSyzCoeffToPOLY(f->coeffp);
    f = f->next;
  }
  return(ap);
}



struct matrixOfPOLY *getBackwardMatrixOfPOLY(struct gradedPolySet *grG)
{
  /* use serial, del.  cf. getBackwardArray() */
  int inputSize,outputSize;
  int i,j,k,p;
  struct arrayOfPOLY *vec;
  struct matrixOfPOLY *mat;
  struct polySet *ps;
  
  inputSize = 0; outputSize = 0;
  for (i=0; i<grG->maxGrade; i++) {
    ps = grG->polys[i];
    for (j=0; j<ps->size; j++) {
      if (ps->serial[j] >= 0) ++inputSize;
      if (ps->del[j] == 0) ++outputSize;
    }
  }

  mat = newMatrixOfPOLY(outputSize,inputSize);
  k = 0;
  for (i=0; i<grG->maxGrade; i++) {
    ps = grG->polys[i];
    for (j=0; j<ps->size; j++) {
      if (ps->del[j] == 0) {
        vec = syzPolyToArrayOfPOLY(inputSize,ps->syz[j]->syz,grG);
        for (p=0; p<inputSize; p++) {
          getMatrixOfPOLY(mat,k,p)=getArrayOfPOLY(vec,p);
        }
        k++;
      }
    }
  }
  return(mat);
}


struct matrixOfPOLY *getNC(newG,n,grBases)
     struct gradedPolySet *newG;   /* F is stored and indexed by serial. */
     int n;                        /* The number of G. */
     struct gradedPolySet *grBases; /* G (G-basis) is stored. */
{
  int size,i,j,k,ii;
  struct matrixOfPOLY *mat;
  struct polySet *ps;
  struct arrayOfPOLY *vec;
  
  if (newG == (struct gradedPolySet *)NULL) {
    return(newMatrixOfPOLY(0,0));
  }
  size = 0;
  for (i=0; i<newG->maxGrade; i++) {
    ps = newG->polys[i];
    for (j=0; j<ps->size; j++) {
      if (ps->serial[j] >= 0) size++;
    }
  }
  mat = newMatrixOfPOLY(size,n);
  for (i=0; i<newG->maxGrade; i++) {
    ps = newG->polys[i];
    for (j=0; j<ps->size; j++) {
      if (ps->serial[j] >= 0) {
        ii = ps->serial[j]; 
        vec = syzPolyToArrayOfPOLY(n,ps->syz[j]->syz,grBases);
        for (k=0; k<n; k++) {
          getMatrixOfPOLY(mat,ii,k) = getArrayOfPOLY(vec,k);
        }
      }
    }
  }
  return(mat);
}
  
struct arrayOfPOLY *getDC(newG)
     struct gradedPolySet *newG;   /* F is stored and indexed by serial. */
{
  int size,i,j,k,ii;
  struct arrayOfPOLY *mat;
  struct polySet *ps;
  extern struct ring *CurrentRingp;
  
  if (newG == (struct gradedPolySet *)NULL) {
    return(newArrayOfPOLY(0));
  }
  size = 0;
  for (i=0; i<newG->maxGrade; i++) {
    ps = newG->polys[i];
    for (j=0; j<ps->size; j++) {
      if (ps->serial[j] >= 0) size++;
    }
  }
  mat = newArrayOfPOLY(size);
  for (i=0; i<newG->maxGrade; i++) {
    ps = newG->polys[i];
    for (j=0; j<ps->size; j++) {
      if (ps->serial[j] >= 0) {
        ii = ps->serial[j];
        getArrayOfPOLY(mat,ii) = ps->syz[j]->cf;
      }
    }
  }
  return(mat);
}
  


/* Syzygy from E-CB */
struct matrixOfPOLY *getSyzygy1(b,nc,dc)
     struct matrixOfPOLY *b;
     struct matrixOfPOLY *nc;
     struct arrayOfPOLY *dc;
{
  int m,n2,n;
  struct matrixOfPOLY *mat;
  int i,j,k;
  POLY r;
  POLY tmp;

  m = nc->m;
  n2 = nc->n;
  n = b->n;
  mat = newMatrixOfPOLY(m,n);
  for (i=0; i<m; i++) {
    for (j=0; j<n; j++) {
      r = ZERO;
      if (i == j) {
        r = getArrayOfPOLY(dc,i);
      }
      for (k=0; k<n2; k++) {
        tmp = ppMult(getMatrixOfPOLY(nc,i,k),getMatrixOfPOLY(b,k,j));
        r = ppAdd(r,tmp);
      }
      getMatrixOfPOLY(mat,i,j) = r;
    }
  }
  return(mat);
}

static int isZeroRow(mat,i)
     struct matrixOfPOLY *mat;
     int i;
{
  int n,j;
  n = mat->n;
  for (j=0; j<n; j++) {
    if (getMatrixOfPOLY(mat,i,j) != ZERO) return(0);
  }
  return(1);
}

void errorSyz0(s)
     char *s;
{
  fprintf(stderr,"Error(syz0.c): %s \n",s);
  exit(10);
}
    
                   
static void printMatrixOfPOLY(mat)
     struct matrixOfPOLY *mat;
{
  int n,m,i,j;
  POLY f;
  m = mat->m; n = mat->n;
  for (i=0; i<m; i++) {
    for (j=0; j<n; j++) {
      f = getMatrixOfPOLY(mat,i,j);
      printf("%s,  ",POLYToString(f,'*',1));
    }
    printf("\n");
  }
  printf("\n\n");
}

static void printArrayOfPOLY(mat)
     struct arrayOfPOLY *mat;
{
  int n,m,i,j;
  POLY f;
  m = mat->n;
  for (i=0; i<m; i++) {
    f = getArrayOfPOLY(mat,i);
    printf("%s,  ",POLYToString(f,'*',1));
  }
  printf("\n\n");
}

struct matrixOfPOLY *getSyzygy01(struct gradedPolySet *reducedBasis,
                                 struct pair *excludePairs)
{
  int r;
  struct gradedPolySet *g;
  struct gradedPairs *d;
  int i,j;
  int grade,indx;
  POLY gt;
  struct pair *top;
  int ig,ii,jg,ji;
  POLY gi,gj;
  struct spValue h;
  struct syz0 syz;
  int pgrade = 0;
  POLY rd;
  POLY syzPoly;
  POLY syzCf;
  struct syz0 *syzp;
  int serial;
  struct pair *listP;
  struct pair *listPfirst;
  extern int Verbose;
  extern struct ring *CurrentRingp;
  struct polySet *ps;
  int size;
  int listPsize = 0;
  struct arrayOfPOLY *vec;
  struct matrixOfPOLY *mp;
  extern int Sugar;


  if (Debugsyz0) printf("--------------------- getSyzygy01 -----------\n");
  if (reducedBasis == (struct gradedPolySet *)NULL)
    return((struct matrixOfPOLY *)NULL);

  g = newGradedPolySet(reducedBasis->maxGrade+1);
  for (i=0; i<g->lim; i++) { g->polys[i] = newPolySet(1); }
  d = newGradedPairs((reducedBasis->maxGrade)*(reducedBasis->maxGrade)+1);
  for (i=0; i< reducedBasis->maxGrade; i++) {
    ps = reducedBasis->polys[i];
    for (j=0; j< ps->size; j++) {
      gt = ps->g[j];
      grade=-1;whereInG(g,gt,&grade,&indx,Sugar);
      d = updatePairs(d,gt,grade,indx,g);
      g = putPolyInG(g,gt,grade,indx,newSyz0(),1,ps->serial[j]);
      if (Debugsyz0) {
        outputGradedPairs(d); outputGradedPolySet(g,1);
      }
    }
  }

  listP = newPair((struct pair *)NULL);  /* A list of syzygies will be stored*/
  listPfirst  = listP;

  while ((top = getPair(d)) != (struct pair *)NULL) {
    if (positionInPairs(top,excludePairs) < 0) {
      ig = top->ig; ii = top->ii; /* [ig,ii] */
      jg = top->jg; ji = top->ji; /* [jg,ji] */
      gi = g->polys[ig]->g[ii];
      gj = g->polys[jg]->g[ji];
      
      h = (*sp)(gi,gj);
      rd = ppAddv(ppMult(h.a,gi),ppMult(h.b,gj));
      rd = (*reduction)(rd,g,1,&syz);
      syzPoly = syz.syz;
      syzCf = syz.cf;
      
      if (KanGBmessage) {
        if (pgrade != top->grade) {
          pgrade = top->grade;
          printf(" %d",pgrade);
          fflush(stdout);
        }else{
          if (rd ISZERO) {
            printf("o"); fflush(stdout);
          }else{
            printf("."); fflush(stdout);
          }
        }
      }
      
      if (!(rd ISZERO)) {
        fprintf(stderr,"The given argument of getSyzygy01 is not a g-basis.\n");
        return((struct matrixOfPOLY *)NULL);
      }else{
        top->syz = ppAdd(toSyzPoly(h.a,ig,ii),toSyzPoly(h.b,jg,ji));
        top->syz = cpMult(toSyzCoeff(syzCf),top->syz);
        top->syz = ppAdd(top->syz,syzPoly);
        listP->next = top; top->prev = listP; listP = listP->next;
        listPsize++;
      }
    }
  }

  if (KanGBmessage) { printf("c"); fflush(stdout); }
  
  size = getSize00OfGradedSet(g);
  listPfirst = listPfirst->next;  /* It is the true top of sygyzies. */
  mp = newMatrixOfPOLY(listPsize,size);
  for (i=0; i<listPsize; i++) {
    vec = syzPolyToArrayOfPOLY(size,listPfirst->syz,g);
    for (j=0; j<size; j++) {
      getMatrixOfPOLY(mp,i,j) = getArrayOfPOLY(vec,j);
    }
    listPfirst = listPfirst->next;
  }
  return(mp);


}

static int getSize00OfGradedSet(struct gradedPolySet *g) {
  int size;
  int i,j;
  struct polySet *ps;
  size = 0;
  if (g == (struct gradedPolySet *)NULL) return(0);
  for (i=0; i<g->maxGrade; i++) {
    ps = g->polys[i];
    for (j=0; j<ps->size; j++) {
      if (ps->del[j] == 0) {
        size += 1;
      }
    }
  }
  return(size);
}

static int positionInPairs(struct pair *top, struct pair *pairs) {
  struct pair *tmp;
  int pos;
  tmp = pairs;
  pos = 0;
  if (top == (struct pair *)NULL) return(-1);
  while (tmp != (struct pair *)NULL) {
    if (((top->ig == tmp->ig) && (top->ii == tmp->ii) &&
         (top->jg == tmp->jg) && (top->ji == tmp->ji)) ||
        ((top->ig == tmp->jg) && (top->ii == tmp->ji) &&
         (top->jg == tmp->ig) && (top->ji == tmp->ii))) {
      return(pos);
    }
    pos++;
    tmp = tmp->next;
  }
  return(-1);
}

static struct pair *oldPairsToNewPairs(struct pair *opairs,
                                       int *table,int size) {
  /* Never loop up prev field. */
  int ig,ii,jg,ji;
  int p,q;
  struct pair *ans;

  if (opairs == (struct pair *)NULL) return((struct pair *)NULL);
  ig = opairs->ig; ii = opairs->ii;
  jg = opairs->jg; ji = opairs->ji;
  for (p=0; p<size; p++) {
    if (table[4*p] == ig && table[4*p+1] == ii ) {
      for (q = 0; q<size; q++) {
        if (table[4*q] == jg && table[4*q+1] == ji) {
          ans = newPair(NULL);
          *ans = *opairs;
          ans->prev = NULL;
          ans->ig = table[4*p+2]; ans->ii = table[4*p+3];
          ans->jg = table[4*q+2]; ans->ji = table[4*q+3];
          ans->next = oldPairsToNewPairs(opairs->next,table,size);
          return(ans);
        }
      }
    }
  }
  return(oldPairsToNewPairs(opairs->next,table,size));
}
