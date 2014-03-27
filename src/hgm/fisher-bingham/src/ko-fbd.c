#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include<string.h>
#include<gsl/gsl_errno.h>
#include<gsl/gsl_eigen.h>
#include<gsl/gsl_statistics.h>
#include <gsl/gsl_cdf.h>
#include "ko-perturbation.h"
#include "ko-time.h"

#define MAXDIM 10      /* maximum size of the matrix x  */
#define NDATA 100    /* number of sample data for computing confidence interval */

int dim;  /* dimension of the sphere */
double eps_pert = 0.00001; 
int flag_verbose = 0;
int flag_confidence = 0;
int flag_time =0;

extern void pow_series(const double *x,const double *y,const double r, double *ans);                                /* defined in ko-fbd-ps.c */
extern int maxdeg;
extern void runge_kutta(double *g, const double *g0dev, const double *xdiag0, const double *ydiag0,const double r); /* defined in ko-fbd-rk.c */

static void
get_xdiag(double **x, const double *y, double *xdiag, double *ydiag, double **P)
{
  gsl_eigen_symmv_workspace *w = gsl_eigen_symmv_alloc( dim+1);
  gsl_matrix *A = gsl_matrix_alloc(dim+1, dim+1);
  gsl_vector *eval = gsl_vector_alloc(dim+1);
  gsl_matrix *evec = gsl_matrix_alloc(dim+1, dim+1);
  int i,j;

  for(i=0; i<dim+1; i++){
	for(j=0; j<dim+1; j++){
	  gsl_matrix_set(A, i, j, x[i][j]);
	}
  }

  gsl_eigen_symmv(A, eval, evec, w);
  gsl_eigen_symmv_sort(eval,evec, GSL_EIGEN_SORT_VAL_DESC);

  for(i=0; i<dim+1; i++){ xdiag[i] = gsl_vector_get(eval, i);}
  for(i=0; i<dim+1; i++){
	ydiag[i] = 0.0;
	for(j=0; j<dim+1; j++){
	  ydiag[i] += gsl_matrix_get(evec, j, i)*y[j]; /* ydiag = evec' * y */
	}
  }
  for (i=0; i<dim+1; i++) {
    for (j=0; j<dim+1; j++){
      P[i][j] = gsl_matrix_get(evec, i,j);
    }
  }

  gsl_eigen_symmv_free(w);
  gsl_matrix_free(A);  
  gsl_vector_free(eval);
  gsl_matrix_free(evec);  
}

static double 
move_near0(double *xdiag, double *ydiag, double *xdiag0, double *ydiag0)
{
  int i;
  double sum, r;

  sum = 0.0;
  for(i=0; i<dim+1; i++){
	sum += fabs(xdiag[i]) + ydiag[i]*ydiag[i];
  }
  r = sum;
  for(i=0; i<dim+1; i++){
	xdiag0[i] = xdiag[i]/(r*r); 
	ydiag0[i] = ydiag[i]/r;
  }
  return r;
}

static void
dev_max_eigen(const double *g0,const double max_eigen, double *g0dev)
{
  int i;
  double tmp = exp(-max_eigen);
  for(i=0; i<2*dim+2; i++)
	g0dev[i] = g0[i]*tmp;
}

static void
g2fdiag(double *fdiag, const double *g, const double r, const double *xdiag0)
{
  double tmp;
  int i;
  tmp = exp(r*r*xdiag0[0]);
  for(i=0; i<2*dim+2; i++){fdiag[i] = g[i]*tmp;}

  tmp = pow(r, dim+1);
  for(i=0; i<dim+1; i++){fdiag[i] /= tmp;  }
  tmp *= r;
  for(i=0; i<dim+1; i++){fdiag[i+dim+1] /= tmp;  }
}

static void
fdiag2f(double *f, const double *fdiag, const double *xdiag, const double *ydiag, double **P)
{
  double h[dim+1][dim+1];
  int i,j,k;

  for(i=0; i<2*dim+2; i++){ f[i] =0.0;}
  for(i=0; i<dim+1; i++){ f[0] += fdiag[i+dim+1];}
  for(i=0; i<dim+1; i++){
	for(j=0; j<dim+1; j++){
	  f[i+1] += P[i][j]*fdiag[j];
	}
  }
  for(i=0; i<dim+1; i++){
	for(j = i+1; j<dim+1; j++){
	  h[i][j] = -(ydiag[i]*fdiag[j]-ydiag[j]*fdiag[i])/(xdiag[i]-xdiag[j]);
	}
  }
  for(i=0; i<dim; i++){
    for(j=0; j<dim+1; j++){
      f[i+dim+2] += P[i][j]*P[i][j]*fdiag[j+dim+1];
    }
    for(j=0; j<dim+1; j++){
      for(k = j+1; k<dim+1; k++){
	f[i+dim+2] += P[i][j] * P[i][k]*h[j][k];
      }
    }
  }	
}

