/* $OpenXM: OpenXM/src/kan96xx/Kan/coeff.c,v 1.2 2000/01/16 07:55:38 takayama Exp $ */
#include <stdio.h>
#include "datatype.h"
#include "stackm.h"
#include "extern.h"
#include "extern2.h"


static void printTag(coeffType t)
{
  switch(t) {
  case UNKNOWN: fprintf(stderr," UNKNOWN "); break;
  case INTEGER: fprintf(stderr," INTEGER "); break;
  case MP_INTEGER: fprintf(stderr," MP_INTEGER "); break;
  case POLY_COEFF: fprintf(stderr," POLY_COEFF "); break;
  default: fprintf(stderr," Unknown tag of coeffType."); break;
  }
}

char *intToString(i)
     int i;
{
  char work[80];
  char *s;
  sprintf(work,"%d",i);
  s = (char *)sGC_malloc((strlen(work)+2)*sizeof(char));
  if (s == (char *)NULL) errorCoeff("No more memory.");
  strcpy(s,work);
  return(s);
}

char *coeffToString(cp)
     struct coeff *cp;
{
  char *s;
  switch(cp->tag) {
  case INTEGER:
    return(intToString((cp->val).i));
    break;
  case MP_INTEGER:
    s = mpz_get_str((char *)NULL,10,(cp->val).bigp);
    return(s);
    break;
  case POLY_COEFF:
    s = POLYToString((cp->val).f,'*',1);
    return(s);
    break;
  default:
    warningCoeff("coeffToString: Unknown tag.");
    return(" Unknown-coeff ");
    break;
  }
}

/* constructors */
struct coeff *intToCoeff(i,ringp)
     int i;
     struct ring *ringp;
{
  struct coeff *c;
  int p;
  c =newCoeff();
  if (ringp->next != (struct ring *)NULL) {
    c->tag = POLY_COEFF;
    c->val.f = cxx(i,0,0,ringp->next);
    c->p = ringp->p;
  }else{
    p = ringp->p;
    if (p) {
      c->tag = INTEGER; c->p = p;
      c->val.i = i % p;
    }else{
      c->tag = MP_INTEGER; c->p = 0;
      c->val.bigp = newMP_INT();
      mpz_set_si(c->val.bigp,(long) i);
    }
  }
  return(c);
}

struct coeff *mpintToCoeff(b,ringp)
     MP_INT *b;
     struct ring *ringp;
{
  struct coeff *c;
  struct coeff *c2;
  int p;
  c =newCoeff();
  p = ringp->p;
  if (ringp->next == (struct ring *)NULL) {
    if (p) {
      c->tag = INTEGER; c->p = p;
      c->val.i = ((int) mpz_get_si(b)) % p;
    }else{
      c->tag = MP_INTEGER; c->p = 0;
      c->val.bigp = b;
    }
    return(c);
  }else{
    c2 = mpintToCoeff(b,ringp->next);
    c->tag = POLY_COEFF;
    c->p = ringp->p;
    c->val.f = coeffToPoly(c2,ringp->next);
    return(c);
    /*
      errorCoeff("mpintToCoeff(): mpint --> poly_coeff has not yet be supported. Returns 0.");
      c->tag = POLY_COEFF;
      c->val.f = ZERO; 
    */
  }
}

struct coeff *polyToCoeff(f,ringp)
     POLY f;
     struct ring *ringp;
{
  struct coeff *c;
  if (f ISZERO) {
    c =newCoeff();
    c->tag = POLY_COEFF; c->p = ringp->p;
    c->val.f = f;
    return(c);
  }
  if (f->m->ringp != ringp->next) {
    errorCoeff("polyToCoeff(): f->m->ringp != ringp->next. Returns 0.");
    f = 0;
  }
  c =newCoeff();
  c->tag = POLY_COEFF; c->p = ringp->p;
  c->val.f = f;
  return(c);
}

/* returns -c */
struct coeff *coeffNeg(c,ringp)
     struct coeff *c;
     struct ring *ringp;
{
  struct coeff *r;
  r = intToCoeff(-1,ringp);
  Cmult(r,r,c);
  return(r);
}

