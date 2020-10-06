/* $OpenXM: OpenXM/src/kan96xx/Kan/poly2.c,v 1.7 2005/07/03 11:08:54 ohara Exp $ */
#include <stdio.h>
#include <stdlib.h>
#include "datatype.h"
#include "stackm.h"
#include "extern.h"
#include "extern2.h"

static POLY mapZmonom(POLY f,struct ring *ringp);

POLY ppAdd(f,g)
	 POLY f; POLY g;  /* The result is read only. */
{
  POLY node;
  struct listPoly nod;
  POLY h;
  struct coeff *c;
  int gt;

  node = &nod;
  node->next = POLYNULL;
  h = node;
  if (f == POLYNULL) return(g);
  if (g == POLYNULL) return(f);
  checkRing(f,g);

  while (f != POLYNULL && g != POLYNULL) {
    /*printf("%s + %s\n",POLYToString(f,'*',1),POLYToString(g,'*',1));*/
    checkRing2(f,g); /* for debug */
    gt = (*mmLarger)(f,g);
    switch (gt) {
    case 1: /* f > g */
      h -> next = newCell(f->coeffp,f->m);
      h = h->next;
      f = f->next;
      if (f == POLYNULL) {
		h->next = g;
		return(node->next);
      }
      break;
    case 0: /* f < g */
      h->next = newCell(g->coeffp,g->m);
      h = h->next;
      g = g->next;
      if (g == POLYNULL) {
		h->next = f;
		return(node->next);
      }
      break;
    case 2:/* f == g */
      c = coeffCopy(f->coeffp);
      Cadd(c,c,g->coeffp);
      if (!isZero(c)) {
		h->next = newCell(c,f->m);
		h = h->next;
		f = f->next;
		g = g->next;
		if (f == POLYNULL) {
		  h->next = g;
		  return(node->next);
		}
		if (g == POLYNULL) {
		  h->next = f;
		  return(node->next);
		}
      }else{
		f = f->next;
		g = g->next;
		if (f == POLYNULL) {
		  h->next = g;
		  return(node->next);
		}
		if (g == POLYNULL) {
		  h->next = f;
		  return(node->next);
		}
      }
      break;
    default:
      errorPoly("ppAdd(). Internal error. Invalid value of mmLarger.");
      break;
    }
  }
  return(node->next);
}

POLY ppSub(f,g)
	 POLY f; POLY g;  /* The result is read only. */
{
  POLY h;
  struct coeff *c;

  if (g == POLYNULL) return(f);
  if (f == POLYNULL) return(cpMult(intToCoeff(-1,g->m->ringp),g));
  checkRing(f,g);

  h = cpMult(intToCoeff(-1,g->m->ringp),g);
  return(ppAdd(f,h));
}


POLY cpMult(c,f)
	 struct coeff *c;
	 POLY f;
{
  POLY node;
  struct listPoly nod;
  POLY h;
  struct coeff *newc;
  int p;
  node = &nod;
  if (f == POLYNULL || isZero(c)) return(POLYNULL);
  p = f->m->ringp->p;
  node ->next = POLYNULL;
  h = node;
  while (f != POLYNULL) {
    newc = coeffCopy(c);
    Cmult(newc,newc,f->coeffp);
    if ((p==0) || !isZero(newc)) {
      h->next = newCell(newc,f->m);
      h = h->next;
    }
    f = f->next;
  }
  return(node->next);
}

MONOMIAL monomialAdd_poly(m,m2)
	 MONOMIAL m,m2;
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
    (f->e)[i].x = (m->e)[i].x + (m2->e)[i].x; 
    (f->e)[i].D = (m->e)[i].D + (m2->e)[i].D;
  }
  return(f);
}

/* Note that mpMult_poly is called from mmLarger_tower! */
POLY mpMult_poly(f,g)
	 POLY f;
	 POLY g;
{
  POLY node;
  struct listPoly nod;
  struct coeff *c;
  int n,i;
  POLY h;
  MONOMIAL m;
  int p;
  node = &nod;
  if (f == POLYNULL || g == POLYNULL) return(POLYNULL);
  node->next = POLYNULL;
  h = node;
  checkRing(f,g);
  n = f->m->ringp->n; p = f->m->ringp->p;
  while(g != POLYNULL) {
    checkRing2(f,g);
    c = coeffCopy(f->coeffp);
    Cmult(c,c,g->coeffp);
    if ((p==0) || !isZero(c)) {
      m = (*monomialAdd)(f->m,g->m);
      h->next = newCell(c,m);
      h = h->next;
    }
    g = g->next;
  }
  return(node->next);
}

