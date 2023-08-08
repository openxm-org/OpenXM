/* $OpenXM: OpenXM/src/kan96xx/Kan/poly.c,v 1.6 2010/01/27 15:10:08 ohara Exp $ */
#include <stdio.h>
#include <stdlib.h>
#include "datatype.h"
#include "stackm.h"
#include "extern.h"
#include "extern2.h"
void initSyzRingp(void);  /* defined in gradedset.h */

void KinitKan(void) {

  extern int Msize;
  extern struct ring SmallRing;
  extern struct ring *CurrentRingp;
  extern MP_INT *Mp_one;
  extern MP_INT *Mp_zero;
  extern MP_INT Mp_work_iiComb;
  extern MP_INT Mp_work_iiPoch;
  extern MP_INT Mp_work_iiPower;

  static MP_INT Mp_One;
  static MP_INT Mp_Zero;

  extern struct coeff *UniversalZero;
  extern struct coeff *UniversalOne;
  
  int i;
  static char *smallx[] = {"x","t"};
  static char *smalld[] = {"h","T"};
  /* t  x  T  h */ 
  static int smallOrder[] = { 1, 0, 0, 0,
                              0, 1, 0, 0,
                              0, 0, 1, 0,
                              0, 0, 0, 1};

  static int outputOrderForSmallRing[] = {1,0,3,2};
  Msize = sizeof(struct monomial) - sizeof(struct monomialDummy);
  if (Msize <= 0) errorPoly("Problem of your C-compiler??!!");

  /* Define the CurrentRingp */
  /* Define the SmallRingp */

  SmallRing.p = 0;
  SmallRing.n = SmallRing.m = SmallRing.l = SmallRing.c = 2;
  SmallRing.nn = SmallRing.mm = SmallRing.ll = 2;
  SmallRing.cc = 1;
  SmallRing.x = smallx; 
  SmallRing.D = smalld;
  SmallRing.order = smallOrder;
  SmallRing.orderMatrixSize = 4;
  setFromTo(&SmallRing);
  SmallRing.next = (struct ring *)NULL;
  SmallRing.multiplication = mpMult_poly;
  SmallRing.schreyer = 0;
  SmallRing.gbListTower = NULL;
  SmallRing.outputOrder = outputOrderForSmallRing;
  SmallRing.name = "SmallRingp";
  SmallRing.partialEcart = 0;
  SmallRing.module_rank = 0;

  CurrentRingp = &SmallRing;
  initSyzRingp();

  switch_init();
  initT();  /* Initialize poly3.c */


#ifndef NOGC
  /* initialize MPZ */
  mp_set_memory_functions(sGC_malloc,sGC_realloc2,sGC_free2);
#endif

  /* Msize is 8 on SUN gcc */

  /* We have defined a ring. Let's initialize Nodes */

  Mp_one = &Mp_One;
  mpz_init(Mp_one); mpz_set_si(Mp_one,(long) 1);
  Mp_zero = &Mp_Zero;
  mpz_init(Mp_zero); mpz_set_si(Mp_zero,(long) 0);
  mpz_init(&Mp_work_iiComb); mpz_init(&Mp_work_iiPoch);
  mpz_init(&Mp_work_iiPower);

  UniversalZero = intToCoeff(0,&SmallRing);
  UniversalOne = intToCoeff(1,&SmallRing);

  KdefaultPolyRing(KpoInteger(0));
}

#ifndef NOGC
void *sGC_realloc2(void *p,size_t old,size_t new)
{
  return(sGC_realloc(p,new));
}
void sGC_free2(void *p,size_t size)
{
  /* Do nothing. */
}
#endif


MONOMIAL newMonomial(ringp)
     struct ring *ringp;
{
  MONOMIAL f;
  extern int Msize;
  int i;
  int n;
  n = ringp->n;
  f = (MONOMIAL) sGC_malloc(sizeof(struct smallMonomial)+n*Msize);
  if (f == (MONOMIAL) NULL) errorPoly("No more memory.");
  f->ringp = ringp;
  for (i=0; i<n; i++) {
    (f->e)[i].x = 0;
    (f->e)[i].D = 0;  /* necessary?->Yes. */
  }
  return(f);
}
 

MONOMIAL monomialCopy(m)
     MONOMIAL m;
{
  extern int Msize;
  MONOMIAL f;
  int i;
  int n;
  n = m->ringp->n;
  f = (MONOMIAL) sGC_malloc(sizeof(struct smallMonomial)+n*Msize);

  if (f == (MONOMIAL) NULL) errorPoly("No more memory.");
  f->ringp = m->ringp;
  for (i=0; i<n; i++) {
    (f->e)[i].x = (m->e)[i].x; 
    (f->e)[i].D = (m->e)[i].D;
  }
  return(f);
}
  

struct coeff *newCoeff() {
  struct coeff *cp;
  cp = (struct coeff *)sGC_malloc(sizeof (struct coeff));
  if (cp == (struct coeff *)NULL) errorPoly("No more memory.");
  cp->tag = UNKNOWN;
  cp->p = -123; /* stupid value */
  return(cp);
}

MP_INT *newMP_INT() {
  MP_INT *ip;
  ip = (MP_INT *)sGC_malloc(sizeof(MP_INT));
  if (ip == (MP_INT *)NULL) errorPoly("No more memory.");
  mpz_init(ip);
  return(ip);
}

POLY newCell(c,mon)
     struct coeff *c;
     MONOMIAL mon;
{
  POLY ff;
  ff = (POLY) sGC_malloc(sizeof(struct listPoly));
  if (ff == POLYNULL) errorPoly("No more memory.");
  ff->next = POLYNULL;
  ff->coeffp = c;
  ff->m = mon;
  return(ff);
}

