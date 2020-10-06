/* $OpenXM: OpenXM/src/kan96xx/Kan/gradedset.h,v 1.8 2005/06/16 06:54:55 takayama Exp $ */
/* gradedset.h */
#define max(a,b) (a>b?a:b)

/* You may use only ppAdd() and cpMult() for the elements of SyzRingp. */
#define srGrade(s) ((s)->m->e[0].x)  /* s is in the SyzRingp */
#define srIndex(s) ((s)->m->e[0].D)
#define srSyzCoeffToPOLY(s) ((s)->val.f)


/* gradedset.c */
void errorGradedSet(char *s);
void warningGradedSet(char *s);
struct polySet *newPolySet(int n);
struct pair *newPair(struct pair *prev);
struct pair *pairCopy(struct pair *node);
struct gradedPolySet *newGradedPolySet(int n);
struct gradedPairs *newGradedPairs(int n);
struct gradedPairs *enlargeGradedPairs(int size,struct gradedPairs *grD);
void insertPair(struct pair *inode,struct pair *before);
                  /* insert inode after before */
struct pair *deletePair(struct pair *p); /* delete p->next and returns
					    p->next */
struct pair *getPair(struct gradedPairs *grD);
void whereInG(struct gradedPolySet *g, POLY fi, int *gradep, int *indexp,int sugar);
struct gradedPolySet *putPolyInG(struct gradedPolySet *g, POLY fi, int grade,
				 int index,struct syz0 *syz,int mark,int serial);
void markRedundant(struct gradedPolySet *g,POLY gt,int grade,int index,int sugar);
void markRedundant0(struct gradedPolySet *g,int grade,int index);
struct syz0 *newSyz0(void);
struct gradedPairs *putPairInGradedPairs(struct gradedPairs *grP,
					 struct pair *top);
void outputGradedPolySet(struct gradedPolySet *grG,int needSyz);
void outputGradedPairs(struct gradedPairs *grP);
void outputNode(struct pair *p);
int countPairs(struct gradedPairs *grD);
int countGradedPolySet(struct gradedPolySet *grD);
struct gradedPolySet *gradedPolySetCopy(struct gradedPolySet *grG);
int deletePairByCriterion2B(struct gradedPairs *grD,POLY gt,struct gradedPolySet *grG);
int markGeneratorInG(struct gradedPolySet *g,int grade,int index);
int clearGmod(struct gradedPolySet *g);

/* red.c */
struct spValue (*sp)(POLY f,POLY g);
struct spValue spZero(void);
int (*isReducible)(POLY f,POLY g);
POLY (*reduction1)(POLY f,POLY g,int needSyz,POLY *cc,POLY *cg);
/*  if needSyz, then  result = *cc f + *cg g. */
/* syzp->cf is in the CurrentRignp and syzp->syz is in the SyzRingp. */
POLY (*reduction)(POLY f,struct gradedPolySet *gset,int needSyz,
		  struct syz0 *syzp);
POLY (*isCdrReducible)(POLY f,POLY g);
POLY (*reduction1Cdr)(POLY f,POLY fs,POLY g,int needSyz,POLY *cc,POLY *cg);
/*  if needSyz, then  result = *cc f + *cg g. */
POLY (*reductionCdr)(POLY f,struct gradedPolySet *gset,int needSyz,
		  struct syz0 *syzp);
POLY (*lcm)(POLY f,POLY g);
POLY reduction1_gen(POLY f,POLY g,int needSyz,POLY *cc,POLY *cg);
POLY reduction1_gen_debug(POLY f,POLY g,int needSyz,POLY *cc,POLY *cg);
POLY reduction1_sugar(POLY f,POLY g,int needSyz,POLY *c,POLY *h,int sugarGrade);
int isReducible_gen(POLY f,POLY g);
POLY reduction_gen(POLY f,struct gradedPolySet *gset,int needSyz,
		   struct syz0 *syzp);
POLY reduction_gen_rev(POLY f,struct gradedPolySet *gset,int needSyz,
		   struct syz0 *syzp);
POLY reduction_sugar(POLY f,struct gradedPolySet *gset,int needSyz,
		     struct syz0 *syzp,int sugarGrade);
POLY reduction1Cdr_gen(POLY f,POLY fs,POLY g,int needSyz,POLY *cc,POLY *cg);
POLY isCdrReducible_gen(POLY f,POLY g);
POLY reductionCdr_gen(POLY f,struct gradedPolySet *gset,int needSyz,
		   struct syz0 *syzp);
