/* $OpenXM: OpenXM/src/kan96xx/Kan/poly3a.c,v 1.1 2002/02/04 07:58:28 takayama Exp $ */
#include <stdio.h>
#include "datatype.h"
#include "extern2.h"

extern int Homogenize;

#define DIFFERENTIAL 0
#define DIFFERENCE 1  
#define I(i,j) (i*N0+j)
#define LSIZE 5000
#define MULTTABLE_MAX 10
struct multTable {
  int V[N0];  /* index of variables */
  int Vtype[N0];
  struct coeff ** CList; /* binomial coeff */
  int *EList;  /* for homogenization */
  int *DList;
  int *MList;
  int *Mark;
  int Lsize;
  int Plist;
  int Maxv;
  POLY *RList;
  POLY *RListRoot;
  struct coeff *Tc;
  int myplace;
};
static struct multTable * MultTable[MULTTABLE_MAX];
static int MultTableMark[MULTTABLE_MAX];
static struct multTable *newMultTable(struct coeff *cf);
static void freeMultTable(struct multTable *mp);
static struct multTable *initMT(int place);
static void makeTable_differential_difference(struct coeff *c, struct exps e[],
  struct ring *ringp, struct multTable *mt);
static void monomialMult_differential_difference(struct exps e[],
                                                 POLY f, struct multTable *mt);
static void outputMultTable(struct multTable *mt);


POLY muMult_differential_difference(POLY f,POLY g);

/*
  These local variables should be set and restored from and to multTable.
*/
#define mt_declare_locals() \
  int *V; int *Vtype; int Plist; int Maxv;  \
  struct coeff ** CList; \
  int *EList; int *DList; int *MList; int *Mark; int Lsize; \
  POLY *RList; POLY *RListRoot; struct coeff *Tc;

#define mt_set_mt(mt) \
 {mt->EList = EList; \
  mt->DList = DList; \
  mt->MList = MList; \
  mt->Mark = Mark; \
  mt->Lsize = Lsize; \
  mt->RList = RList; \
  mt->RListRoot = RListRoot; \
  mt->Tc = Tc; \
  mt->Plist = Plist; \
  mt->Maxv = Maxv; \
  mt->CList = CList; \
 }

#define mt_get_mt(mt) \
 {EList = mt->EList; \
  DList = mt->DList; \
  MList = mt->MList; \
  Mark = mt->Mark; \
  Lsize = mt->Lsize; \
  RList = mt->RList; \
  RListRoot = mt->RListRoot; \
  Tc = mt->Tc; \
  Plist = mt->Plist; \
  Maxv = mt->Maxv; \
  V = mt->V; \
  Vtype = mt->Vtype; \
  CList = mt->CList; \
 }



static struct multTable *initMT(int place) {
  int i;
  mt_declare_locals()  
  struct multTable *mt;
  
  Lsize = LSIZE;
  CList = (struct coeff **)sGC_malloc(sizeof(struct coeff *)*Lsize);
  EList = (int *)sGC_malloc(sizeof(int)*Lsize);
  Mark = (int *)sGC_malloc(sizeof(int)*Lsize);
  DList = (int *)sGC_malloc(sizeof(int)*Lsize*N0);
  MList = (int *)sGC_malloc(sizeof(int)*Lsize*N0);
  RList = (POLY *)sGC_malloc(sizeof(POLY)*Lsize);
  RListRoot = (POLY *)sGC_malloc(sizeof(POLY)*Lsize);
  for (i=0; i<Lsize; i++) 
    RListRoot[i] = newCell((struct coeff *)NULL,(MONOMIAL)NULL);
  Tc = NULL;

  mt = (struct multTable *)sGC_malloc(sizeof(struct multTable));
  mt_set_mt(mt);
  mt->myplace = place;
  return mt;
}

static struct multTable *newMultTable(struct coeff *cf) {
  static int p = -1;
  int i;
  if (p+1 >= MULTTABLE_MAX-1) {
    errorPoly("newMultTable(): MULTTABLE_MAX is not large enough.\n");
  }
  if (p<0) {
    p = 0;
    MultTable[p] = initMT(p); MultTableMark[p] = 1;
    MultTable[p]->Tc = coeffCopy(cf);
    p++; return( MultTable[p-1] );
  }
  for (i=0; i<p; i++) {
    if (! MultTableMark[i]) {
      MultTable[i]->Tc = coeffCopy(cf);
      return( MultTable[i] );
    }
  }
  MultTable[p] = initMT(p); MultTableMark[p] = 1;
  MultTable[p]->Tc = coeffCopy(cf);
  p++; return( MultTable[p-1] );
}
static void freeMultTable(struct multTable *mp) {
  MultTableMark[mp->myplace] = 0;
}

  
void makeTable_differential_difference(c,e,ringp,mt)
     struct coeff *c; /* read only */
     struct exps e[];
     struct ring *ringp;
     struct multTable *mt;
{
  int i,j,k,p,q,deg,m,n,l,t;
  mt_declare_locals() 

  mt_get_mt(mt);
  m = ringp->m; n = ringp->n; l = ringp->l;
  /* initialize */
  Maxv = 0; Plist = 1; deg = 0;

