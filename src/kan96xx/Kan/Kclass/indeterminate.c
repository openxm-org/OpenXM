/* $OpenXM: OpenXM/src/kan96xx/Kan/Kclass/indeterminate.c,v 1.7 2003/11/21 02:10:37 takayama Exp $ */
/* Kclass/indeterminate.c    */
/* This file handles   indeterminate, recursivePolynomial,
    polynomialInOneVariable
*/
#include <stdio.h>
#include "../datatype.h"
#include "../stackm.h"
#include "../extern.h"
#include "../gradedset.h"
#include "../extern2.h"
#include "../kclass.h"


/* Data conversion function : see KclassDataConversion*/
struct object KpoIndeterminate(struct object ob) {
  struct object rob = OINIT;
  struct object *newobp;
  rob.tag = Sclass;
  rob.lc.ival = CLASSNAME_indeterminate;
  newobp = (struct object *) sGC_malloc(sizeof(struct object));
  if (newobp == NULL) errorKan1("%s\n","Kclass/indeterminate.c, no more memory.");
  if (ob.tag != Sdollar) {
    errorKan1("%s\n","Kclass/indeterminate.c, only String object can be transformed into indeterminate.");
  }
  *newobp = ob;
  rob.rc.voidp = newobp;
  return(rob);
}

/* The second constructor. */
struct object KnewIndeterminate(char *s) {
  struct object ob = OINIT;

  ob = KpoString(s);  /* We do not clone s */
  return(KpoIndeterminate(ob));
}


/* Printing function : see fprintClass */
void fprintIndeterminate(FILE *fp,struct object op)
{
  printObject(KopIndeterminate(op),0,fp);
}

/* Functions for trees are moved to tree.c */
/* ---------------------------------------------------- */

struct object KpoRecursivePolynomial(struct object ob) {
  struct object rob = OINIT;
  struct object *newobp;
  rob.tag = Sclass;
  rob.lc.ival = CLASSNAME_recursivePolynomial;
  newobp = (struct object *) sGC_malloc(sizeof(struct object));
  if (newobp == NULL) errorKan1("%s\n","Kclass/indeterminate.c, no more memory.");
  if (ob.tag != Sarray) {
    errorKan1("%s\n","Kclass/indeterminate.c, only array object can be transformed into recusivePolynomial.");
  }
  *newobp = ob;
  rob.rc.voidp = newobp;
  return(rob);
}

static void  printBodyOfRecursivePolynomial(struct object body,
                                            struct object vlist, FILE *fp)
{
  int i,j;
  int k;
  if (ectag(body) != CLASSNAME_polynomialInOneVariable) {
    printObject(body,0,fp);
    return;
  }
  body = KopPolynomialInOneVariable(body);
  if (body.tag != Sarray) {
    errorKan1("%s\n","Kclass/indeterminate.c, format error for recursive polynomial.");
  }
  if (getoaSize(body) == 0) {
    errorKan1("%s\n","printBodyOfRecursivePolynomial: format error for a recursive polynomial.");
  }
  i = KopInteger(getoa(body,0));
  for (j=1; j<getoaSize(body); j = j+2) {
    k = KopInteger(getoa(body,j));
    if (k != 0) {
      if (getoa(vlist,i).tag == Sdollar) {
        fprintf(fp,"%s",KopString(getoa(vlist,i)));
      }else if (ectag(getoa(vlist,i)) == CLASSNAME_tree) {
        fprintClass(fp,getoa(vlist,i));
      }else{
        errorKan1("%s\n","printBodyOfRecursivePolynomial: format error.");
      }
      if (k > 1) {
        fprintf(fp,"^%d ",k);
      }else if (k == 1) {
      }else{
        fprintf(fp,"^(%d) ",k);
      }
      fprintf(fp," * ");
    }
    fprintf(fp,"(");
    printBodyOfRecursivePolynomial(getoa(body,j+1),vlist,fp);
    fprintf(fp,")");
    if (j != getoaSize(body)-2) {
      fprintf(fp," + ");
    }
  }
  return;
}
  
void fprintRecursivePolynomial(FILE *fp,struct object op)
{
  /* old  code 
     printObject(KopRecursivePolynomial(op),0,fp); return;
  */
  struct object ob = OINIT;
  struct object vlist = OINIT;
  struct object body = OINIT;
  ob = KopRecursivePolynomial(op);
  if (ob.tag != Sarray) {
    printObject(ob,0,fp); return;
  }
  if (!isRecursivePolynomial2(op)) {
    printObject(KopRecursivePolynomial(op),0,fp); return;
  }
  vlist = getoa(ob,0);
  body = getoa(ob,1);
  printBodyOfRecursivePolynomial(body,vlist,fp);
  return;
}

