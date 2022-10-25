#include <stdio.h>
#include <stdlib.h>
#define _ISOC99_SOURCE
#include <math.h>
#include <string.h>
#include "sfile.h"

#define VSTRING "%!version2.0"
/*
  $OpenXM: OpenXM/src/hgm/mh/src/jack-n.c,v 1.54 2020/02/06 05:18:12 takayama Exp $
  Ref: copied from this11/misc-2011/A1/wishart/Prog
  jack-n.c, translated from mh.rr or tk_jack.rr in the asir-contrib. License: LGPL
  Koev-Edelman for higher order derivatives.
  cf. 20-my-note-mh-jack-n.pdf,  /Movies/oxvh/priv-j1/2011-12-14-ke-mh-jack.mov
  Todo:
  1. Cash the transposes of partitions.
  2. Use the recurrence to obtain beta().
  3. Easier input data file format for mh-n.c
  Changelog:
  2016.02.15 log of Ef
  2016.02.09 unify 2F1 and 1F1. Parser.
  2016.02.04 Ef_type (exponential or scalar factor type)
  2016.02.01 ifdef C_2F1 ...
  2016.01.12 2F1
  2014.03.15 http://fe.math.kobe-u.ac.jp/Movies/oxvh/2014-03-11-jack-n-c-automatic  see also hgm/doc/ref.html, @s/2014/03/15-my-note-jack-automatic-order-F_A-casta.pdf.
  2014.03.14, --automatic option. Output estimation data.
  2012.02.21, porting to OpenXM/src/hgm
  2011.12.22, --table option, which is experimental.
  2011.12.19, bug fix.  jjk() should return double. It can become more than max int.
  2011.12.15, mh.r --> jack-n.c
*/

/****** from mh-n.c *****/
static int JK_byFile=1;
static int JK_deallocate=0;
#define M_n_default 3
#define Sample_default 1
static int M_n=0;
/* global variables. They are set in setParam() */
static int Mg;  /* n */
static int Mapprox; /* m, approximation degree */
static double *Beta; /* beta[0], ..., beta[m-1] */
static double *Ng;   /* freedom n.  c=(m+1)/2+n/2; Note that it is a pointer */
static double X0g;   /* initial point */
static double *Iv;   /* Initial values of mhg sorted by mhbase() in rd.rr at beta*x0 */
static double Ef;   /* exponential factor at beta*x0 */
static double Hg;   /* step size of rk defined in rk.c */
static int Dp;      /* Data sampling period */
static double Xng=0.0;   /* the last point */
static int Sample = Sample_default;

/* for sample inputs */
static double *Iv2; 
static double Ef2; 

static int SAR_warning = 1;

#ifdef NAN
#else
#define NAN  3.40282e+38  /* for old 32 bit machines. Todo, configure */
#endif

/* #define M_n  3  defined in the Makefile */ /* number of variables */
#define M_n0 3 /* used for tests. Must be equal to M_n */
#define M_m_MAX 200
#define M_nmx  M_m_MAX  /* maximal of M_n */

static int A_LEN=-1;  /* (a_1, ..., a_p), A_LEN=p */
static int B_LEN=-1;  /* (b_1,..., b_q),  B_LEN=q */
static double *A_pFq=NULL;
static double *B_pFq=NULL;
static int Ef_type=1;  /* 1F1 for Wishart */
/* Ef_type=2;   2F1, Hashiguchi note (1) */

static int Debug = 0;
static int Alpha = 2;  /* 2 implies the zonal polynomial */
static int *Darray = NULL;
static int **Parray = NULL; /* array of partitions of size M_n */
static int *ParraySize = NULL; /* length of each partitions */
static int M_kap[M_nmx];
static int M_m=M_m_MAX-2;   /* | | <= M_m, bug do check of M_m <=M_m_MAX-2 */
void (*M_pExec)(void);
static int HS_mu[M_nmx];
static int HS_n=M_nmx;      /* It is initialized to M_n in jk_main */
void (*HS_hsExec)(void);
static double M_x[M_nmx];

/* They are used in pmn */
static int *P_pki=NULL;
static int P_pmn=0;

/* It is used genDarray2(), list partitions... */
static int DR_parray=0;

/* Used in genBeta() and enumeration of horizontal strip. */
static double *M_beta_0=NULL;  /* M_beta[0][*] value of beta_{kappa,mu}, M_beta[1][*] N_mu */
static int *M_beta_1=NULL;
static int M_beta_pt=0;
static int M_beta_kap[M_nmx];
static int UseTable = 0;

static double **M_jack; 
static int M_df=1; /* Compute differentials? */
static int M_2n=0; /* 2^N */

static double Xarray[M_nmx][M_m_MAX];  
/* (x_1, ..., x_n) */
/* Xarray[i][0]  x_{i+1}^0, Xarray[i][1], x_{i+1}^1, ... */

static double *M_qk=NULL;  /* saves pochhammerb */
static double M_rel_error=0.0; /* relative errors */

/* For automatic degree and X0g setting. */
/*
 If automatic == 1, then the series is reevaluated as long as t_success!=1
 by increasing X0g (evaluation point) and M_m (approx degree);
 */
static int M_automatic=1;
/* Estimated degree bound for series expansion. See mh_t */
static int M_m_estimated_approx_deg=0;
/* Let F(i) be the approximation up to degree i. 
   The i-th series_error is defined
   by |(F(i)-F(i-1))/F(i-1)|.
*/
static double M_series_error;
/*
  M_series_error < M_assigend_series_error (A) is required for the 
  estimated_approx_deg.
 */
static double M_assigned_series_error=M_ASSIGNED_SERIES_ERROR_DEFAULT;
/*
  Let Ef be the exponential factor ( Ef=(4)/1F1 of [HNTT] )
  If F(M_m)*Ef < x0value_min (B), the success=0 and X0g is increased. 
  Note that minimal double is about 2e-308
 */
static double M_x0value_min=1e-60;
/*
  estimated_X0g is the suggested value of X0g.
 */
static double M_estimated_X0g=0.0;
/*
 X0g should be less than M_X0g_bound.
 */
static double M_X0g_bound = 1e+100;
/*
 success is set to 1 when (A) and (B) are satisfied.
 */
static int M_mh_t_success=1;
/*
  recommended_abserr is the recommended value of the absolute error
  for the Runge-Kutta method. It is defined as
  assigend_series_error(standing for significant digits)*Ig[0](initial value)
 */
static double M_recommended_abserr;
/*
  recommended_relerr is the recommended value of the relative error
  for the Runge-Kutta method..
 */
static double M_recommended_relerr;
/*
  max of beta(i)*x/2
 */
static double M_beta_i_x_o2_max;
/*
  minimum of |beta_i-beta_j|
 */
static double M_beta_i_beta_j_min;
/*
  Value of matrix hg
*/
static double M_mh_t_value;
/*
 Show the process of updating degree.
 */
int M_show_autosteps=1;

/* prototypes */
static void *mymalloc(int size);
static int myfree(void *p);
static int myerror(char *s);
static double jack1(int K);
static double jack1diff(int k);
static double xval(int i,int p); /* x_i^p */
static int mysum(int L[]);
static int plength(int P[]);
static int plength_t(int P[]);
static void ptrans(int P[M_nmx],int Pt[]);
static int huk(int K[],int I,int J);
static int hdk(int K[],int I,int J);
static double jjk(int K[]);
static double ppoch(double A,int K[]);
static double ppoch2(double A,double B,int K[]);
static double mypower(double x,int n);
static double qk(int K[],double A[],double B[]);
static int bb(int N[],int K[],int M[],int I,int J);
static double beta(int K[],int M[]);
static int printp(int kappa[]);
static double q3_10(int K[],int M[],int SK);
static int nk(int KK[]);
static int myeq(int P1[],int P2[]);
static int pListPartition(int M,int N);
static int pListPartition2(int Less,int From,int To, int M);
static void pExec_0(void);
static int pListHS(int Kap[],int N);
static int pListHS2(int From,int To,int Kap[]);
static void hsExec_0(void);
static int pmn(int M,int N);
static int *cloneP(int a[]);
static int copyP(int p[],int a[]);
static void pExec_darray(void);
static int genDarray2(int M,int N);
static int isHStrip(int Kap[],int Nu[]);
static void hsExec_beta(void);
static int genBeta(int Kap[]);
static int psublen(int Kap[],int Mu[]);
static int genJack(int M,int N);

static int imypower(int x,int n);
static int usage(void);
static int setParamDefault(void);
static int next(struct SFILE *fp,char *s,char *msg);
static int setParam(char *fname);
static int showParam(struct SFILE *fp,int fd);
#ifdef STANDALONE
static int showParam_v1(struct SFILE *fp,int fd);
#endif
static double iv_factor(void);
static double gammam(double a,int n);
static double mypower(double a,int n);

static double iv_factor_ef_type1(void);
static double iv_factor_ef_type2(void);
static double lgammam(double a,int n);
static double liv_factor_ef_type1(void);
static double liv_factor_ef_type2(void);

static void setM_x(void);
static void setM_x_ef_type1(void);
static void setM_x_ef_type2(void);

#ifdef STANDALONE
static int test_ptrans(void);
static int printp2(int kappa[]);
static int test_beta(void);
static void mtest4(void);
static void mtest4b(void);
static int plength2(int P1[],int P2[]);
static int checkBeta1(void);
static int checkJack1(int M,int N);
static int checkJack2(int M,int N);
static int mtest1b(void);
static double q3_5(double A[],double B[],int K[],int I);
#endif

double mh_t(double A[],double B[],int N,int M);
double mh_t2(int J);
struct MH_RESULT *jk_main(int argc,char *argv[]);
struct MH_RESULT *jk_main2(int argc,char *argv[],int automode,double newX0g,int newDegree);
int jk_freeWorkArea(void);
int jk_initializeWorkArea(void);
static void setA(double a[],int alen);  /* set A_LEN and A_pFq */
static void setB(double b[],int blen);

static void setA(double a[],int alen) {
  int i;
  if (alen < 0) {
	if (A_pFq != NULL) myfree(A_pFq);
	A_pFq=NULL; A_LEN=-1;
	return;
  }
  if (alen == 0) {
	A_LEN=0; return;
  }
  A_LEN=alen;
  A_pFq = (double *)mymalloc(A_LEN*sizeof(double));
  if (a != NULL) {
	for (i=0; i<alen; i++) A_pFq[i] = a[i];
  }else{
	for (i=0; i<alen; i++) A_pFq[i] = 0.0;
  }
  return;
}
static void setB(double b[],int blen) {
  int i;
  if (blen < 0) {
	if (B_pFq != NULL) myfree(B_pFq);
	B_pFq=NULL; B_LEN=-1;
	return;
  }
  if (blen == 0) {
	B_LEN=0; return;
  }
  B_LEN=blen;
  B_pFq = (double *)mymalloc(B_LEN*sizeof(double));
  if (b != NULL) {
	for (i=0; i<blen; i++) B_pFq[i] = b[i];
  }else{
	for (i=0; i<blen; i++) B_pFq[i] = 0.0;
  }
  return;
}

