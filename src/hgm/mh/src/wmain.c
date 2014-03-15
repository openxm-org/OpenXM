/*
  $OpenXM: OpenXM/src/hgm/mh/src/wmain.c,v 1.19 2014/03/14 05:58:16 takayama Exp $
  License: LGPL
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "sfile.h"
#include "mh.h"
#define SMAX 4096
#define inci(i) { i++; if (i >= argc) { fprintf(stderr,"Option argument is not given.\n"); return(NULL); }}
int MH_deallocate=0;

/*
  changelog
  2014.03.15  --strategy 1 is default. A new parser in setParam()
 */
extern char *MH_Gfname;
extern char *MH_Dfname;

/* global variables. They are set in setParam() */
int MH_byFile=1;
int MH_RANK;
int MH_M;

int MH_Mg;  /* m */
double *MH_Beta; /* beta[0], ..., beta[m-1] */
double *MH_Ng;   /* freedom n.  c=(m+1)/2+n/2; Note that it is a pointer */
double MH_X0g;   /* initial point */
static double *Iv;   /* Initial values of mhg sorted by mhbase() in rd.rr at beta*x0 */
static double Ef;   /* exponential factor at beta*x0 */
extern double MH_Hg;   /* step size of rk defined in rk.c */
int MH_Dp;      /* Data sampling period */
static double Xng=0.0;   /* the last point */
int MH_RawName = 0;
static int Testrank=0;
/* If MH_success is set to 1, then strategy, MH_abserr, MH_relerr seem to
  be properly set.
*/
int MH_success=0;
/*
  Estimation of the maximal coeff of A in y'=Ay.
  This might be too rough estimate.
 */
double MH_coeff_max;
/*
  Estimation of h by MH_coeff_max;
 */
double MH_estimated_start_step;
extern int MH_Verbose;
extern int MH_strategy;
extern double MH_abserr;
extern double MH_relerr;

extern int MH_P95;  /* 95 % points */
int mh_gopen_file(void);
static int setParamTest(void);
static int setParamDefault(void);
static int setParam(char *fname);
static int showParam(void);
static int next(struct SFILE *fp,char *s, char *msg);
static double estimateHg(int m, double beta[],double x0);

/* #define DEBUG */
#ifdef DEBUG
char *MH_Dfname; char *MH_Gfname; double MH_Hg;
int mh_gopen_file(void) { }
struct MH_RESULT mh_rkmain(double x0,double y0[],double xn) { }
#endif

void mh_freeWorkArea(void) {
  extern int MH_deallocate;
  MH_deallocate=1; /* switch to deallocation mode. */
  mh_main(0,NULL); 
  setParam(NULL);  
  mh_rkmain(0.0, NULL, 0.0);
  mh_rf(0.0, NULL, 0, NULL, 0);
  MH_deallocate=0; /* switch to the normal mode. */
}
static int mypower(int x,int n) {
  int a,i;
  a = 1;
  for (i=0; i<n; i++) a = a*x;
  return(a);
}
#ifdef STANDALONE2
main(int argc,char *argv[]) {
  int strategy=STRATEGY_DEFAULT;
  double err[2]={-1.0,-1.0};
  int i;
  mh_exit(MH_RESET_EXIT); /* standalone mode */
  /*  mh_main(argc,argv);
      mh_freeWorkArea(); */
  mh_main(argc,argv);
  /* showParam(); */
  return(0);
}
#endif
struct MH_RESULT *mh_main(int argc,char *argv[]) {
  static double *y0=NULL;
  double x0,xn;
  double ef;
  int i,rank;
  struct MH_RESULT *rp=NULL;
  extern int MH_deallocate;
  extern int MH_byFile;
  MH_byFile=1;
  if (MH_deallocate) { if (y0) mh_free(y0); return(rp); }
  setParam(NULL); MH_Gfname = MH_Dfname = NULL; MH_Verbose=1;
  for (i=1; i<argc; i++) {
    if (strcmp(argv[i],"--idata")==0) {
      inci(i);
      setParam(argv[i]); MH_Verbose=0;
    }else if (strcmp(argv[i],"--gnuplotf")==0) {
      inci(i);
      MH_Gfname = (char *)mh_malloc(SMAX);
      strcpy(MH_Gfname,argv[i]);
    }else if (strcmp(argv[i],"--dataf")==0) {
      inci(i);
      MH_Dfname = (char *)mh_malloc(SMAX);
      strcpy(MH_Dfname,argv[i]);
    }else if (strcmp(argv[i],"--xmax")==0) {
      inci(i);
      sscanf(argv[i],"%lf",&Xng);
    }else if (strcmp(argv[i],"--step")==0) {
      inci(i);
      sscanf(argv[i],"%lg",&MH_Hg);
    }else if (strcmp(argv[i],"--help")==0) {
      mh_usage(); return(rp);
    }else if (strcmp(argv[i],"--raw")==0) {
      MH_RawName = 1;
    }else if (strcmp(argv[i],"--test")==0) {
      inci(i);
      sscanf(argv[i],"%d",&Testrank);
      setParamTest();
    }else if (strcmp(argv[i],"--95")==0) {
      MH_P95=1;
    }else if (strcmp(argv[i],"--verbose")==0) {
      MH_Verbose=1;
    }else if (strcmp(argv[i],"--bystring")==0) {
      MH_byFile = 0;
    }else if (strcmp(argv[i],"--strategy")==0) {
      i++; sscanf(argv[i],"%d",&MH_strategy);
    }else if (strcmp(argv[i],"--abserr")==0) {
      i++; sscanf(argv[i],"%lg",&MH_abserr);
    }else if (strcmp(argv[i],"--relerr")==0) {
      i++; sscanf(argv[i],"%lg",&MH_relerr);
    }else {
      fprintf(stderr,"Unknown option %s\n",argv[i]);
      mh_usage();
      return(rp);
    }
  }
  x0 = MH_X0g;
  xn = Xng;
  ef = Ef;
  rank = mypower(2,MH_Mg);
  y0 = (double *) mh_malloc(sizeof(double)*rank);
  for (i=0; i<rank; i++) y0[i] = ef*Iv[i];
  mh_gopen_file();
  rp = (struct MH_RESULT*) mh_malloc(sizeof(struct MH_RESULT));

