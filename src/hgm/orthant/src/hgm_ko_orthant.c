/*$OpenXM: OpenXM/src/hgm/orthant/src/hgm_ko_orthant.c,v 1.5 2015/04/01 06:10:34 takayama Exp $*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
/* for runge kutta */
#include "t-gsl_errno.h"
#include "t-gsl_odeiv.h"
#include "oxprint.h"

#ifndef _STANDALONE
#include <R_ext/BLAS.h>
#include <R_ext/Lapack.h>
#ifndef FCONE
#define FCONE
#endif

void hgm_ko_orthant(int *, double *, double *, double *retv);

#ifdef _STANDALONE
//extern double ddot_(int *n, double *x, int *incx, double *y, int *incy);
extern void dcopy_(int *, double *, int *, double *, int *);
extern void dscal_(int *, double *, double *, int *);
//extern void daxpy_(int *, double *, double *, int *, double *, int *);
extern void dsymv_(char *uplo, int *n, double *alpha, double *a, int *lda, double *x, int *incx, double *beta, double *y, int *incy);

extern void dpotrf_(char *uplo, int *n, double *a, int *lda, int *info);
extern void dpotri_(char *uplo, int *n, double *a, int *lda, int *info);
#endif

static int dim, rank, dimdim;
static double *delVI;
static double *X, *X0, *X1, *delx, *sigmaI, *muI;
static double *Y,*Y1, *dY;
//static double det_sigma;

#ifdef _VERBOSE
static void print_vector(FILE *, double *, int, char *);
static void print_matrix2(FILE *fp, int size, double *matrix,const char *str);
#endif
static int check_sigma(double *);
static double norm(double *);
static void sigma_mu2xy(double *sigma, double *mu, double *x, double *y);
static void initialization(double *x, double *y, double *f, double *h);
static void get_prob(double *);
static void update_XY(const double );
static void cal_sigmaI_muI(const int );
static double cal_det_sigmaI(const int, const int);
static double cal_sub_quad_form(int I);
static void cal_del_VI_y(const int I, const double *g);
static double get_delVI(const int, const int, const int,const double *);
static int function(double t, const double g[],
			  double dg[], void *params);
static void move(double *, double);

#ifdef _VERBOSE
static FILE *verbose_out;
void set_verbose_out(FILE *);
#endif


/* gsl-util.c */
//extern gsl_matrix *array2mat(int n, int m, double **X);
//extern gsl_vector *array2vec(int n, double *Y);
//extern void inverse_matrix(gsl_matrix *m, gsl_matrix *invm);
//extern double determinant(gsl_matrix *);
//extern double sub_determinant(const gsl_matrix *m, int I);
//extern  void print_vector(FILE *, double *, int, char *);
//extern void print_matrix(FILE *fp, int size, double **matrix,const char *str);
//extern void print_matrix2(FILE *fp, int size, double *matrix,const char *str);
//extern void print_gsl_matrix(FILE *,gsl_matrix *m, char *str);
//extern void print_gsl_vector(FILE *, gsl_vector *v, char *str);

