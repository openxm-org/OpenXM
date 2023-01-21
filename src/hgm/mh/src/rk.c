/*
  License: LGPL
  Ref: Copied from this11/misc-2011/A1/wishart/Prog
  $OpenXM: OpenXM/src/hgm/mh/src/rk.c,v 1.17 2016/03/02 00:27:02 takayama Exp $
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "sfile.h"
#include "mh.h"

#include "t-gsl_errno.h"
#include "t-gsl_odeiv.h"
extern int MH_RANK;
extern int MH_M;
extern MH_RF mh_rf;

char *MH_Gfname;
char *MH_Dfname;
int MH_Dfname_len;
extern int MH_Mg;
extern double *MH_Beta; extern double *MH_Ng; extern double MH_X0g;
extern int MH_RawName;
double MH_Hg = 0.001;
static struct SFILE *Gf =NULL;
static struct SFILE *Df =NULL;
int MH_P95=0;
int MH_Verbose=0;
int MH_strategy=STRATEGY_DEFAULT;  /* 0: rk4, 1:a-rk4 (adaptive) */
double MH_abserr = 1e-10;
double MH_relerr = MH_RELERR_DEFAULT ;
/* D_i = MH_abserr + MH_relerr*|y_i|
   If observed error E > D*110/100, the stepsize will be decreased.
   cf. GSL, 26.3 Adaptive Step-size control.
*/

int mh_rf_for_gsl(double t,const double y[],double f[],void *params) {
  (*mh_rf)(t,(double *)y,MH_RANK,f,MH_RANK);
  return(GSL_SUCCESS);
}

static int mypower(int x,int n) {
  int a,i;
  a = 1;
  for (i=0; i<n; i++) a = a*x;
  return(a);
}
int mh_gopen_file(void) {
  FILE *fp;
  char fname[MH_SMAX];
  int i;
  extern int MH_byFile;
  Gf=NULL;
  Df=NULL;
  if (MH_Verbose) {
    oxprintfe("MH_Gfname=%s\n",MH_Gfname);
    oxprintfe("MH_Dfname=%s\n",MH_Dfname);
  }
  if (MH_RANK != mypower(2,MH_Mg)) {
    oxprintfe("rk.c MH_RANK=%d is not equal to 2^MH_Mg=%d. input_data_file is broken.\n",MH_RANK,mypower(2,MH_Mg));
    mh_usage();
    mh_exit(-1);
  }
  if (MH_Dfname != NULL) {
    if (!MH_RawName) {
      MH_Dfname_len = MH_SMAX-strlen(MH_Dfname);
      snprintf(&(MH_Dfname[strlen(MH_Dfname)]),MH_Dfname_len,"-m%d-n%lf-x0%lf-b",MH_Mg,*MH_Ng,MH_X0g);
      for (i=0; i<MH_Mg; i++) {
        snprintf(&(MH_Dfname[strlen(MH_Dfname)]),MH_Dfname_len,"%lf,",MH_Beta[i]);
      }
      snprintf(&(MH_Dfname[strlen(MH_Dfname)]),MH_Dfname_len,".txt");
    }
    Df = mh_fopen(MH_Dfname,"w",MH_byFile);
    if (Df == NULL) {
      oxprintfe("Error to open the file %s\n",MH_Dfname);
      return(-1);
    }
  }
  if (MH_Gfname != NULL) {
    Gf = mh_fopen(MH_Gfname,"w",MH_byFile);
    if (Gf == NULL) {
      oxprintfe("Error to open the file %s\n",MH_Gfname);
      return(-1);
    }
  }else return(1);
  if (MH_byFile) {
    snprintf(fname,MH_SMAX,"%s-gp.txt",MH_Gfname);
    fp = fopen(fname,"w");
    fprintf(fp,"set xrange [0:20]\n");
    fprintf(fp,"set yrange [0:1.2]\n");
    fprintf(fp,"plot \"%s\" title \"by hg\" with lines\n",MH_Gfname);
    fclose(fp);
  }
  return(0);
}
/*
  Runge-Kutta
  y_{n+1} = y_n + 1/6 k_1 + 1/3 k_2 + 1/3 k_3 + 1/6 k_4
  k_1 = h f(x_n, y_n)
  k_2 = h f(x_n + 1/2 h, y_n + 1/2 k_1)
  k_3 = h f(x_n + 1/2 h, y_n + 1/2 k_2)
  k_4 = h f(x_n + h, y_n + k_3)
*/