POLY ppMult_old(f,g)
	 POLY f,g;
{
  POLY r;
  POLY tmp;
  r = POLYNULL;
  while( f != POLYNULL) {
    tmp = (*mpMult)(f,g);
    r = ppAddv(r,tmp); /* r and tmp will be broken */
    f = f->next;
  }
  return(r);
}

POLY ppAddv(f,g)
	 POLY f; POLY g;  /* It breaks f and g. Use it just after calling mpMult() */
{
  POLY node;
  struct listPoly nod;
  POLY h;
  struct coeff *c;
  int gt;

  node = &nod;
  /*printf("ppAddv:1%s + %s\n",POLYToString(f,'*',1),POLYToString(g,'*',1));*/
  node->next = POLYNULL;
  h = node;
  if (f == POLYNULL) return(g);
  if (g == POLYNULL) return(f);
  checkRing(f,g);

  while (f != POLYNULL && g != POLYNULL) {
    checkRing2(f,g); /* for debug */
    /*printf("%s + %s\n",POLYToString(f,'*',1),POLYToString(g,'*',1));*/
    gt = (*mmLarger)(f,g);
    switch (gt) {
    case 1: /* f > g */
      h->next = f;
      h = h->next; f = f->next;;
      if (f == POLYNULL) {
		h->next = g;
		return(node->next);
      }
      break;
    case 0: /* f < g */
      h->next =  g;
      h = h->next; g = g->next;
      if (g == POLYNULL) {
		h->next = f;
		return(node->next);
      }
      break;
    case 2:/* f == g */
      c = f->coeffp;
      Cadd(c,c,g->coeffp);
      if (!isZero(c)) {
		h->next = f;
		h = h->next; f = f->next;;
		g = g->next;
		if (f == POLYNULL) {
		  h->next = g;
		  return(node->next);
		}
		if (g == POLYNULL) {
		  h->next = f;
		  return(node->next);
		}
      }else{
		f = f->next;
		g = g->next;
		if (f == POLYNULL) {
		  h->next = g;
		  return(node->next);
		}
		if (g == POLYNULL) {
		  h->next = f;
		  return(node->next);
		}
      }
      break;
    default:
      errorPoly("ppAddv(). Internal error. Invalid value of mmLarger.");
      break;
    }
  }
  return(node->next);
}

POLY pPower(f,k)
	 POLY f;
	 int k;
{
  POLY r;
  int i,n;
  if (f == POLYNULL) return(POLYNULL); /* Is it ok? 0^0 = 0.*/
  if (k == 0) return(cxx(1,0,0,f->m->ringp));
  if (f->next == POLYNULL &&  k<0) {
    /* when f is monomial. */
    r = newCell(coeffCopy(f->coeffp),monomialCopy(f->m));
    n = r->m->ringp->n;
    for (i=0; i<n; i++) {
      r->m->e[i].x *= k;
      r->m->e[i].D *= k;
    }
    if (!isOne(r->coeffp)) {
      warningPoly("pPower(poly,negative integer) not implemented yet. Returns 1.");
      r = cxx(1,0,0,f->m->ringp);
    }
    return(r);
  }
  r = cxx(1,0,0,f->m->ringp);
  if (k < 0) {
    warningPoly("pPower(poly,negative integer) not implemented yet. Returns 1.");
  }
  for (i=0; i<k; i++) {
    r = ppMult(f,r);
  }
  return(r);
}

POLY pPower_poly(f,k)
	 POLY f;
	 int k;
{
  POLY r;
  int i,n;
  if (f == POLYNULL) return(POLYNULL); /* Is it ok? 0^0 = 0.*/
  if (k == 0) return(cxx(1,0,0,f->m->ringp));
  if (f->next == POLYNULL &&  k<0) {
    /* when f is monomial. */
    r = newCell(coeffCopy(f->coeffp),monomialCopy(f->m));
    n = r->m->ringp->n;
    for (i=0; i<n; i++) {
      r->m->e[i].x *= k;
      r->m->e[i].D *= k;
    }
    if (!isOne(r->coeffp)) {
      warningPoly("pPower_poly(poly,negative integer) not implemented yet. Returns 1.");
      r = cxx(1,0,0,f->m->ringp);
    }
    return(r);
  }
  r = cxx(1,0,0,f->m->ringp);
  if (k < 0) {
    warningPoly("pPower_poly(poly,negative integer) not implemented yet. Returns 1.");
  }
  for (i=0; i<k; i++) {
    r = ppMult_poly(f,r);
  }
  return(r);
}

