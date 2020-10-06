/* $OpenXM: OpenXM/src/kan96xx/Kan/hilbert.c,v 1.4 2005/06/16 05:07:23 takayama Exp $ */
/*   hilbert.c
     1992/06/16
     1992/06/18
     1993/04/28
     \bibitem{Cocoa-Hilbert} 
     Bigatti, A., Caboara, M., Robianno, L. (1991):
     On the computation of Hilbert Poincar\'e series.
     Applicable algebra in Engineering, Communication and Computing
     {\bf 2}, 21--33.
     1998/10/31
*/
#include <stdio.h>
#include "datatype.h"
#include "stackm.h"
#include "extern.h"
#include "extern2.h"

void warningHilbert(char str[]);
void errorHilbert(char str[]);

struct arrayOfPOLYold {
  int n;
  POLY *array;
};

static int M;
static int MM;
static int NN;
static int N;

static struct ring *hringp;

#define max(a,b) ((a)>(b)? a: b)

POLY hilbert2(POLY k,int p,POLY pArray[]);
static POLY xx(int i,int k);
static void pWriteln(POLY f)
{ printf("%s\n",POLYToString(f,' ',0)); fflush(stdout);}

struct object hilberto(struct object obgb,struct object obv)
{
  int m; /* size of the base. */
  int n; /* number of variables */
  int i,j,k;
  int n0; /* number of the variables for the polynomials in base. */
  struct object obf = OINIT;
  struct ring *rp;
  struct ring *rr = (struct ring *)NULL;
  POLY *base;
  int *x;
  int *d;
  int debug = 0;
  POLY f;
  POLY g;
  struct object rob = OINIT;
  struct object ccc = OINIT;
  extern struct ring SmallRing;
  int worg;
  extern int WarningNoVectorVariable;

  rob = NullObject;
  
  if (obgb.tag != Sarray) {
    errorKan1("%s\n","Khilbert(): The first argument must be array of poly.");
  }
  if (obv.tag != Sarray) {
    errorKan1("%s\n","Khilbert(): The second argument must be array of variables.");
  }
  m = getoaSize(obgb);
  n = getoaSize(obv);
  if (n <= 0) errorKan1("%s\n","Khilbert(): The number of variables must be more than 0.");
  if (m <= 0) errorKan1("%s\n","Khilbert(): The number of basis must be more than 0.");
  for (i=0; i<m; i++) {
    obf = getoa(obgb,i);
    if (obf.tag != Spoly) {
      errorKan1("%s\n","Khilbert(): The element of the first array must be a polynomials.\n");
    }
    if (KopPOLY(obf) ISZERO) {
      errorKan1("%s\n","Khilbert(): The element of the first array must not be the zero.\n");
    }
    f = KopPOLY(obf);
    if (rr != (struct ring*)NULL) {
      if (rr != f->m->ringp) {
        errorKan1("%s\n","Hhilbert(): the element must belong to the same ring.");
      }
    }else{
      rr =  f->m->ringp;
    }
  }
  for (i=0; i<n; i++) {
    obf = getoa(obv,i);
    if (obf.tag != Spoly) {
      errorKan1("%s\n","Khilbert(): The element of the second array must be a polynomials.\n");
    }
    if (KopPOLY(obf) ISZERO) {
      errorKan1("%s\n","Khilbert(): The element of the second array must not be the zero.\n");
    }
    f = KopPOLY(obf);
    if (rr != (struct ring*)NULL) {
      if (rr != f->m->ringp) {
        errorKan1("%s\n","Hhilbert(): the element must belong to the same ring.");
      }
    }else{
      rr =  f->m->ringp;
    }
  }

  worg =   WarningNoVectorVariable;
  WarningNoVectorVariable = 0;
  rp = KopRingp(KdefaultPolyRing(KpoInteger(n)));
  WarningNoVectorVariable = worg;

  hringp = rp;
  MM=0; M=0; NN=n; N = n;