int jk_freeWorkArea(void) {
  /* bug, p in the cloneP will not be deallocated.
     Nk in genDarray2 will not be deallocated.
  */
  int i;
  JK_deallocate=1;
  if (Darray) {myfree(Darray); Darray=NULL;}
  if (Parray) {myfree(Parray); Parray=NULL;}
  if (ParraySize) {myfree(ParraySize); ParraySize=NULL;}
  if (M_beta_0) {myfree(M_beta_0); M_beta_0=NULL;}
  if (M_beta_1) {myfree(M_beta_1); M_beta_1=NULL;}
  if (M_jack) {
    for (i=0; M_jack[i] != NULL; i++) {
      if (Debug) oxprintf("Free M_jack[%d]\n",i);
      myfree(M_jack[i]); M_jack[i] = NULL;
    }
    myfree(M_jack); M_jack=NULL;
  }
  if (M_qk) {myfree(M_qk); M_qk=NULL;}
  if (P_pki) {myfree(P_pki); P_pki=NULL;}
  setA(NULL,-1); setB(NULL,-1);
  JK_deallocate=0;
  return(0);
}
int jk_initializeWorkArea(void) {
  int i,j;
  JK_deallocate=1;
  xval(0,0);
  JK_deallocate=0;
  Darray=NULL;
  Parray=NULL;
  ParraySize=NULL;
  M_beta_0=NULL;
  M_beta_1=NULL;
  M_jack=NULL;
  M_qk=NULL;
  for (i=0; i<M_nmx; i++) M_kap[i]=HS_mu[i]=0;
  for (i=0; i<M_nmx; i++) M_x[i]=0;
  for (i=0; i<M_nmx; i++) for (j=0; j<M_m_MAX; j++) Xarray[i][j]=0;
  for (i=0; i<M_nmx; i++) M_beta_kap[i]=0;
  M_m=M_m_MAX-2; 
  Alpha = 2;
  HS_n=M_nmx; 
  P_pki=NULL;
  P_pmn=0;
  DR_parray=0;
  M_beta_pt=0;
  M_df=1;
  M_2n=0;
  M_rel_error=0.0;
  Sample = Sample_default;
  Xng=0.0;
  M_n=0;
  return(0);
}

static void *mymalloc(int size) {
  void *p;
  if (Debug) oxprintf("mymalloc(%d)\n",size);
  p = (void *)mh_malloc(size);
  if (p == NULL) {
    oxprintfe("No more memory.\n");
    mh_exit(-1);
  }
  return(p);
}
static int myfree(void *p) {
  if (Debug) oxprintf("myFree at %p\n",p);
  return(mh_free(p));
}
#ifdef STANDALONE2
static int myerror(char *s) { oxprintfe("%s: type in control-C\n",s); getchar(); getchar(); return(0);}
#else
static int myerror(char *s) {
  oxprintfe("Error in jack-n.c: %s\n",s);
  mh_exit(-1);
  return(0);
}
#endif
static double jack1(int K) {
  double F;
  extern int Alpha;
  int J,II,JJ,N;   /*  int I,J,L,II,JJ,N; */
  N = 1;
  if (K == 0) return((double)1);
  F = xval(1,K);
  for (J=0; J<K; J++) {
    II = 1; JJ = J+1;
    F *= (N-(II-1)+Alpha*(JJ-1));
  }
  return(F);
}
static double jack1diff(int K) {
  double F;
  extern int Alpha;
  int J,II,JJ,N;  /* int I,J,S,L,II,JJ,N; */
  N = 1;
  if (K == 0) return((double)1);
  F = K*xval(1,K-1);
  for (J=0; J<K; J++) {
    II = 1; JJ = J+1;
    F *= (N-(II-1)+Alpha*(JJ-1));
  }
  return(F);
}

static double xval(int ii,int p) { /* x_i^p */
  extern double M_x[];
  int i,j;
  static int init=0;
  if (JK_deallocate) { init=0; return(0.0);}
  if (!init) {
    for (i=1; i<=M_n; i++) {
      for (j=0; j<M_m_MAX; j++) {
        if (j != 0) {
          Xarray[i-1][j] = M_x[i-1]*Xarray[i-1][j-1];
        }else{
          Xarray[i-1][0] = 1;
        }
      }
    }
    init = 1;
  }
  if (ii < 1) myerror("xval, index out of bound.");
  if (p > M_m_MAX-2) myerror("xval, p is too large.");
  if (p < 0) {
    myerror("xval, p is negative.");
    oxprintf("ii=%d, p=%d\n",ii,p);
    mh_exit(-1);
  }
  return(Xarray[ii-1][p]);
}

static int mysum(int L[]) {
  int S,I,N;
  N=M_n;
  S=0;
  for (I=0; I<N; I++) S += L[I];
  return(S);
}

/*
  (3,2,2,0,0) --> 3
*/
static int plength(int P[]) {
  int I;
  for (I=0; I<M_n; I++) {
    if (P[I] == 0) return(I);
  }
  return(M_n);
}
/* plength for transpose */
static int plength_t(int P[]) {  
  int I;
  for (I=0; I<M_m; I++) {
    if (P[I] == 0) return(I);
  }
  return(M_m);
}

/*  
    ptrans(P)  returns Pt
*/
static void ptrans(int P[M_nmx],int Pt[]) { /* Pt[M_m] */
  extern int M_m;
  int i,j,len;
  int p[M_nmx];
  for (i=0; i<M_n; i++) p[i] = P[i];
  for (i=0; i<M_m+1; i++) Pt[i] = 0;
  for (i=0; i<M_m; i++) {
    len=plength(p); Pt[i] = len;
    if (len == 0) return;
    for (j=0; j<len; j++) p[j] -= 1;
  }
}

#ifdef STANDALONE
static int test_ptrans(void) {
  extern int M_m;
  int p[M_n0]={5,3,2};
  int pt[10];
  int i;
  M_m = 10;
  ptrans(p,pt);
  if (Debug) {for (i=0; i<10; i++) oxprintf("%d,",pt[i]);  oxprintf("\n");}
  return(0);
}
#endif

/*
  upper hook length
  h_kappa^*(K)
*/
static int huk(int K[],int I,int J) {
  extern int Alpha;
  int Kp[M_m_MAX];
  int A,H;
  A=Alpha;
  /*oxprintf("h^k(%a,%a,%a,%a)\n",K,I,J,A);*/
  ptrans(K,Kp);
  H=Kp[J-1]-I+A*(K[I-1]-J+1);
  return(H);
} 

/*
  lower hook length
  h^kappa_*(K)
*/
static int hdk(int K[],int I,int J) {
  extern int Alpha;
  int Kp[M_m_MAX];
  int A,H;
  A = Alpha;
  /*oxprintf("h_k(%a,%a,%a,%a)\n",K,I,J,A);*/
  ptrans(K,Kp);
  H=Kp[J-1]-I+1+A*(K[I-1]-J);
  return(H);
} 
/*
  j_kappa.  cf. Stanley.
*/
static double jjk(int K[]) {
  extern int Alpha;
  int L,I,J;
  double V;

  V=1;
  L=plength(K);
  for (I=0; I<L; I++) {
    for (J=0; J<K[I]; J++) {
      V *= huk(K,I+1,J+1)*hdk(K,I+1,J+1);
    }
  }
  if (Debug) {printp(K); oxprintf("<--K, jjk=%lg\n",V);}
  return(V);
}
/*
  (a)_kappa^\alpha, Pochhammer symbol
  Note that  ppoch(a,[n]) = (a)_n, Alpha=2
*/
static double ppoch(double A,int K[]) {
  extern int Alpha;
  double V;
  int L,I,J,II,JJ;
  V = 1;
  L=plength(K);
  for (I=0; I<L; I++) {
    for (J=0; J<K[I]; J++) {
      II = I+1; JJ = J+1;
      V *= (A-((double)(II-1))/((double)Alpha)+JJ-1);
    }
  }
  return(V);
}
static double ppoch2(double A,double B,int K[]) {
  extern int Alpha;
  double V;
  int L,I,J,II,JJ;
  V = 1;
  L=plength(K);
  for (I=0; I<L; I++) {
    for (J=0; J<K[I]; J++) {
      II = I+1; JJ = J+1;
      V *= (A-((double)(II-1))/((double)Alpha)+JJ-1);
      V /= (B-((double)(II-1))/((double)Alpha)+JJ-1);
    }
  }
  return(V);
}
static double mypower(double x,int n) {
  int i;
  double v;
  if (n < 0) return(1/mypower(x,-n));
  v = 1;
  for (i=0; i<n; i++) v *= x;
  return(v);
}
/* Q_kappa
 */
static double qk(int K[],double A[],double B[]) {
  extern int Alpha;
  int P,Q,I;
  double V;
  P = A_LEN;
  Q = B_LEN;
  V = mypower((double) Alpha,mysum(K))/jjk(K);
  /* to reduce numerical errors, temporary. */
  if (P == Q) {
    for (I=0; I<P; I++) V = V*ppoch2(A[I],B[I],K);
	return(V);
  }

  if (P > Q) {
	for (I=0; I<Q; I++) V = V*ppoch2(A[I],B[I],K);
	for (I=Q; I<P; I++) V = V*ppoch(A[I],K);
  }else {
	for (I=0; I<P; I++) V = V*ppoch2(A[I],B[I],K);
	for (I=P; I<Q; I++) V = V/ppoch(B[I],K);
  }    
  /* for debug
  printf("K="); 
  for (I=0; I<3; I++) printf("%d, ",K[I]);
  printf("qk=%lg\n",V); 
  */
  return(V);
}

/*
  B^nu_{kappa,mu}(i,j)
  bb(N,K,M,I,J)
*/
static int bb(int N[],int K[],int M[],int I,int J) {
  int Kp[M_m_MAX]; int Mp[M_m_MAX];
  ptrans(K,Kp); 
  ptrans(M,Mp); 

  /*
    printp(K); oxprintf("K<--, "); printp2(Kp); oxprintf("<--Kp\n");
    printp(M); oxprintf("M<--, "); printp2(Mp); oxprintf("<--Mp\n");
  */

  if ((plength_t(Kp) < J) || (plength_t(Mp) < J)) return(hdk(N,I,J));
  if (Kp[J-1] == Mp[J-1]) return(huk(N,I,J));
  else return(hdk(N,I,J));
}
/*
  beta_{kappa,mu}
  beta(K,M)
*/
static double beta(int K[],int M[]) {
  double V;
  int L,I,J,II,JJ;
  V = 1;

  L=plength(K);
  for (I=0; I<L; I++) {
    for (J=0; J<K[I]; J++) {
      II = I+1; JJ = J+1;
      V *= (double)bb(K,K,M,II,JJ);
      /* oxprintf("[%d,%d,%lf]\n",I,J,V); */
    }
  }

  L=plength(M);
  for (I=0; I<L; I++) {
    for (J=0; J<M[I]; J++) {
      II = I+1; JJ = J+1;
      V /= (double)bb(M,K,M,II,JJ);
      /* oxprintf("[%d,%d,%lf]\n",I,J,V);*/
    }
  }

  return(V);
}
static int printp(int kappa[]) {
  int i;
  oxprintf("(");
  for (i=0; i<M_n; i++) {
    if (i <M_n-1) oxprintf("%d,",kappa[i]);
    else oxprintf("%d)",kappa[i]);
  }
  return(0);
}
#ifdef STANDALONE
static int printp2(int kappa[]) {
  int i,ell;
  oxprintf("(");
  ell = plength_t(kappa);
  for (i=0; i<ell+1; i++) {
    if (i <ell+1-1) oxprintf("%d,",kappa[i]);
    else oxprintf("%d)",kappa[i]);
  }
  return(0);
}

static int test_beta(void) {
  int kappa[M_n0]={2,1,0};
  int mu1[M_n0]={1,0,0};
  int mu2[M_n0]={1,1,0};
  int mu3[M_n0]={2,0,0};
  printp(kappa); oxprintf(","); printp(mu3); oxprintf(": beta = %lf\n",beta(kappa,mu3));
  printp(kappa); oxprintf(","); printp(mu1); oxprintf(": beta = %lf\n",beta(kappa,mu1));
  printp(kappa); oxprintf(","); printp(mu2); oxprintf(": beta = %lf\n",beta(kappa,mu2)); return(0);
}
#endif
/* main() { test_beta(); } */