POLY modulop_trash(f,ringp)
	 POLY f;
	 struct ring *ringp;
{
  int p;
  POLY h;
  MP_INT *c;
  int cc;
  POLY node;
  struct ring *nextRing;
  POLY fc;
  
  if (f == POLYNULL) return(f);
  p = ringp->p;
  if (f->m->ringp->p != 0) {
    warningPoly("modulop(f,ringp) must be called with f in the characteristic 0 ring. Returns 0.");
    return(POLYNULL);
  }
  if (f->m->ringp->n != ringp->n) {
    warningPoly("modulop(f,ringp): f->m->ringp->n must be equal to ringp->n. Returns 0.");
    return(POLYNULL);
  }

  /* The case of ringp->next != NULL */
  if (ringp->next != (struct ring *)NULL) {
    nextRing = ringp->next;
    node = newCell(newCoeff(),newMonomial(ringp));
    node->next = POLYNULL;
    h = node;

    while (f != POLYNULL) {
      fc = bxx(f->coeffp->val.bigp,0,0,nextRing);
      h->next = newCell(newCoeff(),monomialCopy(f->m));
      h = h->next;
      h->m->ringp = ringp;
      h->coeffp->tag = POLY_COEFF;
      h->coeffp->p = p;
      h->coeffp->val.f = fc;
      f = f->next;
    }
    return(node->next);
  }

    
  /* In case of ringp->next == NULL */
  if (p != 0) {
    c = newMP_INT();
    node = newCell(newCoeff(),newMonomial(ringp));
    node->next = POLYNULL;
    h = node;
    
    while (f != POLYNULL) {
      mpz_mod_ui(c,f->coeffp->val.bigp,(unsigned long int)p);
      cc = (int) mpz_get_si(c);
      if (cc != 0) {
		h->next = newCell(newCoeff(),monomialCopy(f->m));
		h = h->next;
		h->m->ringp = ringp;
		h->coeffp->tag = INTEGER;
		h->coeffp->p = p;
		h->coeffp->val.i = cc;
      }
      f = f->next;
    }
    return(node->next);
  }else{
    h = f = pcmCopy(f);
    while (f != POLYNULL) {
      f->m->ringp = ringp;
      f = f->next;
    }
    return(h);
  }
      
}
  
POLY modulop(f,ringp)
	 POLY f;
	 struct ring *ringp;
	 /* Z[x] ---> R[x] where R=Z, Z/Zp, ringp->next. */
{
  int p;
  POLY h;
  MP_INT *c;
  int cc;
  POLY node;
  POLY fc;
  
  if (f == POLYNULL) return(f);
  p = ringp->p;
  if (f->m->ringp->p != 0 || f->m->ringp->next != (struct ring *)NULL) {
    warningPoly("modulop(f,ringp) must be called with f in the characteristic 0 ring Z[x]. Returns 0.");
    return(POLYNULL);
  }
  if (f->m->ringp->n != ringp->n) {
    warningPoly("modulop(f,ringp): f->m->ringp->n must be equal to ringp->n. Returns 0.");
    return(POLYNULL);
  }

  /* [1] The case of R = ringp->next */
  if (ringp->next != (struct ring *)NULL) {
    h = ZERO;
    while (f != POLYNULL) {
      h = ppAdd(h,mapZmonom(f,ringp));
      f = f->next;
    }
    return(h);
  }

  /* [2] The case of R = Z/Zp */    
  if (p != 0) {
    c = newMP_INT();
    node = newCell(newCoeff(),newMonomial(ringp));
    node->next = POLYNULL;
    h = node;
    
    while (f != POLYNULL) {
      mpz_mod_ui(c,f->coeffp->val.bigp,(unsigned long int)p);
      cc = (int) mpz_get_si(c);
      if (cc != 0) {
		h->next = newCell(newCoeff(),monomialCopy(f->m));
		h = h->next;
		h->m->ringp = ringp;
		h->coeffp->tag = INTEGER;
		h->coeffp->p = p;
		h->coeffp->val.i = cc;
      }
      f = f->next;
    }
    return(node->next);
  }

  /* [3] The case of R = Z */
  h = f = pcmCopy(f);
  while (f != POLYNULL) {
    f->m->ringp = ringp;
    f = f->next;
  }
  return(h);

      
}
  