  base = (POLY *)sGC_malloc(sizeof(POLY)*(m+1));
  if (base == (POLY *)NULL) errorKan1("%s\n","no more memory.");
  obf = getoa(obgb,0);
  f = KopPOLY(obf);
  n0 = f->m->ringp->n;
  x = (int *)sGC_malloc(sizeof(int)*n0);
  d = (int *)sGC_malloc(sizeof(int)*n0);
  if (d == (int *)NULL) errorKan1("%s\n","no more memory.");

  for (i=0; i<n0; i++) {x[i] = d[i] = -1;}
  for (i=0; i<n; i++) {
    obf = getoa(obv,i);
    f = KopPOLY(obf);
    for (j=0; j<n0; j++) {
      if ((f->m->e)[j].x) {
        x[j] = 1; break;
      }
      if ((f->m->e)[j].D) {
        d[j] = 1; break;
      }
    }
  }

  j = 0;
  for (i=0; i<n0; i++) {
    if (x[i] != -1) x[i] = j++;
  }
  for (i=0; i<n0; i++) {
    if (d[i] != -1) d[i] = j++;
  }

  if (debug) {
    for (i=0; i<n0; i++) {
      printf("x[%d]=%d ",i,x[i]);
    }
    for (i=0; i<n0; i++) {
      printf("d[%d]=%d ",i,d[i]);
    }
    printf("\n");
  }

  for (i=0; i<m; i++) {
    obf = getoa(obgb,i);
    f = KopPOLY(obf);
    g = cxx(1,0,0,rp);
    for (j=0; j<n0; j++) {
      if ((f->m->e)[j].x) {
        if (x[j] != -1) {
          (g->m->e)[x[j]].D = (f->m->e)[j].x;
        }
      }
      if ((f->m->e)[j].D) {
        if (d[j] != -1) {
          (g->m->e)[d[j]].D = (f->m->e)[j].D;
        }
      }
    }
    base[i] = g;
  }

  if (debug) {
    for (i=0; i<m; i++) {
      printf("%s\n",POLYToString(base[i],'*',1));
    }
  }

  rob = newObjectArray(2);
  ccc.tag = SuniversalNumber;
  ccc.lc.universalNumber = intToCoeff(NN-M,&SmallRing);
  putoa(rob,0,ccc);
  putoa(rob,1,KpoPOLY(hilbert2(cdd(1,0,1,hringp),m,base)));
  return(rob);
}

/*      
static POLY getArrayOfPOLYold(struct arrayOfPOLYold set,int i) {
  if (i < 0 || i >= set.n) {
    errorKan1("%s\n","getArrayOfPOLYold(): index is out of bound.");
  }
  return((set.array)[i]);
}
*/
#define getArrayOfPOLYold(set,i) ((set).array)[i]

static struct arrayOfPOLYold newArrayOfPOLYold(int size) {
  static struct arrayOfPOLYold a;
  if (size <= 0) {
    errorKan1("%s\n","newArrayOfPOLYold(): the size must be more than 0.");
  }
  a.n = size;
  a.array = (POLY *)sGC_malloc(sizeof(POLY)*size);
  if (a.array == (POLY *)NULL) errorKan1("%s\n","no more memory.");
  return(a);
}

static POLY iToPoly(int n) {
  return(cxx(n,0,0,hringp));
}
static POLY xx(int i,int k) {
  return(cxx(1,i,k,hringp));
}

static int polyToInt(POLY f) {
  if (f ISZERO) return(0);
  return(coeffToInt(f->coeffp));
}


static void shell(v,n)
     int v[];
     int n;
{
  int gap,i,j,temp;
  
  for (gap = n/2; gap > 0; gap /= 2) {
    for (i = gap; i<n; i++) {
      for (j=i-gap ; j>=0 && v[j]>v[j+gap]; j -= gap) {
        temp = v[j];
        v[j] = v[j+gap];
        v[j+gap] = temp;
      }
    }
  }
}

static POLY ppifac(f,n)
     POLY f;
     int n;
     /*  ppifac returns (f+n) (f+n-1) ... (f+1). */
{
  POLY r;
  int i;
  if (n < 0) {
    warningHilbert(" ppifac() is called with negative argument.");
    return(POLYNULL);
  }else if (n==0) {
    return(pcmCopy(f));
  }else {
    r = iToPoly(1);
    for (i=1; i<=n; i++)  {
      r = ppMult( ppAdd(f,iToPoly(i)), r);
    }
    return(r);
  }
}