/*------------------------------------------*/

struct object KpoPolynomialInOneVariable(struct object ob) {
  struct object rob = OINIT;
  struct object *newobp;
  rob.tag = Sclass;
  rob.lc.ival = CLASSNAME_polynomialInOneVariable;
  newobp = (struct object *) sGC_malloc(sizeof(struct object));
  if (newobp == NULL) errorKan1("%s\n","Kclass/indeterminate.c, no more memory.");
  if (ob.tag != Sarray) {
    errorKan1("%s\n","Kclass/indeterminate.c, only array object can be transformed into polynomialInOneVariable.");
  }
  *newobp = ob;
  rob.rc.voidp = newobp;
  return(rob);
}

void fprintPolynomialInOneVariable(FILE *fp,struct object op)
{
  printObject(KopPolynomialInOneVariable(op),0,fp);
}

struct object polyToRecursivePoly(struct object p) {
  struct object rob = NullObject;
  int vx[N0], vd[N0];
  int i,j,k,n,count;
  POLY f;
  struct object vlist = OINIT;
  struct object vlist2 = OINIT;
  struct object ob1 = OINIT;
  struct object ob2 = OINIT;
  struct object ob3 = OINIT;
  struct object ob4 = OINIT;
  int vn;

  if (p.tag != Spoly) return(rob);
  f = KopPOLY(p);
  if (f == ZERO) {
    rob = p; return(rob);
  }
  /* construct list of variables. */
  for (i=0; i<N0; i++) {
    vx[i] = vd[i] = 0;
  }
  n = f->m->ringp->n; count = 0;
  for (i=0; i<n; i++) {
    if (pDegreeWrtV(f,cxx(1,i,1,f->m->ringp))) {
      vx[i] = 1; count++;
    }
    if (pDegreeWrtV(f,cdd(1,i,1,f->m->ringp))) {
      vd[i] = 1; count++;
    }
  }
  vlist = newObjectArray(count); k = 0;
  vlist2 = newObjectArray(count); k = 0;
  for (i=0; i<n; i++) {
    if (vd[i]) {
      putoa(vlist,k,KpoPOLY(cdd(1,i,1,f->m->ringp)));
      putoa(vlist2,k,KpoString(POLYToString(cdd(1,i,1,f->m->ringp),'*',0)));
      k++;
    }
  }
  for (i=0; i<n; i++) {
    if (vx[i]) {
      putoa(vlist,k,KpoPOLY(cxx(1,i,1,f->m->ringp)));
      putoa(vlist2,k,KpoString(POLYToString(cxx(1,i,1,f->m->ringp),'*',0)));
      k++;
    }
  }
  /* printObject(vlist,1,stdout); */
  if (getoaSize(vlist) == 0) {
    vn = -1;
  }else{
    vn = 0;
  }
  ob1 = polyToRecursivePoly2(p,vlist,vn);
  rob = newObjectArray(2);
  putoa(rob,0,vlist2); putoa(rob,1,ob1);
  /* format of rob
     [ list of variables, poly or universalNumber or yyy to express
     a recursive polynomial. ]
     format of yyy = CLASSNAME_polynomialInOneVariable            
     [Sinteger,    Sinteger, coeff obj, Sinteger, coeff obj, .....]
     name of var, exp,      coeff,     exp,      coeff
     This format is checked by isRecursivePolynomial2().
  */
  rob = KpoRecursivePolynomial(rob);
  if (isRecursivePolynomial2(rob)) {
    return(rob);
  }else{
    errorKan1("%s\n","polyToRecursivePolynomial could not translate this object.");
  }
}

static void objectFormatError_ind0(char *s) {
  char tmp[1024];
  sprintf(tmp,"polyToRecursivePoly2: object format error for the variable %s",s);
  errorKan1("%s\n",tmp);
}

struct object polyToRecursivePoly2(struct object p,struct object vlist, int vn) {
  struct object rob = NullObject;
  POLY f;
  POLY vv;
  struct object v = OINIT;
  struct object c = OINIT;
  struct object e = OINIT;
  int i;