/*
  Evaluating orthant probabilities.
  d: dimension.
  x: a symmetric positive definite matrix with size d.
  y: a vector with length d.
*/
void 
hgm_ko_orthant(int *d, double *sigma, double *mu, double *retv)
{
  dim = *d;
  rank = 1 << dim;   /* rank = 2^dim, see theorem 15 */
  dimdim = dim * dim;
#ifdef _VERBOSE
  fprintf(verbose_out, "dim=%d\nrank=%d\n", dim, rank);
  print_matrix2(verbose_out, dim,sigma, "sigma"); 
  print_vector(verbose_out, mu, dim, "mu"); 
#endif 
  if(check_sigma(sigma)) /* check that sigma is symmetric and positive definite */
    oxexit0(EXIT_FAILURE);

  double x[dim*dim], y[dim];

  sigma_mu2xy(sigma, mu, x, y);
  if(norm(y) > 7.0){ /* norm(y) <- sqrt(y_1^2 +...+y_d^2 ) */
    oxprintfe( "The vector y is large. HGM may take long time!\n");
  }
#ifdef _VERBOSE
  print_matrix2(verbose_out, dim,x, "x"); 
  print_vector(verbose_out, y, dim, "y"); 
#endif 

  double *f = (double *)malloc(sizeof(double)*rank);
  double h = 1e-8; /* h controls the accuracy of the Runge-Kutta method */
  X = (double*) malloc(sizeof(double) * dim * dim);
  X0 = (double*) malloc(sizeof(double) * dim * dim);
  X1 = (double*) malloc(sizeof(double) * dim * dim);
  delx = (double*) malloc(sizeof(double) * dim * dim);
  sigmaI = (double *) malloc(sizeof(double) * dim * dim);
  muI = (double *) malloc(sizeof(double) * dim);
  delVI = (double *) malloc(sizeof(double) * rank * dim);
  Y = (double *) malloc(sizeof(double) * dim);
  Y1 = (double *) malloc(sizeof(double) * dim);
  dY = (double *) malloc(sizeof(double) * dim);

  initialization(x,y,f,&h);
#ifdef _VERBOSE
  fprintf(verbose_out, "h=%g\n", h);
  print_vector(verbose_out, f, rank, "              Initial values:");
#endif

  move(f, h); /* holonomic gradient method */
#ifdef _VERBOSE
  print_vector(verbose_out, f, rank, "Result of Runge-Kutta method:");
#endif

  get_prob(f);
#ifdef _VERBOSE
  print_vector(verbose_out, f, rank, "               Probabilities:");
#endif

  double result = f[rank-1];
  free(f);
  free(X); free(X0); free(X1);
  free(delx);
  free(sigmaI); free(muI);
  free(delVI);
  free(Y); free(Y1); free(dY);
  *retv = result;
  return;
}

#ifdef _VERBOSE
void 
set_verbose_out(FILE *fp){
  verbose_out = fp;
}
#endif

static void 
sigma_mu2xy(double *sigma, double *mu, double *x, double *y)
{
  /* x <- -0.5 * (sigma)^(-1)  */
  /* y <- (sigma)^(-1) * mu  */
  int info;
  int one = 1;
  double alpha = 1.0, beta = 0.0;
#ifdef _STANDALONE
  dcopy_(&dimdim, sigma, &one, x, &one);
  dpotrf_("U", &dim, x, &dim, &info);
  dpotri_("U", &dim, x, &dim, &info);
  dsymv_("U", &dim, &alpha, x, &dim, mu, &one, &beta, y, &one);
  alpha = -0.5;
  dscal_(&dimdim, &alpha, x, &one);
#else
#endif
  F77_CALL(dcopy)(&dimdim, sigma, &one, x, &one);
  F77_CALL(dpotrf)("U", &dim, x, &dim, &info FCONE);
  F77_CALL(dpotri)("U", &dim, x, &dim, &info FCONE);
  F77_CALL(dsymv)("U", &dim, &alpha, x, &dim, mu, &one, &beta, y, &one FCONE);
  alpha = -0.5;
  F77_CALL(dscal)(&dimdim, &alpha, x, &one);
#endif
  int i,j;
  for ( i = 0; i < dim; i++)
    for ( j = i+1; j < dim; j++)
      x[j+i*dim] = x[i+j*dim];
  return;
  /*
  gsl_matrix *minus_two_sigma=gsl_matrix_alloc(dim,dim);
  gsl_matrix *x_gsl=gsl_matrix_alloc(dim,dim);
  //  double y[dim];
  
  int i,j;
  for(i=0; i<dim; i++)
    for(j=0; j<dim; j++)
      gsl_matrix_set(minus_two_sigma, i, j, -2*sigma[i][j]);

  inverse_matrix(minus_two_sigma, x_gsl);

  for(i=0; i<dim; i++){
    y[i] = 0.0;
    for(j=0; j<dim; j++)
      y[i] += gsl_matrix_get(x_gsl, i, j) * mu[j];
    y[i] *= -2.0;
  }  

  for(i=0; i<dim; i++){
    for(j=0; j<dim; j++)
      x[i+j*dim] = gsl_matrix_get(x_gsl, i, j);
    //mu[i] = y[i];
  }
    
  gsl_matrix_free(minus_two_sigma);
  gsl_matrix_free(x_gsl);
  */
}

#ifdef _VERBOSE
static void 
print_vector(FILE *fp, double *v, int length, char *str)
{
  int i;
  fprintf(fp, "%s\t", str);
  for(i=0; i<length; i++)
    fprintf(fp, "%15.10f ", v[i]);
  fprintf(fp, "\n");
}

