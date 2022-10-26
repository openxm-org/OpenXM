/*
  $OpenXM: OpenXM/src/hgm/mh/src/wmain.c,v 1.36 2016/06/06 04:49:11 takayama Exp $
  License: LGPL
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "sfile.h"
#include "mh.h"
#define SMAX 4096
#define inci(i) { i++; if (i >= argc) { oxprintfe("Option argument is not given.\n"); return(NULL); }}
#define VSTRING "%!version2.0"
int MH_deallocate=0;

/*
  changelog
  2016.02.13  code-n.c and code-n-2f1.c are linked together. New idata format.
  2016.02.04  MH_Ef_type  exponential or scalar factor
  2016.02.01  C_2F1
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

int MH_P_pFq=1;
int MH_Q_pFq=1;
double *MH_A_pFq=NULL;
double *MH_B_pFq=NULL;
int MH_Ef_type=1;
MH_RF mh_rf=NULL;  /* function pointer to mh_rf */

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
  if (mh_rf) (*mh_rf)(0.0, NULL, 0, NULL, 0);
  MH_deallocate=0; /* switch to the normal mode. */
}
static int mypower(int x,int n) {
  int a,i;
  a = 1;
  for (i=0; i<n; i++) a = a*x;
  return(a);
}
#ifdef STANDALONE2
int main(int argc,char *argv[]) {
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
  extern double Ef;
  Ef = 1;
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
      oxprintfe("Unknown option %s\n",argv[i]);
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
      oxprintfe("%%%%Warning, abserr seems not to be small enough, abserr=%lg, y[0]=%lg. Increasing the starting point (q0 or X0g(standalone case)) may or making abserr (err[1] or abserror(standalone case))  smaller will help, e.g., err=c(%lg,1e-10)\n",MH_abserr,y0[0],y0[0]*(1e-6));
    }else{
      MH_success = 1;
    }
  }else{
    MH_success = 0;
  }
  MH_estimated_start_step = estimateHg(MH_Mg,MH_Beta,MH_X0g);
  if (MH_Verbose) showParam();
  if (MH_Verbose) {for (i=0; i<rank; i++) oxprintf("%lf\n",y0[i]); }

  *rp=mh_rkmain(x0,y0,xn);
  return(rp);
}

int mh_usage(void) {

#include "usage-w-n.h"

  return(0);
}