double 
*ko_fbd(double **x, double *y)
{
  if(flag_time) ko_time_init(3, "total", "power-series","runge-kutta");
  if(flag_time) ko_time_start("total");
  int i,j;
  double r;
  double xdiag[dim+1], ydiag[dim+1];
  double xdiag0[dim+1], ydiag0[dim+1];
  double g0[2*dim+2], g0dev[2*dim+2], g[2*dim+2];
  double fdiag[2*dim+2];
  double *f = (double *)malloc(sizeof(double) *(2*dim+2));
  double **P, *Py;
  P  = (double **)malloc(sizeof(double *) * (dim+1)); 
  Py = (double *)malloc(sizeof(double) * (dim+1)*(dim+1)); 
  for (i=0; i<dim+1; i++) { /* initialize */
    P[i] = Py + i*(dim+1);
    for (j=0; j<dim+1; j++){
      P[i][j] = 0.0;
    }
  }

  get_xdiag(x, y, xdiag, ydiag, P);
  r=move_near0(xdiag, ydiag, xdiag0, ydiag0);
  if(flag_time)
	ko_time_start("power-series");
  pow_series(xdiag0, ydiag0, 1.0, g0);
  if(flag_time) 
	ko_time_end("power-series");
  dev_max_eigen(g0,xdiag0[0], g0dev);
  if(flag_time) 
	ko_time_start("runge-kutta");  
  runge_kutta(g,g0dev, xdiag0, ydiag0, r);
  if(flag_time) 
	ko_time_end("runge-kutta");  
  g2fdiag(fdiag,g,r,xdiag0);
  fdiag2f(f,fdiag,xdiag,ydiag,P);

  if(flag_verbose){
	printf("diagnalization:\n");
    print_vec(2,dim+1,
	      "\txdiag: ", xdiag,
	      "\tydiag: ", ydiag
	      );
	printf("\tP:\n");
	for(i=0; i<dim+1; i++)
	  print_vec(1,dim+1, "\t\t", P[i]);

    printf("initial point of Runge-Kutta:\n");
    print_vec(2,dim+1,
	      "\txdiag0: ", xdiag0,
	      "\tydiag0: ", ydiag0
	      );
	printf("maximal eigenvalue:%g\n", xdiag0[0]);

    printf("move from r=1 to r=%f\n", r);
    print_vec(4,2*dim+2,
	      "power seriese:\n\t", g0,
	      "initial values of Runge-Kutta:\n\t", g0dev,
	      "output of Runge-Kutta:\n\t", fdiag,
	      "f:\n\t", f
	      );
  }

  if(flag_confidence){  /*perturbation*/  
    double *g0devp;
    int ii;
    double err[2*dim+2];
    double fp[NDATA][2*dim+2];
    double fpt[2*dim+2][NDATA];
    double mean, sd, cdfinv;
    perturbation_init(2*dim+2, eps_pert);
    printf("perturbation:\n");
    for(ii=0; ii< NDATA; ii++){
	  double *g0p = perturbation(g0,err);
	  dev_max_eigen(g0p,xdiag0[0], g0dev);
      runge_kutta(g, g0dev, xdiag0, ydiag0, r);
      g2fdiag(fdiag, g,r,xdiag0);
      fdiag2f(fp[ii],fdiag,xdiag,ydiag,P);
      if(flag_verbose>1)
		print_vec(2,2*dim+2,
				  "error= ", err,
				  "f= ", fp[ii]
				  );
      free(g0p);
    }
    perturbation_free();

    for(i=0; i<NDATA; i++){
      for(j=0; j<2*dim+2; j++){
		fpt[j][i] = fp[i][j];
      }
    }
	if(flag_verbose){
	  printf("eps_pert=%e, NDATA=%d\n", eps_pert, NDATA);
	  printf("eps_pert * exp(-max_eigen) = %g\n", eps_pert * exp(-xdiag0[0]));
	  printf("   i:         f[i],            mean,              sd,   confidence interval\n");
	  for(i=0; i< 2*dim+2; i++){
		mean = gsl_stats_mean(fpt[i], 1, NDATA);
		sd = gsl_stats_sd(fpt[i], 1, NDATA);
		cdfinv = gsl_cdf_gaussian_Pinv (1.0-0.025, sd);
		printf("f[%d]:\t%10g,\t%10g,\t %10e,\t[%10g, %10g]\n",i, f[i], mean, sd, mean-cdfinv, mean+cdfinv);
	  }
	}else{
	  printf("            mean,              sd,   confidence interval\n");
		mean = gsl_stats_mean(fpt[0], 1, NDATA);
		sd = gsl_stats_sd(fpt[0], 1, NDATA);
		cdfinv = gsl_cdf_gaussian_Pinv (1.0-0.25, sd);
		printf("\t%10g,\t %10e,\t[%10g, %10g]\n",mean, sd, mean-cdfinv, mean+cdfinv);	  
	}
  }
  free(*P); free(P);
  if(flag_time) ko_time_end("total");
  if(flag_time) ko_time_print();
  return f;
}

#ifdef _HGD
double*
fbnd(int d, double x0[MAXDIM][MAXDIM], double y0[], int maxd, int w[])
{
  dim = d;
  maxdeg = maxd;
  flag_verbose=1;

  double **X, *Xy, *Y;
  int M = dim+1;
  X = (double **)malloc(sizeof(double *) * M); /* memory allocation */
  Xy = (double *)malloc(sizeof(double) * M*M); 
  Y  = (double *)malloc(sizeof(double) * M);

  int i,j;
  for (i=0; i<M; i++) { /* initializing  */
	X[i] = Xy + i*M;
	Y[i] = y0[i];
	for (j=0; j<M; j++){
	  X[i][j] = x0[i][j];
	}
  }

  double *ans = ko_fbd( X, Y);
  free(*X); free(X); free(Y);
  return ans;
}
#endif