/*
  cf. w1m.rr 
  matrix hypergeometric by jack
  N variables, up to degree M.
*/
/* todo
   def mhgj(A,B,N,M) {
   F = 0;
   P = partition_a(N,M);
   for (I=0; I<length(P); I++) {
   K = P[I];
   F += qk(K,A,B)*jack(K,N);
   }
   return(F);
   }
*/


/* The quotient of (3.10) of Koev-Edelman K=kappa, M=mu, SK=k */
static double q3_10(int K[],int M[],int SK) {
  extern int Alpha;
  int Mp[M_m_MAX];
  //  int ML[M_nmx];
  int N[M_nmx];
  int i,R;
  double T,Q,V,Ur,Vr,Wr;
  ptrans(M,Mp);
  for (i=0; i<M_n; i++) {N[i] = M[i];}
  N[SK-1] = N[SK-1]-1; 

  T = SK-Alpha*M[SK-1];
  Q = T+1; 
  V = Alpha;
  for (R=1; R<=SK; R++) {
    Ur = Q-R+Alpha*K[R-1];
    V *= Ur/(Ur+Alpha-1);
  }
  for (R=1; R<=SK-1; R++) {
    Vr = T-R+Alpha*M[R-1];
    V *= (Vr+Alpha)/Vr;
  }
  for (R=1; R<=M[SK-1]-1; R++) {
    Wr = Mp[R-1]-T-Alpha*R;
    V *= (Wr+Alpha)/Wr;
  }
  return(V);
}

#ifdef STANDALONE
static double q3_5(double A[],double B[],int K[],int I) {
  extern int Alpha;
  int Kp[M_m_MAX];
  double C,D,V,Ej,Fj,Gj,Hj,Lj;
  int J,P,Q;
  ptrans(K,Kp);
  P=A_LEN;; Q = B_LEN;
  C = -((double)(I-1))/Alpha+K[I-1]-1;
  D = K[I-1]*Alpha-I;

  V=1; 

  for (J=1; J<=P; J++)  {
    V *= (A[J-1]+C);
  }
  for (J=1; J<=Q; J++) {
    V /= (B[J-1]+C);
  }

  for (J=1; J<=K[I-1]-1; J++) {
    Ej = D-J*Alpha+Kp[J-1];
    Gj = Ej+1;
    V *= (Gj-Alpha)*Ej/(Gj*(Ej+Alpha));
  } 
  for (J=1; J<=I-1; J++) {
    Fj=K[J-1]*Alpha-J-D;
    Hj=Fj+Alpha;
    Lj=Hj*Fj;
    V *= (Lj-Fj)/(Lj+Hj);
  }
  return(V);
}
#endif
#ifdef STANDALONE
static void mtest4(void) {
  double A[1] = {1.5};
  double B[1]={6.5};
  int K[M_n0] = {3,2,0};
  int I=2; 
  int Ki[M_n0]={3,1,0};
  double V1,V2;
  setA(A,1); setB(B,1);
  V1=q3_5(A,B,K,I);
  V2=qk(K,A,B)/qk(Ki,A,B);
  oxprintf("%lf== %lf?\n",V1,V2);
}
static void mtest4b(void) {
  int K[M_n0]={3,2,0}; 
  int M[M_n0]={2,1,0}; 
  int N[M_n0]={2,0};
  int SK=2;
  double V1,V2;
  V1=q3_10(K,M,SK);
  V2=beta(K,N)/beta(K,M);
  oxprintf("%lf== %lf?\n",V1,V2);
}
#endif

/* main() { mtest4(); mtest4b(); } */

/* nk in (4.1),
 */
static int nk(int KK[]) {
  extern int *Darray;
  int N,I,Ki;
  int Kpp[M_nmx];
  int i;
  N = plength(KK);
  if (N == 0) return(0);
  if (N == 1) return(KK[0]);
  for (i=0; i<M_n; i++) Kpp[i] = 0;
  for (I=0; I<N-1; I++) Kpp[I] = KK[I];
  Ki = KK[N-1];
  /* K = (Kpp,Ki) */
  return(Darray[nk(Kpp)]+Ki-1);
}
#ifdef STANDALONE
static int plength2(int P1[],int P2[]) {
  int S1,S2;
  S1 = plength(P1); S2 = plength(P2);
  if (S1 > S2) return(1);
  else if (S1 == S2) {
    S1=mysum(P1); S2=mysum(P2);
    if(S1 > S2) return(1);
    else if (S1 == S2) return(0);
    else return(-1);
  }
  else return(-1);
}
#endif
static int myeq(int P1[],int P2[]) {
  int I,L1;
  if ((L1=plength(P1)) != plength(P2)) return(0);
  for (I=0; I<L1; I++) {
    if (P1[I] != P2[I]) return(0);
  }
  return(1);
}
/*
  M is a degree, N is a number of variables
  genDarray(3,3);
  N(0)=0;
  N(1)=1;
  N(2)=2;
  N(3)=3;
  N(1,1)=4;  D[1] = 4
  N(2,1)=5;  D[2] = 5;
  N(1,1,1)=6; D[4] = 6;
  still buggy.
*/

static int pListPartition(int M,int N) {
  extern int M_m;
  extern int M_kap[];
  int I;
  /* initialize */
  if (M_n != N) {
    oxprintfe("M_n != N\n"); mh_exit(-1);
  }
  M_m = M;
  /* M_plist = []; */
  /* end of initialize */
  (*M_pExec)();  /* exec for 0 */
  for (I=1; I<=M_n; I++) { 
    pListPartition2(M_m,1,I,M_m);
  }
  /* M_plist = reverse(M_plist); */
  return(1);
}

/*
  Enumerate all such that
  Less >= M_kap[From], ..., M_kap[To],  |(M_kap[From],...,M_kap[To])|<=M, 
*/
static int pListPartition2(int Less,int From,int To, int M) {
  int I;
  mh_check_intr(100);
  if (To < From) {
    (*M_pExec)(); return(0);
  }
  for (I=1; (I<=Less) && (I<=M) ; I++) {
    M_kap[From-1] = I;
    pListPartition2(I,From+1,To,M-I);
  } 
  return(1);
}

/*
  Commands to do for each partition are given here.
*/
static void pExec_0(void) {
  if (Debug) {
    oxprintf("M_kap=");
    printp(M_kap);
    oxprintf("\n");
  }
}

/* Test.
   Compare pListPartition(4,3);  genDarray(4,3);
   Compare pListPartition(5,3);  genDarray(5,3);

*/

/*
  main() {
  M_pExec = pExec_0;
  pListPartition(5,3);
  }
*/


/*
  List all horizontal strips.
  Kap[0] is not a dummy in C code. !(Start from Kap[1].)
*/
static int pListHS(int Kap[],int N) {
  extern int HS_n;
  extern int HS_mu[];
  int i;
  HS_n = N;
  /* Clear HS_mu. Do not forget when N < M_n  */
  for (i=0; i<M_n; i++) HS_mu[i] = 0;
  return(pListHS2(1,N,Kap));
}

static int pListHS2(int From,int To,int Kap[]) {
  int More,I;
  if (To <From) {(*HS_hsExec)(); return(0);}
  if (From == HS_n) More=0; else More=Kap[From];
  for (I=Kap[From-1]; I>= More; I--) {
    HS_mu[From-1] = I;
    pListHS2(From+1,To,Kap);
  } 
  return(1);
}

static void hsExec_0(void) {
  /* int i; */
  if(Debug) {oxprintf("hsExec: "); printp(HS_mu); oxprintf("\n");}
}

/*
  pListHS([0,4,2,1],3);
*/
/*
  main() {
  int Kap[3]={4,2,1};
  HS_hsExec = hsExec_0;
  pListHS(Kap,3);
  }
*/

/* The number of partitions <= M, with N parts.
   (0,0,...,0) is excluded.
*/
#define aP_pki(i,j) P_pki[(i)*(M+1)+(j)]
static int pmn(int M,int N) {
  int Min_m_n,I,K,S,T,i,j;
  extern int P_pmn;
  extern int *P_pki;
  Min_m_n = (M>N?N:M);
  /* P_pki=newmat(Min_m_n+1,M+1); */
  P_pki = (int *) mymalloc(sizeof(int)*(Min_m_n+1)*(M+1));
  for (i=0; i<Min_m_n+1; i++) for (j=0; j<M+1; j++) aP_pki(i,j) = 0;
  for (I=1; I<=M; I++) aP_pki(1,I) = 1;
  for (K=1; K<=Min_m_n; K++) aP_pki(K,0) = 0;
  S = M;
  for (K=2; K<=Min_m_n; K++) {
    for (I=1; I<=M; I++) {
      if (I-K < 0) T=0; else T=aP_pki(K,I-K);
      aP_pki(K,I) = aP_pki(K-1,I-1)+T;
      S += aP_pki(K,I);
    }
  }
  P_pmn=S;
  if (Debug) {
    oxprintf("P_pmn=%d\n",P_pmn);
    for (i=0; i<=Min_m_n; i++) {
      for (j=0; j<=M; j++) oxprintf("%d,",aP_pki(i,j));
      oxprintf("\n");
    }
  }
  myfree(P_pki); P_pki=NULL;
  return(S);
}

/*
  main() {pmn(4,3); oxprintf("P_pmn=%d\n",P_pmn);}
*/

static int *cloneP(int a[]) {
  int *p;
  int i;
  p = (int *) mymalloc(sizeof(int)*M_n);
  for (i=0; i<M_n; i++) p[i] = a[i];
  return(p);
}
static int copyP(int p[],int a[]) {
  int i;
  for (i=0; i<M_n; i++) p[i] = a[i];
  return(0);
}

static void pExec_darray(void) {
  extern int DR_parray;
  extern int M_kap[];
  extern int **Parray;
  extern int *ParraySize;
  int *K;
  pExec_0();
  K = cloneP(M_kap);
  Parray[DR_parray] = K;
  ParraySize[DR_parray] = mysum(K);
  DR_parray++;
}
static int genDarray2(int M,int N) {
  extern int *Darray;
  extern int **Parray;
  extern int DR_parray;
  extern int M_m;
  int Pmn,I,J,Ksize,i;
  int **L;
  int *Nk;
  int *K;
  int Kone[M_nmx];

  M_m = M;
  Pmn = pmn(M,N)+1;
  if (Debug) oxprintf("Degree M = %d, N of vars N = %d, Pmn+1=%d\n",M,N,Pmn);
  Darray=(int *) mymalloc(sizeof(int)*Pmn);
  for (i=0; i<Pmn; i++) Darray[i] = 0;
  Parray=(int **) mymalloc(sizeof(int *)*Pmn);
  for (i=0; i<Pmn; i++) Parray[i] = NULL;
  ParraySize=(int *) mymalloc(sizeof(int *)*Pmn);
  for (i=0; i<Pmn; i++) ParraySize[i] = 0;
  DR_parray=0;
  M_pExec = pExec_darray;  
  pListPartition(M,N);  /* pExec_darray() is executed for all partitions */
  L = Parray;

  Nk = (int *) mymalloc(sizeof(int)*(Pmn+1));
  for (I=0; I<Pmn; I++) Nk[I] = I;
  for (I=0; I<Pmn; I++) {
    mh_check_intr(100);
    K = L[I]; /* N_K = I; D[N_K] = N_(K,1) */
    Ksize = plength(K);
    if (Ksize >= M_n) {
      if (Debug) {oxprintfe("Ksize >= M_n\n");}
      continue;
    }
    for (i=0; i<M_n; i++) Kone[i] = 0;
    for(J=0; J<Ksize; J++) {Kone[J]=K[J];} Kone[Ksize] = 1;
    for (J=0; J<Pmn; J++) {
      if (myeq(L[J],Kone)) Darray[I] = J; /* J is the next of I */
    }
  }
  if (Debug) {
    oxprintf("Darray=\n");
    for (i=0; i<Pmn; i++) oxprintf("%d\n",Darray[i]);
    oxprintf("-----------\n");
  }
  return(0);
}