/* ベクトル値関数 rk is defined in tmp-code-?.c,
   f(x, y) (x : scalar, y : vector) */ 

static void show_v(double x,double *v, int n)
{
  int i;
  static int counter=0;
  extern int MH_Dp;
  char swork[MH_SSIZE];

  if (MH_Dp <= 0) return;
  if ((counter % MH_Dp) != 0) { counter++; return;} else counter=1;
  snprintf(swork,MH_SSIZE,"%lf\n",x); mh_fputs(swork,Df);
  for (i = 0; i < n; i++) {snprintf(swork,MH_SSIZE," %le\n", v[i]); mh_fputs(swork,Df);}
}

struct MH_RESULT mh_rkmain(double x0,double y0[],double xn)
{
  static int initialized=0;
  int i;
  double h;
  double x;
  char swork[MH_SSIZE];
  struct MH_RESULT result;
  extern int MH_deallocate;
  /*
    double y[MH_RANK];
    double k1[MH_RANK], k2[MH_RANK], k3[MH_RANK], k4[MH_RANK];
    double temp[MH_RANK];
    double ty[MH_RANK];
  */
  static double *y,*k1,*k2,*k3,*k4,*temp,*ty;

  if (MH_deallocate && initialized) {
    if (y) mh_free(y);
    if (k1) mh_free(k1);
    if (k2) mh_free(k2);
    if (k3) mh_free(k3);
    if (k4) mh_free(k4);
    if (temp) mh_free(temp);
    if (ty) mh_free(ty);
    y = k1 = k2 = k3 = k4 = temp = ty = NULL;
    initialized=0;  result.x=0;/*dummy init to avoid warnings.*/
    return(result);
  }
  if (!initialized) {
    y = (double *)mh_malloc(sizeof(double)*MH_RANK);
    k1 = (double *)mh_malloc(sizeof(double)*MH_RANK);
    k2 = (double *)mh_malloc(sizeof(double)*MH_RANK);
    k3 = (double *)mh_malloc(sizeof(double)*MH_RANK);
    k4 = (double *)mh_malloc(sizeof(double)*MH_RANK);
    temp = (double *)mh_malloc(sizeof(double)*MH_RANK);
    ty = (double *)mh_malloc(sizeof(double)*MH_RANK);
    initialized=1;
  }
  h = MH_Hg;
  for (i = 0; i < MH_RANK; i++)
    y[i] = y0[i];
if (MH_strategy == 0) {
  for (x = x0; (h>0?(x<xn):(x>xn)); x += h) {
    if (Df) show_v(x,y, MH_RANK);
    if (Gf) {
      snprintf(swork,MH_SSIZE,"%lf %le\n",x,y[0]);
      mh_fputs(swork,Gf);
    }
    /* Output 95% point */
    if (MH_P95) {
      if ((MH_P95==1) && (y[0] >= 0.95)) {
        oxprintf("x=%le, y[0]=%lf\n",x,y[0]);
        MH_P95=2;
      }else if ((MH_P95==2) && (y[0] >=0.9500000001)) {
        oxprintf("x=%le, y[0]=%lf\n",x,y[0]);
        MH_P95=0;
      }
    }
    (*mh_rf)(x, y, MH_RANK, temp, MH_RANK);
    for (i = 0; i < MH_RANK; i++)
      k1[i] = h * temp[i];

    for (i = 0; i < MH_RANK; i++)
      ty[i] = y[i] + 0.5 * k1[i];
    (*mh_rf)(x + 0.5 * h, ty, MH_RANK, temp, MH_RANK);
    for (i = 0; i < MH_RANK; i++)
      k2[i] = h * temp[i];

    for (i = 0; i < MH_RANK; i++)
      ty[i] = y[i] + 0.5 * k2[i];
    (*mh_rf)(x + 0.5 * h, ty, MH_RANK, temp, MH_RANK);
    for (i = 0; i < MH_RANK; i++)
      k3[i] = h * temp[i];

    for (i = 0; i < MH_RANK; i++)
      ty[i] = y[i] + k3[i];
    (*mh_rf)(x + h, ty, MH_RANK, temp, MH_RANK);
    for (i = 0; i < MH_RANK; i++)
      k4[i] = h * temp[i];
                
    for (i = 0; i < MH_RANK; i++)
      y[i] = y[i] + 1.0/6.0 * k1[i] + 1.0/3.0 * k2[i] + 1.0/3.0 * k3[i] + 1.0/6.0 * k4[i];
  }
}else
  {
    extern int MH_Dp;
    double dh;
    double mh_dp_orig; 
    double x1;
    const gsl_odeiv_step_type *T = gsl_odeiv_step_rkf45;
    gsl_odeiv_step *s = gsl_odeiv_step_alloc(T, MH_RANK); 
    gsl_odeiv_control *c = gsl_odeiv_control_y_new(MH_abserr, MH_relerr);
    /* We should use the relative error.
       hgm.cwishart(m=5,n=20,beta=c(1,2,3,4,5),x=20,x0=2,approxdeg=20);
       0.977
       hgm.cwishart(m=8,n=20,beta=c(1,2,3,4,5,5.6,5.7,5.8),x=30,x0=1,approxdeg=20);
       0.962 (non-gsl) or 0.969 (gsl, abs error 0.0, relative error 1e-10), 
       NaN abs=0, rel=1e-5 or abs=1e-18, rel=1e-6
       ./mh causes NaN when rel error is 1e-10 on old 32bit machines.
     */
    gsl_odeiv_evolve *e = gsl_odeiv_evolve_alloc(MH_RANK);
    gsl_odeiv_system sys = {mh_rf_for_gsl, NULL, 0, NULL};
    sys.dimension = MH_RANK;
    /* oxprintf("MH_RANK=%d\n",MH_RANK); */
    if (x0 >= xn) {oxprintfe("Error: x0 < x must hold.\n"); mh_exit(-30);}
    x = x0;
    if (MH_Dp > 0) dh = MH_Dp*h; else dh=xn-x0;
    mh_dp_orig = MH_Dp; MH_Dp=1; 
    while (x < xn)  {
      if (Df) show_v(x,y, MH_RANK);
      if (Gf) {
        snprintf(swork,MH_SSIZE,"%lf %le\n",x,y[0]);
        mh_fputs(swork,Gf);
      }
      /* Output 95% point */
      if (MH_P95) {
        if ((MH_P95==1) && (y[0] >= 0.95)) {
          oxprintf("x=%le, y[0]=%lf\n",x,y[0]);
          MH_P95=2;
        }else if ((MH_P95==2) && (y[0] >=0.9500000001)) {
          oxprintf("x=%le, y[0]=%lf\n",x,y[0]);
          MH_P95=0;
        }
      }
      x1 = x+dh;
      while ((x < x1) && (x < xn)) {
        int status = gsl_odeiv_evolve_apply(e, c, s, &sys, &x, x1, &h, y);
        if (status != GSL_SUCCESS) {
	  oxprintfe("gsl_odeiv_evolve_apply failed.\n");
	  break;
	}
      }
    }
    gsl_odeiv_evolve_free(e);
    gsl_odeiv_control_free(c);
    gsl_odeiv_step_free(s);
    MH_Dp=mh_dp_orig;
  }

  if (MH_Verbose) oxprintf("x=%lf, y[0]=%lg\n",x,y[0]);
  result.x = x;
  result.rank = MH_RANK;
  result.y = (double *)mh_malloc(sizeof(double)*MH_RANK); /* todo, how to free it */
  for (i=0; i<MH_RANK; i++) (result.y)[i] = y[i];
  result.size=2;
  result.sfpp = (struct SFILE **)mh_malloc(sizeof(struct SFILE *)*(result.size)); /* todo, free */
  (result.sfpp)[0] = Df;  
  (result.sfpp)[1] = Gf;
  return result;
}


/*
 rk4  MH_strategy==0;
 a-rk4  MH_strategy==1;  adaptive
 a-rk4-m  MH_strategy==2;  adaptive and multiply
*/
void mh_set_strategy(int s,double err[2]) {
  MH_strategy = s;
  if (err[0] >= 0.0) MH_abserr = err[0];
  if (err[1] >= 0.0) MH_relerr = err[1];
}