POLY modulopZ(f,pcoeff)
	 POLY f;
	 struct coeff *pcoeff;
	 /* Z[x] ---> Z[x] , f ---> f mod pcoeff*/
{
  int p;
  POLY h;
  struct coeff *c;
  int cc;
  POLY node;
  POLY fc;
  MP_INT *bigp;
  MP_INT *tmp;
  struct ring *ringp;

  if (f == POLYNULL) return(f);
  ringp = f->m->ringp;
  if (pcoeff->tag != MP_INTEGER) {
    warningPoly("modulopZ(): pcoeff must be a universalNumber.");
    warningPoly("Returns 0.");
    return(POLYNULL);
  }
  bigp = pcoeff->val.bigp;
  if (f->m->ringp->p != 0 || f->m->ringp->next != (struct ring *)NULL) {
    warningPoly("modulopZ(f,p) must be called with f in the characteristic 0 ring Z[x]. Returns 0.");
    return(POLYNULL);
  }
  if (f->m->ringp->n != ringp->n) {
    warningPoly("modulop(f,ringp): f->m->ringp->n must be equal to ringp->n. Returns 0.");
    return(POLYNULL);
  }

  /* [1] The case of R = ringp->next */
  if (ringp->next != (struct ring *)NULL) {
    warningPoly("modulopZ workds only for flat polynomials. Returns 0.");
    return(POLYNULL);
  }

  /* [2] The case of R = Z */
  node = newCell(newCoeff(),newMonomial(ringp));
  node->next = POLYNULL;
  h = node;
  
  c = newCoeff();
  tmp = newMP_INT();
  while (f != POLYNULL) {
    mpz_mod(tmp,f->coeffp->val.bigp,bigp);
    if (mpz_sgn(tmp) != 0) {
      c->tag = MP_INTEGER;
      c->p = 0;
      c->val.bigp = tmp;
      h->next = newCell(c,monomialCopy(f->m));
      h = h->next;
      h->m->ringp = ringp;

      c = newCoeff();
      tmp = newMP_INT();
    }
    f = f->next;
  }
  return(node->next);
      
}
  
struct pairOfPOLY quotientByNumber(f,pcoeff)
	 POLY f;
	 struct coeff *pcoeff;
	 /* Z[x] ---> Z[x],Z[x] ,  f = first*pcoeff + second */
{
  int p;
  POLY h;
  POLY h2;
  struct coeff *c;
  int cc;
  POLY node;
  struct coeff *c2;
  POLY node2;
  POLY fc;
  MP_INT *bigp;
  MP_INT *tmp;
  MP_INT *tmp2;
  struct ring *ringp;
  struct pairOfPOLY r;

  if (f == POLYNULL) {
    r.first = f; r.second = f;
    return(r);
  }
  ringp = f->m->ringp;
  if (pcoeff->tag != MP_INTEGER) {
    warningPoly("quotientByNumber(): pcoeff must be a universalNumber.");
    warningPoly("Returns (0,0).");
    r.first = f; r.second = f;
    return(r);
  }
  bigp = pcoeff->val.bigp;
  if (f->m->ringp->p != 0 || f->m->ringp->next != (struct ring *)NULL) {
    warningPoly("quotientByNumber(f,p) must be called with f in the characteristic 0 ring Z[x]. Returns (0,0).");
    r.first = f; r.second = f;
    return(r);
  }
  if (f->m->ringp->n != ringp->n) {
    warningPoly("quotientByNumber(f,p): f->m->ringp->n must be equal to ringp->n. Returns 0.");
    r.first = f; r.second = f;
    return(r);
  }

  /* [1] The case of R = ringp->next */
  if (ringp->next != (struct ring *)NULL) {
    warningPoly("quotientByNumber() workds only for flat polynomials. Returns 0.");
    r.first = f; r.second = f;
    return(r);
  }

  /* [2] The case of R = Z */
  node = newCell(newCoeff(),newMonomial(ringp));
  node->next = POLYNULL;
  h = node;
  node2 = newCell(newCoeff(),newMonomial(ringp));
  node2->next = POLYNULL;
  h2 = node2;
  
  c = newCoeff();
  tmp = newMP_INT();
  c2 = newCoeff();
  tmp2 = newMP_INT();
  while (f != POLYNULL) {
    mpz_mod(tmp,f->coeffp->val.bigp,bigp);
    if (mpz_sgn(tmp) != 0) {
      c->tag = MP_INTEGER;
      c->p = 0;
      c->val.bigp = tmp;
      h->next = newCell(c,monomialCopy(f->m));
      h = h->next;
      h->m->ringp = ringp;

      c = newCoeff();
      tmp = newMP_INT();
    }
    mpz_tdiv_q(tmp2,f->coeffp->val.bigp,bigp);
    if (mpz_sgn(tmp2) != 0) {
      c2->tag = MP_INTEGER;
      c2->p = 0;
      c2->val.bigp = tmp2;
      h2->next = newCell(c2,monomialCopy(f->m));
      h2 = h2->next;
      h2->m->ringp = ringp;

      c2 = newCoeff();
      tmp2 = newMP_INT();
    }
    f = f->next;
  }
  r.first = node2->next;
  r.second = node->next;
  return(r);
      
}
  