/* main() {  genDarray2(4,3);}  */

/* M_beta_0[*] value of beta_{kappa,mu}, M_beta_1[*] N_mu */
static int isHStrip(int Kap[],int Nu[]) {
  int N1,N2,I,P;
  N1 = plength(Kap); N2 = plength(Nu);
  if (N2 > N1) return(0);
  for (I=0; I<N2; I++) {
    if (I >= N1-1) P = 0; else P=Kap[I+1]; 
    if (Kap[I] < Nu[I]) return(0);
    if (Nu[I]  < P) return(0);
  }
  return(1);
}

static void hsExec_beta(void) {
  int *Mu;
  int N,Nmu,Nnu,Done,J,K,OK,I,RR;
  int Kapt[M_m_MAX];
  int Nut[M_m_MAX];
  int Nu[M_nmx];
  int rrMax;
  hsExec_0();
  /* oxprintf("M_beta_pt=%a\n",M_beta_pt); */
  /* Mu = cdr(vtol(HS_mu)); */
  Mu = HS_mu; /* buggy? need cloneP */
  if (M_beta_pt == 0) {
    M_beta_0[0] = 1; M_beta_1[0] = nk(Mu); 
    M_beta_pt++; return;
  }

  N = HS_n;
  Nmu = nk(Mu);
  M_beta_1[M_beta_pt] = Nmu;
  ptrans(M_beta_kap,Kapt);
  /* Mu, Nu is exchanged in this code. cf. the K-E paper  */
  copyP(Nu,Mu); /* buggy need clone? */
  for (I=0; I<N; I++) {
    Nu[I]++;
    if (!isHStrip(M_beta_kap,Nu)) {Nu[I]--; continue;}
    Nnu = nk(Nu);
    ptrans(Nu,Nut);
    Done=0;
    for (J=M_beta_pt-1; J>=0; J--) {
      if (M_beta_1[J] == Nnu) {
        K=I+1;
        if (Debug) {
          oxprintf("Found at J=%d, K=%d, q3_10(Kap,Nu,K)=%lf,Nu,Mu= \n",
                 J,K,q3_10(M_beta_kap,Nu,K));
          printp(Nu); oxprintf("\n");
          printp(Mu); oxprintf("\n");
        }
        /* Check other conditions. See Numata's mail on Dec 24, 2011. */
        rrMax = Nu[I]-1;                 
        if ((plength_t(Kapt) < rrMax) || (plength_t(Nut) < rrMax)) {
          if (Debug) oxprintf(" is not taken (length). \n");
          break;
        }
        OK=1; 
        for (RR=0; RR<rrMax; RR++) {
          if (Kapt[RR] != Nut[RR]) { OK=0; break;}
        }
        if (!OK) { if (Debug) oxprintf(" is not taken.\n"); break; }
        /* check done. */
        M_beta_0[M_beta_pt]=M_beta_0[J]*q3_10(M_beta_kap,Nu,K);
        Done = 1; break;
      } 
    }
    if (Done) break; else Nu[I]--;
  }
  if (!Done) {
    if (Debug) oxprintf("BUG: not found M_beta_pt=%d.\n",M_beta_pt);
    /* M_beta_0[M_beta_pt] = NAN;  error("Not found."); */
    M_beta_0[M_beta_pt] = beta(M_beta_kap,Mu); 
  }
  /* Fix the bug of mh.rr */
  M_beta_pt++;
}
static int genBeta(int Kap[]) {
  extern double *M_beta_0;
  extern int *M_beta_1;
  extern int M_beta_pt;
  extern int M_beta_kap[];
  extern int P_pmn;
  int I,N;
  if (Debug) {printp(Kap); oxprintf("<-Kappa, P_pmn=%d\n",P_pmn);}
  /* M_beta = newmat(2,P_pmn+1); */
  M_beta_0 = (double *)mymalloc(sizeof(double)*(P_pmn+1));
  M_beta_1 = (int *)mymalloc(sizeof(int)*(P_pmn+1));
  M_beta_pt = 0;
  for (I=0; I<=P_pmn; I++) {M_beta_0[I] = NAN; M_beta_1[I] = -1;}
  N = plength(Kap);
  HS_hsExec = hsExec_beta;
  copyP(M_beta_kap,Kap);
  pListHS(Kap,N); return(0);
}
/*
  genDarray2(4,3);
  genBeta([2,2,0]);
  genBeta([2,1,1]);
*/
#ifdef STANDALONE
static int checkBeta1(void) {
  int Kap[3] = {2,2,0};
  int Kap2[3] = {2,1,0};
  int I;
  int *Mu;
  double Beta_km;
  genDarray2(4,3);
  genBeta(Kap);
  for (I=0; I<M_beta_pt; I++) {
    Mu = Parray[M_beta_1[I]];
    Beta_km = M_beta_0[I];
    if (Debug) {
      printp(Kap); oxprintf("<--Kap, ");
      printp(Mu); oxprintf("<--Mu,");
      oxprintf("Beta_km(by table)=%lf, beta(Kap,Mu)=%lf\n",Beta_km,beta(Kap,Mu));
    }
  }
  if (Debug) oxprintf("-------------------------------------\n");
  genBeta(Kap2);
  for (I=0; I<M_beta_pt; I++) {
    Mu = Parray[M_beta_1[I]];
    Beta_km = M_beta_0[I];
    if (Debug) {
      printp(Kap2); oxprintf("<--Kap, ");
      printp(Mu); oxprintf("<--Mu,");
      oxprintf("Beta_km(by table)=%lf, beta(Kap,Mu)=%lf\n",Beta_km,beta(Kap2,Mu));
    }
  }
  return(0);
}
#endif
/*
  def checkBeta2() {
  genDarray2(3,3);
  Kap = [2,1,0];
  oxprintf("Kap=%a\n",Kap);
  genBeta(Kap);
  for (I=0; I<M_beta_pt; I++) {
  Mu = Parray[M_beta[1][I]];
  Beta_km = M_beta[0][I];
  oxprintf("Mu=%a,",Mu);
  oxprintf("Beta_km(by table)=%a, beta(Kap,Mu)=%a\n",Beta_km,beta(Kap,Mu));
  }
  }
*/

/* main() { checkBeta1(); } */

static int psublen(int Kap[],int Mu[]) {
  int L1,L2,A,I;
  L1 = plength(Kap);
  L2 = plength(Mu);
  if (L2 > L1) myerror("psub, length mismatches.");
  A = 0;
  for (I=0; I<L2; I++) {
    if (Kap[I] < Mu[I]) myerror("psub, not Kap >= Mu"); 
    A += Kap[I]-Mu[I];
  }
  for (I=L2; I<L1; I++) A += Kap[I];
  return(A);
}


/* Table of Jack polynomials
   Jack[1]* one variable.
   Jack[2]* two variables.
   ...
   Jack[M_n]* n variables.
   Jack[P][J]*  
   D^J(P variables jack of p variables). Example. J=001 d_1, 010 d_2, 100 d_3
   0<=J<=2^{M_n}-1
   Jack[P][J][nk(Kappa)]  Jack_Kappa, Kappa is a partition.
   0<=nk(Kappa)<=pmn(M_m,M_n)  
*/

#define aM_jack(i,j,k) ((M_jack[i])[(j)*(Pmn+1)+(k)])
static int genJack(int M,int N) {
  extern double **M_jack;
  extern int M_2n;
  extern int P_pmn;
  extern int *M_beta_1;
  int Pmn,I,J,K,L,Nv,H,P;
  int *Kap,*Mu;
  double Jack,Beta_km;
  int Nk,JJ, two_to_I;
  if (Debug) oxprintf("genJack(%d,%d)\n",M,N);
  M_jack = (double **) mymalloc(sizeof(double *)*(N+2));
  M_2n = imypower(2,N);
  Pmn = pmn(M,N);  /*P_pmn is initializeded. 
                     Warning. It is reset when pmn is called.*/
  for (I=0; I<=N; I++) M_jack[I] = (double *)mymalloc(sizeof(double)*(M_2n*(Pmn+1))); /* newmat(M_2n,Pmn+1); */
  M_jack[N+1] = NULL;
  genDarray2(M,N); /* Darray, Parray is initialized */
  for (I=1; I<=N; I++) aM_jack(I,0,0) = 1; 
  if (M_df) {
    for (I=1; I<=N; I++) {
      for (J=1; J<M_2n; J++) aM_jack(I,J,0) = 0;
    }
  }

  /* N must satisfies N > 0 */
  for (K=1; K<=M; K++) {
    aM_jack(1,0,K) = jack1(K);
    if (M_df) {
      aM_jack(1,1,K) = jack1diff(K); /* diff(jack([K],1),x_1); */
      for (J=2; J<M_2n; J++) aM_jack(1,J,K) = 0;
    }
  }
  for (I=1; I<=N; I++) {   two_to_I = imypower(2,I);
    for (K=M+1; K<Pmn+1; K++) {
      aM_jack(I,0,K) = NAN;
      if (M_df) {
        for (J=1; J<M_2n; J++) {
          if (J >= two_to_I) aM_jack(I,J,K) = 0; /* J >= 2^I */
          else aM_jack(I,J,K) = NAN;
        }
      }
    }
  }

  /* Start to evaluate the entries of the table */
  for (K=1; K<=Pmn; K++) {
    Kap = Parray[K]; /* bug. need copy? */
    L = plength(Kap);
    for (I=1; I<=L-1; I++) {
      aM_jack(I,0,K) = 0;
      if (M_df) {
        for (J=1; J<M_2n; J++) aM_jack(I,J,K) = 0;
      } 
    }
    if (Debug) {oxprintf("Kappa="); printp(Kap);}
    /* Enumerate horizontal strip of Kappa */
    genBeta(Kap);  /* M_beta_pt stores the number of hs */
    /* Nv is the number of variables */
    for (Nv = (L==1?2:L); Nv <= N; Nv++) {
      Jack = 0;
      for (H=0; H<M_beta_pt; H++) {
        Nk = M_beta_1[H];
        Mu = Parray[Nk];
        if (UseTable) {
          Beta_km = M_beta_0[H];
        }else{
          Beta_km = beta(Kap,Mu);
          /* do not use the M_beta table. It's buggy. UseTable is experimental.*/
        }
        if (Debug) {oxprintf("Nv(number of variables)=%d, Beta_km=%lf, Mu=",Nv,Beta_km);
          printp(Mu); oxprintf("\n");}
        P = psublen(Kap,Mu);
        Jack += aM_jack(Nv-1,0,Nk)*Beta_km*xval(Nv,P); /* util_v(x,[Nv])^P;*/
        if (Debug) oxprintf("xval(%d,%d)=%lf\n",Nv,P,xval(Nv,P));
      }
      aM_jack(Nv,0,K) = Jack;
      if (M_df) {
        /* The case of M_df > 0. */ 
        for (J=1; J<M_2n; J++) {
          mh_check_intr(100);
          Jack = 0;
          for (H=0; H<M_beta_pt; H++) {
            Nk = M_beta_1[H];
            Mu = Parray[Nk];
            if (UseTable) {
              Beta_km = M_beta_0[H];
            }else{
              Beta_km = beta(Kap,Mu); /* do not use the M_beta table. It's buggy. */
            }
            if (Debug) {oxprintf("M_df: Nv(number of variables)=%d, Beta_km=%lf, Mu= ",Nv,Beta_km);
              printp(Mu); oxprintf("\n"); }
            P = psublen(Kap,Mu);
            if (J & (1 << (Nv-1))) {
              JJ = J & ((1 << (Nv-1)) ^ 0xffff);  /* NOTE!! Up to 16 bits. mh-15 */
              if (P != 0) {
                Jack += aM_jack(Nv-1,JJ,Nk)*Beta_km*P*xval(Nv,P-1);
              }
            }else{
              Jack += aM_jack(Nv-1,J,Nk)*Beta_km*xval(Nv,P);
            }
          }
          aM_jack(Nv,J,K) = Jack;
          if (Debug) oxprintf("aM_jack(%d,%d,%d) = %lf\n",Nv,J,K,Jack);
        } /* end of J loop */
      }
    }
  } return(0);
}  