  if (MH_strategy) {
    if (MH_abserr > SIGDIGIT_DEFAULT*myabs(y0[0])) {
      MH_success = 0;
      fprintf(stderr,"%%%%Warning, abserr seems not to be small enough, abserr=%lg, y[0]=%lg\n",MH_abserr,y0[0]);
    }else{
      MH_success = 1;
    }
  }else{
    MH_success = 0;
  }
  MH_estimated_start_step = estimateHg(MH_Mg,MH_Beta,MH_X0g);
  if (MH_Verbose) showParam();
  if (MH_Verbose) {for (i=0; i<rank; i++) printf("%lf\n",y0[i]); }

  *rp=mh_rkmain(x0,y0,xn);
  return(rp);
}

int mh_usage() {
  fprintf(stderr,"Usages:\n");
  fprintf(stderr,"hgm_w-n [--idata input_data_file --gnuplotf gnuplot_file_name\n");
  fprintf(stderr," --dataf output_data_file --raw --xmax xmax --test m --step h]\n");
  fprintf(stderr,"[ --95 --verbose] \n");
  fprintf(stderr,"[ --strategy s --abserr ae --relerr re] \n");
  fprintf(stderr,"s:0 rk, s:1 adaptive, s:2 adaptive&multiply, see rk.c for the default value of ae and re.\n");
  fprintf(stderr,"strategy default = %d\n",MH_strategy);
  fprintf(stderr,"--raw does not add data parameters to the output_data_file.\n");
  fprintf(stderr,"\nThe command hgm_w-n [options] evaluates Pr({y | y<xmax}), which is the cumulative distribution function of the largest root of the m by m Wishart matrix with n degrees of freedom and the covariantce matrix sigma.\n");
  fprintf(stderr,"All the eigenvalues of sigma must be simple.\n");
  fprintf(stderr,"Parameters are specified by the input_data_file.\n");
  fprintf(stderr,"Parameters are redefined when they appear more than once in the idata file and the command line options.\n");
  fprintf(stderr,"The format of the input_data_file, which should be generated by the command hgm_jack-n.\n");
  fprintf(stderr," MH_Mg: m, MH_Beta: beta=sigma^(-1)/2 (diagonized), MH_Ng: n, MH_X0g: starting value of x,\n");
  fprintf(stderr," Iv: initial values at MH_X0g*MH_Beta (see our paper how to order them), \n");
  fprintf(stderr," Ef: a scalar factor to the initial value. It may set to 1.\n");
  fprintf(stderr," MH_Hg: h (step size),\n");
  fprintf(stderr," MH_Dp: output data is stored in every MH_Dp steps when output_data_file is specified.\n");
  fprintf(stderr," Xng: terminating value of x.\n");
  fprintf(stderr," --95: output the 95%% point. --verbose: verbose mode.\n");
  fprintf(stderr," The line started with %% is a comment line.\n");
  fprintf(stderr," An example format of the input_data_file can be obtained by executing hgm_jack-n with no option.\n");
  fprintf(stderr,"When --idata option is used, this command is quiet. Use --verbose option if you want to see some messages.\n");
  fprintf(stderr,"\nExamples:\n");
  fprintf(stderr,"[1] ./hgm_w-n \n");
  fprintf(stderr,"[2] ./hgm_w-n --xmax 20\n");
  fprintf(stderr,"[3] ./hgm_w-n --test 6\n");
  fprintf(stderr,"   A test run in Mg=6.\n");
  fprintf(stderr,"[4] ./hgm_jack-n --idata Testdata/tmp-idata3.txt --degree 15 >t.txt\n");
  fprintf(stderr,"    ./hgm_w-n --idata t.txt --gnuplotf test-g --verbose\n");
  fprintf(stderr,"    gnuplot -persist <test-g-gp.txt\n");
  fprintf(stderr,"  tmp-idata3.txt is a sample input data distributed with this file.\n");
  fprintf(stderr,"  test-g-gp.txt is an input file of the gnuplot\n");
  fprintf(stderr,"  test-g is the table of x and the values of Pr({y | y<x}).\n");
}