int coeffToInt(cp)
     struct coeff *cp;
{
  POLY f;
  switch(cp->tag) {
  case INTEGER:
    return(cp->val.i);
    break;
  case MP_INTEGER:
    return(mpz_get_si(cp->val.bigp));
    break;
  case POLY_COEFF:
    f = cp->val.f;
    if (f == ZERO) return(0);
    else return(coeffToInt(f->coeffp));
    break;
  default:
    errorCoeff("Unknown tag in coeffToInt().\n");
  }
}

void Cadd(r,a,b)
     struct coeff *r;
     struct coeff *a;
     struct coeff *b;
{
  int p;
  POLY f;

  if (a->tag == INTEGER && b->tag == INTEGER && r->tag == INTEGER) {
    r->p = p = a->p;
    r->val.i = ((a->val.i) + (b->val.i)) % p;
  }else if (a->tag == MP_INTEGER && b->tag == MP_INTEGER
            && r->tag == MP_INTEGER) {
    mpz_add(r->val.bigp,a->val.bigp,b->val.bigp);
  }else if (a->tag == POLY_COEFF && b->tag == POLY_COEFF
            && r->tag == POLY_COEFF) {
    f = ppAdd(a->val.f,b->val.f);
    r->val.f = f;
  }else {
    warningCoeff("Cadd(): The data type is not supported.");
  }
}

void Csub(r,a,b)
     struct coeff *r;
     struct coeff *a;
     struct coeff *b;
{
  int p;

  if (a->tag == INTEGER && b->tag == INTEGER && r->tag == INTEGER) {
    r->p = p = a->p;
    r->val.i = ((a->val.i) - (b->val.i)) % p;
  }else if (a->tag == MP_INTEGER && b->tag == MP_INTEGER
            && r->tag == MP_INTEGER) {
    mpz_sub(r->val.bigp,a->val.bigp,b->val.bigp);
  }else {
    warningCoeff("Csub(): The data type is not supported.");
  }
}

void Cmult(r,a,b)
     struct coeff *r;
     struct coeff *a;
     struct coeff *b;
{
  int p;
  POLY f;

  if (a->tag == INTEGER && b->tag == INTEGER && r->tag == INTEGER) {
    r->p = p = a->p;
    r->val.i = ((a->val.i) * (b->val.i)) % p;
  }else if (a->tag == MP_INTEGER && b->tag == MP_INTEGER
            && r->tag == MP_INTEGER) {
    mpz_mul(r->val.bigp,a->val.bigp,b->val.bigp);
  }else if (a->tag == POLY_COEFF && b->tag == POLY_COEFF
            && r->tag == POLY_COEFF) {
    f = ppMult(a->val.f,b->val.f);
    r->val.f = f;
  }else {
    warningCoeff("Cmult(): The data type is not supported.");
    printTag(a->tag); printTag(b->tag); printTag(r->tag); fprintf(stderr,"\n");
    warningCoeff("Returns coeffCopy(a) ------------------");
    printf("Type in return.\n");getchar(); getchar();
    *r = *(coeffCopy(a));
  }
}

int isZero(a)
     struct coeff *a;
{
  int r;
  if (a->tag == INTEGER) {
    if (a->val.i) return(0);
    else return(1);
  }else if (a->tag == MP_INTEGER) {
    r = mpz_cmp_si(a->val.bigp,(long)0);
    if (r == 0) return(1);
    else return(0);
  }else if (a->tag == POLY_COEFF) {
    if (a->val.f ISZERO) return(1);
    else return(0);
  }else{
    warningCoeff("CisZero(): The data type is not supported.");
    return(1);
  }
}

struct coeff *coeffCopy(c)
     struct coeff *c;
     /* Deep copy */
{
  struct coeff *r;
  r = newCoeff();
  switch(c->tag) {
  case INTEGER:
    r->tag = INTEGER;
    r->p = c->p;
    r->val.i = c->val.i;
    break;
  case MP_INTEGER:
    r->tag = MP_INTEGER; r->p = 0;
    r->val.bigp = newMP_INT();
    mpz_set(r->val.bigp,c->val.bigp);
    break;
  case POLY_COEFF:
    r->tag = POLY_COEFF;
    r->p = c->p;
    r->val.f = pcmCopy(c->val.f); /* The polynomial is deeply copied. */
    break;
  default:
    warningCoeff("coeffCopy(): Unknown tag for the argument.");
    break;
  }
  return(r);
}