static void 
print_matrix2(FILE *fp, int size, double *matrix, const char *str)
{
  int i, j;

  fprintf(fp,"%s\n", str);
  for (i=0; i<size; i++){
    for(j=0; j<size; j++)
      fprintf(fp,"%15.10lf ",matrix[i+j*size]);
    fprintf(fp,"\n");
  }
}    
#endif

static int 
check_sigma(double *sigma)
{
  /* check the symmetricity of sigma */
  int is_symmetric = 1;
  int i,j;
  for(i=0; i<dim; i++)
    for(j=i+1; j<dim; j++)
      if(sigma[i+j*dim]!=sigma[j+i*dim]){
	sigma[i+j*dim]=sigma[j+i*dim]=(sigma[i+j*dim]+sigma[j+i*dim]) * 0.5;
	is_symmetric = 0;
      }
  if(!is_symmetric)
    oxprintfe( "Warning:sigma is not symmetric.\n");

  /* check the positive definiteness of sigma */
  /*
  int is_positive_definite = 1;
  gsl_matrix *gsl_sigma = array2mat(dim, dim, sigma);
  gsl_vector *e_values = gsl_vector_alloc(dim);
  gsl_matrix *P = gsl_matrix_alloc(dim, dim);
  gsl_eigen_symmv_workspace *w = gsl_eigen_symmv_alloc(dim);

  gsl_eigen_symmv(gsl_sigma, e_values, P, w);

#ifdef _VERBOSE
  print_gsl_vector(verbose_out, e_values, "eigen values of sigma:");
#endif

  double t;
  det_sigma=1.0;
  for(i = 0; i<dim; i++){
    t= gsl_vector_get(e_values, i);
    det_sigma *= t;
    if( t < 0.0)
      is_positive_definite = 0;
  }
  gsl_matrix_free(gsl_sigma);
  gsl_vector_free(e_values);
  gsl_matrix_free(P);
  gsl_eigen_symmv_free(w);

  if(!is_positive_definite){
    oxprintfe("Error: Sigma is not positive definite.\n");
    return -1;
  }
  */
  return 0;
}

static double 
norm(double *y)
{
  double sum = 0.0;
  int i;
  for(i=0; i<dim; i++)
    sum += y[i]*y[i];
  return sqrt(sum);
}

static void 
initialization(double *x, double *y, double *f, double *hp)
{
  int i,j;

  for(i=0; i<dim; i++)
    for(j=0; j<dim; j++){
      X1[j+i*dim] = x[i+j*dim];
      if(i==j)
	X0[j+i*dim] = X1[j+i*dim] ;
      else
	X0[j+i*dim] = 0.0;
      delx[j+i*dim] = -X0[j+i*dim] +X1[j+i*dim];
    }
  for(i=0; i<dim; i++){
    Y1[i] = y[i];
    dY[i] = Y1[i];
  }

  /* set the initial value */
  int I;
  for(I=0; I<rank; I++){
    f[I] = 1.0;
    for(i=0; i<dim; i++)
      if(I&1<<i)
	f[I] *= M_PI/(-4*X0[i+i*dim]);
    f[I] = sqrt(f[I]);
  }
}

static void 
get_prob(double *f)
{
  int i, I, deg_I;
  double power_2pi[dim+1];

  power_2pi[0] = 1.0;
  for(i=0; i<dim; i++)
    power_2pi[i+1] = power_2pi[i] * 2*M_PI;

  for(I=1; I<rank; I++){
    deg_I = 0;
    for(i=0; i<dim; i++)
      if(I&1<<i) deg_I++;
    f[I] /= sqrt( power_2pi[deg_I] * cal_det_sigmaI(I, deg_I) );
    f[I] *= exp(-0.5 * cal_sub_quad_form(I));
  }
}

static void
update_XY(const double t)
{
  int i,j;
  const double s = 1-t;
  for(i=0; i<dim; i++){
    for(j=0; j<dim; j++)
      if(i==j)
	X[j+i*dim] = X0[j+i*dim]*s + X1[j+i*dim]*t;
      else
	X[j+i*dim] = X1[j+i*dim]*t;
    Y[i] = t*Y1[i];
  }
}