static int isReducibleD1(f,g)
     POLY f;
     POLY g;
{
  int i;
  for (i = M; i < NN; i++) {
    if (((f->m->e)[i].x >= (g->m->e)[i].x) &&
        ((f->m->e)[i].D >= (g->m->e)[i].D)) {
      /* do nothing */
    } else {
      return(0);
    }
  }
  return(1);
}


static struct arrayOfPOLYold
reduceMonomials(set)
     struct arrayOfPOLYold set;
{
  int *drop;  /* 1--> yes. drop set[i]. 0--> no. */
  int i,j;
  int size;
  int newsize;
  struct arrayOfPOLYold ans;
  size = set.n; /* get the size of set */
  
  drop = (int *)sGC_malloc(sizeof(int)*(size+1)); 
  if (drop == (int *)NULL) errorHilbert("No more memory.");
  for (i=0; i < size; i++) {
    drop[i]=0;
  }
  /* O(n^2) algorithm to reduced basis */
  for (i = 0; i < size; i++) {
    if (!drop[i]) {
      for (j=i+1; j < size; j++) {
        if (!drop[j]) {
          if (isReducibleD1(getArrayOfPOLYold(set,i),
                            getArrayOfPOLYold(set,j))) {
            drop[i] = 1; break;
          }else if (isReducibleD1(getArrayOfPOLYold(set,j),
                                  getArrayOfPOLYold(set,i))) {
            drop[j] = 1; break;
          }else { }
        }
      }
    }
  }
  newsize = 0;
  for (i = 0; i < size; i++) {
    if (!drop[i]) ++newsize;
  }
  ans = newArrayOfPOLYold(newsize);
  j = 0;


  for (i = 0; i < size; i++) {


    if (!drop[i]) {
      getArrayOfPOLYold(ans,j) = pcmCopy(getArrayOfPOLYold(set,i));
      j++;
    }
  }
  return(ans);
}
  
      
static int tryDecompose(set,i,j,vA,vWhich)
     struct arrayOfPOLYold set;    /* input: monomials */
     int     i,j;               /* decompose with respect to the (i,j)-th
                                   variable: i=0-->x_j, i=1--->D_j */
     int vA[];                  /* Return value: vA[0] is a_0, ... */
     int vWhich[];              /* Return value: vWhich[i] is <<a>> of set[i] */
     /* set ---> x_j^(a_0) I_{a_0} + .... + x_j^{a_{p-1}} I_{a_{p-1}} */
     /* return value is p */
     /* tryDecompose is used to find the best decomposition. */
{
  int size,k,ell,ell2;
  POLY f;
  int p;
  int *tmp;
  
  size = set.n;
  if ( i == 0) { /* focus on x */
    for (k = 0; k < size; k++) {
      f = getArrayOfPOLYold(set,k);
      vWhich[k] = (f->m->e)[j].x;
    }
  }else{  /* focus on D */
    for (k = 0; k < size; k++) {
      f = getArrayOfPOLYold(set,k);
      vWhich[k] = (f->m->e)[j].D;
    }
  }
#ifdef DEBUG3
  /*for (i=0; i<size; i++) printf("%3d", vWhich[i]);
    printf("     vWhich\n");*/
#endif
  /* sort and prune */
  tmp = (int *)sGC_malloc(sizeof(int)*((size+1)*2));
  if (tmp == (int *) NULL) errorHilbert("No more memory.");
  for (i=0; i<size; i++) tmp[i] = vWhich[i];
  shell(tmp,size);
  /* prune */
  p = 0; vA[p] = tmp[0]; p++;
  for (i=1; i<size; i++) {
    if (vA[p-1] != tmp[i]) vA[p++] = tmp[i];
  }

#ifdef DEBUG3  
  /*for (i=0; i<p; i++) printf("%3d", vA[i]);
    printf("---vA\n");*/
#endif
  return(p);
}