void CiiComb(r,p,q)
     struct coeff *r;
     int p,q;
     /* r->val.bigp is read only. */
{
  switch(r->tag) {
  case INTEGER:
    r->val.i = iiComb(p,q,r->p);
    break;
  case MP_INTEGER:
    r->val.bigp = BiiComb(p,q);
    break;
  default:
    warningCoeff("CiiComb(): Unknown tag.");
    break;
  }
}
    
    

MP_INT *BiiComb(p,q)
     int p,q;
     /* It returns {p \choose q} when p>=0, 0<=q<=p.
        The value is read only. DON'T CHANGE IT.
        p=0         0                1
        p=1       1   2            1    1
        p=2     3   4   5        1   2    1
        q=0 q=1 q=2
        [p(p+1)/2+q]
     */
{
  extern MP_INT *Mp_one;
  int i,j;
  MP_INT **newTable;
  static MP_INT **table;
  static int tableSize = 0;
  static int maxp = 0; /* You have {p \choose q} in the table when p<maxp. */
  if (p<=0 || q<=0) return(Mp_one);
  if (p<=q) return(Mp_one);
  if (p<maxp) return(table[(p*(p+1))/2+q]);
  /* Enlarge the table if it's small. */
  if ( !(p < tableSize) ) {
    /* The new size is 2*p. */
    newTable = (MP_INT **)sGC_malloc(sizeof(MP_INT *)*( (2*p*(2*p+1))/2));
    if (newTable == (MP_INT **) NULL) errorCoeff("BiiComb(): No more memory.");
    for (i=0; i<tableSize; i++) {
      for (j=0; j<=i; j++) {
        newTable[(i*(i+1))/2 + j] = table[(i*(i+1))/2 + j];
      }
    }
    for (i=tableSize; i< 2*p; i++) {
      for (j=0; j<=i; j++) {
        if (j==0 || j==i) newTable[(i*(i+1))/2 + j] = Mp_one;
        else{
          newTable[(i*(i+1))/2 + j] = newMP_INT();
        }
      }
    }
    tableSize = 2*p;
    table = newTable;
  }
  /* Compute the binomial coefficients up to {p \choose p} */
  for (i=maxp; i<=p; i++) {
    for (j=1; j<i; j++) {
      mpz_add(table[(i*(i+1))/2 + j],
              table[((i-1)*i)/2 + j-1],
              table[((i-1)*i)/2 +j]);  /* [p-1,q-1]+[p-1,q] */
    }
  }
  maxp = p+1;
  return(table[(p*(p+1))/2 +q]);
  /*   ^^^^^^ No problem for the optimizer? */
}