POLY modulo0(f,ringp)
	 POLY f;
	 struct ring *ringp;
{
  int p;
  POLY h;
  struct coeff *c;
  POLY node;
  if (f == POLYNULL) return(f);
  p = ringp->p;
  if (p != 0) {
    warningPoly("modulo0(f,ringp) must be called with the characteristic 0 ring*ringp. Returns 0.");
    return(POLYNULL);
  }
  switch (f->coeffp->tag) {
  case MP_INTEGER:
    if (f->m->ringp->p == 0) {
      node = pcmCopy(f);
      f = node;
      while (f != POLYNULL) {
		f->m->ringp = ringp; /* Touch the monomial "ringp" field. */
		f = f->next;
      }
      return(node);
    }
    break;
  case POLY_COEFF:
    node = pcmCopy(f);
    f = node;
    while (f != POLYNULL) {
      f->m->ringp = ringp; /* Touch the monomial "ringp" field. */
      f = f->next;
    }
    return(node);
    break;
  case INTEGER:
    node = newCell(newCoeff(),newMonomial(ringp));
    node->next = POLYNULL;
    h = node;
    
    while (f != POLYNULL) {
      c = newCoeff();
      c->tag = MP_INTEGER;
      c->p = 0;
      c->val.bigp = newMP_INT();
      mpz_set_si(c->val.bigp,f->coeffp->val.i);
      h->next = newCell(c,monomialCopy(f->m));
      h = h->next;
      h->m->ringp = ringp;
      f = f->next;
    }
    return(node->next);
    break;
  default:
    warningPoly("modulo0(): coefficients have to be MP_INTEGER or INTEGER. Returns 0");
    return(POLYNULL);
    break;
  }
}
  

struct object test(ob)  /* test3 */
	 struct object ob;
{
  struct object rob = OINIT;
  int k;
  static POLY f0;
  static POLY f1;
  
  POLY addNode;
  POLY f;
  int i;
  static int s=0;
  MP_INT *mp;
  extern struct ring *SmallRingp;
  extern struct ring *CurrentRingp;
  addNode = pMalloc(SmallRingp);
  k = ob.lc.ival;
  switch(s) {
  case 0:
    f0 = addNode;
    for (i=k; i>=0; i--) {
      f0->next = bxx(BiiPower(-k,i),0,i,CurrentRingp);
      if (f0->next != POLYNULL) {
		f0 = f0->next;
      }
    }
    f0 = addNode->next;
    s++;
    rob.lc.poly = f0;
    break;
  case 1:
    f1 = addNode;
    for (i=k; i>=0; i--) {
      f1->next = bxx(BiiPower(k,i),0,i,CurrentRingp);
      if (f1->next != POLYNULL) {
		f1 = f1->next;
      }
    }
    f1 = addNode->next;
    s = 0;
    rob.lc.poly = f1;
    break;
  default:
    rob.lc.poly = POLYNULL;
    s = 0;
    break;
  }

    
  rob.tag = Spoly;
  return(rob);
}