static struct arrayOfPOLYold getJA(a,set,vWhich,ja,ii,xd,ith)
     /* get J_{a_{i+1}} from J_{a_i}
   J_{a_{i+1}} = J_{a_i} \cup I_{a_{i+1}}
*/
     int a;                  /* each a_i */
struct arrayOfPOLYold set; /* polynomials */
int *vWhich;            /* vWhich[0] is exponent a of set[0], .... */
struct arrayOfPOLYold ja;  /* J_i */
int ii;                 /* ii is i */
int xd;                 /* xd == 0 --> x, xd == 1 --> D */
int ith;                /* x_{ith} or D_{ith} is the best variable */
{
  int size;
  struct arrayOfPOLYold input;
  POLY f;
  int start,i;

#ifdef DEBUG3
  printf("var is %d ",a);
  printf("set is "); outputarrayOfPOLYold(set);
#endif
  
  size = set.n;
  start = 0;
  /*  input = newArrayOfPOLYold(size); */
  input = newArrayOfPOLYold(size+ja.n+1);  /* 1993/09/08 */


  if (ii != 0) {
    for (i = 0; i < ja.n ; i++) {
      getArrayOfPOLYold(input,start) = getArrayOfPOLYold(ja,i);
      start++;
    }
  }
  for (i = 0; i < size; i++) {
    if (vWhich[i] == a) {
      f = pcmCopy(getArrayOfPOLYold(set,i));
      if (xd == 0) {
        (f->m->e)[ith].x = 0;
      }else{
        (f->m->e)[ith].D = 0;
      }
      getArrayOfPOLYold(input,start) = f;
      start++ ;
    }
  }
  input.n = start; /* This is not good code. */
#ifdef DEBUG2
  for (i=0; i<start; i++) {checkh(input,i);} /****/
#endif
#ifdef DEBUG3
  printf("input is "); outputarrayOfPOLYold(input); 
#endif
  input= reduceMonomials(input);
#ifdef DEBUG3
  /*printf("input is "); outputarrayOfPOLYold(input);*/
#endif
  return( input );
}
    

static struct arrayOfPOLYold *getDecomposition(set,activeX,activeD)
     struct arrayOfPOLYold set;
     int activeX[N0];
     int activeD[N0];
{
  int i;
  int size;
  int p;
  int *vA,*vWhich;
  int xmax = 0;
  int dmax = 0;
  int xi,di;
  int resultsize;
  struct arrayOfPOLYold ja;
  struct arrayOfPOLYold *ans;
  
  size = set.n;
  vA = (int *)sGC_malloc(sizeof(int)*(size+1));
  vWhich = (int *)sGC_malloc(sizeof(int)*(size+1));
  if (vA == (int *)NULL || vWhich == (int *)NULL) errorHilbert("No more memory.\n");
  /* find the best decomposition */
  for ( i = M; i < NN; i++) {
    if (activeX[i]) {
      p = tryDecompose(set,0,i,vA,vWhich);
      if (p > xmax) {
        xmax = p;
        xi = i;
        if (xmax == size) {
          break;
        }
      }
    }
  }
  if (xmax != size) {
    for ( i = M; i < NN; i++) {
      if (activeD[i]) {
        p = tryDecompose(set,1,i,vA,vWhich);
        if (p > dmax) {
          dmax = p;
          di = i;
          if (dmax == size) {
            break;
          }
        }
      }
    }
  }
  /*
    ans[0] = (a_0,...,a_{p-1})
    ans[1] = generators of J_{a_0}
    ....
    ans[p] = generators of J_{a_{p-1}},   p = xmax or dmax
  */
  if (xmax > dmax) {
    tryDecompose(set,0,xi,vA,vWhich); /* xi is the best variable */
    ans = (struct arrayOfPOLYold *)sGC_malloc(sizeof(struct arrayOfPOLYold)*(xmax+1));
    if (ans == (struct arrayOfPOLYold *)NULL) errorHilbert("No more memory.");
    ans[0] = newArrayOfPOLYold(xmax);
    for (i = 0; i < xmax; i++) {
      getArrayOfPOLYold(ans[0],i) = iToPoly(vA[i]);
    }
    /* compute J for for each a_i */
    for (i = 0; i < xmax; i++) {
      ans[i+1] = getJA(vA[i],set,vWhich,ans[i],i,0,xi);
    }
    return(ans);
  }else {
    tryDecompose(set,1,di,vA,vWhich); /* di is the best variable */
    ans = (struct arrayOfPOLYold *)sGC_malloc(sizeof(struct arrayOfPOLYold)*(dmax+1));
    if (ans == (struct arrayOfPOLYold *)NULL) errorHilbert("No more memory.");
    ans[0] = newArrayOfPOLYold(dmax);
    for (i = 0; i < dmax; i++) {
      getArrayOfPOLYold((ans[0]),i) = iToPoly(vA[i]);
    }
    /* compute J for for each a_i */
    for (i = 0; i < dmax; i++) {
      ans[i+1] = getJA(vA[i],set,vWhich,ans[i],i,1,di);
    }
    return(ans);
  }
}
  