#ifdef STANDALONE
/* checkJack1(3,3) 
 */
static int checkJack1(int M,int N) {
  int I,K;
  extern int P_pmn;
  extern double M_x[];
  int Pmn; /* used in aM_jack */
  /* initialize x vars. */
  for (I=1; I<=N; I++) {
    M_x[I-1] = ((double)I)/10.0;
  }
  genJack(M,N);
  Pmn = P_pmn;
  for (I=1; I<=N; I++) {
    for (K=0; K<=P_pmn; K++) {
      printp(Parray[K]);
      oxprintf("<--Kap, Nv=%d, TableJack=%lf\n",I,aM_jack(I,0,K));
    }
  }
  for (I=1; I<=N; I++) oxprintf("%lf, ",M_x[I-1]);
  oxprintf("<--x\n");
  return(0);
}
/*main() {  checkJack1(3,3);  }*/


static int checkJack2(int M,int N) {
  int I,K,J;
  extern int P_pmn;
  extern double M_x[];
  extern int M_df;
  int Pmn; /* used in aM_jack */
  M_df=1;
  /* initialize x vars. */
  for (I=1; I<=N; I++) {
    M_x[I-1] = ((double)I)/10.0;
  }
  genJack(M,N);
  Pmn = P_pmn;
  for (I=1; I<=N; I++) {
    for (K=0; K<=P_pmn; K++) {
      printp(Parray[K]);
      oxprintf("<--Kap, Nv=%d, TableJack=%lf\n",I,aM_jack(I,0,K));
    }
  }
  for (I=1; I<=N; I++) oxprintf("%lf, ",M_x[I-1]);
  oxprintf("<--x\n");

  for (I=1; I<=N; I++) {
    for (K=0; K<=P_pmn; K++) {
      for (J=0; J<M_2n; J++) {
        printp(Parray[K]);
        oxprintf("<--Kap, Nv=%d,J(diff)=%d, D^J Jack=%lf\n",
               I,J,aM_jack(I,J,K));
      }
    }
  }
  return(0);
}

/* main() { checkJack2(3,3); } */
#endif

double mh_t(double A[],double B[],int N,int M) {
  double F,F2;
  extern int M_df;
  extern int P_pmn;
  extern double *M_qk;
  extern double M_rel_error;
  extern int M_m;
  extern int M_m_estimated_approx_deg;
  extern double M_assigned_series_error;
  int Pmn;
  int K;
  int *Kap;
  int size;
  int i;
  double partial_sum[M_m_MAX+1];
  double iv;
  double serror;
  F = 0; F2=0;
  M_df=1;
  genJack(M,N);
  M_qk = (double *)mymalloc(sizeof(double)*(P_pmn+1)); /* found a bug. */
  Pmn = P_pmn;
  size = ParraySize[P_pmn];
  for (K=0; K<=P_pmn; K++) {
    mh_check_intr(100);
    Kap = Parray[K];
    M_qk[K] = qk(Kap,A,B);
    F += M_qk[K]*aM_jack(N,0,K);
    if (ParraySize[K] < size) F2 += M_qk[K]*aM_jack(N,0,K);
    if (Debug) oxprintf("ParraySize[K] = %d, size=%d\n",ParraySize[K],size);
    if (Debug && (ParraySize[K] == size)) oxprintf("M_qk[K]=%lg, aM_jack=%lg\n",M_qk[K],aM_jack(N,0,K));
  }
  M_rel_error = F-F2;

  M_m_estimated_approx_deg = -1; serror=1;
  for (i=0; i<=M_m; i++) {
    partial_sum[i] = 0.0; partial_sum[i+1] = 0.0;
    for (K=0; K<=P_pmn; K++) {
      if (ParraySize[K] == i) partial_sum[i] += M_qk[K]*aM_jack(N,0,K);
    }
    if (i>0) partial_sum[i] += partial_sum[i-1];
    if (i>0) serror = myabs((partial_sum[i]-partial_sum[i-1])/partial_sum[i-1]);
    if ((i>0)&&(M_m_estimated_approx_deg < 0)&&(serror<M_assigned_series_error)) {
      M_m_estimated_approx_deg = i; break;
    }
  }
  if (M_m_estimated_approx_deg < 0) {
    M_m_estimated_approx_deg = M_m+mymin(5,mymax(1,(int)log(serror/M_assigned_series_error))); /* Heuristic */
  }
  /*
  for (K=0; K<=P_pmn; K++) {
    oxprintf("Kappa="); for (i=0; i<N; i++) oxprintf("%d ",Parray[K][i]); oxprintf("\n"); 
    oxprintf("ParraySize(%d)=%d (|kappa|),   M_m=%d\n",K,ParraySize[K],M_m);
  }
  for (i=0; i<=M_m; i++) {
    oxprintf("partial_sum[%d]=%lg\n",i,partial_sum[i]);
  }
  */
  M_estimated_X0g = X0g;
  iv=myabs(F*iv_factor());
  if (iv < M_x0value_min) M_estimated_X0g = X0g*mymax(2,log(log(1/iv)));   /* This is heuristic */
  M_estimated_X0g = mymin(M_estimated_X0g,M_X0g_bound);
  M_mh_t_success = 1;
  if (M_estimated_X0g != X0g) M_mh_t_success=0;
  if (M_m_estimated_approx_deg > M_m) M_mh_t_success=0;

  M_series_error = serror;
  M_recommended_abserr = iv*M_assigned_series_error;
  M_recommended_relerr = M_series_error;

  if (M_show_autosteps) {
    oxprintf("%%%%serror=%lg, M_assigned_series_error=%lg, M_m_estimated_approx_deg=%d,M_m=%d\n",serror,M_assigned_series_error,M_m_estimated_approx_deg,M_m);
    oxprintf("%%%%x0value_min=%lg, x0g_bound=%lg\n",M_x0value_min, M_X0g_bound);
    oxprintf("%%%%F=%lg,Ef=%lg,M_estimated_X0g=%lg, X0g=%lg\n",F,iv_factor(),M_estimated_X0g,X0g);
    oxprintfe("%%%%(stderr) serror=%lg, M_assigned_series_error=%lg, M_m_estimated_approx_deg=%d,M_m=%d\n",serror,M_assigned_series_error,M_m_estimated_approx_deg,M_m);
    oxprintfe("%%%%(stderr) x0value_min=%lg, x0g_bound=%lg\n",M_x0value_min, M_X0g_bound);
    oxprintfe("%%%%(stderr) F=%lg,Ef=%lg,iv=|F*Ef|=%lg,M_estimated_X0g=%lg, X0g=%lg\n",F,iv_factor(),iv,M_estimated_X0g,X0g);
  }
  if (isnan(F)) myerror("F is nan. Make q0 smaller (or X0g smaller on the standalone system).\n");
  if (!isnormal(iv)) myerror("F*Ef (initial value) is zero. Make q0 larger (or X0g larger on the standalone system).\n");

  M_mh_t_value=F;
  return(F);
}
double mh_t2(int J) {
  extern double *M_qk;
  double F;
  int K;
  int Pmn;
  extern int P_pmn;
  if (M_qk == NULL) {myerror("Call mh_t first."); mh_exit(-1); }
  F = 0; 
  Pmn = P_pmn;
  for (K=0; K<P_pmn; K++) {
    F += M_qk[K]*aM_jack(M_n,J,K);
  }
  return(F);
}

#ifdef STANDALONE
static int mtest1b(void) {
  double A[1] = {1.5};
  double B[1] = {1.5+5};
  int I,N,M,J;
  double F;
  N=3; M=6;
  for (I=1; I<=N; I++) {
    M_x[I-1] = ((double)I)/10.0;
  }
  mh_t(A,B,N,M);
  for (J=0; J<M_2n; J++) {
    F=mh_t2(J);
    oxprintf("J=%d, D^J mh_t=%lf\n",J,F);
  }
  return(0);
}

/* main() { mtest1b(); }*/
#endif



#define TEST 1
#ifndef TEST

#endif

/****** from mh-n.c *****/ 

#define SMAX 4096
#define inci(i) { i++; if (i >= argc) { oxprintfe("Option argument is not given.\n"); return(NULL); }}

static int imypower(int x,int n) {
  int i;
  int v;
  if (n < 0) {myerror("imypower"); mh_exit(-1);}
  v = 1;
  for (i=0; i<n; i++) v *= x;
  return(v);
}

#ifdef STANDALONE2
int main(int argc,char *argv[]) {
  mh_exit(MH_RESET_EXIT);
  /*  jk_main(argc,argv);
      oxprintf("second run.\n"); */
  jk_main(argc,argv);
  return(0);
}
#endif

struct MH_RESULT *jk_main(int argc,char *argv[]) {
  int i;
  struct MH_RESULT *ans;
  extern int M_automatic;
  extern int M_mh_t_success;
  extern double M_estimated_X0g;
  extern int M_m_estimated_approx_deg;
  for (i=1; i<argc; i++) {
    if (strcmp(argv[i],"--automatic")==0) {
      inci(i);
      sscanf(argv[i],"%d",&M_automatic);
      break;
    }
  }
  ans=jk_main2(argc,argv,0,0.0,0);
  if (!M_automatic) return(ans);
  if (M_mh_t_success) return(ans);
  while (!M_mh_t_success) {
    ans=jk_main2(argc,argv,1,M_estimated_X0g,M_m_estimated_approx_deg);
  }
  return(ans);
}

struct MH_RESULT *jk_main2(int argc,char *argv[],int automode,double newX0g,int newDegree) {
  // double *y0;
  //  double x0,xn;  
  // double ef;

  int i,j; // int i,j,rank;
  extern double M_x[];
  extern double *Beta;
  extern int M_2n;
  extern int M_mh_t_success;
  char swork[1024];
  struct MH_RESULT *ans=NULL;
  struct SFILE *ofp = NULL;
  int idata=0;
  JK_byFile = 1; for (i=0; i<1024; i++) swork[i]=0;
  jk_initializeWorkArea();
  UseTable = 1;
  Mapprox=6;
  for (i=1; i<argc; i++) {
    if (strcmp(argv[i],"--idata")==0) {
      inci(i);
      setParam(argv[i]); idata=1;
    }else if (strcmp(argv[i],"--degree")==0) {
      inci(i);
      sscanf(argv[i],"%d",&Mapprox);
    }else if (strcmp(argv[i],"--x0")==0) {
      inci(i);
      sscanf(argv[i],"%lg",&X0g);
    }else if (strcmp(argv[i],"--notable")==0) {
      UseTable = 0;
    }else if (strcmp(argv[i],"--debug")==0) {
      Debug = 1;
    }else if (strcmp(argv[i],"--help")==0) {
      usage(); return(0);
    }else if (strcmp(argv[i],"--bystring")==0) {
      if (idata) {oxprintfe("--bystring must come before --idata option.\n"); mh_exit(-1);}
      JK_byFile = 0;
    }else if (strcmp(argv[i],"--automatic")==0) {
      inci(i); /* ignore, in this function */
    }else if (strcmp(argv[i],"--assigned_series_error")==0) {
      inci(i);
      sscanf(argv[i],"%lg",&M_assigned_series_error);
    }else if (strcmp(argv[i],"--x0value_min")==0) {
      inci(i);
      sscanf(argv[i],"%lg",&M_x0value_min);
    }else {
      oxprintfe("Unknown option %s\n",argv[i]);
      usage();
      return(NULL);
    }
  }
  if (!idata) setParam(NULL);
  if (automode) {
    Mapprox = newDegree;
    X0g = newX0g;
  }

