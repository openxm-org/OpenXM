#include <stdio.h>
#include "datatype.h"
#include "extern2.h"
#include "matrix.h"
#include "gradedset.h"

#define INITGRADE 4
#define INITSIZE 2

int KanGBmessage = 1;

static int Debug = 0;
static int sugarGrade(struct pair *inode,struct gradedPolySet *grG,
		      POLY gt,int gtSugarGrade);
extern int Sugar;
extern int Statistics;
extern int Criterion1;
extern int UseCriterion1;
extern int UseCriterion2B;
extern int Spairs;
extern int Criterion2B, Criterion2F, Criterion2M;


struct gradedPairs *updatePairs(grD,gt,gtGrade,t,grG)
struct gradedPairs *grD;  /* set of pairs */
POLY gt;                 /* new polynomial */
int gtGrade;
int t;
struct gradedPolySet *grG; /* (f,gt), f \in grG, should be added to grD. */
{
  int gmax,newGrade;
  struct pair *node,*new,*inode,*jnode;
  int i,k;
  struct polySet *g;
  POLY lcmp;
  POLY it,jt;
  extern int Verbose;


  if (Verbose) {
    printf("\nupdatePairs(): ");
  }

  gmax = grG->maxGrade;
  node = new = newPair((struct pair *)NULL);
  /* (f,gt) are stored in new as a list. */
  for (k=0; k<gmax; k++) {
    g = grG->polys[k];
    for (i=0; i<g->size; i++) {
      if (g->del[i] == 0 && (gtGrade != k || t != i)) {
	lcmp = (*lcm)(g->g[i],gt);
	if (lcmp == ZERO) {

	}else {
	  new->del = 0;
	  new->next = newPair(new);
	  new = new->next;
	  new->lcm = lcmp;
	  new->ig = k; new->ii = i;  /* g->g[i] */
	  new->jg = gtGrade; new->ji = t; /* gt */
	  new->grade = -1; /* one do not need to set grade here. */
	}
      }
    }
  }


  /* Check the criterion */
  /*  See Gebauer and Mora, 1988, Journal of Symbolic Computation
      279.  We must not use the test T(i)T(j) = T(i,j) because it
      does not hold in the ring of diff op.
   */
  
  inode = node->next;
  while (inode != (struct pair *)NULL) {
    jnode = inode->next;
    while (jnode != (struct pair *)NULL) {
      it = inode->lcm;
      jt = jnode->lcm;
      if ((*mmLarger)(it,jt) == 2) {
	/* F(j,t), i<j */
	jnode->del = 1;
	if (Verbose) printf("F[%d,%d]([%d,%d],[%d,%d]) ",
			    inode->ig,inode->ii,
			    jnode->ig,jnode->ii,
			    gtGrade,t);
	Criterion2F++;
      }else {
	/* g[i] > g[j] ?, M(i,t) */
	if ((*isReducible)(it,jt)) {
	  inode->del = 1;
	  if (Verbose) printf("M[%d,%d]([%d,%d],[%d,%d]) ",
			      jnode->ig,jnode->ii,
			      inode->ig,inode->ii,
			      gtGrade,t);
	  Criterion2M++;
	}
        /* M(j,t) */
	if ((*isReducible)(jt,it)) {
	  jnode->del = 1;
	  if (Verbose) printf("M[%d,%d]([%d,%d],[%d,%d]) ",
			      inode->ig,inode->ii,
			      jnode->ig,jnode->ii,
			      gtGrade,t);
	  Criterion2M++;
	}
      }
      jnode = jnode->next;
    }
    inode = inode->next;
  }
	  
  if (UseCriterion1) {
    inode = node->next;
    while (inode != NULL) {
      if (inode->del == 0) {
	if (criterion1(gt,grG->polys[inode->ig]->g[inode->ii],inode->lcm)) {
	  inode->del = 1;
	  if (Verbose) printf("Criterion1([%d,%d],[%d,%d]) ",
			      inode->ig,inode->ii,
			      gtGrade,t);
	  Criterion1++;
	}
      }
      inode = inode->next;
    }
  }

  if (UseCriterion2B) {
    Criterion2B += deletePairByCriterion2B(grD,gt,grG);
  }
    
  /* Merge to grD */
  inode = node->next;
  if (Debug) outputNode(inode);
  while (inode != (struct pair *)NULL) {
    if (inode->del == 0) {
      if (Sugar) {
	inode->grade = sugarGrade(inode,grG,gt,gtGrade);
      }else{
	inode->grade = (*grade)(inode->lcm);
      }
      if (grD->lim <= inode->grade) {
	grD = enlargeGradedPairs(2*(inode->grade)+1,grD);
      }
      insertPair(inode,grD->pairs[inode->grade]);
      grD->maxGrade = max(grD->maxGrade,inode->grade+1); /* don't forget */
    }
    inode = inode->next;
  }
  if (Debug) printf("OK.\n");
  if (Verbose) {
    printf(" Remaining pairs are %d. maxGrade=%d\n",countPairs(grD),grD->maxGrade);
  }
  return(grD);
}