  if (p.tag != Spoly) return(rob);
  f = KopPOLY(p);
  if (f == ZERO) {
    rob = p; return(rob);
  }
  if (vn < 0 || vn >= getoaSize(vlist)) {
    return(coeffToObject(f->coeffp));
  }
  v = getoa(vlist,vn);
  if (v.tag != Spoly) objectFormatError_ind0("v");
  vv = KopPOLY(v);
  c = parts2(f,vv);
  e = getoa(c,0);  /* exponents. Array of integer. */
  if (e.tag != Sarray) objectFormatError_ind0("e");
  c = getoa(c,1);  /* coefficients. Array of POLY. */
  if (c.tag != Sarray) objectFormatError_ind0("c");
  rob = newObjectArray(getoaSize(e)*2+1);

  putoa(rob,0,KpoInteger(vn)); /* put the variable number. */
  for (i=0; i < getoaSize(e); i++) {
    putoa(rob,1+i*2, getoa(e,i));
    putoa(rob,1+i*2+1, polyToRecursivePoly2(getoa(c,i),vlist,vn+1));
  }
  /* printObject(rob,0,stderr); */
  return(KpoPolynomialInOneVariable(rob));
}

static int isRecursivePolynomial2a(struct object ob2, int n) {
  char *s = "Format error (isRecursivePolynomial2a) : ";
  struct object tmp = OINIT;
  int i;
  if (ectag(ob2) == CLASSNAME_polynomialInOneVariable) {
    ob2 = KopPolynomialInOneVariable(ob2);
  }else if (ob2.tag == Sarray) {
    fprintf(stderr,"isRecursivePolynomial2, argument is an array.\n");
    printObject(ob2,0,stderr);
    fprintf(stderr,"\n");
    return(0);   /* Array must be an error, but other objects are OK. */
  }else {
    return(1);
  }
  if (ob2.tag != Sarray) {
    return(1);
    /* coeff can be any. */
  }
  if (getoaSize(ob2) % 2 == 0) {
    fprintf(stderr,"%s list body. The size of body must be odd.\n",s); printObject(ob2,1,stderr);
    return(0);
  }
  tmp = getoa(ob2,0);
  if (tmp.tag != Sinteger) {
    fprintf(stderr,"%s list body. body[0] must be integer.\n",s); printObject(ob2,1,stderr);
    return(0);
  }
  if (KopInteger(tmp) < 0 || KopInteger(tmp) >= n) {
    fprintf(stderr,"%s list body. body[0] must be integer between 0 and the size of vlist -1.\n",s); printObject(ob2,1,stderr);
    return(0);
  }
  for (i=1; i<getoaSize(ob2); i = i+2) {
    tmp = getoa(ob2,i);
    if (tmp.tag != Sinteger) {
      fprintf(stderr,"%s [list vlist, list body]. body[%d] must be integer.\n",s,i);
      printObject(ob2,1,stderr);
      return(0);
    }
  }
  for (i=2; i<getoaSize(ob2); i = i+2) {
    tmp = getoa(ob2,i);
    if (ectag(tmp) == CLASSNAME_polynomialInOneVariable) {
      if (isRecursivePolynomial2a(tmp,n)) {
      }else{
        fprintf(stderr,"isRecursivePolynomial2a: entry is not a polynomial in one variable.\n");
        printObject(tmp,0,stderr); fprintf(stderr,"\n");
        return(0);
      }
    }
  }
  return(1);
}
    
int isRecursivePolynomial2(struct object ob) {
  /* This checks only the top level */
  char *s = "Format error (isRecursivePolynomial2) : ";
  struct object ob1 = OINIT;
  struct object ob2 = OINIT;
  struct object tmp = OINIT;
  int i;
  int n;
  if (ob.tag != Sclass) return(0);
  if (ectag(ob) != CLASSNAME_recursivePolynomial) return(0);
  ob = KopRecursivePolynomial(ob);
  if (ob.tag != Sarray) {
    fprintf(stderr,"%s [vlist, body]\n",s); printObject(ob,1,stderr);
    return(0);
  }
  if (getoaSize(ob) != 2) {
    fprintf(stderr,"%s [vlist, body]. The length must be 2. \n",s);
    printObject(ob,1,stderr);
    return(0);
  }
  ob1 = getoa(ob,0);
  ob2 = getoa(ob,1);
  if (ob1.tag != Sarray) {
    fprintf(stderr,"%s [list vlist, body].\n",s); printObject(ob,1,stderr);
    return(0);
  }
  n = getoaSize(ob1);
  for (i=0; i<n; i++) {
    tmp = getoa(ob1,i);
    if (tmp.tag == Sdollar) {
    }else if (ectag(tmp) == CLASSNAME_tree) {
    }else{
      fprintf(stderr,"%s [list vlist, body]. Element of the vlist must be a string or a tree.\n",s); printObject(ob,1,stderr);
      return(0);
    }
  }
  return(isRecursivePolynomial2a(ob2,n));
}
    
  
struct object coeffToObject(struct coeff *cp) {
  struct object rob = NullObject;
  switch(cp->tag) {
  case INTEGER:
    rob = KpoInteger( coeffToInt(cp) );
    return(rob);
    break;

  case MP_INTEGER:
    rob.tag = SuniversalNumber;
    rob.lc.universalNumber = newUniversalNumber2((cp->val).bigp);
    return(rob);
    break;

  case POLY_COEFF:
    rob = KpoPOLY((cp->val).f);
    return(rob);
    break;

  default:
    return(rob);
  }
}

  
struct object recursivePolyToPoly(struct object rp) {
  struct object rob = NullObject;
  POLY f;
  errorKan1("%s\n","recursivePolyToPoly() has not yet been implemented. Use ascii parsing or sm1 macros to reconstruct a polynomial.");
  
