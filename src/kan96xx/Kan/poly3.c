/* $OpenXM: OpenXM/src/kan96xx/Kan/poly3.c,v 1.8 2004/06/12 07:29:46 takayama Exp $ */
#include <stdio.h>
#include "datatype.h"
#include "extern2.h"

void outputTable(void);

int Homogenize = 1;
  
#define I(i,j) (i*N0+j)
#define LSIZE 5000
static int V[N0];  /* index of variables */
static struct coeff ** CList;
static int *EList;
static int *DList;
static int *MList;
static int *Mark;
static int Lsize;
static int Plist;
static int Maxv;
static POLY *RList;
static POLY *RListRoot;
static struct coeff *Tc = (struct coeff *)NULL;
/* It is initialized in mpMult_diff() */

void initT(void) {
  int i;
  Lsize = LSIZE;
  CList = (struct coeff **)sGC_malloc(sizeof(struct coeff *)*Lsize);
  EList = (int *)sGC_malloc(sizeof(int)*Lsize);
  Mark = (int *)sGC_malloc(sizeof(int)*Lsize);
  /* The following line causes the warning 'needed to allocate blacklisted..'
     DList = (int *)sGC_malloc(sizeof(int)*Lsize*N0);
     MList = (int *)sGC_malloc(sizeof(int)*Lsize*N0);
  */
  DList = (int *)sGC_malloc(sizeof(int)*Lsize*N0);
  MList = (int *)sGC_malloc(sizeof(int)*Lsize*N0);

  RList = (POLY *)sGC_malloc(sizeof(POLY)*Lsize);
  RListRoot = (POLY *)sGC_malloc(sizeof(POLY)*Lsize);
  for (i=0; i<Lsize; i++)
    RListRoot[i] = newCell((struct coeff *)NULL,(MONOMIAL)NULL);
}
  
void makeTable(c,e,ringp)
     struct coeff *c; /* read only */
     struct exps e[];
     struct ring *ringp;
{
  int i,j,k,p,q,deg,m,n;
  m = ringp->m; n = ringp->n;
  /* initialize */
  Maxv = 0; Plist = 1; deg = 0;
  for (i=m; i<n; i++) {
    if (e[i].D != 0) {
      V[Maxv] = i;
      DList[I(0,Maxv)] = e[i].D;
      MList[I(0,Maxv)] = 0;
      deg += e[i].D;
      Maxv++;
    }
  }
  CList[0] = coeffCopy(c);
  EList[0] = deg*2; Mark[0] = 0;

  for (i=0; i<Maxv; i++) {
    k = Plist;
    /* Copy j-th row to k-th row and modify it. */
    for (j=0; j<Plist; j++) {
      for (q=1; q<=DList[I(j,i)]; q++) {
        for (p=0; p<Maxv; p++) { /* copy */
          DList[I(k,p)] = DList[I(j,p)];
          MList[I(k,p)] = MList[I(j,p)];
        }
        /* modify */
        DList[I(k,i)] -= q;
        MList[I(k,i)] += q;

        CiiComb(Tc,DList[I(j,i)],q);
        /* Tc->val.bigp is read only. */
        CList[k] = coeffCopy(Tc);
        Cmult(CList[k],CList[k],CList[j]);
        /*CList[k] = normalize(CList[j]*BiiComb(DList[I(j,i)],q));*/
    
        EList[k] = EList[j]-2*q;
        Mark[k] = 0;
        k++;
        if (k>= Lsize) {
          errorPoly("makeTable(): Lsize is not large enough.\n");
        }
      }
    }
    Plist = k;
  }
}