struct gradedPolySet *groebner_gen(f,needBack,needSyz,grP,countDown,forceReduction)
struct arrayOfPOLY *f;
int needBack;
int needSyz;
struct pair **grP;  /* if (needSyz), it is set. */
int countDown;
int forceReduction;
{
  int r;
  struct gradedPolySet *g;
  struct gradedPairs *d;
  int i;
  int grade,indx;
  POLY gt;
  struct pair *top;
  int ig,ii,jg,ji;
  POLY gi,gj;
  struct spValue h;
  struct syz0 syz;
  int pgrade = 0;
  POLY rd;
  POLY syzPoly;
  POLY syzCf;
  struct syz0 *syzp;
  int serial;
  struct pair *listP;
  extern int Verbose;
  extern int ReduceLowerTerms;
  extern int StopDegree;
  extern struct ring *CurrentRingp;
  extern char *F_mpMult;
  struct ring *rp;

  r = f->n;
  if (r<=0) return((struct gradedPolySet *)NULL);
  if (UseCriterion1) {
    if (needSyz) {
      warningGradedSet("You cannot use both the options needSyz and UseCriterion1.\nInput [(UseCriterion1) 0] system_variable to turn off the use of the Criterion 1.");
    }
    if (strcmp(F_mpMult,"poly")!=0) {
      warningGradedSet("You can use the option UseCriterion1 only for the computation in the ring of polynomials.\nInput [(UseCriterion1) 0] system_variable to turn off the use of the Criterion 1.");
    }
  }

  Spairs = Criterion1 = Criterion2B = Criterion2F = Criterion2M = 0;
  
  g = newGradedPolySet(INITGRADE);
  d = newGradedPairs(INITGRADE*2);
  for (i=0; i<g->lim; i++) {
    g->polys[i] = newPolySet(INITSIZE);
  }

  for (i=0; i<r; i++) {
    gt = getArrayOfPOLY(f,i);
    if (gt ISZERO) { rp = CurrentRingp; } else { rp = gt->m->ringp; }
    grade = -1; whereInG(g,gt,&grade,&indx,Sugar);
    if (KanGBmessage == 2) {
      printf("init=%s, ",POLYToString(head(gt),'*',1));
      printf("(gr,indx)=(%d,%d).\n",grade,indx);
    }
    d = updatePairs(d,gt,grade,indx,g);
    serial = i;
    if (!needBack) {
      g = putPolyInG(g,gt,grade,indx,
		     (struct syz0 *)NULL,1,serial);
    }else {
      syzp = newSyz0();
      syzp->cf = cxx(1,0,0,rp);
      syzp->syz = toSyzPoly(cxx(1,0,0,rp),grade,indx);
      g = putPolyInG(g,gt,grade,indx,syzp,1,serial);
    }

    markRedundant0(g,grade,indx);
    if (Debug) {
      outputGradedPairs(d); outputGradedPolySet(g,needSyz);
    }
  }
  if (needSyz) {
    *grP = newPair((struct pair *)NULL);
    listP = *grP;
  }

  while ((top = getPair(d)) != (struct pair *)NULL) {
    ig = top->ig; ii = top->ii; /* [ig,ii] */
    jg = top->jg; ji = top->ji; /* [jg,ji] */
    gi = g->polys[ig]->g[ii];
    gj = g->polys[jg]->g[ji];

    Spairs++;
    h = (*sp)(gi,gj);
    rd = ppAddv(ppMult(h.a,gi),ppMult(h.b,gj));
    if (!Sugar || forceReduction) {
      rd = (*reduction)(rd,g,needBack,&syz);
    }else{
      rd = reduction_sugar(rd,g,needBack,&syz,top->grade);
    }
    syzPoly = syz.syz;
    syzCf = syz.cf;

    if (KanGBmessage) {
      if (KanGBmessage == 2) {
	printf("sp([%d,%d],[%d,%d]), ",ig,ii,jg,ji);
	if (rd ISZERO) {
	  printf(" 0 \n"); 
	} else{
	  printf(" terms=%d, grade=%d, ",pLength(rd),top->grade);
	  printf(" init=%s, ",POLYToString(head(rd),'*',1));
	}
      }else{
	if (pgrade != top->grade) {
	  pgrade = top->grade;
	  printf(" %d",pgrade);
	  fflush(stdout);
	}
	if (rd ISZERO) {
	  printf("o"); fflush(stdout);
	}else{
	  printf("."); fflush(stdout);
	}
      }
    }

    if (!(rd ISZERO)) {
      if (needBack || needSyz) {
	syzp = newSyz0();
	syzp->cf = syzCf; /* no meaning */
	syzp->syz = ppAdd(toSyzPoly(h.a,ig,ii),toSyzPoly(h.b,jg,ji));
	syzp->syz = cpMult(toSyzCoeff(syzCf),syzp->syz);
	syzp->syz = ppAdd(syzp->syz,syzPoly);
      }
      
      if (ReduceLowerTerms && !(Sugar)) {
	rd = (*reductionCdr)(rd,g,needBack,&syz);
	if (needBack || needSyz) {
	  /* syzp->cf = ppMult(syz.cf,syzp->cf); no meaning */
	  syzp->syz = ppAdd(syz.syz,
			    cpMult(toSyzCoeff(syz.cf),syzp->syz)); 
	}
      }

      if(Sugar && (!forceReduction)){grade=top->grade;}else{grade=-1;}whereInG(g,rd,&grade,&indx,Sugar);
      if (KanGBmessage == 2) {
	printf("(gr,indx)=(%d,%d).\n",grade,indx);
	/*
	printf("sp(%s,%s)-->%s\n",POLYToString(gi,' ',1),
	                          POLYToString(gj,' ',1),
	                          POLYToString(rd,' ',1));
	*/
      }
	
      d = updatePairs(d,rd,grade,indx,g);
      g = putPolyInG(g,rd,grade,indx,syzp,0,-1);
      if (Sugar) { markRedundant0(g,grade,indx);}
      else {markRedundant(g,rd,grade,indx,Sugar);}

      if (KanGBmessage && (StopDegree < pgrade)) {
	printf("Computation of the Groebner basis is suspended bacause of StopDegree < computing grade.\n");
	printf("Note that the result is NOT groebner basis.\n");
	break;
      }
      if (countDown) {
	if (eliminated(rd) == 1) {
	  --countDown;
	  printf("x"); fflush(stdout);
	  if (countDown == 0) {
	    printf("\nThe computation of the Groebner basis is suspended because of countDown==0.\n");
	    printf("Note that the result is NOT groebner basis.\n");
	    break;
	  }
	}
      }
      if (Debug) {
	outputGradedPairs(d); outputGradedPolySet(g,needSyz);
      }
    }else{
      if (needSyz) {
	top->syz = ppAdd(toSyzPoly(h.a,ig,ii),toSyzPoly(h.b,jg,ji));
	top->syz = cpMult(toSyzCoeff(syzCf),top->syz);
	top->syz = ppAdd(top->syz,syzPoly);
	listP->next = top; top->prev = listP; listP = listP->next;
      }
    }
  }

  if (KanGBmessage == 2) {
    outputGradedPolySet(g,needSyz);
  }
  if (KanGBmessage) {
    if (Sugar) {
      printf("\nCompleted (GB with sugar).\n");
    }else{
      printf("\nCompleted.\n");
    }
  }

  if (Statistics) {
    printf("\nThe number of s-pairs is %d.\n",Spairs);
    printf("Criterion1 is applied %d times.\n",Criterion1);
    printf("Criterions M,F and B are applied M=%d, F=%d, B=%d times.\n",Criterion2M,Criterion2F,Criterion2B);
    Spairs = Criterion1 = Criterion2M = Criterion2F = Criterion2B = 0;
  }

  
  return(g);
}

  
static int sugarGrade(struct pair *inode,struct gradedPolySet *grG,
		      POLY gt,int gtSugarGrade)
{
  int a,b,ans;
  int debug = 0;
  a = grade_gen(inode->lcm)-grade_gen(grG->polys[inode->ig]->g[inode->ii]);
  b = grade_gen(inode->lcm)-grade_gen(gt);
  /* inode = lcm(f_i, gt) = p f_i = q gt  modulo lower order terms.
     a = tdeg(p), b = tdeg(gt);
     */
  if (debug) {
    printf("Sugar grade of sp([%d,%d],[%d,%d]) ",inode->ig,inode->ii,
	   inode->jg,inode->ji);
    printf("is max(%d+%d,%d+%d)\n",a,inode->ig,b,gtSugarGrade);
  }
  a = a+(inode->ig); /* new sugar degree of p f_i. */
  b = b + gtSugarGrade;  /* new sugar degree of q gt */
  return( a > b ? a : b);
}



