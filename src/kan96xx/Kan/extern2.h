/* $OpenXM: OpenXM/src/kan96xx/Kan/extern2.h,v 1.11 2006/12/21 05:29:49 takayama Exp $ */
/* poly.c */
void KinitKan(void);
MONOMIAL newMonomial(struct ring *ringp);
MONOMIAL monomialCopy(MONOMIAL m);
struct coeff *newCoeff(void);
MP_INT *newMP_INT();
POLY newCell(struct coeff *c,MONOMIAL m);
POLY cxx(int c,int i,int k,struct ring *ringp);
POLY bxx(MP_INT *c,int i,int k,struct ring *ringp); /* *c is not copied */
POLY cdd(int c,int i,int k,struct ring *ringp);
POLY bdd(MP_INT *c,int i,int k,struct ring *ringp); /* *c is not copied */
void errorPoly(char *s);
void warningPoly(char *s);

/** Note that the results of all functions are read only except
  ppMult, mpMult, sp and constructors.
  The results of ppMult, mpMult, sp and constructors may be rewritten.
**/
/* poly2.c */
POLY ppAdd(POLY f,POLY g);   
POLY ppSub(POLY f,POLY g);
POLY ppMult(POLY f,POLY g);
POLY ppMult_poly(POLY f,POLY g);
POLY (*mpMult)(POLY f,POLY g);
POLY mpMult_poly(POLY f,POLY g); 
MONOMIAL (*monomialAdd)(MONOMIAL f,MONOMIAL g);
MONOMIAL monomialAdd_poly(MONOMIAL f,MONOMIAL g);
POLY ppAddv(POLY f,POLY g);  /* non-reentrant. It breaks f and g */
POLY ppAddv2(POLY f,POLY g,POLY top,POLY *nexttop); 
/* non-reentrant. It breaks f and g */
POLY pCopy(POLY f);  /* shallow copy of poly */
POLY pcCopy(POLY f); /* shallow copy of poly and coeff */
POLY pmCopy(POLY f); /* shallow copy of poly and monomial */
POLY pcmCopy(POLY f);/* Deep copy of poly, coeff and monomial */
POLY head(POLY f);
void pvSort(POLY f);
POLY pPower(POLY f,int k);
POLY pPower_poly(POLY f,int k);
POLY pcDiv(POLY f,struct coeff *c);
POLY cpMult(struct coeff *c,POLY f); /* non-reentrant */
POLY modulop(POLY f,struct ring *ringp);
POLY modulo0(POLY f,struct ring *ringp);
POLY modulopZ(POLY f,struct coeff *pcoeff);
struct pairOfPOLY quotientByNumber(POLY f,struct coeff *pcoeff);
int pLength(POLY f);
struct coeff *gcdOfCoeff(POLY f);
int coeffSizeMin(POLY f);
POLY reduceContentOfPoly(POLY f,struct coeff **contp);
int shouldReduceContent(POLY f,int ss);


/* poly3.c */
void initT(void);
void makeTable(struct coeff *c,struct exps e[],struct ring *ringp);
void monomialMult_diff(struct exps e[],POLY f);
POLY mpMult_diff(POLY f,POLY g);  /* It is non-reentrant!!! */
POLY mpMult_difference(POLY f,POLY g); 
/* replace.c */
POLY mReplace(POLY f,int lSideX[],POLY rSideX[],int sizex,
	             int lSideD[],POLY rSideD[],int sized,int commutative);
POLY replace(POLY f,POLY lRule[],POLY rRule[],int num);
POLY replace_poly(POLY f,POLY lRule[],POLY rRule[],int num);

/* poly4.c */
struct matrixOfPOLY *parts(POLY f,POLY v);
int pDegreeWrtV(POLY f,POLY v);
POLY homogenize(POLY f);
POLY homogenize_vec(POLY f);
int isHomogenized(POLY f);
int isHomogenized_vec(POLY f);
int containVectorVariable(POLY f);
POLY POLYToPrincipalPart(POLY f);
POLY POLYToInitW(POLY f,int w[]);
POLY POLYToInitWS(POLY f,int w[],int s[]);
int ordWsAll(POLY f,int w[],int s[]);

POLY polyGCD(POLY f,POLY g);
int isTheSameRing(struct ring *rstack[], int rp, struct ring *newRingp);
POLY goDeHomogenizeS(POLY f);
POLY goHomogenize(POLY f,int u[],int v[],int ds[],int dssize,int ei,int onlyS);
POLY goHomogenize11(POLY f,int ds[],int dssize,int ei,int onlyS);
POLY goHomogenize_dsIdx(POLY f,int u[],int v[],int dsIdx,int ei,int onlyS);
POLY goHomogenize11_dsIdx(POLY f,int ds[],int dsIdx,int ei,int onlyS);
struct ring *newRingOverFp(struct ring *rp, int p);
int getPrime(int p);