/* constructors */
POLY cxx(c,i,k,ringp)
     int c,i,k;
     struct ring *ringp;
     /*  c x_i^k where p is the characteristic. */
     /* New cell, monomial and coeff. */
{
  POLY f;
  int p;
  p = ringp->p;
  if (c == 0) return(POLYNULL);
  f = pMalloc(ringp);
  if (ringp->next == (struct ring *)NULL) {
    if (p) {
      f->coeffp->tag = INTEGER; f->coeffp->p = p;
      f->coeffp->val.i = c % p;
      if (f->coeffp->val.i == 0) return(POLYNULL);
    }else{
      f->coeffp->tag = MP_INTEGER; f->coeffp->p = 0;
      f->coeffp->val.bigp = newMP_INT(); 
      mpz_set_si(f->coeffp->val.bigp,(long) c);
    }
    f->m->e[i].x = k;
    return(f);
  }else{
    f->coeffp->tag = POLY_COEFF; f->coeffp->p = p;
    f->coeffp->val.f = cxx(c,0,0,ringp->next);
    if (f->coeffp->val.f == POLYNULL) return(POLYNULL);
    f->m->e[i].x = k;
    return(f);
  }
}

POLY bxx(c,i,k,ringp)
     MP_INT *c;     
     int i,k;
     struct ring *ringp;
     /*  c x_i^k.  c is not copied. */
{
  /* new cell, monomial, coeff. MP_INT c is not copied. */
  POLY f;
  int p;
  p = ringp->p;
  if (mpz_cmp_si(c,(long)0) == 0) return(POLYNULL);
  f = pMalloc(ringp);
  if (ringp->next == (struct ring *)NULL) {
    if (p) {
      f->coeffp->tag = INTEGER; f->coeffp->p = p;
      f->coeffp->val.i = (int) mpz_get_si(c);
      f->coeffp->val.i %= p;
      if (f->coeffp->val.i == 0) return(POLYNULL);
    }else{
      f->coeffp->tag = MP_INTEGER; f->coeffp->p = 0;
      f->coeffp->val.bigp = c;
    }
    f->m->e[i].x = k;
    return(f);
  }else{
    warningPoly("cbb(): ringp->next is not NULL. Returns 0.");
    return(POLYNULL);
  }
}

POLY cdd(c,i,k,ringp)
     int c,i,k;
     struct ring *ringp;
     /*  c D_i^k where p is the characteristic. */
     /* New cell, monomial and coeff. */
{
  POLY f;
  int p;
  p = ringp->p;
  if (c == 0) return(POLYNULL);
  f = pMalloc(ringp);
  if (ringp->next == (struct ring *)NULL) {
    if (p) {
      f->coeffp->tag = INTEGER; f->coeffp->p = p;
      f->coeffp->val.i = c % p;
      if (f->coeffp->val.i == 0) return(POLYNULL);
    }else{
      f->coeffp->tag = MP_INTEGER; f->coeffp->p = 0;
      f->coeffp->val.bigp = newMP_INT(); 
      mpz_set_si(f->coeffp->val.bigp,(long) c);
    }
    f->m->e[i].D = k;
    return(f);
  }else{
    f->coeffp->tag = POLY_COEFF; f->coeffp->p = p;
    f->coeffp->val.f = cdd(c,0,0,ringp->next);
    if (f->coeffp->val.f == POLYNULL) return(POLYNULL);
    f->m->e[i].D = k;
    return(f);
  }
    
}


POLY pCopy(f)
     POLY f;
{
  POLY node;
  struct listPoly nod;
  POLY h;
  node = &nod;
  if (f == POLYNULL) return(POLYNULL);
  node->next = POLYNULL;
  h = node;
  while (f != POLYNULL) {
    h->next = newCell(f->coeffp,f->m); /* shallow */
    h = h->next;
    f = f->next;
  }
  return(node->next);
}

POLY pcCopy(f)
     POLY f;
{
  
  POLY node;
  struct listPoly nod;
  POLY h;
  struct coeff *c;
  node = &nod;
  if (f == POLYNULL) return(POLYNULL);
  node->next = POLYNULL;
  h = node;
  while (f != POLYNULL) {
    c = coeffCopy(f->coeffp);
    h->next = newCell(c,f->m); /* poly and coeff. */
    h = h->next;
    f = f->next;
  }
  return(node->next);
}

POLY pmCopy(f)
     POLY f;
{
  POLY node;
  struct listPoly nod;
  POLY h;
  struct coeff *c;
  node = &nod;
  if (f == POLYNULL) return(POLYNULL);
  node->next = POLYNULL;
  h = node;
  while (f != POLYNULL) {
    h->next = newCell(f->coeffp,monomialCopy(f->m));
    h = h->next;
    f = f->next;
  }
  return(node->next);
}

POLY pcmCopy(f)
     POLY f;
{
  POLY node;
  struct listPoly nod;
  POLY h;
  struct coeff *c;
  node = &nod;
  if (f == POLYNULL) return(POLYNULL);
  node->next = POLYNULL;
  h = node;
  while (f != POLYNULL) {
    h->next = newCell(coeffCopy(f->coeffp),monomialCopy(f->m));
    h = h->next;
    f = f->next;
  }
  return(node->next);
}

POLY head(f)
     POLY f;
{
  if (f == ZERO) return(f);
  else {
    return(newCell(f->coeffp,f->m));
  }
}

void errorPoly(str)
     char *str;
{
  fprintf(stderr,"Error(poly.c): %s\n",str);
  exit(20);
}

void warningPoly(str)
     char *str;
{
  fprintf(stderr,"Warning(poly.c): %s\n",str);
}



    

  


  