static POLY hilbert1T(set)
     struct arrayOfPOLYold set;
     /* <<set>> must be reduced basis and each polynomial must have the length 1 */
     /* Unnecessary exponents should be set to zero. For example, f = x_{M-1} x_M
   is illegal input. It should be x_M ( M <= index <= NN ).
   cf. hilbert1() and hilbert2() 
*/
/* Example: hilbert1T of x^a is
   x0^0 - x0^(-a) <=== {{\ell+n} \choose n} - {{\ell+n-a} \choose n}
*/
{
  int activeX[N0];
  int activeD[N0];
  int size;
  int i,j;
  POLY f;
  int active = 0;
  int xxdd,xi,di,a;
  struct arrayOfPOLYold *ja;
  struct arrayOfPOLYold hja;
  int p;
  POLY ansp;

#ifdef DEBUG  
  static int cc = 0;
  /* debugging */ /***/
  printf("hilbert1T: size of set is %d\n",set.n);
  for (i=0; i<set.n; i++) {
    printf("%s\n", POLYToString(getArrayOfPOLYold(set,i),' ',0));
  }
  printf("^^%d--------------------------------------------\n",cc++);
#endif
  
  size = set.n;
  if (size < 1) {
#ifdef DEBUG
    cc--; printf("<<%d>> value is 1.\n",cc); /***/
#endif
    return(iToPoly(1));
  }
  for (i = 0; i < N; i++) {
    activeX[i] = activeD[i] = 0;
  }
  for (i = 0; i < size; i++) {
    f = getArrayOfPOLYold(set,i);
    for (j = M; j < NN; j++) {
      if ((f->m->e)[j].x) {
        activeX[j] = 1;
      }
      if ((f->m->e)[j].D) {
        activeD[j] = 1;
      }
    }
  }
  for (i = M; i < NN; i++) {
    if (activeX[i]) {
      if (active) {
        active = 2;
        break;
      }
      active = 1;
      xxdd = 0; xi = i;
    }
    if (activeD[i]) {
      if (active) {
        active = 2;
        break;
      }
      active = 1;
      xxdd = 1; di = i;
    }
  }
  if (active == 0) {
#ifdef DEBUG    
    cc--; printf("<<%d>> value is 0.\n",cc); /***/
#endif
    return(POLYNULL);
  }else if (active == 1) {
    f = getArrayOfPOLYold(set,0);
    if (xxdd == 0) {
      a = (f->m->e)[xi].x;
#ifdef BEBUG
      cc--; printf("<<%d>> 1-x0^(%d).\n",cc,a); /***/
#endif
      return(ppSub(iToPoly(1),xx(0,-a)));
    }else {
      a = (f->m->e)[di].D;
#ifdef DEBUG
      cc--; printf("<<%d>> 1-x0^(%d).\n",cc,a); /***/
#endif
      return(ppSub(iToPoly(1),xx(0,-a)));
    }
  }

  /* compute <J_{a_0}>, ..., <J_{a_{p-1}}> */
  ja = getDecomposition(set,activeX,activeD);
  p = (ja[0]).n;
#ifdef DEBUG3
  outputDecomposition(p,activeX,activeD,ja);
#endif  
  hja = newArrayOfPOLYold(p);
  for (i=0; i<p; i++) {
    getArrayOfPOLYold(hja,i) = hilbert1T(ja[i+1]);
  }

  a = polyToInt(getArrayOfPOLYold(ja[0],0));
  ansp = ppSub(iToPoly(1),xx(0,-a));
  f = ppMult(getArrayOfPOLYold(hja,0),xx(0,-a));
  ansp = ppAdd(ansp,f);
  for (i=1; i<p; i++) {
    f = ppSub(getArrayOfPOLYold(hja,i),getArrayOfPOLYold(hja,i-1));
    a = polyToInt(getArrayOfPOLYold(ja[0],i));
    f = ppMult(f,xx(0,-a));
    ansp = ppAdd(ansp,f);
  }

#ifdef DEBUG
  printf("<<%d>>",--cc); pWriteln(ansp);/***/
#endif

  return(ansp);
}  

       
POLY hilbert2(k,p,pArray)
     POLY k; 
     int p;
     POLY pArray[];
     /* This returns n! H(k,p,a^0, ..., a^{p-1}) */
     /* n = (NN-M); */
     /* Expample: hilbert2(xx(0,1),3,...) */
{
  struct arrayOfPOLYold set;
  int i,j;
  POLY f;
  POLY ans;
  POLY c;
  POLY g;
  int n;
  int size,a;
  
  set = newArrayOfPOLYold(p);
  for (i=0; i<p; i++) {
    if (pArray[i] ISZERO) {
      warningHilbert("Argument of hilbert1 contains 0.\n");
      return(POLYNULL);
    }
    f = pcmCopy(pArray[i]);  /* pArray is already monomial poly */
    /* This is for hilbert2 */
    for (j = 0; j < MM; j++) {
      (f->m->e)[j].x = 0; (f->m->e)[j].D = 0;
    }
    for (j = M; j < NN; j++) {
      (f->m->e)[j].x = 0;
    }
    getArrayOfPOLYold(set,i) = f;
  }
  set = reduceMonomials(set);
  f = hilbert1T(set);  /* c x0^a ---> c {{k+n+a} \choose n} */

  n = NN-M;
  ans = POLYNULL;
  while (!(f ISZERO)) {
    a = (f->m->e)[0].x;
    c = newCell(f->coeffp,f->m);     c = pcmCopy(c);
    (c->m->e)[0].x = 0;
    g = ppifac(ppAdd(k,iToPoly(a)),n);
    ans = ppAdd(ans, ppMult(c,g));
    f = f->next;
  }
  return(ans);
  
}
  
       