  for (i=l; i<m; i++) {
    if (e[i].D != 0) {
      V[Maxv] = i;
      Vtype[Maxv] = DIFFERENCE;
      DList[I(0,Maxv)] = e[i].D;
      MList[I(0,Maxv)] = 0;
      deg += e[i].D;
      Maxv++;
    }
  }
  for (i=m; i<n; i++) {
    if (e[i].D != 0) {
      V[Maxv] = i;
      Vtype[Maxv] = DIFFERENTIAL;
      DList[I(0,Maxv)] = e[i].D;
      MList[I(0,Maxv)] = 0;
      deg += e[i].D;
      Maxv++;
    }
  }
  CList[0] = coeffCopy(c);
  Mark[0] = 0;

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

        Mark[k] = 0;
        k++;
        if (k>= Lsize) {
          errorPoly("makeTable_differential_difference(): Lsize is not large enough.\n");
        }
      }
    }
    Plist = k;
  }

  if (Homogenize == 1) {
    for (i=0; i<Plist; i++) {
      t = deg;
      for (j=0; j<Maxv; j++) {
	t -= MList[I(i,j)];
      }
      for (j=0; j<Maxv; j++) {
	if (Vtype[j] == DIFFERENTIAL) {
          t += DList[I(i,j)];
	}
      }
      EList[i] = t;
    }
  }else if (Homogenize == 2) { /* Dx x = x Dx + h */
    for (i=0; i<Plist; i++) {
      t = deg;
      for (j=0; j<Maxv; j++) {
	t -= MList[I(i,j)];
      }
      EList[i] = t;
    }
  }

  
  mt_set_mt(mt);

}


void monomialMult_differential_difference(e,f,mt)
     struct exps e[];
     POLY f;
     /* (e) * f = [Plist] monomials  */
     struct multTable *mt;
{

  int n,k,c,l,q,i,m;
  struct coeff *a;
  struct monomial tmp;
  struct ring *ringp;
  POLY mm;
  mt_declare_locals()

  mt_get_mt(mt);

  tmp.ringp = ringp = f->m->ringp;
  n = ringp->n; c = ringp->c; l = ringp->l; m = ringp->m;
  for (k=Plist-1; k>=0; k--) {
    /* coeff */
    a = coeffCopy(CList[k]);
    Cmult(a,a,f->coeffp);
    if (isZero(a)) goto no;
    /* initialize tmp */
    for (i=0; i<n; i++) {
      tmp.e[i] = f->m->e[i];
    }
    if (Homogenize) {
      tmp.e[0].D += EList[k]; /* homogenization.
                                 e[0].D will be added later. */
    }

    /* from m to n:  Differential and difference variables. */
    for (i=0; i<Maxv; i++) {
      if (Vtype[i] == DIFFERENTIAL) {
        CiiPoch(Tc,tmp.e[V[i]].x,DList[I(k,i)]);
        /* Tc->val.bigp is read only */
        Cmult(a,a,Tc);
        /*printf("k=%d V[i]=%d a=%s\n",k,V[i],coeffToString(a));*/
        if (isZero(a)) goto no;
        tmp.e[V[i]].D += MList[I(k,i)]; /* monomial add for D. */
        tmp.e[V[i]].x -= DList[I(k,i)]; /* differentiate */
      }else{
        CiiPower(Tc,tmp.e[V[i]].x,DList[I(k,i)]);
        /* Tc->val.bigp is read only */
        Cmult(a,a,Tc);
        /*printf("k=%d V[i]=%d a=%s\n",k,V[i],coeffToString(a));*/
        if (isZero(a)) goto no;
        tmp.e[V[i]].D += MList[I(k,i)]; /* monomial add for D. */
      }
    }


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

    mm = newCell(a,monomialCopy(&tmp));
    RList[k]->next = mm;
    RList[k] = RList[k]->next;
  no: ;
  }
}

/* Note also that mpMult_diff assumes coefficients and Dx commutes each other*/
POLY mpMult_difference(POLY f,POLY g)
{
  int k;
  POLY r,temp;
  mt_declare_locals()
  struct multTable *mt;

  /* printf("mpMult_difference(%s,%s)\n",POLYToString(f,'*',1),POLYToString(g,'*',1)); */
  
  if (f == POLYNULL || g == POLYNULL) return(POLYNULL);
  checkRing(f,g);


  if (isConstant(f)) return(cpMult(f->coeffp,g));
  
  mt = newMultTable(f->coeffp);
  makeTable_differential_difference(f->coeffp,f->m->e,f->m->ringp,mt);
  mt_get_mt(mt);
  /* outputMultTable(mt);  */

  for (k=0; k<Plist; k++) {
    RList[k] = RListRoot[k];
    RList[k]->next = POLYNULL;
  }

  while (g != POLYNULL) {
    monomialMult_differential_difference(f->m->e,g,mt);
    g = g->next; 
  }
  r = POLYNULL;
  for (k=0; k<Plist; k++) {
    temp = RListRoot[k]->next;
    r = ppAddv(r,temp);
  }

  freeMultTable(mt);
  return(r);
}


static void outputMultTable(struct multTable *mt) {
  int i,j;
  mt_declare_locals()

  mt_get_mt(mt);
  printf("Maxv = %d Plist=%d",Maxv,Plist);
  printf("\n------- V     ----------------\n");
  for (i=0; i<Maxv; i++) printf("%5d",V[i]);
  printf("\n------- Vtype ----------------\n");
  for (i=0; i<Maxv; i++) printf("%5d",Vtype[i]);
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