static void 
cal_sigmaI_muI(const int I)
{
  int i,j, subi, subj;
  /* deg_I <- |I| */
  int deg_I = 0;
  for(i=0; i<dim; i++)
    if(I&1<<i)
      deg_I++;
  /* cal g_sigmaI */
  //gsl_matrix *g_sigmaI = gsl_matrix_alloc(deg_I,deg_I);
  double g_sigmaI[deg_I*deg_I];
  //gsl_matrix *g_submat  = gsl_matrix_alloc(deg_I,deg_I);
  double g_submat[deg_I*deg_I];
  subi = 0;
  for(i=0; i<dim; i++)
    if(I&1<<i){
      subj = 0;
      for(j=0; j<dim; j++)
	if(I&1<<j){
	  //gsl_matrix_set(g_submat, subi, subj, -2.0*X[j+i*dim]);
	  g_submat[subj+subi*deg_I] = -2.0*X[j+i*dim];
	  subj++;
	}
      subi++;
    }
  //inverse_matrix(g_submat, g_sigmaI);
  int info, n, m;
  n = m = deg_I;
#ifdef _STANDALONE
  dpotrf_("U", &n, g_submat, &m, &info);
#else
  F77_CALL(dpotrf)("U", &n, g_submat, &m, &info FCONE);
#endif
  //oxprintfe( "info=%d\n", info);
  n = m = deg_I;
#ifdef _STANDALONE
  dpotri_("U", &n, g_submat, &m, &info);
#else
  F77_CALL(dpotri)("U", &n, g_submat, &m, &info FCONE);
#endif
  //oxprintfe( "info=%d\n", info);
  for ( i = 0; i < deg_I; i++)
    for ( j = i; j < deg_I; j++)
      g_sigmaI[j+i*deg_I] = g_sigmaI[i+j*deg_I] = g_submat[i+j*deg_I];

  /* sigmaI <- g_sigmaI */
  subi = 0;
  for(i=0; i<dim; i++)
    if(I&1<<i){
      subj = 0;
      for(j=0; j<dim; j++)
	if(I&1<<j){
	  //sigmaI[j+i*dim] = gsl_matrix_get(g_sigmaI, subi, subj);
	  sigmaI[j+i*dim] = g_sigmaI[subj+subi*deg_I];
	  subj++;
	}else if(i==j){
	  sigmaI[i+j*dim] = 1.0;
	}else{
	  sigmaI[i+j*dim] = 0.0;
	}
      subi++;
    }else{
      for(j=0; j<dim; j++)
	if(i==j)
	  sigmaI[i+j*dim] = 1.0;
	else
	  sigmaI[i+j*dim] = 0.0;
    }
  /* muI <- sigmaI * yI */
  for(i=0; i<dim; i++){
    muI[i] = 0.0;
    if(I&1<<i)
      for(j=0; j<dim; j++)
	if(I&1<<j)
	  muI[i] += sigmaI[j+i*dim] * Y[j];
  }
  /* free */
  // gsl_matrix_free(g_sigmaI);
  //gsl_matrix_free(g_submat);
}

static double 
cal_det_sigmaI(const int I, const int deg_I)
{
  int i,j, subi, subj;
  /* cal g_sigmaI */
  //gsl_matrix *g_submat  = gsl_matrix_alloc(deg_I,deg_I);
  double g_submat[deg_I*deg_I];
  subi = 0;
  for(i=0; i<dim; i++)
    if(I&1<<i){
      subj = 0;
      for(j=0; j<dim; j++)
	if(I&1<<j){
	  //gsl_matrix_set(g_submat, subi, subj, -2*X[j+i*dim]);
	  g_submat[subj+subi*deg_I] = -2*X[j+i*dim];
	  subj++;
	}
      subi++;
    }
  double det = 1.0;
  int info, n, m;
  n = m = deg_I;
  //oxprintfe( "cal_det_sigmaI: %d %d ", I, deg_I);
#ifdef _STANDALONE
  dpotrf_("U", &n, g_submat, &m, &info);
#else
  F77_CALL(dpotrf)("U", &n, g_submat, &m, &info FCONE);
#endif
  //oxprintfe( "info=%d\n", info);
  for ( i = 0; i < deg_I; i++)
    det *= g_submat[i+i*deg_I];
  det *= det;

  double result = 1.0 / det;
  //gsl_matrix_free(g_submat);
  return result;
}

