/* $OpenXM: OpenXM/src/kan96xx/Kan/datatype.h,v 1.14 2015/09/29 01:52:14 takayama Exp $ */
#include "gmp.h"
#include "gc.h"

/* GC */
void *GC_malloc(size_t size);
void *GC_realloc(void *p,size_t new);
void *sGC_malloc(size_t size);
void *sGC_realloc(void *p,size_t new);
void *sGC_realloc2(void *p,size_t old,size_t new);
void sGC_free2(void *p,size_t size);
void sGC_free(void *p);
/* six function for kan/protocol/0 */
int KSexecuteString(char *s);
char *KSpopString(void);
int KSset(char *name);
int KSpushBinary(int size,char *data);
char *KSpopBinary(int *size);
void KSstart();
void KSstart_quiet();
void KSstop();

/*********** You may modify these system constants below **********************/
#define N0     100    /* maximal number of variables.   !-VARS=N0  */

/*******************************************************************/

#define INPUTLIMIT 600 /* used for input data */ /* 300 */
#define AGLIMIT 110  /* dbm3.c */   /* 100, 300 */
                     /* NEWSIZE, NEWLIMIET in dbm3.c
                        and OB_ARRAY_MAX, ARGV_WORK_MAX in stackmachine.c 
			must be larger than AGLIMIT. They are automatically
			determined by the value of AGLIMIT. */

#define LARGE_NEGATIVE_NUMBER (-0x7fffffff)  /* for 32 bit */

typedef struct listPoly *   POLY;
typedef struct monomial *   MONOMIAL;
typedef enum {UNKNOWN,INTEGER,MP_INTEGER,POLY_COEFF} coeffType;

/************** definition for the coeff ****************/
union number {
  int i;
  MP_INT *bigp;
  MP_RAT *ratp;
  POLY   f;
};

struct coeff {
  coeffType tag;
  int p;   /* characteristic */
  union number val;
};

/******************************************************/

struct ring {
  int p;
  int n;
  int nn;
  int m;
  int mm;
  int l;
  int ll;
  int c;    /* c must be larger than or equal 1. D[0] is homog. var.
	       cf. mmLarger*/
  int cc;
  char **x;
  char **D;
  char **Dsmall;
  int *order;   /* [i][j] ---> [i*2*N+j] */
  int orderMatrixSize;
  int *from;
  int *to;
  struct ring *next;
  POLY (*multiplication)();
  int schreyer;
  void *gbListTower;
  int *outputOrder;
  char *name;
  int weightedHomogenization;
  int degreeShiftSize;
  int degreeShiftN;
  int *degreeShift;
  int partialEcart;
  int *partialEcartGlobalVarX;

/* To be used. */
  void *ringInInputForm;
/* 2023.08.08 for module order */
  int order_col_size;
  int order_row_size;
  int module_rank;
  int *order_orig;
};


/* exponents */
struct exps {
  int x;
  int D;
};

struct expl {
  int x;
};  
/* linear access to exponent vector */
/* Example: (struct monomial *) f;   ((struct expl)f->exps).x[i] */

struct monomial {
  struct ring *ringp;
  struct exps e[N0];
};

struct monomialDummy {
  struct ring *ringp;
  struct exps e[N0-1];
};

struct smallMonomial {
  struct ring *ringp;
  struct exps e[1];
};

struct listPoly {
  struct listPoly *next;
  struct coeff *coeffp;
  MONOMIAL m;
};


#define MNULL      (MONOMIAL)NULL
#define POLYNULL   (POLY)NULL  
#define ISZERO     == POLYNULL
#define ZERO       POLYNULL

struct pairOfPOLY {
  POLY first;
  POLY second;
};

/*          n
   ----------------------------
m  |                          |
   |                          |
   ----------------------------

   c.f. matrix.h,   mat[i,j] = mat[ i*n + j ]
*/
struct matrixOfPOLY {
  int m;
  int n;
  POLY *mat;
};

struct arrayOfPOLY {
  int n;
  POLY *array;
};



/*  gradedSet */
struct syz0 {
  POLY cf;      /* cf*f + \sum syz(grade,i)*g(grade,i) */
  POLY syz;     /* syz is the element of R[x,y] where R is the current ring. */
                /* cf is the element of R. syz(grade,i) is the coefficient of
		   x^{grade} y^{i}. */
};

struct polySet {
  POLY *g;            /* g[0], g[1], ... are the elements of the set of poly*/
  int *del;           /* del[i] == 1 ---> redundant element. */
  struct syz0 **syz;  /* syz[i] is the syzygy to get g[i]. */
  int *mark;          /* if (mark[i]), then syz[i] is simplified. */
  int *serial;        /* serial[i]=k ===> g[i] is input[k] */
  int size;           /* size of this set. i.e., g[0], g[1], ..., g[size-1] */
  int lim;
  POLY *gh;           /* gh[i] = homogenize(g[i]) for ecart division */
  int *gen;           /* gen[i] == 1 --> given generators */
  POLY *gmod;         /* gmod = g mod p for TraceLift. */
};

struct pair {
  POLY lcm;        /* lcm of i and j */
  int ig; int ii;  /* grade and index of i. */
  int jg; int ji;  /* grade and index of j. */
  int del;
  int grade;       /* grade of lcm */
  struct pair *next;
  struct pair *prev;
  POLY syz; /* if the sp(i,j)-->0, the division process is stored. */
};

struct gradedPolySet {
  struct polySet **polys;  /* polys[0]: grade=0, polys[1]:grade=1, ... */
  int maxGrade;            /* maximal grade in this set */
  int lim;
  int gb;  /* it is gb or not. */
  int reduced; /* it is reduced gb or not. */
};

struct gradedPairs {
  struct pair **pairs;    /* pairs[0]: grade=0, .... */
  int maxGrade;
  int lim;
};

struct spValue {
  /* POLY sp;      sp(i,j) = a*i+b*j */
  POLY a;    
  POLY b;
};

struct monomialSyz {
  int i;
  int j;
  int deleted;
  POLY a;
  POLY b;
};

struct arrayOfMonomialSyz {
  int size;
  int limit;
  struct monomialSyz **p;
};

  