void monomialMult_diff(e,f)
     struct exps e[];
     POLY f;
     /* (e) * f = [Plist] monomials  */
{

  int n,k,c,l,q,i,m, weightedHomogenization;
  struct coeff *a;
  struct monomial tmp;
  struct ring *ringp;
  POLY mm;

  tmp.ringp = ringp = f->m->ringp;
  n = ringp->n; c = ringp->c; l = ringp->l; m = ringp->m;
  weightedHomogenization = ringp->weightedHomogenization;
  for (k=Plist-1; k>=0; k--) {
    /* coeff */
    a = coeffCopy(CList[k]);
    Cmult(a,a,f->coeffp);
    if (isZero(a)) goto no;
    /* initialize tmp */
    for (i=0; i<n; i++) {
      tmp.e[i] = f->m->e[i];
    }
    if ((!weightedHomogenization) && Homogenize) {
      if (Homogenize == 3) {
        tmp.e[0].D += EList[k]/2; /* Double homogenization. */
        tmp.e[0].x += EList[k]/2; /* Dx x = x Dx + h H */
      }else{
        tmp.e[0].D += EList[k]; /* homogenization.
                                   e[0].D will be added later. */
      }
    }else if (weightedHomogenization && Homogenize) {
      tmp.e[0].D += EList[k]/2 ; /* homogenization.  Weight is (1,0) (special).
								  */
	}

    /* from m to n:  Differential variables. */
    for (i=0; i<Maxv; i++) {
      CiiPoch(Tc,tmp.e[V[i]].x,DList[I(k,i)]);
      /* Tc->val.bigp is read only */
      Cmult(a,a,Tc);
      /*printf("k=%d V[i]=%d a=%s\n",k,V[i],coeffToString(a));*/
      if (isZero(a)) goto no;
      tmp.e[V[i]].D += MList[I(k,i)]; /* monomial add */
      tmp.e[V[i]].x -= DList[I(k,i)]; /* differentiate */
    }


    /* difference variables are commutative */

    /* q-variables. Compute q before updating tmp.e[i]. */
    if (l-c > 0) {
      q =0;
      for (i=c; i<l; i++) {
        q += (e[i].D)*(tmp.e[i].x);  /* Don't repeat these things. */
        tmp.e[i].D += e[i].D;
      }
      /*printf("l=%d, q=%d\n",l,q);*/
      if (ringp->next == (struct ring *)NULL) {
        tmp.e[0].x += q;
      }else{
        Cmult(a,a,polyToCoeff(cxx(1,0,q,ringp->next),ringp));
        /* x[0]^q */
      }
    }
      
    /* Update tmp.e[i].x */
    for (i=0; i<n; i++) {
      tmp.e[i].x += e[i].x;
    }


    /* commutative variables */
    for (i=0; i<c; i++) {
      tmp.e[i].D += e[i].D;
    }
    /* Difference variables */
    for (i=l; i<m; i++) {
      tmp.e[i].D += e[i].D;
    }
    /***** x_i ----> x_i + e[i].D h  Substitution and homogenization */
    /*** They will be done in mpMult_diff() */

    mm = newCell(a,monomialCopy(&tmp));
    RList[k]->next = mm;
    RList[k] = RList[k]->next;
  no: ;
  }
}

/* Note that you cannot call mpMult_diff recursively. */
/* Note also that mpMult_diff assumes coefficients and Dx commutes each other*/
POLY mpMult_diff(POLY f,POLY g)
{
  int k;
  POLY r,temp;

  /* printf("mpMult_diff(%s,%s)\n",POLYToString(f,'*',1),POLYToString(g,'*',1)); */
  
  if (f == POLYNULL || g == POLYNULL) return(POLYNULL);
  checkRing(f,g);
  Tc = coeffCopy(f->coeffp);

  if (isConstant(f)) return(cpMult(f->coeffp,g));
  
  makeTable(f->coeffp,f->m->e,f->m->ringp);
  /* outputTable(); */
  for (k=0; k<Plist; k++) {
    RList[k] = RListRoot[k];
    RList[k]->next = POLYNULL;
  }

  while (g != POLYNULL) {
    monomialMult_diff(f->m->e,g);
    g = g->next;
  }
  r = POLYNULL;
  for (k=0; k<Plist; k++) {
    temp = RListRoot[k]->next;
    r = ppAddv(r,temp);
  }

  
  /***** x_i ----> x_i + e[i].D h  Substitution and homogenization
         for difference variables */
  /*** They are implemented in _difference, but slow.*/

  return(r);
}

POLY mpMult_difference_org(POLY f,POLY g)
{
  POLY r;
  int m,l;
  POLY lRule[N0];
  POLY rRule[N0];
  int size;
  int i;
  
  if (f == POLYNULL || g == POLYNULL) return(POLYNULL);
  checkRing(f,g);
  m = f->m->ringp->m;
  l = f->m->ringp->l;
  
  r = mpMult_diff(f,g);

  /***** x_i ----> x_i + e[i].D h  Substitution and homogenization
         for difference variables */
  size = 0;
  for (i=l; i<m; i++) {
    if (f->m->e[i].D) {
      lRule[size] = cxx(1,i,1,f->m->ringp);
      if (Homogenize) {
        rRule[size] = ppAdd(cxx(1,i,1,f->m->ringp),cdd(f->m->e[i].D,0,1,f->m->ringp));
        /* x_i               + e[i].D  h */
      }else{
        rRule[size] = ppAdd(cxx(1,i,1,f->m->ringp),cdd(f->m->e[i].D,0,0,f->m->ringp));
        /* x_i               + e[i].D   */
      } 
      size++;
    }
  }

  /* It's a dirty trick. */
  r = replace_poly(r,lRule,rRule,size);

  return(r);
}

void outputTable() {
  int i,j;
  printf("Maxv = %d Plist=%d\n",Maxv,Plist);
  for (i=0; i<Maxv; i++) printf("%5d",V[i]);
  printf("\n------ DList --------------\n");
  for (i=0; i<Plist; i++) {
    for (j=0; j<Maxv; j++) {
      printf("%5d",DList[I(i,j)]);
    }
    putchar('\n');
  }
  printf("\n--------- MList ------------\n");
  for (i=0; i<Plist; i++) {
    for (j=0; j<Maxv; j++) {
      printf("%5d",MList[I(i,j)]);
    }
    printf(" |  e=%5d M=%1d c=%s\n",EList[i],Mark[i],coeffToString(CList[i]));
  }
}