static double 
cal_sub_quad_form(int I)
{
  int i,j;
  double s=0.0;
  cal_sigmaI_muI(I);
  for(i=0; i<dim; i++)
    if(I&1<<i)
      for(j=0; j<dim; j++)
	if(I&1<<j)
	  s += Y1[i] * sigmaI[i+j*dim] * Y1[j];
  return s;
}


static void
cal_del_VI_y(const int I, const double *g)
{
  int i,j;
  double *p = delVI + I*dim;
  for(i=0; i<dim; i++){
    *p = 0.0;
    if(I&1<<i){
      *p += muI[i] * g[I];
      for(j=0; j<dim; j++)
	if(I&1<<j)
	  *p += sigmaI[j+i*dim] * g[I&~(1<<j)];
    }
    p++;
  }
}

static double
get_delVI(const int p, const int q,const int I, const double *g)
{
  double result;
  int i;

  result = sigmaI[q+p*dim] * g[I];
  result += muI[p] * delVI[q+I*dim];
  for(i=0; i<dim; i++)
    if((I&1<<i) && i!=q)
      result += sigmaI[i+p*dim] * delVI[q+ (I&~(1<<i))*dim];
  if(p!=q)
    result *= 2.0;
  return result;
}

static int 
function(double t, const double g[], double dg[], void *params)
{
  update_XY(t);

  int i,j, I;
  dg[0] = 0.0;
  for(I=1; I<rank; I++){
    dg[I] = 0.0;
    cal_sigmaI_muI(I);
    cal_del_VI_y(I, g);
    for(i=0; i<dim; i++){
      if(I&1<<i){
	dg[I] += delVI[i+I*dim] * dY[i];
	for(j=i+1; j<dim; j++) /* X0[i][i] == X1[i][i] */
	  if(I&1<<j)
	    dg[I] += get_delVI(i,j,I,g) * delx[j+i*dim];
      }
    }
  }
  return GSL_SUCCESS;
}

static void 
move(double g[], double h)
{
  const gsl_odeiv_step_type *T = gsl_odeiv_step_rkf45;
  gsl_odeiv_step * s  = gsl_odeiv_step_alloc (T, rank);
  gsl_odeiv_control * c 
    = gsl_odeiv_control_y_new (h, 0.0);
  gsl_odeiv_evolve * e  = gsl_odeiv_evolve_alloc (rank);

  gsl_odeiv_system sys = {function, NULL, rank, NULL};
  double t = 0.0;

  while (t < 1.0){
    int status = gsl_odeiv_evolve_apply (e, c, s, &sys,
					 &t, 1.0, &h, g);
    if (status != GSL_SUCCESS)
      break;
#ifdef _DEBUG
    oxprintfe( "t=%f\n", t);
    print_vector(stderr, g, rank, "g:");
#endif
    
  }
  /* free */
  gsl_odeiv_evolve_free (e);
  gsl_odeiv_control_free (c);
  gsl_odeiv_step_free (s);
}


#ifdef _STANDALONE
#include<stdio.h>
#include<stdlib.h>
#include<getopt.h> 
#include<string.h>

static int input_data(FILE *fp, int *dim, double *data);
static void gen_data_tri_diag(void);
static void gen_data_same_non_diag(void);
static void gen_data_random(int dim);
//extern double orthant(int *dim, double *x, double *y);
//extern void set_verbose_out(FILE *);

#define frand() ((double) rand()/((double)RAND_MAX+1))
#define MAX_DIM  32
#define MAX_DATA 1056 /*  32*32 + 32  */