  return(rob);
}


struct object KrvtReplace(struct object rp_o,struct object v_o, struct object t_o) {
  /* rp_o : recursive polynomial.
     v_o  : variable name (indeterminate).
     t_o  : tree.
  */
  struct object rp = OINIT;
  struct object vlist = OINIT;
  struct object newvlist = OINIT;
  struct object newrp = OINIT;
  int i,m;
  /* Check the data types. */
  if (ectag(rp_o) != CLASSNAME_recursivePolynomial) {
    errorKan1("%s\n","KrvtReplace() type mismatch in the first argument.");
  }
  if (ectag(v_o) != CLASSNAME_indeterminate) {
    errorKan1("%s\n","KrvtReplace() type mismatch in the second argument.");
  }
  if (ectag(t_o) != CLASSNAME_tree) {
    errorKan1("%s\n","KrvtReplace() type mismatch in the third argument.");
  }
  
  rp = KopRecursivePolynomial(rp_o);
  vlist = getoa(rp,0);
  m = getoaSize(vlist);
  newvlist = newObjectArray(m);
  for (i=0; i<m; i++) {
    if (KooEqualQ(getoa(vlist,i),KopIndeterminate(v_o))) {
      /* should be KooEqualQ(getoa(vlist,i),v_o). It's not a bug.
         Internal expression of vlist is an array of string
         (not indetermiante). */
      putoa(newvlist,i,t_o);
    }else{
      putoa(newvlist,i,getoa(vlist,i));
    }
  }
  newrp = newObjectArray(getoaSize(rp));
  m = getoaSize(rp);
  putoa(newrp,0,newvlist);
  for (i=1; i<m; i++) {
    putoa(newrp,i,getoa(rp,i));
  }
  return(KpoRecursivePolynomial(newrp));
}


struct object KreplaceRecursivePolynomial(struct object of,struct object rule) {
  struct object rob = OINIT;
  struct object f = OINIT;
  int i;
  int n;
  struct object trule = OINIT;
  

  if (rule.tag != Sarray) {
    errorKan1("%s\n"," KreplaceRecursivePolynomial(): The second argument must be array.");
  }
  n = getoaSize(rule);

  if (of.tag ==Sclass && ectag(of) == CLASSNAME_recursivePolynomial) {
  }else{
    errorKan1("%s\n"," KreplaceRecursivePolynomial(): The first argument must be a recursive polynomial.");
  }
  f = of;

  for (i=0; i<n; i++) {
    trule = getoa(rule,i);
    if (trule.tag != Sarray) {
      errorKan1("%s\n"," KreplaceRecursivePolynomial(): The second argument must be of the form [[a b] [c d] ....].");
    }
    if (getoaSize(trule) != 2) {
      errorKan1("%s\n"," KreplaceRecursivePolynomial(): The second argument must be of the form [[a b] [c d] ....].");
    }

    if (ectag(getoa(trule,0)) != CLASSNAME_indeterminate) {
      errorKan1("%s\n"," KreplaceRecursivePolynomial(): The second argument must be of the form [[a b] [c d] ....] where a,b,c,d,... are polynomials.");
    }
    /* Do not check the second argument. */
    /*
      if (getoa(trule,1).tag != Spoly) {
      errorKan1("%s\n"," KreplaceRecursivePolynomial(): The second argument must be of the form [[a b] [c d] ....] where a,b,c,d,... are polynomials.");
      }
    */

  }

  rob = f;
  for (i=0; i<n; i++) {
    trule = getoa(rule,i);
    rob = KrvtReplace(rob,getoa(trule,0),getoa(trule,1));
  }
  return(rob);
}