int pLength(f)
	 POLY f;
{
  int c=0;
  if (f ISZERO) return(0);
  while (f != POLYNULL) {
    c++;
    f = f->next;
  }
  return(c);
}


POLY ppAddv2(f,g,top,nexttop)
	 POLY f; POLY g;  /* It breaks f and g. Use it just after calling mpMult() */
	 POLY top;
	 POLY *nexttop;
	 /* top is the starting address in the list f.
		if top == POLYNULL, start from f.

		*nexttop == 0
		== g
		== h or 0

		It must be called as r = ppAddv2(r,g,...); 	    
*/
{
  POLY node;
  struct listPoly nod;
  POLY h;
  struct coeff *c;
  int gt;
  POLY g0;
  POLY f0; /* for debug */

  node = &nod;
  /* printf("ppAddv:1%s + %s\n",POLYToString(f,'*',1),POLYToString(g,'*',1)); */
  node->next = POLYNULL;
  h = node;
  *nexttop = POLYNULL;
  if (f == POLYNULL) return(g);
  if (g == POLYNULL) return(f);
  checkRing(f,g);

  f0 = f;
  if (top != POLYNULL) {
    while (f != top) {
      if (f == POLYNULL) {
		fprintf(stderr,"\nppAddv2(): Internal error.\n");fflush(stderr);
		fprintf(stderr,"f = %s\n",POLYToString(f0,'*',0));
		fprintf(stderr,"g = %s\n",POLYToString(g0,'*',0));
		fprintf(stderr,"top=%s\n",POLYToString(top,'*',0));
		errorPoly("ppAddv2(). Internal error=1.");
      }
      h->next = f;
      h = h->next;
      f = f->next;
    }
  }
  g0 = g;
  *nexttop = g0;
    
  while (f != POLYNULL && g != POLYNULL) {
    checkRing2(f,g); /* for debug */
    /* printf("%s + %s\n",POLYToString(f,'*',1),POLYToString(g,'*',1)); */
    gt = (*mmLarger)(f,g);
    switch (gt) {
    case 1: /* f > g */
      h->next = f;
      h = h->next; f = f->next;;
      if (f == POLYNULL) {
		h->next = g;
		return(node->next);
      }
      break;
    case 0: /* f < g */
      h->next =  g;
      h = h->next; g = g->next;
      if (g == POLYNULL) {
		h->next = f;
		return(node->next);
      }
      break;
    case 2:/* f == g */
      c = g->coeffp;
      Cadd(c,f->coeffp,c);
      if (!isZero(c)) {
		h->next = g;
		h = h->next;
		f = f->next;;
		g = g->next;
		if (f == POLYNULL) {
		  h->next = g;
		  return(node->next);
		}
		if (g == POLYNULL) {
		  h->next = f;
		  return(node->next);
		}
      }else{
		if (g == g0) {
		  if (h != node) {
			*nexttop = h;
		  }else{
			*nexttop = POLYNULL;
		  }
		}
	
		f = f->next;
		g = g->next;

		if (f == POLYNULL) {
		  h->next = g;
		  return(node->next);
		}
		if (g == POLYNULL) {
		  h->next = f;
		  return(node->next);
		}
      }
      break;
    default:
      errorPoly("ppAddv(). Internal error. Invalid value of mmLarger.");
      break;
    }
  }
  return(node->next);
}

POLY ppMult(f,g)
	 POLY f,g;
{
  POLY r;
  POLY tmp;
  POLY top;
  POLY nexttop;
  r = POLYNULL; top = POLYNULL;
  while( f != POLYNULL) {
    /* tmp = (*mpMult)(f,g);  (*mpMult) is no more used. */
    tmp = (*(f->m->ringp->multiplication))(f,g);
    /*printf("mpMult(%s,%s) ->%s\n",POLYToString(f,'*',1),POLYToString(g,'*',1),POLYToString(tmp,'*',1)); */
    r = ppAddv2(r,tmp,top,&nexttop); /* r and tmp will be broken */
    top = nexttop;
    f = f->next;
  }
  return(r);
}