#define CASE_GEN_DATA_TRI_DIAG 1
#define CASE_GEN_DATA_SAME_NON_DIAG 2
#define CASE_GEN_DATA_RANDOM 3
#define S_LOG_FILE 100
int main(int argc, char *argv[])
{
  FILE *ifp = stdin, *ofp=stdout;
  int dim;
  char log_file[S_LOG_FILE] = "log";

  int c;
  while (1) {
    int option_index = 0;
    static struct option long_options[] = {
      {"file", 1, 0, 'f'},
      {"gen_data_tri_diag", 0, 0, 0},
      {"gen_data_same_non_diag", 0, 0, 0},
      {"gen_data_random", 1, 0, 0},
      {0,0,0,0}                 
    };

    c = getopt_long(argc, argv, "f:",
		    long_options, &option_index);
  
    if (c == -1)
      break;
    
    switch (c) {
    case 0: /* long option */
      switch(option_index){
      case CASE_GEN_DATA_TRI_DIAG:
	gen_data_tri_diag();
	return 0;
	break;
      case CASE_GEN_DATA_SAME_NON_DIAG:
	gen_data_same_non_diag();
	return 0;
	break;
      case CASE_GEN_DATA_RANDOM:
	sscanf(optarg, "%d", &dim);
	gen_data_random(dim);
	return 0;
	break;
      default:
	oxprintfe("Error:unknown option\n");
	return -1;
	break;
      }
      break;
    case 'f':
      ifp = fopen(optarg, "r");
      if(ifp == NULL){
	oxprintfe( "Error: can not open %s.\n", optarg);
	oxexit(EXIT_FAILURE);
      }
      snprintf(log_file,S_LOG_FILE, "%s.log", optarg);
      ofp = fopen(log_file, "w");
      if(ofp == NULL){
	oxprintfe( "Error: can not open %s.\n", log_file);
	oxexit(EXIT_FAILURE);
      }
      break;
    default:
      oxprintf("?? getopt returned character code 0%o ??\n", c);
    }
  }

#ifdef _VERBOSE
  set_verbose_out(ofp);
#endif

  double data[MAX_DATA];
  input_data(ifp, &dim, data);

  double *x = data;
  double *y = data + dim*dim;

  double result;
  hgm_ko_orthant(&dim, x, y, &result);
  oxprintf("probability=%10.9f\n", result);
 
  if(ifp!=stdin)
      fclose(ifp);
  if(ofp!=stdout)
      fclose(ofp);

  return 0;
}

int 
input_data(FILE *fp, int *dim, double *data)
{
  fscanf(fp, "%d", dim);
  int i;
  double length = *dim * (*dim+1);
  double *p = data;
  for(i=0; i< length; i++)
    fscanf(fp, "%lf", p++); 
  return 0;
}

static void 
gen_data_tri_diag(void)
{
  int dim;
  double rho;
  oxprintfe( "dim=");
  scanf("%d", &dim);
  oxprintfe( "rho=");
  scanf("%lf",&rho);

  int i,j;
  oxprintf("%d\n\n",dim);
  for(i=0; i<dim; i++){
    for(j=0; j<dim; j++)
      if(i==j)
	oxprintf("%f ",1.0);
      else if(i-j==1 || i-j==-1)
	oxprintf("%f ",rho);
      else
	oxprintf("%f ",0.0);
    oxprintf("\n");
  }
  oxprintf("\n");
  for(i=0; i<dim; i++)
    	oxprintf("%f ",0.0);
  oxprintf("\n");
}

static void 
gen_data_same_non_diag(void)
{
  int dim;
  double rho;
  oxprintfe( "dim=");
  scanf("%d", &dim);
  oxprintfe( "rho=");
  scanf("%lf",&rho);

  int i,j;
  oxprintf("%d\n\n",dim);
  for(i=0; i<dim; i++){
    for(j=0; j<dim; j++)
      if(i==j)
	oxprintf("%f ",1.0);
      else
	oxprintf("%f ",rho);
    oxprintf("\n");
  }
  oxprintf("\n");
  for(i=0; i<dim; i++)
    	oxprintf("%f ",0.0);
  oxprintf("\n");
}

static void 
gen_data_random(int dim)
{
  int i,j,k;
  double x[dim][dim], sigma[dim][dim],s;
  for(i=0; i<dim; i++)
    for(j=0; j<dim; j++)
      if(i<j)
	x[i][j] = 0.0;
      else
	x[i][j] = frand();

  for(i=0; i<dim; i++)
    for(j=i; j<dim; j++){
      s = 0.0;
      for(k=0; k<dim; k++)
	s += x[i][k]*x[j][k];
      sigma[i][j] = sigma[j][i] = s;
    }

  oxprintf("%d\n\n",dim);
  for(i=0; i<dim; i++){
    for(j=0; j<dim; j++)
	oxprintf("%f ",sigma[i][j]);
    oxprintf("\n");
  }
  oxprintf("\n");
  for(i=0; i<dim; i++)
    oxprintf("%f ",frand());
  oxprintf("\n");
}

#endif