#ifdef DEBUG2
checkh(set,i)
     struct arrayOfPOLYold set;
     int i;
{
  if (pLength(getArrayOfPOLYold(set,i)) != 1) {
    printf("Size is %d.",pSize(getArrayOfPOLYold(set,i)));
    getchar(); getchar(); getchar(); getchar();
    getchar();getchar();
  }
}
#endif

#ifdef DEBUG3
outputDecomposition(p,activeX,activeD,ja)
     int p;
     int activeX[];
     int activeD[];
     struct arrayOfPOLYold ja[];
{
  int i;
  printf("\nActiveX: ");
  for (i=0; i<N; i++) {
    printf("%3d",activeX[i]);
  }
  printf("\nActiveD: ");
  for (i=0; i<N; i++) {
    printf("%3d",activeD[i]);
  }
  printf("                  Decomposed into J_0 to J_%d\n",p-1);
  if (1) {
    printf("Exponents:  ");
    for (i=0; i<p; i++) {
      printf("%s, ",POLYToString((ja[0]).array[i],' ',0));
    }
    printf("\n");
    for (i=0; i<p; i++) {
      outputarrayOfPOLYold(ja[i+1]);
      printf("\n");
    }
  }
}

outputarrayOfPOLYold(set)
     struct arrayOfPOLYold set;
{
  int i;
  for (i=0; i< set.n ; i++) {
    printf("%s, ",POLYToString(set.array[i],' ',0));
  }
}
#endif
  
  
void warningHilbert(char str[])
{
  fprintf(stderr,"Warning (hilbert.c): %s\n",str);
}

void errorHilbert(char str[])
{
  errorKan1("%s\n",str);
}


