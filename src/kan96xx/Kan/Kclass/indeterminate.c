/* $OpenXM$ */
/* Kclass/indeterminate.c    */
/* This file handles   indeterminate, tree, recursivePolynomial,
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
  struct object rob;
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
  struct object ob;

  ob = KpoString(s);  /* We do not clone s */
  return(KpoIndeterminate(ob));
}


/* Printing function : see fprintClass */
void fprintIndeterminate(FILE *fp,struct object op)
{
  printObject(KopIndeterminate(op),0,fp);
}


/* ---------------------------------------------------- */
/* Data conversion function : see KclassDataConversion*/
struct object KpoTree(struct object ob) {
  struct object rob;
  struct object ob1,ob2,ob3;
  struct object *newobp;
  rob.tag = Sclass;
  rob.lc.ival = CLASSNAME_tree;
  newobp = (struct object *) sGC_malloc(sizeof(struct object));
  if (newobp == NULL) errorKan1("%s\n","Kclass/indeterminate.c, no more memory.");
  if (ob.tag != Sarray) {
    errorKan1("%s\n","Kclass/indeterminate.c, only properly formatted list object can be transformed into tree. [name, cdname, arglist].");
  }
  if (getoaSize(ob) < 3) {
    errorKan1("%s\n","Kclass/indeterminate.c, the length must 3 or more than 3. [name, cdname, arglist].");
  }
  ob1 = getoa(ob,0); ob2 = getoa(ob,1); ob3 = getoa(ob,2);
  if (ob1.tag != Sdollar || ob2.tag != Sdollar || ob3.tag != Sarray) {
    errorKan1("%s\n","Kclass/indeterminate.c, [string name, string cdname, list arglist].");
  }
  *newobp = ob;
  rob.rc.voidp = newobp;
  return(rob);
}


/* Printing function : see fprintClass */
void fprintTree(FILE *fp,struct object op)
{
  printObject(KopTree(op),0,fp);
}

int isTreeAdd(struct object ob) {
  struct object name;
  if (ob.tag != Sclass) {
    return(0);
  }
  if (ectag(ob) != CLASSNAME_tree) {
    return(0);
  }
  ob = KopTree(ob);
  if (ob.tag != Sarray) {
    errorKan1("%s\n","CLASSNAME_tree is broken. Should be array.");
  }
  name = getoa(ob,0);
  if (name.tag != Sdollar) {
    errorKan1("%s\n","CLASSNAME_tree is broken. Should be string.");
  }
  if (strcmp(KopString(name),"add") == 0) {
    return(1);
  }else{
    return(0);
  }
}

struct object addTree(struct object ob1, struct object ob2)
{
  struct object rob,aob;
  struct object ob3,ob4;
  int i;
  if (isTreeAdd(ob1) && !isTreeAdd(ob2)) {
    ob1 = KopTree(ob1);
    ob3 = getoa(ob1,2);
    aob = newObjectArray(getoaSize(ob3)+1);
    for (i=0; i<getoaSize(ob3); i++) {
      putoa(aob,i,getoa(ob3,i));
    }
    putoa(aob,getoaSize(ob3),ob2);
  }else if (!isTreeAdd(ob1) && isTreeAdd(ob2)) {
    ob2 = KopTree(ob2);
    ob3 = getoa(ob2,2);
    aob = newObjectArray(getoaSize(ob3)+1);
    putoa(aob,0,ob1);
    for (i=0; i<getoaSize(ob3); i++) {
      putoa(aob,1+i,getoa(ob3,i));
    }
  }else if (isTreeAdd(ob1) && isTreeAdd(ob2)) {
    ob1 = KopTree(ob1);
    ob2 = KopTree(ob2);
    ob3 = getoa(ob1,2);
    ob4 = getoa(ob2,2);
    aob = newObjectArray(getoaSize(ob3)+getoaSize(ob4));
    for (i=0; i<getoaSize(ob3); i++) {
      putoa(aob,i,getoa(ob3,i));
    }
    for (i=0; i<getoaSize(ob4); i++) {
      putoa(aob,getoaSize(ob3)+i,getoa(ob4,i));
    }
  }else{
    aob = newObjectArray(2);
    putoa(aob,0,ob1);
    putoa(aob,1,ob2);
  }
  rob = newObjectArray(3);
  putoa(rob,0,KpoString("add"));
  putoa(rob,1,KpoString("Basic"));
  putoa(rob,2,aob);
  return(KpoTree(rob));
}


/*------------------------------------------*/

struct object KpoRecursivePolynomial(struct object ob) {
  struct object rob;
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
      fprintf(fp,"%s",KopString(getoa(vlist,i)));
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
  struct object ob;
  struct object vlist;
  struct object body;
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
  struct object rob;
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
  struct object vlist,vlist2;
  struct object ob1,ob2,ob3,ob4;
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
  struct object v;
  struct object c;
  struct object e;
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
  struct object tmp;
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
  struct object ob1, ob2,tmp;
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
    if (tmp.tag != Sdollar) {
      fprintf(stderr,"%s [list vlist, body]. Element of the vlist must be a string.\n",s); printObject(ob,1,stderr);
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







