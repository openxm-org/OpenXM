/*
$OpenXM: OpenXM/src/hgm/mh/src/code-n-2f1.c,v 1.4 2016/02/04 06:56:04 takayama Exp $
License: LGPL
Ref: code-n.c, 2016.01.30, 31.
 */
#include <stdio.h>
#include <stdlib.h> 
#include <math.h>
#include "sfile.h"
#include "mh.h"

static void error_code(char *s);
#ifdef STANDALONE
static void showf(char *s,double *v,int m);
static void showf(char *s,double *v,int m);
static void showf2(char *s,double *v,int m,int n);
#endif

extern int MH_M;
extern int MH_RANK;
extern int MH_Ef_type;
extern double *MH_A_pFq;
extern double *MH_B_pFq;

static int bitcount(int n) {int c; c=0; while (n) { ++c; n &= (n-1); } return(c);}
#define join(k,jj) ( (1 << k) | jj)
#define delete(k,jj) ( (~(1 << k)) & jj)
#define member(k,ii) ( (1 << k) & ii)
#define NaN 9.999e100

#define idxM(i,j) ((i)*MH_M+j)
#define idxRank(i,j) ((i)*MH_RANK+j)


void mh_rf_ef_type_2(double x, double *f, int rank_not_used, double *val, int n_not_used)
{ extern double *MH_Beta; /* beta = (1/2)*\simga^(-1) */
  extern double *MH_Ng;   /* freedom */
  extern int MH_Mg;       /* number of variables, MH_Mg=MH_M */
  static int initialized = 0;
  extern int MH_deallocate;
  /* static double b[MH_M]; */
  static double *b;
  /* static double bsum = 0; */
  static int m;
  /*
  static int bitSize[MH_RANK];
  static int invbitSize[MH_RANK];
  */
  static int *bitSize;
  static int *invbitSize;
  int i,j,k,p;
  int ii,jj; /* stands for I and J */
  double rijj;
  double dd;

  static double *pp=NULL;  /* p(x_i) */
  static double *qq=NULL;  /* q(x_i,x_k) */
  static double *qq2=NULL; /* q_2(x_i,x_k) */
  static double *rr=NULL;  /* r(x_i) */
  static double *qq_pd=NULL;   /* dx_k q */
  static double *qq2_pd=NULL;  /* dx_k q_2 */ 
  /* double f2[MH_M][MH_RANK];*/
  static double *f2=NULL;
  static double *y=NULL;
  static double aaa=NaN;
  static double bbb,ccc;
  static double *b2=NULL;  /* b_i/(x+b_i)^2 */

  mh_check_intr(100);
  if (MH_deallocate && initialized) {
        if (b) mh_free(b);
        if (bitSize) mh_free(bitSize);
        if (invbitSize) mh_free(invbitSize);
        if (f2) mh_free(f2);
        if (pp) mh_free(pp);
        if (qq) mh_free(qq);
        if (qq2) mh_free(qq2);
        if (rr) mh_free(rr);
        if (qq_pd) mh_free(qq_pd);
        if (qq2_pd) mh_free(qq2_pd);
		if (b2) mh_free(b2);
	if (y) mh_free(y);
    b = f2 =  y = NULL;
    pp = qq = qq2 = rr = qq_pd = qq2_pd = NULL;
    bitSize = invbitSize = NULL;
        initialized = 0; return;
  }
  if (!initialized) {
    b = (double *)mh_malloc(sizeof(double)*MH_M);
    bitSize = (int *)mh_malloc(sizeof(int)*MH_RANK);
    invbitSize = (int *)mh_malloc(sizeof(int)*MH_RANK);
    y = (double *)mh_malloc(sizeof(double)*MH_RANK);

    f2 = (double *)mh_malloc(sizeof(double)*MH_M*MH_RANK);
    pp   = (double *)mh_malloc(sizeof(double)*MH_M);
    qq = (double *)mh_malloc(sizeof(double)*MH_M*MH_M);
    qq2 = (double *)mh_malloc(sizeof(double)*MH_M*MH_M);
    rr   = (double *)mh_malloc(sizeof(double)*MH_M);
    qq_pd = (double *)mh_malloc(sizeof(double)*MH_M*MH_M);
    qq2_pd = (double *)mh_malloc(sizeof(double)*MH_M*MH_M);
    b2 = (double *)mh_malloc(sizeof(double)*MH_M);

    m = MH_Mg;
    if (m != MH_M) error_code("MH_M != m. MH_M is given by -DMH_M...");
    aaa = MH_A_pFq[0];
    bbb = MH_A_pFq[1];
    ccc = MH_B_pFq[0];

    if(MH_Beta != NULL) for (i=0;i<MH_M;i++) b[i]=MH_Beta[i];
    else error_code("MH_Beta is null.");

    /* bsum = 0; for (i=0; i<m; i++) bsum += b[i]; */

    for (i=0; i<MH_RANK; i++) bitSize[i] = bitcount(i);
    for (i=0, k=0; i<=MH_M; i++) {
      for (j=0; j<MH_RANK; j++) {
        if (bitSize[j] == i) {invbitSize[k] = j; k++; }
      }
    }
    initialized = 1;
  }
  if (MH_Ef_type == 2) {
    for (i=0; i<MH_M; i++) {
      y[i] = x/(b[i]+x);    
    }
  }else{
    for (i=0; i<MH_M; i++) {
      y[i] = b[i]*x;    
    }
  }
  for (i=0; i<MH_M; i++) {
    pp[i] = (ccc-(m-1.0)/2.0-(aaa+bbb+1-(m-1.0)/2.0)*y[i])/(y[i]*(1-y[i]));
    rr[i] = aaa*bbb/(y[i]*(1-y[i]));
    for (k=0; k<MH_M; k++) {
      if (i != k) {
        qq2[idxM(i,k)] = 1.0/(2.0*(y[i]-y[k]));
        qq[idxM(i,k)] = y[k]*(1-y[k])/(2.0*y[i]*(1-y[i])*(y[i]-y[k]));
        qq2_pd[idxM(i,k)] = 1.0/(2.0*(y[i]-y[k])*(y[i]-y[k]));
        qq_pd[idxM(i,k)] = 1/(2.0*y[i]*(1-y[i]));
        qq_pd[idxM(i,k)] *= ((1-2*y[k])*(y[i]-y[k])+y[k]*(1-y[k]))/((y[i]-y[k])*(y[i]-y[k]));
      }
    }
  }

  /* 
     generation of f2[i][jj] = pd{i}^2 \pd{J} F 
  */
  for (i=0; i<MH_M; i++) for (j=0; j<MH_RANK; j++) f2[idxRank(i,j)] = NaN;
  /* The case J = jj = \emptyset */  
  for (i=0; i<MH_M; i++) {
    f2[idxRank(i,0)] = -pp[i]*f[join(i,0)]+rr[i]*f[0];
    for (k=0; k<MH_M; k++) {
      if (i!=k) {
        f2[idxRank(i,0)] -= qq2[idxM(i,k)]*f[join(i,0)]-qq[idxM(i,k)]*f[join(k,0)];
      }
    }
  }
  for (p=1; p<MH_RANK; p++) {
    jj = invbitSize[p];   /* evaluate from jj of small bitSize */
    for (i=0; i<MH_M; i++) {
      if (member(i,jj)) continue;
      ii = join(i,jj);
      rijj = -pp[i]*f[ii]+rr[i]*f[jj];
      for (k=0; k<MH_M; k++) {
	if (k==i) continue;
        rijj -= qq2[idxM(i,k)]*f[ii];
        if (member(k,jj)) {
          rijj -= qq2_pd[idxM(i,k)]*f[delete(k,ii)]
	    -qq_pd[idxM(i,k)]*f[jj];
        }else{
	  rijj -= -qq[idxM(i,k)]*f[join(k,jj)];
	}
      }
      /* term of the form dx_k^2 dx_{jj-k} */
      for (k = 0; k<MH_M; k++) {
	if (k == i) continue;
        if (member(k,jj)) {
	  rijj -= -qq[idxM(i,k)]*f2[idxRank(k,delete(k,jj))];
        }
      }
      f2[idxRank(i,jj)] = rijj;
    }
  }
  /** showf2("f2",f2,MH_M,MH_RANK); exit(0);  */

  /* sum_j b_j dx_j Base */
  if (MH_Ef_type==2) {
	for (i=0; i<MH_M; i++) {
	  b2[i] = b[i]/((b[i]+x)*(b[i]+x));
	}
  }else{
	b2[i] = b[i];
  }
  for (jj=0; jj<MH_RANK; jj++) {
    val[jj] = 0;
    for (i=0; i<MH_M; i++) {
      if (member(i,jj)) {
        val[jj] += b2[i]*f2[idxRank(i,delete(i,jj))];
      }else{
        val[jj] += b2[i]*f[join(i,jj)];
      }
    }
  }

  /*   If there is a diagonal shift, add a code .*/
  if (MH_Ef_type==2) {
	dd = (ccc-aaa)*(2*aaa-1)/x;
    for (i=0; i<MH_M; i++) {
      dd += -bbb/(b[i]+x);
	}
	for (jj=0; jj<MH_RANK; jj++) {
	  val[jj] += dd*f[jj];
	}
  }
}

static void error_code(char *s) {
  oxprintfe("%s",s);
  mh_exit(10);
}


#ifdef STANDALONE
/* for debug */
static void showf(char *s,double *v,int m) {
  int i;
  oxprintf("%s=\n",s);
  for (i=0; i<m; i++) {
    oxprintf("%e, ",v[i]);
  }
  oxprintf("\n");
}

static void showd(char *s,int *v,int m) {
  int i;
  oxprintf("%s=\n",s);
  for (i=0; i<m; i++) {
    oxprintf("%5d, ",v[i]);
  }
  oxprintf("\n");
}

static void showf2(char *s,double *v,int m,int n) {
  int i,j;
  oxprintf("%s=\n",s);
  for (i=0; i<m; i++) {
    for (j=0; j<n; j++) {
      oxprintf("%e, ",v[i*n+j]);
    }
    oxprintf("\n");
  }
  oxprintf("\n");
  oxprintf("member(0,0)=%d, member(0,1)=%d, member(0,2)=%d,member(0,3)=%d\n",
         member(0,0),member(0,1),member(0,2),member(0,3));
}
#endif