static int setParamTest(void) {
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
  Xng = 10.0; return(0);
}
static int setParamDefault(void) {
  int rank;
  double a[1];
  double b[1];
  MH_M=MH_Mg = 2 ;
  MH_RANK=rank = mypower(2,MH_Mg);
  MH_Beta = (double *)mh_malloc(sizeof(double)*MH_Mg);
  MH_Beta[0] = 1.0; MH_Beta[1] = 2.0;
  MH_Ng = (double *)mh_malloc(sizeof(double)); *MH_Ng = 3.0;
  a[0] = (MH_M+1.0)/2.0;
  b[0] = a[0]+(*MH_Ng)/2.0;
  mh_setA(a,1); mh_setB(b,1);
  mh_rf = & mh_rf_ef_type_1;
  Iv = (double *)mh_malloc(sizeof(double)*rank);
  Iv[0] = 1.58693;
  Iv[1] = 0.811369;
  Iv[2] = 0.846874;
  Iv[3] = 0.413438;
  Ef = 0.01034957388338225707;
  MH_X0g = 0.3;
  MH_Hg = 0.001;
  MH_Dp = 1000;
  Xng = 10.0; return(0);
}
#ifdef STANDALONE
/* may remove */
static int next_old1(struct SFILE *sfp,char *s,char *msg) {
  s[0] = '%';
  while (s[0] == '%') {
    if (!mh_fgets(s,SMAX,sfp)) {
      oxprintfe("Data format error at %s\n",msg);
      mh_exit(-1);
    }
    if (s[0] != '%') return(0);
  } return(0);
}
#endif
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
  int version;
  double a[1]; double b[1]; /* for old inputs */
  struct mh_token tk;
  extern int MH_deallocate;
  extern int MH_byFile;
  if (MH_deallocate) {
    if (MH_Beta) {mh_free(MH_Beta); MH_Beta=NULL;}
    if (MH_Ng) {mh_free(MH_Ng); MH_Ng=NULL;}
    if (Iv) {mh_free(Iv); Iv = NULL;}
	if (MH_A_pFq) {mh_free(MH_A_pFq); MH_A_pFq=NULL; MH_P_pFq = 0;}
	if (MH_B_pFq) {mh_free(MH_B_pFq); MH_B_pFq=NULL; MH_Q_pFq = 0;}
    return(0);
  }
  if (fname == NULL) return(setParamDefault());

  if ((fp=mh_fopen(fname,"r",MH_byFile)) == NULL) {
    oxprintfe("File %s is not found.\n",fname);
    mh_exit(-1);
  }
  /* set default initial values */
  MH_Mg=MH_M=-1;  /* number of variables */
  MH_Ng=(double *) mh_malloc(sizeof(double)); *MH_Ng=-1; /* *Ng is the degree of freedom 1F1 */
  MH_X0g=0.1;   /* evaluation point */
  Ef=1.0;    /* exponential factor */
  MH_Ef_type=1;
  MH_Hg=0.01;   /* step size for RK */
  MH_Dp = -1;   /* sampling rate */
  Xng = 10.0; /* terminal point for RK */

  /* Parser for the old style (version <2.0) input */
  version=next(fp,s,"Mg(m)");
  if (version == 2) goto mh_myparse;
  sscanf(s,"%d",&MH_Mg); MH_M=MH_Mg;
  MH_RANK=rank = mypower(2,MH_Mg);

  MH_Beta = (double *)mh_malloc(sizeof(double)*MH_Mg);
  for (i=0; i<MH_Mg; i++) {
    next(fp,s,"MH_Beta");
    sscanf(s,"%lf",&(MH_Beta[i]));
  }

  next(fp,s,"MH_Ng(freedom parameter n)");
  sscanf(s,"%lf",MH_Ng);
  /* for old style input only for 1F1 ef_type==1. */
  MH_P_pFq = 1; MH_Q_pFq = 1;
  a[0] = (MH_M+1.0)/2.0;
  b[0] = a[0]+(*MH_Ng)/2.0;
  mh_setA(a,1); mh_setB(b,1);
  mh_rf = & mh_rf_ef_type_1;
  
  
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
 mh_myparse:
  while ((tk = mh_getoken(s,SMAX-1,fp)).type != MH_TOKEN_EOF) {
    /* expect ID */
    if (tk.type != MH_TOKEN_ID) {
      oxprintfe("Syntax error at %s\n",s); mh_exit(-1);
    }
    if ((strcmp(s,"abserr")==0) || (strcmp(s,"abserror")==0)) {
      if (mh_getoken(s,SMAX-1,fp).type != MH_TOKEN_EQ) {
        oxprintfe("Syntax error at %s\n",s); mh_exit(-1);
      }
      if ((tk=mh_getoken(s,SMAX-1,fp)).type == MH_TOKEN_DOUBLE) {
	MH_abserr = tk.dval;
      }else if (tk.type == MH_TOKEN_INT) {
	MH_abserr = tk.ival;
      }else{
        oxprintfe("Syntax error at %s\n",s); mh_exit(-1);
      }
      continue;
    }
    if ((strcmp(s,"relerr")==0) || (strcmp(s,"relerror")==0)) {
      if (mh_getoken(s,SMAX-1,fp).type != MH_TOKEN_EQ) {
        oxprintfe("Syntax error at %s\n",s); mh_exit(-1);
      }
      if ((tk=mh_getoken(s,SMAX-1,fp)).type == MH_TOKEN_DOUBLE) {
	MH_relerr = tk.dval;
      }else if (tk.type == MH_TOKEN_INT) {
	MH_relerr = tk.dval;
      }else{
        oxprintfe("Syntax error at %s\n",s); mh_exit(-1);
      }
      continue;
    }
    if (strcmp(s,"strategy")==0) {
      if (mh_getoken(s,SMAX-1,fp).type != MH_TOKEN_EQ) {
        oxprintfe("Syntax error at %s\n",s); mh_exit(-1);
      }
      if ((tk=mh_getoken(s,SMAX-1,fp)).type != MH_TOKEN_INT) {
        oxprintfe("Syntax error at %s\n",s); mh_exit(-1);
      }
      MH_strategy = tk.ival;
      continue;
    }
    // Format: #p_pFq=2  1.5  3.2   override by new input format.
    if (strcmp(s,"p_pFq")==0) {
      if (mh_getoken(s,SMAX-1,fp).type != MH_TOKEN_EQ) {
        oxprintfe("Syntax error at %s\n",s); mh_exit(-1);
      }
      if ((tk=mh_getoken(s,SMAX-1,fp)).type != MH_TOKEN_INT) {
        oxprintfe("Syntax error at %s\n",s); mh_exit(-1);
      }
      MH_P_pFq = tk.ival;
	  mh_setA(NULL,tk.ival); /* allocate mem */
      for (i=0; i<MH_P_pFq; i++) { 
	if ((tk=mh_getoken(s,SMAX-1,fp)).type == MH_TOKEN_DOUBLE) {
	  MH_A_pFq[i] = tk.dval;
	}else if (tk.type == MH_TOKEN_INT) {
	  MH_A_pFq[i] = tk.ival;
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
      MH_Q_pFq = tk.ival;
	  mh_setB(NULL,tk.ival); /* allocate mem */
      for (i=0; i<MH_Q_pFq; i++) { 
	if ((tk=mh_getoken(s,SMAX-1,fp)).type == MH_TOKEN_DOUBLE) {
	  MH_B_pFq[i] = tk.dval;
	}else if (tk.type == MH_TOKEN_INT) {
	  MH_B_pFq[i] = tk.ival;
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
      MH_Ef_type = tk.ival;
      continue;
    }

    if (strcmp(s,"Mg")==0) {
      if (mh_getoken(s,SMAX-1,fp).type != MH_TOKEN_EQ) {
        oxprintfe("Syntax error at %s\n",s); mh_exit(-1);
      }
      if ((tk=mh_getoken(s,SMAX-1,fp)).type != MH_TOKEN_INT) {
        oxprintfe("Syntax error at %s\n",s); mh_exit(-1);
      }
      MH_Mg = tk.ival;  MH_M = MH_Mg;
      MH_RANK = rank = mypower(2,MH_Mg);  
      continue;
    }
    if (strcmp(s,"Beta")==0) {
      if (mh_getoken(s,SMAX-1,fp).type != MH_TOKEN_EQ) {
        oxprintfe("Syntax error at %s\n",s); mh_exit(-1);
      }
	  if (MH_Mg <= 0) {
        oxprintfe("Mg should be set before reading Beta.\n"); mh_exit(-1);
      }
      MH_Beta = (double *)mh_malloc(sizeof(double)*MH_Mg);
	  for (i=0; i<MH_Mg; i++) {
        if ((tk=mh_getoken(s,SMAX-1,fp)).type == MH_TOKEN_DOUBLE) {
           MH_Beta[i] = tk.dval;
        }else if (tk.type == MH_TOKEN_INT) {
           MH_Beta[i] = tk.ival;
        }else {
          oxprintfe("Syntax error at %s\n",s); mh_exit(-1);
        }
      }
      Iv = (double *)mh_malloc(sizeof(double)*rank);
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
		*MH_Ng = tk.dval;
      }else if (tk.type == MH_TOKEN_INT) {
        *MH_Ng = tk.ival;
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
		MH_X0g = tk.dval;
	  }else if (tk.type == MH_TOKEN_INT) {
		MH_X0g = tk.ival;
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
		MH_Hg = tk.dval;
	  }else if (tk.type == MH_TOKEN_INT) {
		MH_Hg = tk.ival;
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
      MH_Dp = tk.dval;
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

    oxprintfe("Unknown ID for wmain.c (old...) at %s.\n",s);  mh_exit(-1);
  }
  if (MH_Ef_type == 1) {
	mh_rf = &mh_rf_ef_type_1;
  }else if (MH_Ef_type == 2) {
	mh_rf = &mh_rf_ef_type_2;
  }else{
	oxprintfe("Ef_type = %d is not implemented.\n",MH_Ef_type); mh_exit(-1);
  }
  if ((MH_M < 0) || (MH_A_pFq==NULL)) {
	oxprintfe("MH_M and p_pFq, q_pFq are compulsory parameters.\n"); mh_exit(-1);
  }
  if (MH_Dp < 0) MH_Dp = (int) floor(1/MH_Hg);
  if (MH_P95 && (MH_Dp*MH_Hg < 2.0)) {
	oxprintfe("%%%%Warning, the resolution to find 95 percent point is less than 2.0. Decrease Dp\n");
  }

  mh_fclose(fp); return(0);
}

static int showParam(void) {
  int rank,i;
  extern int MH_strategy;
  extern double MH_abserr;
  extern double MH_relerr;
  rank = mypower(2,MH_Mg);
  oxprintf("%%MH_Mg=\n%d\n",MH_Mg);
  for (i=0; i<MH_Mg; i++) {
    oxprintf("%%MH_Beta[%d]=\n%lf\n",i,MH_Beta[i]);
  }
  oxprintf("%%MH_Ng=\n%lf\n",*MH_Ng);
  oxprintf("%%MH_X0g=\n%lf\n",MH_X0g);
  for (i=0; i<rank; i++) {
    oxprintf("%%Iv[%d]=\n%lg\n",i,Iv[i]);
  }
  oxprintf("%%Ef=\n%lf\n",Ef);
  oxprintf("%%MH_Hg=\n%lf\n",MH_Hg);
  oxprintf("%%MH_Dp=\n%d\n",MH_Dp);
  oxprintf("%%Xng=\n%lf\n",Xng);
  oxprintf("%%strategy=%d\n",MH_strategy);
  oxprintf("%%abserr=%lg, %%relerr=%lg\n",MH_abserr,MH_relerr);
  oxprintf("#MH_success=%d\n",MH_success);
  oxprintf("#MH_coeff_max=%lg\n",MH_coeff_max);
  oxprintf("#MH_estimated_start_step=%lg\n",MH_estimated_start_step); 
  oxprintf("%%ef_type=%d\n",MH_Ef_type);
  oxprintf("%%q_pFq=%d, ",MH_P_pFq);
  for (i=0; i<MH_P_pFq; i++) {
    if (i != MH_P_pFq-1) oxprintf(" %lg,",MH_A_pFq[i]); 
    else oxprintf(" %lg\n",MH_A_pFq[i]); 
  }
  oxprintf("%%q_pFq=%d, ",MH_Q_pFq); 
  for (i=0; i<MH_Q_pFq; i++) {
    if (i != MH_Q_pFq-1) oxprintf(" %lg,",MH_B_pFq[i]); 
    else oxprintf(" %lg\n",MH_B_pFq[i]); 
  }
  return(0);
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

void mh_setA(double a[],int alen) {
  int i;
  if (alen < 0) {
	if (MH_A_pFq != NULL) mh_free(MH_A_pFq);
	MH_A_pFq=NULL; MH_P_pFq=-1;
	return;
  }
  if (alen == 0) {
	MH_P_pFq=0; return;
  }
  MH_P_pFq=alen;
  MH_A_pFq = (double *)mh_malloc(MH_P_pFq*sizeof(double));
  if (a != NULL) {
	for (i=0; i<alen; i++) MH_A_pFq[i] = a[i];
  }else{
	for (i=0; i<alen; i++) MH_A_pFq[i] = 0.0;
  }
  return;
}
void mh_setB(double b[],int blen) {
  int i;
  if (blen < 0) {
	if (MH_B_pFq != NULL) mh_free(MH_B_pFq);
	MH_B_pFq=NULL; MH_Q_pFq=-1;
	return;
  }
  if (blen == 0) {
	MH_Q_pFq=0; return;
  }
  MH_Q_pFq=blen;
  MH_B_pFq = (double *)mh_malloc(MH_Q_pFq*sizeof(double));
  if (b != NULL) {
	for (i=0; i<blen; i++) MH_B_pFq[i] = b[i];
  }else{
	for (i=0; i<blen; i++) MH_B_pFq[i] = 0.0;
  }
  return;
}