int iiComb(p,q,P)
     int p,q,P;
{
  /**
     foo[n_]:=Block[{p,q,ans},
     ans={};
     For[p=0,p<=n,p++,ans=Append[ans,Table[Binomial[p,q],{q,0,n}]]];
     Return[ans];
     ]
  **/
  /* We assume that int is 32 bit */
  static int table[26][26]=
  {{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {1,2,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {1,3,3,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {1,4,6,4,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {1,5,10,10,5,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {1,6,15,20,15,6,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {1,7,21,35,35,21,7,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {1,8,28,56,70,56,28,8,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {1,9,36,84,126,126,84,36,9,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {1,10,45,120,210,252,210,120,45,10,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {1,11,55,165,330,462,462,330,165,55,11,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {1,12,66,220,495,792,924,792,495,220,66,12,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {1,13,78,286,715,1287,1716,1716,1287,715,286,78,13,1,0,0,0,0,0,0,0,0,0,0,0,0},
   {1,14,91,364,1001,2002,3003,3432,3003,2002,1001,364,91,14,1,0,0,0,0,0,0,0,0,0,0,0},
   {1,15,105,455,1365,3003,5005,6435,6435,5005,3003,1365,455,105,15,1,0,0,0,0,0,0,0,0,0,0},
   {1,16,120,560,1820,4368,8008,11440,12870,11440,8008,4368,1820,560,120,16,1,0,0,0,0,0,0,0,0,0},
   {1,17,136,680,2380,6188,12376,19448,24310,24310,19448,12376,6188,2380,680,136,17,1,0,0,0,0,0,0,0,0},
   {1,18,153,816,3060,8568,18564,31824,43758,48620,43758,31824,18564,8568,3060,816,153,18,1,0,0,0,0,0,0,0},
   {1,19,171,969,3876,11628,27132,50388,75582,92378,92378,75582,50388,27132,11628,3876,969,171,19,1,0,0,0,0,0,0},
   {1,20,190,1140,4845,15504,38760,77520,125970,167960,184756,167960,125970,77520,38760,15504,4845,1140,190,20,1,0,0,0,0,0},
   {1,21,210,1330,5985,20349,54264,116280,203490,293930,352716,352716,293930,203490,116280,54264,20349,5985,1330,210,21,1,0,0,0,0},
   {1,22,231,1540,7315,26334,74613,170544,319770,497420,646646,705432,646646,497420,319770,170544,74613,26334,7315,1540,231,22,1,0,0,0},
   {1,23,253,1771,8855,33649,100947,245157,490314,817190,1144066,1352078,1352078,1144066,817190,490314,245157,100947,33649,8855,1771,253,23,1,0,0},
   {1,24,276,2024,10626,42504,134596,346104,735471,1307504,1961256,2496144,2704156,2496144,1961256,1307504,735471,346104,134596,42504,10626,2024,276,24,1,0},
   {1,25,300,2300,12650,53130,177100,480700,1081575,2042975,3268760,4457400,5200300,5200300,4457400,3268760,2042975,1081575,480700,177100,53130,12650,2300,300,25,1}};

  int a,b;
  extern MP_INT Mp_work_iiComb;

  if ((p<=0) || (q<=0)) return( 1 );
  if (p <= q) return( 1 );
  if (p<26)  return( table[p][q] % P);
  else {
    mpz_mod_ui(&Mp_work_iiComb,BiiComb(p,q),(unsigned long) P);
    return((int) mpz_get_si(&Mp_work_iiComb));
  }
  /*a=iiComb(p-1,q-1,P); b=iiComb(p-1,q,P);
    a = a+b; a %= P;
    return(a);
  */
}


MP_INT *BiiPoch(p,q)
     int p,q;
{
  /* It returns  p(p-1) ... (p-q+1) = [p,q] when p>=0 and 0<=q or
     p<0  and 0<=q.
     [p,q] = p*[p-1,q-1].  [p,0] = 1.
     The value is read only. DON'T CHANGE IT.

     When p>=0,
     p=0         0                1
     p=1       1   2            1    1
     p=2     3   4   5        1   2    2*1
     p=3   6   7   8   9    1   3   3*2    3*2*1,  if p<q,then 0.
     [p(p+1)/2+q]

     When p<0, [p,q] is indexed by (-p+q)(-p+q+1)/2 + q.
     -p+q = pq.

     q=3  0  (-1)(-2)(-3)  (-2)(-3)(-4)   (-3)(-4)(-5)
     q=2  0  (-1)(-2)      (-2)(-3)       (-3)(-4)
     q=1  0   -1            -2             -3
     q=0  1    1             1              1
     -p=0  -p=2          -p=3           -p=4  
  */
  extern MP_INT *Mp_one;
  extern MP_INT *Mp_zero;
  MP_INT mp_work;
  int i,j;
  MP_INT **newTable;
  static MP_INT **table;
  static int tableSize = 0;
  static int maxp = 0;
  static MP_INT **tablepq;
  static int tablepqSize = 0;
  static int maxpq = 0;
  int pq;

  if (p>=0) {
    if (q < 0) {
      warningCoeff("Negative argument to BiiPoch(). Returns zero.");
      return(Mp_zero);
    }
    if (q == 0) return(Mp_one);
    if (p<q) return(Mp_zero);
    if (p<maxp) return(table[(p*(p+1))/2+q]);
    /* Enlarge the table if it's small. */
    if ( !(p < tableSize) ) {
      /* The new size is 2*p. */
      newTable = (MP_INT **)sGC_malloc(sizeof(MP_INT *)*( (2*p*(2*p+1))/2));
      if (newTable == (MP_INT **) NULL) errorCoeff("BiiPoch(): No more memory.");
      for (i=0; i<tableSize; i++) {
        for (j=0; j<=i; j++) {
          newTable[(i*(i+1))/2 + j] = table[(i*(i+1))/2 + j];
        }
      }
      for (i=tableSize; i< 2*p; i++) {
        for (j=0; j<=i; j++) {
          if (j==0) newTable[(i*(i+1))/2 + j] = Mp_one;
          else{
            newTable[(i*(i+1))/2 + j] = newMP_INT();
          }
        }
      }
      tableSize = 2*p;
      table = newTable;
    }
    /* Compute the binomial coefficients up to {p \choose p} */
    for (i=maxp; i<=p; i++) {
      for (j=1; j<=i; j++) {
        mpz_mul_ui(table[(i*(i+1))/2 + j],
                   table[((i-1)*i)/2 + j-1],
                   (unsigned long int) i); /* [i,j] = i*[i-1,j-1] */
      }
    }
    maxp = p+1;
    return(table[(p*(p+1))/2 +q]);
    /*   ^^^^^^ No problem for the optimizer? */
  }else{
    if (q < 0) {
      warningCoeff("Negative argument to BiiPoch(). Returns zero.");
      return(Mp_zero);
    }
    if (q == 0) return(Mp_one);
    pq = -p+q;
    if (pq<maxpq) return(tablepq[(pq*(pq+1))/2+q]);
    /* Enlarge the table if it's small. */
    if ( !(pq < tablepqSize) ) {
      /* The new size is 2*pq. */
      newTable = (MP_INT **)sGC_malloc(sizeof(MP_INT *)*( (2*pq*(2*pq+1))/2));
      if (newTable == (MP_INT **) NULL) errorCoeff("BiiPoch(): No more memory.");
      for (i=0; i<tablepqSize; i++) {
        for (j=0; j<=i; j++) {
          newTable[(i*(i+1))/2 + j] = tablepq[(i*(i+1))/2 + j];
        }
      }
      for (i=tablepqSize; i< 2*pq; i++) {
        for (j=0; j<=i; j++) {
          if (j==0) newTable[(i*(i+1))/2 + j] = Mp_one;
          else if (j==i) newTable[(i*(i+1))/2+j] = Mp_zero;
          else { /*^^^ no problem for optimizer? */
            newTable[(i*(i+1))/2 + j] = newMP_INT();
          }
        }
      }
      tablepqSize = 2*pq;
      tablepq = newTable;
    }
    /* Compute the Poch up to [p,q], -p+q<=pq */
    mpz_init(&mp_work);
    for (i=maxpq; i<=pq; i++) {
      for (j=1; j<i; j++) {
        mpz_set_si(&mp_work,-(i-j));
        mpz_mul(tablepq[(i*(i+1))/2 + j],
                tablepq[(i*(i+1))/2 + j-1],
                &mp_work); /* [i,j] = i*[i-1,j-1] */
      }
    }
    maxpq = pq+1;
    return(tablepq[(pq*(pq+1))/2 +q]);
    /*   ^^^^^^ No problem for the optimizer? */
  }
}


int iiPoch(p,k,P) int p,k,P; /* p(p-1)...(p-k+1) */ {
  int r,i;

  /*
    extern MP_INT Mp_work_iiPoch;
    mpz_mod_ui(&Mp_work_iiPoch,BiiPoch(p,k),(unsigned long) P);
    return((int) mpz_get_si(&Mp_work_iiPoch));
    100 test takes 16ms.
  */

  if (p>=0 && p < k) return(0);
  r = 1;
  for (i=0;i<k;i++) {
    r = r*(p-i);  r %= P;
  }
  return(r);
}

void CiiPoch(r,p,q)
     struct coeff *r;
     int p,q;
     /* r->val.bigp is read only. */
{
  switch(r->tag) {
  case INTEGER:
    r->val.i = iiPoch(p,q,r->p);
    break;
  case MP_INTEGER:
    r->val.bigp = BiiPoch(p,q);
    break;
  default:
    warningCoeff("CiiPoch(): Unknown tag.");
    break;
  }
}

MP_INT *BiiPower(p,q)
     int p,q;
     /* It returns  p^q.  q must be >=0.
        p^q = [p,q] = p*[p,q-1].
        The value is read only. DON'T CHANGE IT.
     */
     /*
       [p,q] is indexed by table2D[p+q,q]=table1D[(p+q)(p+q+1)/2 + q].
       p+q = pq.

       q=3  0    1             8             27
       q=2  0    1             4              9
       q=1  0    1             2              3
       q=0  1    1             1              1
       -p=0  -p=1          -p=2           -p=3
     */
{
  extern MP_INT *Mp_one;
  extern MP_INT *Mp_zero;
  MP_INT mp_work;
  int i,j;
  MP_INT **newTable;
  MP_INT **newTable2;
  static MP_INT **table;
  static int tableSize = 0;
  static int maxp = 0;
  static MP_INT **tableneg;
  int pq;

  
  if (q < 0) {
    warningCoeff("Negative argument to BiiPower(). Returns zero.");
    return(Mp_zero);
  }
  if (q == 0) return(Mp_one);
  if (p>=0) {
    pq = p+q;
  }else {
    pq = -p+q;
  }
  if (pq<maxp && p>=0) return(table[(pq*(pq+1))/2+q]);
  if (pq<maxp && p<0) return(tableneg[(pq*(pq+1))/2+q]);

  /* Enlarge the table if it's small. */
  if ( !(pq < tableSize) ) {
    /* The new size is 2*pq. */
    newTable = (MP_INT **)sGC_malloc(sizeof(MP_INT *)*( (2*pq*(2*pq+1))/2));
    newTable2 = (MP_INT **)sGC_malloc(sizeof(MP_INT *)*( (2*pq*(2*pq+1))/2));
    if (newTable == (MP_INT **) NULL || newTable2 == (MP_INT **)NULL)
      errorCoeff("BiiPower(): No more memory.");
    for (i=0; i<tableSize; i++) {
      for (j=0; j<=i; j++) {
        newTable[(i*(i+1))/2 + j] = table[(i*(i+1))/2 + j];
        newTable2[(i*(i+1))/2 + j] = tableneg[(i*(i+1))/2 + j];
      }
    }
    for (i=tableSize; i< 2*pq; i++) {
      for (j=0; j<=i; j++) {
        if (j==0) {
          newTable[(i*(i+1))/2 + j] = Mp_one;
          newTable2[(i*(i+1))/2 + j] = Mp_one;
        } else if (j==i) {
          newTable[(i*(i+1))/2+j] = Mp_zero;
          newTable2[(i*(i+1))/2+j] = Mp_zero;
        }else { /*^^^ no problem for optimizer? */
          newTable[(i*(i+1))/2 + j] = newMP_INT();
          newTable2[(i*(i+1))/2 + j] = newMP_INT();
        }
      }
    }
    table = newTable;
    tableneg = newTable2;
    tableSize = 2*pq;
  }
  /* Compute the Power up to [p,q], p+q<=pq */
  mpz_init(&mp_work);
  for (i=maxp; i<=pq; i++) {
    for (j=1; j<i; j++) {
      mpz_set_si(&mp_work,-(i-j));
      mpz_mul(tableneg[(i*(i+1))/2 + j],
              tableneg[((i-1)*i)/2 + j-1],
              &mp_work); /* (-(i-j))^j=[i,j] = (-i+j)*[i-1,j-1] */
      mpz_set_si(&mp_work,i-j);
      mpz_mul(table[(i*(i+1))/2 + j],
              table[((i-1)*i)/2 + j-1],
              &mp_work); /* (i-j)^j=[i,j] = (i-j)*[i-1,j-1] */
    }
  }
  maxp = pq+1;
  if (p>=0) {
    return(table[(pq*(pq+1))/2 +q]);
  }else{
    return(tableneg[(pq*(pq+1))/2 +q]);
  }
}

int iiPower(k,s,P)
     int k;
     int s;  /* k^s ,  s>=0*/
     int P;
{
  int kk,r;
  r = 1;
  for (kk=0; kk<s; kk++ ) {
    r *= k; r %= P;
  }
  return(r);
}

void CiiPower(r,p,q)
     struct coeff *r;
     int p,q;
     /* r->val.bigp is read only. */
{
  switch(r->tag) {
  case INTEGER:
    r->val.i = iiPower(p,q,r->p);
    break;
  case MP_INTEGER:
    r->val.bigp = BiiPower(p,q);
    break;
  default:
    warningCoeff("CiiPower(): Unknown tag.");
    break;
  }
}

struct coeff *stringToUniversalNumber(s,flagp)
     char *s;
     int *flagp;
{
  MP_INT *value;
  struct coeff * c;
  value =newMP_INT();
  *flagp = mpz_set_str(value,s,10);
  c = newCoeff();
  c->tag = MP_INTEGER; c->p = 0;
  c->val.bigp = value;
  return(c);
}

struct coeff *newUniversalNumber(i)
     int i;
{ extern struct ring *SmallRingp;
 struct coeff *c;
 c = intToCoeff(i,SmallRingp);
 return(c);
}

struct coeff *newUniversalNumber2(i)
     MP_INT *i;
{ extern struct ring *SmallRingp;
 struct coeff *c;
 c = mpintToCoeff(i,SmallRingp);
 return(c);
}

int coeffEqual(a,b)
     struct coeff *a;
     struct coeff *b;
{
  if (a->tag == INTEGER && b->tag == INTEGER) {
    return((a->val.i) == (b->val.i));
  }else if (a->tag == MP_INTEGER && b->tag == MP_INTEGER) {
    if (mpz_cmp(a->val.bigp,b->val.bigp)==0) return(1);
    else return(0);
  }else if (a->tag == POLY_COEFF && b->tag == POLY_COEFF) {
    return(ppSub(a->val.f,b->val.f) ISZERO);
  }else {
    warningCoeff("coeffEqual(): The data type is not supported.");
  }
}

int coeffGreater(a,b)
     struct coeff *a;
     struct coeff *b;
{
  if (a->tag == INTEGER && b->tag == INTEGER) {
    return((a->val.i) - (b->val.i));
  }else if (a->tag == MP_INTEGER && b->tag == MP_INTEGER) {
    return(mpz_cmp(a->val.bigp,b->val.bigp));
  }else if (a->tag == POLY_COEFF && b->tag == POLY_COEFF) {
    warningCoeff("coeffGreater(POLY,POLY) always returns 0.");
    return(0);
  }else {
    warningCoeff("coeffGreater(): The data type is not supported.");
  }
}

void universalNumberDiv(r,a,b)
     struct coeff *r;
     struct coeff *a;
     struct coeff *b;
{
  if (a->tag == MP_INTEGER && b->tag == MP_INTEGER && r->tag == MP_INTEGER) {
    mpz_div(r->val.bigp,a->val.bigp,b->val.bigp);
  }else {
    warningCoeff("universalNumberDiv(): The data type is not supported.");
  }
}

/* Note that it does not check if c is zero or not. cf isZero(). */
POLY coeffToPoly(c,ringp)
     struct coeff *c;
     struct ring *ringp;
{ int p;
 struct coeff *tc;

 if (c->tag == INTEGER) {
   tc = intToCoeff(c->val.i,ringp);
   return(newCell(tc,newMonomial(ringp)));
 }else if (c->tag == MP_INTEGER) {
   tc = mpintToCoeff(c->val.bigp,ringp);
   return(newCell(tc,newMonomial(ringp)));
 } else if (c->tag == POLY_COEFF) {
   return(newCell(c,newMonomial(ringp)));
 }else {
   warningCoeff("coeffToPoly(): The data type is not supported. Return 0.");
   return(ZERO);
 }
}
void errorCoeff(str)
     char *str;
{
  fprintf(stderr,"Error(coeff.c): %s\n",str);
  fprintf(stderr,"Type in ctrl-\\");getchar();
  exit(15);
}

void warningCoeff(str)
     char *str;
{
  fprintf(stderr,"Warning(coeff.c): %s\n",str);
}