POLY ppMult_poly(f,g)
	 POLY f,g;
{
  POLY r;
  POLY tmp;
  POLY top;
  POLY nexttop;
  r = POLYNULL; top = POLYNULL;
  while( f != POLYNULL) {
    tmp = mpMult_poly(f,g);
    r = ppAddv2(r,tmp,top,&nexttop); /* r and tmp will be broken */
    top = nexttop;
    f = f->next;
  }
  return(r);
}

POLY mapZmonom(f,ringp)
	 POLY f; /* assumes monomial. f \in Z[x] */
	 struct ring *ringp;  /* R[x] */
{
  struct ring *nextRing;
  struct ring nextRing0;
  POLY ff;
  POLY node;
  POLY gg;
  int l,c,d;

  nextRing = ringp->next;
  nextRing0 = *nextRing; nextRing0.p = 0; nextRing0.next = 0;
  /* nextRing0 = Z[y] where y is the variables of R. */

  ff = bxx(f->coeffp->val.bigp,0,0,&nextRing0);
  ff = modulop(ff,nextRing);

  node = newCell(newCoeff(),monomialCopy(f->m));
  node->next = POLYNULL;
  node->m->ringp = ringp;

  node->coeffp->p = ringp->p;

  l = ringp->l; c = ringp->c;
  /* If q-analog  q x ---> (q) x. */
  if (l-c > 0) {
    d = node->m->e[0].x;  /* degree of q in R[x].*/
    node->m->e[0].x = 0;
    gg = cxx(1,0,d,nextRing); /* q^d = x[0]^d */
    ff = ppMult(gg,ff);
  }

  node->coeffp->tag = POLY_COEFF;
  node->coeffp->val.f = ff;
  return(node);
}

POLY reduceContentOfPoly(POLY f,struct coeff **contp) {
  struct coeff *cont;
  struct coeff *cOne = NULL;
  extern struct ring *SmallRingp;
  if (cOne == NULL) cOne = intToCoeff(1,SmallRingp);
  *contp = cOne;
  
  if (f == POLYNULL) return f;
  if (f->m->ringp->p != 0) return f;
  if (f->coeffp->tag != MP_INTEGER) return f;
  cont = gcdOfCoeff(f);
  *contp = cont;
  if (coeffGreater(cont,cOne)) {
	f = quotientByNumber(f,cont).first;
  }
  return f;
}

int coeffSizeMin(POLY f) {
  int size;
  int t;
  if (f == POLYNULL) return 0;
  if (f->m->ringp->p != 0) return 0;
  if (f->coeffp->tag != MP_INTEGER) return 0;
  size = mpz_size(f->coeffp->val.bigp);
  while (f != POLYNULL) {
	t = mpz_size(f->coeffp->val.bigp);
	if (t < size)  size = t;
	if (size == 1) return size;
	f = f->next;
  }
}

struct coeff *gcdOfCoeff(POLY f) {
  extern struct ring *SmallRingp;
  struct coeff *t;
  MP_INT *tmp;
  MP_INT *tmp2;
  static MP_INT *cOne = NULL;
  if (cOne == NULL) {
	cOne = newMP_INT();
	mpz_set_si(cOne,(long) 1);
  }
  if (f == POLYNULL) return intToCoeff(0,SmallRingp);
  if (f->m->ringp->p != 0) return intToCoeff(0,SmallRingp);
  if (f->coeffp->tag != MP_INTEGER) return intToCoeff(0,SmallRingp);
  tmp = f->coeffp->val.bigp;
  tmp2 = newMP_INT();
  while (f != POLYNULL) {
    mpz_gcd(tmp2,tmp,f->coeffp->val.bigp); /* tmp = tmp2 OK? */
	tmp = tmp2;
	if (mpz_cmp(tmp,cOne)==0) return intToCoeff(1,SmallRingp);
	f = f->next;
  }
  return mpintToCoeff(tmp,SmallRingp);
  
}

int shouldReduceContent(POLY f,int ss) {
  extern int DoCancel;
  static int prevSize = 1;
  int size;
  if (f == POLYNULL) return 0;
  if (f->m->ringp->p != 0) return 0;
  if (f->coeffp->tag != MP_INTEGER) return 0;
  if (DoCancel & 2) return 1;
  /* Apply the Noro strategy to reduce content. */
  size = mpz_size(f->coeffp->val.bigp);
  if (ss > 0) {
	prevSize = size;
	return 0;
  }
  if (size > 2*prevSize) {
	return 1;
  }else{
	return 0;
  }
}