  /* Initialize global variables */
  M_n = Mg;
  HS_n=M_n;
  if (!JK_byFile) {
    ans = (struct MH_RESULT *)mymalloc(sizeof(struct MH_RESULT));
    ans->message = NULL;
    ans->t_success = 0;
    ans->series_error = 1.0e+10;
    ans->recommended_abserr = 1.0e-10;
  }
  else ans = NULL;
  if (M_automatic) {
    /* Differentiation can be M_m in the bit pattern in the M_n variable case.*/
    if (M_n > Mapprox) Mapprox=M_n;
  }
  /* Output by a file=stdout */
  ofp = mh_fopen("stdout","w",JK_byFile);

  sprintf(swork,"%%%%Use --help option to see the help.\n"); mh_fputs(swork,ofp);
  sprintf(swork,"%%%%Mapprox=%d\n",Mapprox); mh_fputs(swork,ofp);
  if (M_n != Mg) {
    myerror("Mg must be equal to M_n\n"); mh_exit(-1);
  }
  if (Debug) showParam(NULL,1);
  setM_x();

  M_beta_i_x_o2_max=myabs(M_x[0]/2);
  if (M_n <= 1) M_beta_i_beta_j_min = myabs(Beta[0]);
  else M_beta_i_beta_j_min = myabs(Beta[1]-Beta[0]);
  for (i=0; i<M_n; i++) {
    if (myabs(M_x[i]/2) > M_beta_i_x_o2_max) M_beta_i_x_o2_max = myabs(M_x[i]/2);
    for (j=i+1; j<M_n; j++) {
      if (myabs(Beta[i]-Beta[j]) < M_beta_i_beta_j_min) 
        M_beta_i_beta_j_min = myabs(Beta[i]-Beta[j]);
    }
  }

  mh_t(A_pFq,B_pFq,M_n,Mapprox);
  if ((!M_mh_t_success) && M_automatic) {
    jk_freeWorkArea();
    return NULL;
  }
  if (imypower(2,M_n) != M_2n) {
    sprintf(swork,"M_n=%d,M_2n=%d\n",M_n,M_2n); mh_fputs(swork,ofp);
    myerror("2^M_n != M_2n\n"); mh_exit(-1);
  }
  sprintf(swork,"%%%%M_rel_error=%lg\n",M_rel_error); mh_fputs(swork,ofp);
  for (j=0; j<M_2n; j++) {
    Iv[j] = mh_t2(j);
  }
  Ef = iv_factor();
  showParam(ofp,0);

  /* return the result */
  if (!JK_byFile) { 
    ans->x = X0g;
    ans->rank = imypower(2,Mg);
    ans->y = (double *)mymalloc(sizeof(double)*(ans->rank));
    for (i=0; i<ans->rank; i++) (ans->y)[i] = Iv[i];
    ans->size=1;
    ans->sfpp = (struct SFILE **)mymalloc(sizeof(struct SFILE *)*(ans->size));
    (ans->sfpp)[0] = ofp;

    ans->t_success = M_mh_t_success;
    ans->series_error = M_series_error;
    ans->recommended_abserr = M_recommended_abserr;
  }
  if (Debug) oxprintf("jk_freeWorkArea() starts\n");
  jk_freeWorkArea();
  if (Debug) oxprintf("jk_freeWorkArea() has finished.\n");
  return(ans);
}

static int usage(void) {
  oxprintfe("Usages:\n");
#include "usage-jack-n.h"
  return(0);
}

static int setParamDefault(void) {
  int rank;
  int i;
  Mg = M_n_default ;
  rank = imypower(2,Mg);
  Beta = (double *)mymalloc(sizeof(double)*Mg);
  for (i=0; i<Mg; i++) Beta[i] = 1.0+i;
  Ng = (double *)mymalloc(sizeof(double)); *Ng = 3.0;
  Iv = (double *)mymalloc(sizeof(double)*rank);
  Iv2 = (double *)mymalloc(sizeof(double)*rank);
  for (i=0; i<rank; i++) Iv[i] = 0;
  Ef = 0;
  Ef2 = 0.01034957388338225707;
  if (M_n == 2) {
    Iv2[0] = 1.58693;
    Iv2[1] = 0.811369;
    Iv2[2] = 0.846874;
    Iv2[3] = 0.413438;
  }
  X0g = (Beta[0]/Beta[Mg-1])*0.5;
  Hg = 0.001;
  Dp = 1;
  Xng = 10.0;
  setA(NULL,1); setB(NULL,1);
  A_pFq[0] = ((double)Mg+1.0)/2.0;
  B_pFq[0] = ((double)Mg+1.0)/2.0 + ((double) (*Ng))/2.0; 
  return(0);
}