static int setParamTest() {
  int rank;
  int i;
  extern int Testrank;
  extern int MH_Verbose;
  MH_Verbose=1;
  MH_M= MH_Mg = Testrank ;
  MH_RANK = rank = mypower(2,MH_Mg);
  MH_Beta = (double *)mh_malloc(sizeof(double)*MH_Mg);
  for (i=0; i<MH_Mg; i++) MH_Beta[i] = 1.0+0.1*i;
  MH_Ng = (double *)mh_malloc(sizeof(double)); *MH_Ng = 3.0;
  Iv = (double *)mh_malloc(sizeof(double)*rank);
  for (i=0; i<rank; i++) Iv[i] = 0;
  Iv[0] = 0.001;
  Ef = 1;
  MH_X0g = 0.3;
  MH_Hg = 0.001;
  MH_Dp = 1;
  Xng = 10.0;
}
static int setParamDefault() {
  int rank;
  MH_M=MH_Mg = 2 ;
  MH_RANK=rank = mypower(2,MH_Mg);
  MH_Beta = (double *)mh_malloc(sizeof(double)*MH_Mg);
  MH_Beta[0] = 1.0; MH_Beta[1] = 2.0;
  MH_Ng = (double *)mh_malloc(sizeof(double)); *MH_Ng = 3.0;
  Iv = (double *)mh_malloc(sizeof(double)*rank);
  Iv[0] = 1.58693;
  Iv[1] = 0.811369;
  Iv[2] = 0.846874;
  Iv[3] = 0.413438;
  Ef = 0.01034957388338225707;
  MH_X0g = 0.3;
  MH_Hg = 0.001;
  MH_Dp = 1;
  Xng = 10.0;
}