POLY lcm_gen(POLY f,POLY g);
struct spValue sp_gen(POLY f,POLY g);
struct spValue sp_q(POLY f,POLY g);
int grade_gen(POLY f);
POLY reductionCdr_except_grd_i(POLY f,struct gradedPolySet *gset,
			       int needSyz,struct syz0 *syzp,
			       int grd,int i, int *reduced);
 /* Polynomial at (grd,i) is not used for reduction.
    if del[j] == 1, then the attached polynomial is not used for reduction.*/

/* ecart.c */
POLY reduction_ecart(POLY f,struct gradedPolySet *gset,
					 int needSyz,struct syz0 *syzp);

/* constructor */
POLY toSyzPoly(POLY cg,int grade,int index);
struct coeff *toSyzCoeff(POLY f);
void initSyzRingp(void);
				
/* gb.c */
struct gradedPolySet *(*groebner)(struct arrayOfPOLY *f,
				  int needBack,
				  int needSyz, struct pair **grP,
				  int countDown,int forceReduction,int reduceOnly,int gbCheck);
struct gradedPolySet *groebner_gen(struct arrayOfPOLY *f,
				  int needBack,
				  int needSyz, struct pair **grP,
				  int countDown,int forceReduction,int reduceOnly,int gbCheck);
struct gradedPairs *updatePairs(struct gradedPairs *grD, POLY gt,
				int gtGrade, int t,
				struct gradedPolySet *grG);
/* add gt to grD. gt is indexed by (gtGrade,t) */
void toReducedBasis(struct gradedPolySet *grP,int needBack, int needSyz);


/* gbGM.c */
struct gradedPolySet *groebner_gm(struct arrayOfPOLY *f,
				  int needBack,
				  int needSyz, struct pair **grP,
				  int countDown,int forceReduction,int reduceOnly,int gbCheck);

/* syz0 */
void simplifyBT(int grd,int index, struct gradedPolySet *grG);
/* grG->polys[i]->syz[j],mark[j] is modified. */

void getBackwardTransformation(struct gradedPolySet *grG);
/* grG->polys[i]->syz[j],mark[j] is modified. */

struct arrayOfPOLY *getSyzygy0(struct gradedPolySet *grG,
			       struct pair *zeroPairs);
/* grG->polys[i]->mark[j] are modified.
   grG->polys[i]->del[j] and syz[j] must be set.
   syz[j] is the result of the reduction by grBases.  cf. getSyzygy(). */

struct matrixOfPOLY *getSyzygy(struct gradedPolySet *grG,struct pair *zp,
			       struct gradedPolySet **grBasesp,
			       struct matrixOfPOLY **backwardMatp);
/* grBasesp is also returned. */

POLY getSyzPolyFromSp(struct pair *spij,struct gradedPolySet *grG);
/* del and syz of grG and grBases must be properly set. mark of grG
   will be modified. */

struct matrixOfPOLY *getBackwardMatrixOfPOLY(struct gradedPolySet *grG);
/* get B */

struct matrixOfPOLY *getSyzygy1(struct matrixOfPOLY *b,struct matrixOfPOLY *nc,
				struct arrayOfPOLY *dc);

struct matrixOfPOLY *getNC(struct gradedPolySet *newG,int n,
			   struct gradedPolySet *grBases);
/* get the numerator of C */

struct arrayOfPOLY *getDC(struct gradedPolySet *newG);
/* diag(DC) f + C G = 0. */

void errorSyz0(char *s);
		      
/* conversion */
struct arrayOfPOLY *syzPolyToArrayOfPOLY(int size,POLY f,
					 struct gradedPolySet *grG);
/* f is in SyzRingp. */

int criterion1(POLY f,POLY g,POLY lc);
struct gradedPolySet *groebner_gen(
     struct arrayOfPOLY *f,
     int needBack,
     int needSyz,
     struct pair **grP,  /* if (needSyz), it is set. */
     int countDown,
     int forceReduction,
     int reduceOnly,
     int gbCheck);


#define checkRingSp(f,g,r) {\
  if ((f)->m->ringp != (g)->m->ringp) {\
    warningPoly("f and g must be in a same ring. Returns 0.\n");\
    fprintf(stderr,"f=%s\n",POLYToString(f,'*',0));\
    fprintf(stderr,"g=%s\n",POLYToString(g,'*',0));\
    r.a = ZERO; r.b = ZERO;\
    return(r);\
    }\
}

#define checkRingIsR(f,g) {\
  if ((f)->m->ringp != (g)->m->ringp) {\
    warningPoly("f and g must be in a same ring. Returns 0.\n");\
    fprintf(stderr,"f=%s\n",POLYToString(f,'*',0));\
    fprintf(stderr,"g=%s\n",POLYToString(g,'*',0));\
    return(0);\
    }\
}