static int next(struct SFILE *sfp,char *s,char *msg) {
  static int check=1;
  char *ng="%%Ng=";
  // int i;
  s[0] = '%';
  while ((s[0] == '%') || (s[0] == '#')) {
    if (!mh_fgets(s,SMAX,sfp)) {
      oxprintfe("Data format error at %s\n",msg);
      oxprintfe("Is it version 2.0 format? If so, add\n%s\nat the top.\n",VSTRING);
      mh_exit(-1);
    }
	if ((s[0] == '%') && (s[1] == '%')) continue;
    if (s[0] == '#') continue;
    if (strncmp(s,VSTRING,strlen(VSTRING)) == 0) {
	  return(2);
	}
    if (check && (strncmp(msg,ng,4)==0)) {
      if (strncmp(s,ng,5) != 0) {
        oxprintfe("Warning, there is no %%Ng= at the border of Beta's and Ng, s=%s\n",s);
      }
      /* check=0; */
    }
    if (s[0] != '%') return(0);
  }
  return(0);
}
static int setParam(char *fname) {
  int rank=2;
  char s[SMAX];
  struct SFILE *fp;
  int i;
  struct mh_token tk;
  int version;
  if (fname == NULL) return(setParamDefault());

  Sample = 0; for (i=0; i<SMAX; i++) s[i]=0;
  if ((fp=mh_fopen(fname,"r",JK_byFile)) == NULL) {
    if (JK_byFile) oxprintfe("File %s is not found.\n",fname);
    mh_exit(-1);
  }
  /* set default initial values */
  Mg=-1;  /* number of variables */
  Ng=(double *) mymalloc(sizeof(double)); *Ng=-1; /* *Ng is the degree of freedom 1F1 */
  X0g=0.1;   /* evaluation point */
  Ef=1.0;    /* exponential factor */
  Ef_type=1;
  Hg=0.01;   /* step size for RK */
  Dp = 1;   /* sampling rate */
  Xng = 10.0; /* terminal point for RK */

  /* Parser for the old style (version <2.0) input */
  version=next(fp,s,"Mg(m)");
  if (version == 2) goto myparse;
  sscanf(s,"%d",&Mg);
  rank = imypower(2,Mg);

  Beta = (double *)mymalloc(sizeof(double)*Mg);
  for (i=0; i<Mg; i++) {
    next(fp,s,"Beta");
    sscanf(s,"%lf",&(Beta[i]));
  }

  next(fp,s,"%Ng= (freedom parameter n)");
  sscanf(s,"%lf",Ng);
  
  next(fp,s,"X0g(initial point)");
  sscanf(s,"%lf",&X0g);

  Iv = (double *)mymalloc(sizeof(double)*rank);
  for (i=0; i<rank; i++) {
    next(fp,s,"Iv(initial values)");
	if (strncmp(s,"*",1)==0) {
	  for (i=0; i<rank; i++) Iv[i] = 0.0;
	  break;
	}
    sscanf(s,"%lg",&(Iv[i]));
  }

  next(fp,s,"Ef(exponential factor)");
  if (strncmp(s,"*",1)==0) Ef=0.0;
  else sscanf(s,"%lg",&Ef);

  next(fp,s,"Hg (step size of rk)");
  sscanf(s,"%lf",&Hg);

  next(fp,s,"Dp (data sampling period)");
  sscanf(s,"%d",&Dp);

  next(fp,s,"Xng (the last point, cf. --xmax)");
  sscanf(s,"%lf",&Xng);

  /* Reading the optional parameters */
 myparse: 
  while ((tk = mh_getoken(s,SMAX-1,fp)).type != MH_TOKEN_EOF) {
    /* expect ID */
    if (tk.type != MH_TOKEN_ID) {
      oxprintfe("Syntax error at %s\n",s); mh_exit(-1);
    }
    if (strcmp(s,"automatic")==0) {
      if (mh_getoken(s,SMAX-1,fp).type != MH_TOKEN_EQ) {
        oxprintfe("Syntax error at %s\n",s); mh_exit(-1);
      }
      if ((tk=mh_getoken(s,SMAX-1,fp)).type != MH_TOKEN_INT) {
        oxprintfe("Syntax error at %s\n",s); mh_exit(-1);
      }
      M_automatic = tk.ival;
      continue;
    }
    if (strcmp(s,"assigned_series_error")==0) {
      if (mh_getoken(s,SMAX-1,fp).type != MH_TOKEN_EQ) {
        oxprintfe("Syntax error at %s\n",s); mh_exit(-1);
      }
      if ((tk=mh_getoken(s,SMAX-1,fp)).type != MH_TOKEN_DOUBLE) {
        oxprintfe("Syntax error at %s\n",s); mh_exit(-1);
      }
      M_assigned_series_error = tk.dval;
      continue;
    }
    if (strcmp(s,"x0value_min")==0) {
      if (mh_getoken(s,SMAX-1,fp).type != MH_TOKEN_EQ) {
        oxprintfe("Syntax error at %s\n",s); mh_exit(-1);
      }
      if ((tk=mh_getoken(s,SMAX-1,fp)).type != MH_TOKEN_DOUBLE) {
        oxprintfe("Syntax error at %s\n",s); mh_exit(-1);
      }
      M_x0value_min = tk.dval;
      continue;
    }
    if ((strcmp(s,"Mapprox")==0) || (strcmp(s,"degree")==0)) {
      if (mh_getoken(s,SMAX-1,fp).type != MH_TOKEN_EQ) {
        oxprintfe("Syntax error at %s\n",s); mh_exit(-1);
      }
      if ((tk=mh_getoken(s,SMAX-1,fp)).type != MH_TOKEN_INT) {
        oxprintfe("Syntax error at %s\n",s); mh_exit(-1);
      }
      Mapprox = tk.ival;
      continue;
    }
    if (strcmp(s,"X0g_bound")==0) {
      if (mh_getoken(s,SMAX-1,fp).type != MH_TOKEN_EQ) {
        oxprintfe("Syntax error at %s\n",s); mh_exit(-1);
      }
      if ((tk=mh_getoken(s,SMAX-1,fp)).type != MH_TOKEN_DOUBLE) {
        oxprintfe("Syntax error at %s\n",s); mh_exit(-1);
      }
      M_X0g_bound = tk.dval;
      continue;
    }
    if (strcmp(s,"show_autosteps")==0) {
      if (mh_getoken(s,SMAX-1,fp).type != MH_TOKEN_EQ) {
        oxprintfe("Syntax error at %s\n",s); mh_exit(-1);
      }
      if ((tk=mh_getoken(s,SMAX-1,fp)).type != MH_TOKEN_INT) {
        oxprintfe("Syntax error at %s\n",s); mh_exit(-1);
      }
      M_show_autosteps = tk.ival;
      continue;
    }
    // Format: #p_pFq=2  1.5  3.2
    if (strcmp(s,"p_pFq")==0) {
      if (mh_getoken(s,SMAX-1,fp).type != MH_TOKEN_EQ) {
        oxprintfe("Syntax error at %s\n",s); mh_exit(-1);
      }
      if ((tk=mh_getoken(s,SMAX-1,fp)).type != MH_TOKEN_INT) {
        oxprintfe("Syntax error at %s\n",s); mh_exit(-1);
      }
      setA(NULL,tk.ival);
      for (i=0; i<A_LEN; i++) { 
	if ((tk=mh_getoken(s,SMAX-1,fp)).type == MH_TOKEN_DOUBLE) {
	  A_pFq[i] = tk.dval;
	}else if (tk.type == MH_TOKEN_INT) {
	  A_pFq[i] = tk.ival;
	}else{
	  oxprintfe("Syntax error at %s\n",s); mh_exit(-1);
	}
      }
      continue;
    }
    if (strcmp(s,"q_pFq")==0) {
      if (mh_getoken(s,SMAX-1,fp).type != MH_TOKEN_EQ) {
        oxprintfe("Syntax error at %s\n",s); mh_exit(-1);
      }
      if ((tk=mh_getoken(s,SMAX-1,fp)).type != MH_TOKEN_INT) {
        oxprintfe("Syntax error at %s\n",s); mh_exit(-1);
      }
      setB(NULL,tk.ival);
      for (i=0; i<B_LEN; i++) { 
	if ((tk=mh_getoken(s,SMAX-1,fp)).type == MH_TOKEN_DOUBLE) {
	  B_pFq[i] = tk.dval;
	}else if (tk.type == MH_TOKEN_INT) {
	  B_pFq[i] = tk.ival;
	}else{
	  oxprintfe("Syntax error at %s\n",s); mh_exit(-1);
	}
      }
      continue;
    }
    if (strcmp(s,"ef_type")==0) {
      if (mh_getoken(s,SMAX-1,fp).type != MH_TOKEN_EQ) {
        oxprintfe("Syntax error at %s\n",s); mh_exit(-1);
      }
      if ((tk=mh_getoken(s,SMAX-1,fp)).type != MH_TOKEN_INT) {
        oxprintfe("Syntax error at %s\n",s); mh_exit(-1);
      }
      Ef_type = tk.ival;
      continue;
    }

    if (strcmp(s,"Mg")==0) {
      if (mh_getoken(s,SMAX-1,fp).type != MH_TOKEN_EQ) {
        oxprintfe("Syntax error at %s\n",s); mh_exit(-1);
      }
      if ((tk=mh_getoken(s,SMAX-1,fp)).type != MH_TOKEN_INT) {
        oxprintfe("Syntax error at %s\n",s); mh_exit(-1);
      }
      Mg = tk.ival;
      rank = imypower(2,Mg);
      continue;
    }
    if (strcmp(s,"Beta")==0) {
      if (mh_getoken(s,SMAX-1,fp).type != MH_TOKEN_EQ) {
        oxprintfe("Syntax error at %s\n",s); mh_exit(-1);
      }
	  if (Mg <= 0) {
        oxprintfe("Mg should be set before reading Beta.\n"); mh_exit(-1);
      }
      Beta = (double *)mymalloc(sizeof(double)*Mg);
	  for (i=0; i<Mg; i++) {
        if ((tk=mh_getoken(s,SMAX-1,fp)).type == MH_TOKEN_DOUBLE) {
           Beta[i] = tk.dval;
        }else if (tk.type == MH_TOKEN_INT) {
           Beta[i] = tk.ival;
        }else {
          oxprintfe("Syntax error at %s\n",s); mh_exit(-1);
        }
      }
      Iv = (double *)mymalloc(sizeof(double)*rank);
	  for (i=0; i<rank; i++) {
		Iv[i] = 0.0;
	  }
      continue;
    }
    if (strcmp(s,"Ng")==0) {
      if (mh_getoken(s,SMAX-1,fp).type != MH_TOKEN_EQ) {
        oxprintfe("Syntax error at %s\n",s); mh_exit(-1);
      }
      
      if ((tk=mh_getoken(s,SMAX-1,fp)).type== MH_TOKEN_DOUBLE) {
		*Ng = tk.dval;
      }else if (tk.type == MH_TOKEN_INT) {
        *Ng = tk.ival;
      }else{
        oxprintfe("Syntax error at %s\n",s); mh_exit(-1);
      }
      continue;
    }
    if (strcmp(s,"X0g")==0) {
      if (mh_getoken(s,SMAX-1,fp).type != MH_TOKEN_EQ) {
        oxprintfe("Syntax error at %s\n",s); mh_exit(-1);
      }
      if ((tk=mh_getoken(s,SMAX-1,fp)).type == MH_TOKEN_DOUBLE) {
		X0g = tk.dval;
	  }else if (tk.type == MH_TOKEN_INT) {
		X0g = tk.ival;
      }else{
		oxprintfe("Syntax error at %s\n",s); mh_exit(-1);
	  }			
      continue;
    }
	if (strcmp(s,"Iv")==0) {
      if (mh_getoken(s,SMAX-1,fp).type != MH_TOKEN_EQ) {
        oxprintfe("Syntax error at %s\n",s); mh_exit(-1);
      }
	  for (i=0; i<rank; i++) {
        if ((tk=mh_getoken(s,SMAX-1,fp)).type == MH_TOKEN_DOUBLE) {
		  Iv[i] = tk.dval;
		}else if (tk.type == MH_TOKEN_INT) {
		  Iv[i] = tk.ival;
        }else{
          oxprintfe("Syntax error at %s\n",s); mh_exit(-1);
		}
      }
      continue;
    }
    if (strcmp(s,"Ef")==0) {
      if (mh_getoken(s,SMAX-1,fp).type != MH_TOKEN_EQ) {
        oxprintfe("Syntax error at %s\n",s); mh_exit(-1);
      }
      if ((tk=mh_getoken(s,SMAX-1,fp)).type == MH_TOKEN_DOUBLE) {
		Ef = tk.dval;
	  }else if (tk.type == MH_TOKEN_INT) {
		Ef = tk.ival;
      }else{
        oxprintfe("Syntax error at %s\n",s); mh_exit(-1);
	  }
      continue;
    }
    if (strcmp(s,"Hg")==0) {
      if (mh_getoken(s,SMAX-1,fp).type != MH_TOKEN_EQ) {
        oxprintfe("Syntax error at %s\n",s); mh_exit(-1);
      }
      if ((tk=mh_getoken(s,SMAX-1,fp)).type == MH_TOKEN_DOUBLE) {
		Hg = tk.dval;
	  }else if (tk.type == MH_TOKEN_INT) {
		Hg = tk.ival;
      }else{
        oxprintfe("Syntax error at %s\n",s); mh_exit(-1);
      }
      continue;
    }
    if (strcmp(s,"Dp")==0) {
      if (mh_getoken(s,SMAX-1,fp).type != MH_TOKEN_EQ) {
        oxprintfe("Syntax error at %s\n",s); mh_exit(-1);
      }
      if ((tk=mh_getoken(s,SMAX-1,fp)).type != MH_TOKEN_INT) {
        oxprintfe("Syntax error at %s\n",s); mh_exit(-1);
      }
      Dp = tk.dval;
      continue;
    }
    if (strcmp(s,"Xng")==0) {
      if (mh_getoken(s,SMAX-1,fp).type != MH_TOKEN_EQ) {
        oxprintfe("Syntax error at %s\n",s); mh_exit(-1);
      }
      if ((tk=mh_getoken(s,SMAX-1,fp)).type == MH_TOKEN_DOUBLE) {
		Xng = tk.dval;
	  }else if (tk.type == MH_TOKEN_INT) {
		Xng = tk.ival;
      }else{
        oxprintfe("Syntax error at %s\n",s); mh_exit(-1);
	  }
      continue;
    }

    oxprintfe("Unknown ID at %s\n",s); mh_exit(-1);
  }
  
  /* 1F1, original wishart case. */  
  if ((A_LEN <= 1) && (B_LEN <= 1) && (Ef_type==1) && (*Ng >= 0)) {
    if (A_LEN <1) setA(NULL,1);
	if (B_LEN <1) setB(NULL,1);
    A_pFq[0] = ((double)Mg+1.0)/2.0;
    B_pFq[0] = ((double)Mg+1.0)/2.0 + ((double) (*Ng))/2.0; /* bug, double check */
    if (Debug) oxprintf("Calling mh_t with ([%lf],[%lf],%d,%d)\n",A_pFq[0],B_pFq[0],M_n,Mapprox);
  }
  mh_fclose(fp);
  return(0);
}
#ifdef STANDALONE
/* may remove */
static int showParam_v1(struct SFILE *fp,int fd) {
  int rank,i;
  char swork[1024];
  if (fd) {
    fp = mh_fopen("stdout","w",1);
  }
  rank = imypower(2,Mg);
  sprintf(swork,"%%Mg=\n%d\n",Mg); mh_fputs(swork,fp);
  for (i=0; i<Mg; i++) {
    sprintf(swork,"%%Beta[%d]=\n%lf\n",i,Beta[i]); mh_fputs(swork,fp);
  }
  if (*Ng >= 0) {
    sprintf(swork,"%%Ng=\n%lf\n",*Ng); mh_fputs(swork,fp);
  }
  sprintf(swork,"%%X0g=\n%lf\n",X0g); mh_fputs(swork,fp);
  for (i=0; i<rank; i++) {
    sprintf(swork,"%%Iv[%d]=\n%lg\n",i,Iv[i]); mh_fputs(swork,fp);
    if (Sample && (M_n == 2) && (X0g == 0.3)) {
      sprintf(swork,"%%Iv[%d]-Iv2[%d]=%lg\n",i,i,Iv[i]-Iv2[i]); mh_fputs(swork,fp);
    }
  }
  sprintf(swork,"%%Ef=\n%lg\n",Ef); mh_fputs(swork,fp);
  sprintf(swork,"%%Hg=\n%lf\n",Hg); mh_fputs(swork,fp);
  sprintf(swork,"%%Dp=\n%d\n",Dp);  mh_fputs(swork,fp);
  sprintf(swork,"%%Xng=\n%lf\n",Xng);mh_fputs(swork,fp);
  
  sprintf(swork,"%%%% Optional paramters\n"); mh_fputs(swork,fp);
  sprintf(swork,"#success=%d\n",M_mh_t_success); mh_fputs(swork,fp);
  sprintf(swork,"#automatic=%d\n",M_automatic); mh_fputs(swork,fp);
  sprintf(swork,"#series_error=%lg\n",M_series_error); mh_fputs(swork,fp);
  sprintf(swork,"#recommended_abserr\n"); mh_fputs(swork,fp);
  sprintf(swork,"%%abserror=%lg\n",M_recommended_abserr); mh_fputs(swork,fp);
  if (M_recommended_relerr < MH_RELERR_DEFAULT) {
    sprintf(swork,"%%relerror=%lg\n",M_recommended_relerr); mh_fputs(swork,fp);
  }
  sprintf(swork,"#mh_t_value=%lg # Value of matrix hg at X0g.\n",M_mh_t_value); mh_fputs(swork,fp);
  sprintf(swork,"# M_m=%d  # Approximation degree of matrix hg.\n",M_m); mh_fputs(swork,fp);
  sprintf(swork,"#beta_i_x_o2_max=%lg #max(|beta[i]*x|/2)\n",M_beta_i_x_o2_max); mh_fputs(swork,fp);
  sprintf(swork,"#beta_i_beta_j_min=%lg #min(|beta[i]-beta[j]|)\n",M_beta_i_beta_j_min); mh_fputs(swork,fp);
  sprintf(swork,"# change # to %% to read as an optional parameter.\n"); mh_fputs(swork,fp);
  sprintf(swork,"%%p_pFq=%d, ",A_LEN); mh_fputs(swork,fp);
  for (i=0; i<A_LEN; i++) {
    if (i != A_LEN-1) sprintf(swork," %lg,",A_pFq[i]); 
    else sprintf(swork," %lg\n",A_pFq[i]); 
    mh_fputs(swork,fp);
  }
  sprintf(swork,"%%q_pFq=%d, ",B_LEN); mh_fputs(swork,fp);
  for (i=0; i<B_LEN; i++) {
    if (i != B_LEN-1) sprintf(swork," %lg,",B_pFq[i]); 
    else sprintf(swork," %lg\n",B_pFq[i]); 
    mh_fputs(swork,fp);
  }
  sprintf(swork,"%%ef_type=%d\n",Ef_type); mh_fputs(swork,fp);
  return(0);
}
#endif
/* version2.0 format */
static int showParam(struct SFILE *fp,int fd) {
  int rank,i;
  char swork[1024];
  if (fd) {
    fp = mh_fopen("stdout","w",1);
  }
  rank = imypower(2,Mg);
  sprintf(swork,"%s\n",VSTRING); mh_fputs(swork,fp);
  sprintf(swork,"%%Mg=\n%d\n",Mg); mh_fputs(swork,fp);
  sprintf(swork,"%%p_pFq=%d, ",A_LEN); mh_fputs(swork,fp);
  for (i=0; i<A_LEN; i++) {
    if (i != A_LEN-1) sprintf(swork," %lg,",A_pFq[i]); 
    else sprintf(swork," %lg\n",A_pFq[i]); 
    mh_fputs(swork,fp);
  }
  sprintf(swork,"%%q_pFq=%d, ",B_LEN); mh_fputs(swork,fp);
  for (i=0; i<B_LEN; i++) {
    if (i != B_LEN-1) sprintf(swork," %lg,",B_pFq[i]); 
    else sprintf(swork," %lg\n",B_pFq[i]); 
    mh_fputs(swork,fp);
  }
  sprintf(swork,"%%ef_type=%d\n",Ef_type); mh_fputs(swork,fp);
  sprintf(swork,"%%Beta=\n"); mh_fputs(swork,fp);
  for (i=0; i<Mg; i++) {
    sprintf(swork,"#Beta[%d]=\n%lf\n",i,Beta[i]); mh_fputs(swork,fp);
  }
  if (*Ng >= 0) {
    sprintf(swork,"%%Ng=\n%lf\n",*Ng); mh_fputs(swork,fp);
  }
  sprintf(swork,"%%X0g=\n%lf\n",X0g); mh_fputs(swork,fp);
  sprintf(swork,"%%Iv=\n"); mh_fputs(swork,fp);
  for (i=0; i<rank; i++) {
    sprintf(swork,"#Iv[%d]=\n%lg\n",i,Iv[i]); mh_fputs(swork,fp);
    if (Sample && (M_n == 2) && (X0g == 0.3)) {
      sprintf(swork,"%%Iv[%d]-Iv2[%d]=%lg\n",i,i,Iv[i]-Iv2[i]); mh_fputs(swork,fp);
    }
  }
  sprintf(swork,"%%Ef=\n%lg\n",Ef); mh_fputs(swork,fp);
  sprintf(swork,"%%Hg=\n%lf\n",Hg); mh_fputs(swork,fp);
  sprintf(swork,"%%Dp=\n%d\n",Dp);  mh_fputs(swork,fp);
  sprintf(swork,"%%Xng=\n%lf\n",Xng);mh_fputs(swork,fp);
  
  sprintf(swork,"%%%% Optional paramters\n"); mh_fputs(swork,fp);
  sprintf(swork,"#success=%d\n",M_mh_t_success); mh_fputs(swork,fp);
  sprintf(swork,"#automatic=%d\n",M_automatic); mh_fputs(swork,fp);
  sprintf(swork,"#series_error=%lg\n",M_series_error); mh_fputs(swork,fp);
  sprintf(swork,"#recommended_abserr\n"); mh_fputs(swork,fp);
  sprintf(swork,"#abserror=%lg\n",M_recommended_abserr); mh_fputs(swork,fp);
  if (M_recommended_relerr < MH_RELERR_DEFAULT) {
    sprintf(swork,"%%relerror=%lg\n",M_recommended_relerr); mh_fputs(swork,fp);
  }
  sprintf(swork,"#mh_t_value=%lg # Value of matrix hg at X0g.\n",M_mh_t_value); mh_fputs(swork,fp);
  sprintf(swork,"# M_m=%d  # Approximation degree of matrix hg.\n",M_m); mh_fputs(swork,fp);
  sprintf(swork,"#beta_i_x_o2_max=%lg #max(|beta[i]*x|/2)\n",M_beta_i_x_o2_max); mh_fputs(swork,fp);
  sprintf(swork,"#beta_i_beta_j_min=%lg #min(|beta[i]-beta[j]|)\n",M_beta_i_beta_j_min); mh_fputs(swork,fp);
  sprintf(swork,"# change # to %% to read as an optional parameter.\n"); mh_fputs(swork,fp);
  return(0);
}

