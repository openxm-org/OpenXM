/*
$OpenXM: OpenXM/src/hgm/mh/src/code-n.c,v 1.2 2013/02/19 08:03:14 takayama Exp $
License: LGPL
Ref: Copied from this11/misc-2011/A1/wishart/Prog
cf. @s/2011/12/01-my-note-code-n.pdf
 */
#include <stdio.h>
#include <stdlib.h> 
#include <math.h>

static void error_code(char *s);
static void showf(char *s,double *v,int m);
static void showd(char *s,int *v,int m);

extern int MH_M;
extern int MH_RANK;

static int bitcount(int n) {int c; c=0; while (n) { ++c; n &= (n-1); } return(c);}
#define join(k,jj) ( (1 << k) | jj)
#define delete(k,jj) ( (~(1 << k)) & jj)
#define member(k,ii) ( (1 << k) & ii)
#define NaN 9.999e100

#define idxM(i,j) ((i)*MH_M+j)
#define idxRank(i,j) ((i)*MH_RANK+j)

/*
  p(x) = exp(-bsum*x)*x^(MH_M*(c-a))
  f is p(x)*pd{J}F
 */
void mh_rf(double x, double *f, int rank_not_used, double *val, int n_not_used)
{ extern double *MH_Beta; /* beta = (1/2)*\simga^(-1) */
  extern double *MH_Ng;   /* freedom */
  extern int MH_Mg;       /* number of variables, MH_Mg=MH_M */
  static int initialized = 0;
  extern int MH_deallocate;
  /* static double b[MH_M]; */
  static double *b;
  static double bsum = 0;
  static double a;
  static double c;
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
  /*
  double yik[MH_M][MH_M];
  double yik2[MH_M][MH_M];
  double y[MH_M];
  */
  static double *yik=NULL;
  static double *yik2=NULL;
  static double *y=NULL;
  /* double f2[MH_M][MH_RANK];*/
  static double *f2=NULL;

  if (MH_deallocate && initialized) {
	if (b) mh_free(b);
	if (bitSize) mh_free(bitSize);
	if (invbitSize) mh_free(invbitSize);
	if (f2) mh_free(f2);
	if (yik) mh_free(yik);
	if (yik2) mh_free(yik2);
	if (y) mh_free(y);
    b = f2 = yik = yik2 = y = NULL;
    bitSize = invbitSize = NULL;
	initialized = 0; return;
  }
  if (!initialized) {
    b = (double *)mh_malloc(sizeof(double)*MH_M);
    bitSize = (int *)mh_malloc(sizeof(int)*MH_RANK);
    invbitSize = (int *)mh_malloc(sizeof(int)*MH_RANK);

    f2 = (double *)mh_malloc(sizeof(double)*MH_M*MH_RANK);
    yik = (double *)mh_malloc(sizeof(double)*MH_M*MH_M);
    yik2 = (double *)mh_malloc(sizeof(double)*MH_M*MH_M);
    y   = (double *)mh_malloc(sizeof(double)*MH_M);

    m = MH_Mg;
    if (m != MH_M) error_code("MH_M != m. MH_M is given by -DMH_M...");
    a = ((double)m+1.0)/2.0;

    if(MH_Beta != NULL) for (i=0;i<MH_M;i++) b[i]=MH_Beta[i];
    else error_code("MH_Beta is null.");
    if (MH_Ng != NULL) c=a+*(MH_Ng)/2;  /* set c, a+n/2, n is given by the option n */
    else error_code("MH_Ng is null.");

    bsum = 0; for (i=0; i<m; i++) bsum += b[i];

    for (i=0; i<MH_RANK; i++) bitSize[i] = bitcount(i);
    for (i=0, k=0; i<=MH_M; i++) {
      for (j=0; j<MH_RANK; j++) {
	if (bitSize[j] == i) {invbitSize[k] = j; k++; }
      }
    }
    initialized = 1;
  }
  for (i=0; i<MH_M; i++) {
    y[i] = b[i]*x;    
  }
  for (i=0; i<MH_M; i++) {
    for (k=0; k<MH_M; k++) {
      if (i != k) {
	yik[idxM(i,k)] = y[k]/(y[i]-y[k]);
	yik2[idxM(i,k)] = y[i]/((y[i]-y[k])*(y[i]-y[k]));
      }
    }
  }

  /* 
     generation of f2[i][jj] = p(x) pd{i}^2 \pd{J} F 
     Do not forget to devide by y[i]
  */
  for (i=0; i<MH_M; i++) for (j=0; j<MH_RANK; j++) f2[idxRank(i,j)] = NaN;
  for (i=0; i<MH_M; i++) {
    f2[idxRank(i,0)] = -((c-y[i])*f[join(i,0)]-a*f[0]);
    for (k=0; k<MH_M; k++) {
      if (i!=k) {
	f2[idxRank(i,0)] += -0.5*yik[idxM(i,k)]*(f[join(i,0)]-f[join(k,0)]);
      }
    }
    f2[idxRank(i,0)] = f2[idxRank(i,0)]/y[i];
  }
  for (p=1; p<MH_RANK; p++) {
    jj = invbitSize[p];
    for (i=0; i<MH_M; i++) {
      if (member(i,jj)) continue;
      ii = join(i,jj);
      rijj = -( (c-y[i])*f[ii]-a*f[jj] );
      for (k=0; k<MH_M; k++) {
	if (!member(k,ii)) rijj += -(0.5)*yik[idxM(i,k)]*(f[ii]-f[join(k,jj)]);
	if (member(k,jj)) {
	  rijj += -(0.5)*yik[idxM(i,k)]*f[ii];
	  rijj += -(0.5)*yik2[idxM(i,k)]*(f[join(i,delete(k,jj))]-f[jj]);
	}
      }
      f2[idxRank(i,jj)] = rijj;
      for (k = 0; k<MH_M; k++) {
	if (member(k,jj)) {
	  f2[idxRank(i,jj)] += (0.5)*yik[idxM(i,k)]*f2[idxRank(k,delete(k,jj))];
	}
      }
      f2[idxRank(i,jj)] = f2[idxRank(i,jj)]/y[i];
    }
  }
  /** showf2("f2",f2,MH_M,MH_RANK); exit(0);  */

  for (jj=0; jj<MH_RANK; jj++) {
    val[jj] = 0;
    for (i=0; i<MH_M; i++) {
      if (member(i,jj)) {
	val[jj] += b[i]*f2[idxRank(i,delete(i,jj))];
      }else{
	val[jj] += b[i]*f[join(i,jj)];
      }
    }
  }

   /*   diagonal shift. check MH_M.*/
  for (i=0; i<MH_RANK; i++) {
    val[i] += (-bsum+MH_M*(c-a)/x)*f[i];
  }
}

static void error_code(char *s) {
  fprintf(stderr,"%s",s);
  mh_exit(10);
}

/* for debug */
static void showf(char *s,double *v,int m) {
  int i;
  printf("%s=\n",s);
  for (i=0; i<m; i++) {
    printf("%e, ",v[i]);
  }
  printf("\n");
}

static void showd(char *s,int *v,int m) {
  int i;
  printf("%s=\n",s);
  for (i=0; i<m; i++) {
    printf("%5d, ",v[i]);
  }
  printf("\n");
}

showf2(char *s,double *v,int m,int n) {
  int i,j;
  printf("%s=\n",s);
  for (i=0; i<m; i++) {
    for (j=0; j<n; j++) {
      printf("%e, ",v[i*n+j]);
    }
    printf("\n");
  }
  printf("\n");
  printf("member(0,0)=%d, member(0,1)=%d, member(0,2)=%d,member(0,3)=%d\n",
	 member(0,0),member(0,1),member(0,2),member(0,3));
}