/* coeff.c */
char *intToString(int i);
char *coeffToString(struct coeff *cp);
struct coeff *intToCoeff(int i,struct ring *ringp);
int coeffToInt(struct coeff *cp);
struct coeff *mpintToCoeff(MP_INT *b,struct ring *ringp);
struct coeff *polyToCoeff(POLY f,struct ring *ringp);
struct coeff *coeffNeg(struct coeff *c,struct ring *ringp);
void errorCoeff(char *s);
void warningCoeff(char *str);
void Cadd(struct coeff *r,struct coeff *a,struct coeff *b);
void Csub(struct coeff *r,struct coeff *a,struct coeff *b);
void Cmult(struct coeff *r,struct coeff *a,struct coeff *b);
void Cdiv(struct coeff *r,struct coeff *a,struct coeff *b);
MP_INT *BiiComb(int p,int q);  /* read only */
int iiComb(int p,int q,int P);
void CiiComb(struct coeff *r,int p,int q); /* r->val.bigp is read only */
MP_INT *BiiPoch(int p,int k);  /* read only */
int iiPoch(int p,int k,int P);
void CiiPoch(struct coeff *r,int p,int k); /* r->val.bigp is read only */
MP_INT *BiiPower(int p,int k);  /* read only */
int iiPower(int p,int k,int P);
void CiiPower(struct coeff *r,int p,int k); /* r->val.bigp is read only */
struct coeff *coeffCopy(struct coeff *c);
struct coeff *stringToUniversalNumber(char *s,int *flagp);
struct coeff *newUniversalNumber(int i);
struct coeff *newUniversalNumber2(MP_INT *i);
#define is_this_coeff_MP_INT(c) ((c)->tag == MP_INTEGER?1:0)
#define coeff_to_MP_INT(c) ((c)->val.bigp)
int coeffEqual(struct coeff *c,struct coeff *d);
int coeffGreater(struct coeff *c,struct coeff *d);
POLY coeffToPoly(struct coeff *c,struct ring *rp);
void universalNumberDiv(struct coeff *q,struct coeff *a, struct coeff *b);
int isZero(struct coeff *a);
struct coeff *universalNumberGCD(struct coeff *a,struct coeff *b);


/* output.c */
char *POLYToString(POLY f,int multSym,int brace);
int isOne(struct coeff *cp);
int isMinusOne(struct coeff *cp);
int isNegative(struct coeff *cp);
int isConstant(POLY f);
int isConstantAll(POLY f);
void errorOutput(char *s);
int validOutputOrder(int w[],int n);
char **makeDsmall(char **dvars,int n);


/* order.c */
void setOrderByMatrix(int order[],int n,int c, int l,int omsize);
void printOrder(struct ring *ringp);
void showRing(int level,struct ring *ringp);
int (*mmLarger)(POLY f,POLY g);
int mmLarger_matrix(POLY f,POLY g);
int mmLarger_pureLexicographic(POLY f, POLY g);
int mmLarger_tower(POLY f,POLY g);
void setFromTo(struct ring *ringp);

/* switch.c */
void print_switch_status(void);
char *switch_function(char *fun,char *arg);
void switch_init(void);
void switch_mmLarger(char *arg);
void switch_mpMult(char *arg);
void switch_monomialAdd(char *arg);
void switch_red(char *arg);
void switch_sp(char *arg);
void switch_groebner(char *arg);
void switch_grade(char *arg);
void switch_isSameComponent(char *arg);

/* parser.c */
POLY stringToPOLY(char *s,struct ring *ringp);
char *str2strPass0(char *s,struct ring *rp);

/* matrix.c : Arguments are read only.*/
struct arrayOfPOLY *aaAdd(struct arrayOfPOLY *aa,struct arrayOfPOLY *bb);
struct matrixOfPOLY *aaMult(struct matrixOfPOLY *aa,struct matrixOfPOLY *bb);
struct arrayOfPOLY *newArrayOfPOLY(int size);
struct matrixOfPOLY *newMatrixOfPOLY(int m,int n);
struct arrayOfPOLY *carrayToArrayOfPOLY(POLY a[],int size);
void errorMatrix(char *s);

/* redm.c (red.c for modules) */
int isReducible_module(POLY f,POLY g);
int (*isSameComponent)(POLY f,POLY g);
int isSameComponent_x(POLY f,POLY g);
int isSameComponent_xd(POLY f,POLY g);
POLY lcm_module(POLY f,POLY g);
int grade_module1(POLY f);
int grade_module1v(POLY f);
int grade_module2(POLY f);
int grade_firstvec(POLY f);
int grade_sugar(POLY f);
int eliminated(POLY f);
int isOrdered(POLY f);
int dGrade(POLY f);
int dGrade1(POLY f);
int uvGrade(POLY f, int u[],int v[],int ds[],int dssize,int ei);
int uvGrade1(POLY f, int u[],int v[],int ds[],int dssize,int ei);

/* resol.c */
struct arrayOfMonomialSyz schreyerSkelton(struct arrayOfPOLY g);
struct monomialSyz *newMonomialSyz(void);
struct arrayOfMonomialSyz enlargeArrayOfMonomialSyz(struct arrayOfMonomialSyz p);
						    

/* from gradedset.h */
int (*grade)(POLY f);

/* macro */
#define pMalloc(ringp) newCell(newCoeff(),newMonomial(ringp))

#define xset0(f,i) f->m->e[i].x = 0
#define dset0(f,i) f->m->e[i].D = 0

#define checkRing2(f,g) {\
  if ((f)->m->ringp != (g)->m->ringp) {\
    fprintf(stderr,"f and g must be in a same ring. Type in ctrl-\\");\
    getchar(); getchar(); exit(1);\
    }\
}

#define checkRing(f,g) {\
  if ((f)->m->ringp != (g)->m->ringp) {\
    warningPoly("f and g must be in a same ring. Returns 0.\n");\
    fprintf(stderr,"f=%s\n",POLYToString(f,'*',0));\
    fprintf(stderr,"g=%s\n",POLYToString(g,'*',0));\
    return(POLYNULL);\
    }\
}