static double gammam(double a,int n) {
  double v,v2;
  int i;
  v=mypower(sqrt(M_PI),(n*(n-1))/2); /* pi^(n*(n-1)/2) */
  v2=0;
  for (i=1; i<=n; i++) {
    v2 += lgamma(a-((double)(i-1))/2.0); /* not for big n */
  }
  if (Debug) oxprintf("gammam(%lg,%d)=%lg\n",a,n,v*exp(v2));
  return(v*exp(v2));
}

static double iv_factor(void) {
  double ef;
  double lef;
  double r;
  ef=1; lef=0;
  if (Ef_type < 1) return(1.0);
  if (Ef_type == 1) {
	ef=iv_factor_ef_type1();
	lef=liv_factor_ef_type1();
  }else if (Ef_type==2) {
	ef = iv_factor_ef_type2();
	lef = liv_factor_ef_type2();
  }else{
	return(1.0);
  }
  if (isnan(ef) || isinf(ef)) {
	if (Debug) oxprintfe("Exponential factor (Ef) seems to be large or ill-conditioned.\n");
	return(exp(lef));
  }else {
	r = ef/exp(lef);
	if ((0.9 < r) && (r < 1.1)) return(ef);
	else {
	  oxprintfe("Warning: There seems to be a numerical error to get Ef. We use a log value of Ef");
      oxprintfe(" Ef=%lg, exp(lef)=%lg\n",ef,exp(lef));
	  return(exp(lef));
	}
  }
  return(exp(lef));
}

static double iv_factor_ef_type1(void) {
  double v1;
  double t;
  double b;
  double detSigma;
  double c;
  int i,n;
  n = (int) (*Ng);
  v1= mypower(sqrt(X0g),n*M_n);
  t = 0.0;
  for (i=0; i<M_n; i++)  t += -X0g*Beta[i];
  v1 = v1*exp(t);

  b = 1; for (i=0; i<M_n; i++) b *= Beta[i];
  detSigma = 1.0/(b*mypower(2.0,M_n));

  c = gammam(((double)(M_n+1))/2.0, M_n)/
    ( mypower(sqrt(2), M_n*n)*mypower(sqrt(detSigma),n)*gammam(((double)(M_n+n+1))/2.0,M_n) );
  return( c*v1);
}

static double iv_factor_ef_type2(void) {
  double ef;
  int i,m;
  double a,b,c;
  double t;
  a = A_pFq[0]; b = A_pFq[1]; c= B_pFq[0];
  m = M_n;
  ef = 1.0;

  /*fprintf(stderr,"iv_factor_ef_type2: a=%lf, b=%lf, c=%lf, m=%d\n",a,b,c,m);*/
  /* Ref: note 2016.02.04 */
  if (X0g<0){ myerror("Negative X0g\n"); mh_exit(-1);}
  t = 0;
  for (i=0; i<m; i++) if (Beta[i]<0){ myerror("Negative beta\n"); mh_exit(-1);}
  for (i=0; i<m; i++) t += log(Beta[i]);
  ef *= exp((a+b-c)*t);

  t = 0;
  for (i=0; i<m; i++) t += log(Beta[i]+X0g);
  ef *= exp(-b*t);

  ef *= exp((c-a)*(2*a-1)*log(X0g));

  ef *= gammam(b,m)/gammam(a+b-c,m);
  ef *= gammam(a,m)/gammam(c,m);
  return(ef);  
}
static void setM_x(void) {
  if (Ef_type <= 1)    {setM_x_ef_type1(); return;}
  else if (Ef_type==2) {setM_x_ef_type2(); return;}
  setM_x_ef_type1();
}
static void setM_x_ef_type1(void) {
  int i;
  for (i=0; i<M_n; i++) {
	M_x[i] = Beta[i]*X0g;
	if (myabs(M_x[i]) > SERIES_ADMISSIBLE_RADIUS_TYPE1) {
	  if (SAR_warning) oxprintfe("Warning: evaluation point %lf for %d-th variable of the series 1F1 might be far from 0. Decrease q0 (or X0g for the standalone) if necessary.\n",M_x[i],i);
	  SAR_warning=0;
	}
  }
}
static void setM_x_ef_type2(void) {
  int i;
  for (i=0; i<M_n; i++) {
	M_x[i] = X0g/(Beta[i]+X0g);
	if (myabs(M_x[i]) > SERIES_ADMISSIBLE_RADIUS_TYPE2) {
	  if (SAR_warning) oxprintfe("Warning: evaluation point %lf for %d-th point of the series 2F1 is near 1. Decrease q0 (or X0g for the standalone).\n",M_x[i],i);
	  SAR_warning=0;
	}
  }
}
int reset_SAR_warning(int n) {
  SAR_warning = n;
  return(n);
}
/* log of gammam */
static double lgammam(double a,int n) {
  double v,v2;
  int i;
  v=log(M_PI)*n*(n-1)/2.0; /* log pi^(n*(n-1)/2) */
  v2=0;
  for (i=1; i<=n; i++) {
    v2 += lgamma(a-((double)(i-1))/2.0); /* not for big n */
  }
  return(v+v2);
}

/* log of iv_factor_ef_type1() */
static double liv_factor_ef_type1(void) {
  double v1;
  double t;
  double b;
  double detSigma;
  double c;
  int i,n;
  if (X0g<0){ myerror("Negative X0g\n"); mh_exit(-1);}
  n = (int) (*Ng);
  v1= log(X0g)*n*M_n/2.0;
  t = 0.0;
  for (i=0; i<M_n; i++)  t += -X0g*Beta[i];
  v1 += t;

  b = 1; for (i=0; i<M_n; i++) b *= Beta[i];
  detSigma = -log(b)-M_n*log(2);

  c = lgammam(((double)(M_n+1))/2.0, M_n)-log(2)*M_n*n/2.0
	-detSigma*n/2.0-lgammam(((double)(M_n+n+1))/2.0,M_n);
  return(c+v1);
}

static double liv_factor_ef_type2(void) {
  double ef;
  int i,m;
  double a,b,c;
  double t;
  a = A_pFq[0]; b = A_pFq[1]; c= B_pFq[0];
  m = M_n;
  ef = 0.0;

  if (X0g<0){ myerror("Negative X0g\n"); mh_exit(-1);}
  t = 0;
  for (i=0; i<m; i++) if (Beta[i]<0){ myerror("Negative beta\n"); mh_exit(-1);}
  for (i=0; i<m; i++) t += log(Beta[i]);
  ef += (a+b-c)*t;

  t = 0;
  for (i=0; i<m; i++) t += log(Beta[i]+X0g);
  ef += -b*t;

  ef += (c-a)*(2*a-1)*log(X0g);

  ef += lgammam(b,m)-lgammam(a+b-c,m);
  ef += lgammam(a,m)-lgammam(c,m);
  return(ef);  
}