static int next(struct SFILE *sfp,char *s,char *msg) {
  s[0] = '%';
  while (s[0] == '%') {
    if (!mh_fgets(s,SMAX,sfp)) {
      fprintf(stderr,"Data format error at %s\n",msg);
      mh_exit(-1);
    }
    if (s[0] != '%') return(0);
  }
}
static int setParam(char *fname) {
  int rank;
  char s[SMAX];
  struct SFILE *fp;
  int i;
  struct mh_token tk;
  extern int MH_deallocate;
  extern int MH_byFile;
  if (MH_deallocate) {
    if (MH_Beta) mh_free(MH_Beta);
    if (MH_Ng) mh_free(MH_Ng);
    if (Iv) mh_free(Iv);
    return(0);
  }
  if (fname == NULL) return(setParamDefault());

  if ((fp=mh_fopen(fname,"r",MH_byFile)) == NULL) {
    fprintf(stderr,"File %s is not found.\n",fname);
    mh_exit(-1);
  }
  next(fp,s,"MH_Mg(m)");
  sscanf(s,"%d",&MH_Mg); MH_M=MH_Mg;
  MH_RANK=rank = mypower(2,MH_Mg);

  MH_Beta = (double *)mh_malloc(sizeof(double)*MH_Mg);
  for (i=0; i<MH_Mg; i++) {
    next(fp,s,"MH_Beta");
    sscanf(s,"%lf",&(MH_Beta[i]));
  }

  MH_Ng = (double *)mh_malloc(sizeof(double));
  next(fp,s,"MH_Ng(freedom parameter n)");
  sscanf(s,"%lf",MH_Ng);
  
  next(fp,s,"MH_X0g(initial point)");
  sscanf(s,"%lf",&MH_X0g);
  
  Iv = (double *)mh_malloc(sizeof(double)*rank);
  for (i=0; i<rank; i++) {
    next(fp,s,"Iv(initial values)");
    sscanf(s,"%lg",&(Iv[i]));
  }

  next(fp,s,"Ef(exponential factor)");
  sscanf(s,"%lg",&Ef);

  next(fp,s,"MH_Hg (step size of rk)");
  sscanf(s,"%lg",&MH_Hg);

  next(fp,s,"MH_Dp (data sampling period)");
  sscanf(s,"%d",&MH_Dp);

  next(fp,s,"Xng (the last point, cf. --xmax)");
  sscanf(s,"%lf",&Xng);

  /* Reading the optional parameters */
  while ((tk = mh_getoken(s,SMAX-1,fp)).type != MH_TOKEN_EOF) {
    /* expect ID */
    if (tk.type != MH_TOKEN_ID) {
      fprintf(stderr,"Syntax error at %s\n",s); mh_exit(-1);
    }
    if ((strcmp(s,"abserr")==0) || (strcmp(s,"abserror")==0)) {
      if (mh_getoken(s,SMAX-1,fp).type != MH_TOKEN_EQ) {
        fprintf(stderr,"Syntax error at %s\n",s); mh_exit(-1);
      }
      if ((tk=mh_getoken(s,SMAX-1,fp)).type != MH_TOKEN_DOUBLE) {
        fprintf(stderr,"Syntax error at %s\n",s); mh_exit(-1);
      }
      MH_abserr = tk.dval;
      continue;
    }
    if ((strcmp(s,"relerr")==0) || (strcmp(s,"relerror")==0)) {
      if (mh_getoken(s,SMAX-1,fp).type != MH_TOKEN_EQ) {
        fprintf(stderr,"Syntax error at %s\n",s); mh_exit(-1);
      }
      if ((tk=mh_getoken(s,SMAX-1,fp)).type != MH_TOKEN_DOUBLE) {
        fprintf(stderr,"Syntax error at %s\n",s); mh_exit(-1);
      }
      MH_relerr = tk.dval;
      continue;
    }
    if (strcmp(s,"strategy")==0) {
      if (mh_getoken(s,SMAX-1,fp).type != MH_TOKEN_EQ) {
        fprintf(stderr,"Syntax error at %s\n",s); mh_exit(-1);
      }
      if ((tk=mh_getoken(s,SMAX-1,fp)).type != MH_TOKEN_INT) {
        fprintf(stderr,"Syntax error at %s\n",s); mh_exit(-1);
      }
      MH_strategy = tk.ival;
      continue;
    }
    fprintf(stderr,"Unknown ID at %s\n",s); mh_exit(-1);
  }

  mh_fclose(fp);
}

static int showParam() {
  int rank,i;
  extern int MH_strategy;
  extern double MH_abserr;
  extern double MH_relerr;
  rank = mypower(2,MH_Mg);
  printf("%%MH_Mg=\n%d\n",MH_Mg);
  for (i=0; i<MH_Mg; i++) {
    printf("%%MH_Beta[%d]=\n%lf\n",i,MH_Beta[i]);
  }
  printf("%%MH_Ng=\n%lf\n",*MH_Ng);
  printf("%%MH_X0g=\n%lf\n",MH_X0g);
  for (i=0; i<rank; i++) {
    printf("%%Iv[%d]=\n%lg\n",i,Iv[i]);
  }
  printf("%%Ef=\n%lf\n",Ef);
  printf("%%MH_Hg=\n%lf\n",MH_Hg);
  printf("%%MH_Dp=\n%d\n",MH_Dp);
  printf("%%Xng=\n%lf\n",Xng);
  printf("%%strategy=%d\n",MH_strategy);
  printf("%%abserr=%lg, %%relerr=%lg\n",MH_abserr,MH_relerr);
  printf("#MH_success=%d\n",MH_success);
  printf("#MH_coeff_max=%lg\n",MH_coeff_max);
  printf("#MH_estimated_start_step=%lg\n",MH_estimated_start_step);
}

static double estimateHg(int m, double beta[],double x0) {
  int i,j;
  double dmin;
  double cmax;
  double h;
  /* mynote on 2014.03.15 */
  if (m>1) dmin = myabs(beta[1]-beta[0]);
  else dmin=myabs(beta[0]);
  for (i=0; i<m; i++) {
    for (j=i+1; j<m; j++) {
      if (myabs(beta[i]-beta[j]) < dmin) dmin = myabs(beta[i]-beta[j]);
    }
  }
  dmin = dmin*x0*2;
  cmax = 1.0;
  for (i=0; i<m; i++) cmax = cmax*dmin;
  cmax = 1.0/cmax;
  MH_coeff_max=cmax;
  h = exp(log(MH_abserr/cmax)/5.0);
  MH_estimated_start_step = h;
  return h;
}
