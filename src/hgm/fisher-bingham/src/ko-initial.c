/* gcc tkoyama-initial.c -lm -lgsl -lblas  on orange */
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<gsl/gsl_sf_gamma.h>
#include<gsl/gsl_errno.h>
#include<gsl/gsl_matrix.h>
#include<gsl/gsl_odeiv.h>
#include<gsl/gsl_eigen.h>

#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

#define MAXSIZE 10      /* maximum size of the matrix x  */
#define RKERROR 1e-6   /* error of runge-kutta  */
#define NEWINITIAL
#define RANGE 1000
#define NDATA 100
#define S_FNAME 1024

struct list{
  double c;
  struct list *next;
};

struct tkoyama_params
{
  gsl_matrix *pfs0, *pfs1;  
};

#if SAMPLE > 0
double *fbnd( double x[MAXSIZE][MAXSIZE], double y[]);
#else
double *fbnd( double **x, double *y);
#endif

double *fbnd_diag( double x1[], double y1[]);
int rk_func (double t, const double y[], double f[], void *params);
struct tkoyama_params* set_pfs( double x[], double y[]);
void free_pfs(struct tkoyama_params *params);
double *pow_series( double x[], double y[], double r);
struct list *mk_coeff( double r);
void free_coeff(struct list *coeff);
struct list *cons(double c, struct list *coeff);
double pow_series_dyi( double x[], double y[], double r, struct list *coeff, int i);
double pow_series_dyii( double x[], double y[], double r, struct list *coeff, int i);
int set_tdev(double);
void runge_kutta(double *g, double *xdiag0, double *ydiag0,double r);
void dev_max_eigen(double *g, double max_eigen);
void g2fdiag(double *g, double r, double *xdiag0);
double move_near0(double *xdiag, double *ydiag, double *xdiag0, double *ydiag0);


static int dim;  /* dimension of the sphere */
static double eps_pert = 0.00001;
static int maxdeg = 10;
static int *weight;

void 
perturbation(int length, double v[], double eps,  const gsl_rng_type *T, gsl_rng * r)
{
  int i;
  double err[length];
  printf("perturbation:\nerror=");
  for(i=0; i<length; i++){
    err[i]= gsl_ran_gaussian (r, eps);
    v[i] += err[i];
    printf("%g ", err[i]);
  }
  printf("\n");
}

#if SAMPLE == 0
int main(int argc, char *argv[])
{
  double r = 1.0;
  double Two;
  int i,j,k,M;
  FILE *fp;
  char fname[S_FNAME];
  double **X, *Xy, *Y;

  if (argc<2) {usage(); return(-1);}
  for (k=1; k<argc; k++) {
    if (strcmp(argv[k],"--1")==0) {
      Two = 0.5;
    }else if (strcmp(argv[k],"--2")==0) {
      Two = 1.0;
    }else if (strcmp(argv[k],"--help")==0) {
      usage(); return(0);
    }else if (strcmp(argv[k],"--i")==0) {
      sscanf(argv[++k],"%g",&eps_pert);
      printf("eps_pert=%g\n", eps_pert);
      usage(); return(-1);
    }else if (strncmp(argv[k],"--",2)==0) {
      fprintf(stderr,"Unknown option\n");
      usage(); return(-1);
    }else{
      if (k==2){
        M=caldim(argc-2);
        if (M>0){
          dim=M-1;
          printf("dim: %d\n",dim);
          X = (double **)malloc(sizeof(double *) * M); 
          Xy = (double *)malloc(sizeof(double) * M*M); 
          Y  = (double *)malloc(sizeof(double) * M);
          for (i=0; i<M; i++) {
            X[i] = Xy + i*M;
            Y[i] = 0.0;
            for (j=0; j<M; j++){
              X[i][j] = 0.0;
	    }
          }
          weight = (int *)malloc(sizeof(int) * 2*M);
          for (i=0; i<2*M; i++) weight[i]=1;
        }else{
          fprintf(stderr,"argument mismatch!\n"); usage(); return(-1);
        }
      }
      for (i=0; i<M; i++) {
	for (j=i; j<M; j++) {
	  sscanf(argv[k],"%lf",&(X[i][j]));
          if (j>i) X[i][j] *= Two;
	  X[j][i] = X[i][j];
	  k++;
	  if (k >= argc) break;
	}
	if (k>= argc) break;
      }
      for (i=0; i<M; i++) {
	sscanf(argv[k],"%lf",&(Y[i]));
	k++;
	if (k >= argc) break;
      }
    }
  }

  for (i=0; i<M; i++) {
    for (j=0; j<M; j++)
      printf(" %lf",X[i][j]);
    printf(": %lf\n",Y[i]);
  }

  double *f = fbnd(X, Y);

  free(*X); free(X);
  free(Y);
  free(weight);

  snprintf(fname,S_FNAME,"tmp-out-fb%dd.txt",dim);
  fp = fopen(fname,"w");
  if (fp == NULL) {
    fprintf(stderr,"File open error.\n"); return(-1);
  }

  for(i=0; i<2*dim+2; i++){
    printf("f[%d]=%f\n", i, f[i]);
    fprintf(fp,"%lf\n",f[i]);
  }
  fclose(fp);
  return 0;
}

int caldim(int nv)
{
  int s=0,n=2;
  while (s<nv)
    if (nv==(s+=n++)) return(n-2);
  return(-1);
}

usage(void) {
  int i,j;
  int M=3;
  fprintf(stderr,"usage:\n");
  fprintf(stderr,"./a.out [--help] [--1 | --2] [");
  for (i=1; i<=M; i++){
    for (j=i; j<=M; j++)
      fprintf(stderr,"x%d%d ",i,j);
    fprintf(stderr,"... ");
  }
  fprintf(stderr,"... ");

  for (i=1; i<=M; i++) fprintf(stderr,"y%d ",i);
  fprintf(stderr,"... ]\n");
  fprintf(stderr,"The output data will be written to tmp-out-fbnd.txt\n");
}
#endif

gsl_matrix 
*get_xdiag(double **x, double *y, double *xdiag, double *ydiag)
{
  gsl_eigen_symmv_workspace *w = gsl_eigen_symmv_alloc( dim+1);
  gsl_matrix *A = gsl_matrix_alloc(dim+1, dim+1);
  gsl_vector *eval = gsl_vector_alloc(dim+1);
  gsl_matrix *evec = gsl_matrix_alloc(dim+1, dim+1);
  int i,j;

  /* gsl_matrix A <- double x[][] */
  for(i=0; i<dim+1; i++){
	for(j=0; j<dim+1; j++){
	  gsl_matrix_set(A, i, j, x[i][j]);
	}
  }

  gsl_eigen_symmv(A, eval, evec, w);
  gsl_eigen_symmv_free(w);
  gsl_eigen_symmv_sort(eval,evec, GSL_EIGEN_SORT_VAL_DESC);

  for(i=0; i<dim+1; i++){ xdiag[i] = gsl_vector_get(eval, i);}
  for(i=0; i<dim+1; i++){
	ydiag[i] = 0.0;
	for(j=0; j<dim+1; j++){
	  ydiag[i] += gsl_matrix_get(evec, j, i)*y[j]; /* ydiag = evec' * y */
	}
  }
  gsl_matrix_free(A);  
  gsl_vector_free(eval);
  return evec;
}

void
fdiag2f(double *fdiag, double *xdiag, double *ydiag, gsl_matrix *P)
{
  double f[2*dim+2];
  double h[dim+1][dim+1];
  int i,j,k;

  for(i=0; i<2*dim+2; i++){ f[i] =0.0;}
  for(i=0; i<dim+1; i++){ f[0] += fdiag[i+dim+1];}
  for(i=0; i<dim+1; i++){
	for(j=0; j<dim+1; j++){
	  f[i+1] += gsl_matrix_get(P,i,j)*fdiag[j];
	}
  }
  for(i=0; i<dim+1; i++){
	for(j = i+1; j<dim+1; j++){
	  h[i][j] = -(ydiag[i]*fdiag[j]-ydiag[j]*fdiag[i])/(xdiag[i]-xdiag[j]);
	}
  }
  
  for(i=0; i<dim; i++){
	for(j=0; j<dim+1; j++){
	  f[i+dim+2] += gsl_matrix_get(P,i,j)*gsl_matrix_get(P,i,j)*fdiag[j+dim+1];
	}
	for(j=0; j<dim+1; j++){
	  for(k = j+1; k<dim+1; k++){
	    f[i+dim+2] += gsl_matrix_get(P,i,j)*gsl_matrix_get(P,i,k)*h[j][k];
	  }
	}
  }	
  for(i=0; i<2*dim+2; i++){ fdiag[i] =f[i];}
}

double 
*fbnd( double **x, double *y)
{
  int i;
  double xdiag[dim+1], ydiag[dim+1];
  gsl_matrix *P= get_xdiag(x, y, xdiag, ydiag);

  double xdiag0[dim+1], ydiag0[dim+1];
  double r=move_near0(xdiag, ydiag, xdiag0, ydiag0);
  double *f = pow_series(xdiag0, ydiag0, 1.0);
  double f0[2*dim+2];
  dev_max_eigen(f,xdiag0[0]);
  for(i=0; i<2*dim+2; i++){ f0[i]=f[i];}

  runge_kutta(f, xdiag0, ydiag0, r);
  g2fdiag(f,r,xdiag0);
  fdiag2f(f,xdiag,ydiag,P);
  printf("return of the function fbnd:\n[");
  for(i = 0; i< 2*dim+1; i++){
	printf("\t %g,", f[i]);
  }
  printf("\t %g]\n\n", f[i]);


  /*perturbation*/  
  int ii;
  double g[2*dim+2];
  const gsl_rng_type * T;
  gsl_rng * rr;
  gsl_rng_env_setup();     
  T = gsl_rng_default;
  rr = gsl_rng_alloc (T);
  for(ii=0; ii< NDATA; ii++){
    for(i=0; i<2*dim+2; i++){ g[i] = f0[i]; }
    perturbation(2*dim+2, g, eps_pert, T,rr);
    runge_kutta(g, xdiag0, ydiag0, r);
    g2fdiag(g,r,xdiag0);
    fdiag2f(g,xdiag,ydiag,P);
    printf("f= ");
    for(i=0; i<2*dim+2; i++){
      printf("%f ", g[i]);
    }
    printf("\n");
  }  
  gsl_rng_free (rr);


  gsl_matrix_free(P);
  return f;
}

/*********************************************
         Runge-Kutta
**********************************************/
int 
set_tdev(double r)
{
  double rr = r*r;

  if(rr > RANGE){
    return (int)(rr/RANGE) ;
  } else {
    return 1;
  }
}

void 
runge_kutta(double *g, double *xdiag0, double *ydiag0,double r)
{
  double t,t1, dt;
  double h;
  double rkerror2;
  int i,ii;

  int tdev = set_tdev(r);
  dt = (r*r-1.0)/tdev;

  for(ii=0; ii<tdev; ii++){
    struct tkoyama_params *params = set_pfs(xdiag0, ydiag0);
    const gsl_odeiv_step_type *T = gsl_odeiv_step_rkf45;  
    gsl_odeiv_step * s = gsl_odeiv_step_alloc (T, 2*dim+2);
    gsl_odeiv_control *c;
    if(ii == 0 ){
      c = gsl_odeiv_control_y_new (RKERROR, 0.0); 
    }else{
      rkerror2 = fabs(g[0])/100000;
      c = gsl_odeiv_control_y_new (rkerror2, 0.0); 
    }
    gsl_odeiv_evolve * e = gsl_odeiv_evolve_alloc (2*dim+2);
    gsl_odeiv_system sys = {rk_func, NULL, 2*dim+2, params};
    t = 1.0+ ii*dt;   t1 = 1.0+(ii+1)*dt;
    h = 1e-6;
    /*printf("ii = %d,\t s = %f -> %f \n", ii,t, t1);*/
    int status;
    while (t < t1){
      status = gsl_odeiv_evolve_apply (e, c, s, &sys, &t, t1, &h, g);
      if (status != GSL_SUCCESS) break;
#ifdef DEBUG_INITIAL
      printf("s = %f [", t);
      for(i = 0; i< 2*dim+1; i++)
	printf("%g, ", g[i]);
      printf("%g]\n ", g[i]);
#endif
    }
    gsl_odeiv_evolve_free (e);
    gsl_odeiv_control_free (c);
    gsl_odeiv_step_free (s);
    free_pfs(params);
  }
}

void 
dev_max_eigen(double *g, double max_eigen)
{
  int i;
  double tmp = exp(-max_eigen);
  for(i=0; i<2*dim+2; i++)
	g[i] = g[i]*tmp; 
}

void
g2fdiag(double *g, double r, double *xdiag0)
{
  double tmp;
  int i;
  tmp = exp(r*r*xdiag0[0]);
  for(i=0; i<2*dim+2; i++){g[i] *=  tmp;}

  tmp = pow(r, dim+1);
  for(i=0; i<dim+1; i++){g[i] /= tmp;  }
  tmp *= r;
  for(i=0; i<dim+1; i++){g[i+dim+1] /= tmp;  }
}

double 
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

double*
fbnd_diag( double xdiag[], double ydiag[])
{
  double xdiag0[dim+1], ydiag0[dim+1];
  double r=move_near0(xdiag, ydiag, xdiag0, ydiag0);
  double *g = pow_series(xdiag0, ydiag0, 1.0);
  /*  perturbation(2*dim+2, g, 0.00001);*/
  dev_max_eigen(g,xdiag0[0]);
  runge_kutta(g, xdiag0, ydiag0, r);
  g2fdiag(g,r,xdiag0);
  return g;
}

int
rk_func (double s, const double y[], double f[], void *params)
{
  struct tkoyama_params *prms = (struct tkoyama_params*) params;
  gsl_matrix *pfs0 = prms->pfs0;
  gsl_matrix *pfs1 = prms->pfs1;
  int i, j;

  for (i = 0; i < 2*dim+2; i++) {
    f[i] = 0.0;
    for (j = 0; j < 2*dim+2; j++)
      f[i] += (gsl_matrix_get(pfs0, i, j)+ gsl_matrix_get(pfs1, i, j)/(2*s) )* y[j];
  }
  return GSL_SUCCESS;
}

struct tkoyama_params*
set_pfs( double x[], double y[])
{
  struct tkoyama_params *ans = (struct tkoyama_params *)malloc(sizeof(struct tkoyama_params));
  gsl_matrix *pfs0, *pfs1;
  int i,j;

  pfs0 = gsl_matrix_alloc(2*dim+2, 2*dim+2);
  pfs1 = gsl_matrix_alloc(2*dim+2, 2*dim+2);

  gsl_matrix_set_zero(pfs0);
  gsl_matrix_set_zero(pfs1);

  for(i=1; i<dim+1; i++){
	gsl_matrix_set(pfs0,       i,       i, x[i]-x[0]);
	gsl_matrix_set(pfs0, i+dim+1, i+dim+1, x[i]-x[0]);
  }
  for(i=0; i<dim+1; i++){
	gsl_matrix_set(pfs0, i+dim+1,       i, y[i]/2);
	gsl_matrix_set(pfs1,       i,       i,      1);
	gsl_matrix_set(pfs1, i+dim+1, i+dim+1,      2);
  }
  for(i=0; i<dim+1; i++){
	  for(j=0; j<dim+1; j++){
		gsl_matrix_set(pfs1, i, j+dim+1, y[i]);
		if(i!=j) gsl_matrix_set(pfs1, i+dim+1, j+dim+1, 1);
	  }
  }
  ans->pfs0 = pfs0;
  ans->pfs1 = pfs1;
  return ans;
}

void
free_pfs(struct tkoyama_params *params)
{
  gsl_matrix_free(params->pfs0);
  gsl_matrix_free(params->pfs1);
  free(params);
}

/**********************************************
       Calculating the power series
 *********************************************/
double*
pow_series( double x[], double y[], double r)
{
  double *ans = (double*) malloc((2*dim+2)*sizeof(double));
  double s = 2*pow(M_PI,((dim+1)/2.0)) / gsl_sf_gamma((dim+1)/2.0);
  struct list *coeff = mk_coeff(r);
  int i;

  for(i=0; i<dim+1; i++){
	ans[i] = s*pow(r,dim)*pow_series_dyi(x,y,r,coeff,i);
  }
  for(i=0; i<dim+1; i++){
	ans[i+dim+1] = s*pow(r,dim)*pow_series_dyii(x,y,r, coeff,i);
  }
  free_coeff(coeff);
  return ans;
}


struct list*
mk_coeff( double r)
{
  struct list *coeff = NULL;
  int maxdp = 2*dim+2;
  int max = maxdeg;
  double ans[maxdp+1];
  int s[maxdp+1];
  int idx[maxdp];
  int sum[maxdp];
  int dp=0;
  int i;

  ans[0] = 1;
  s[0] = 0;
  for(i=0; i<maxdp; i++){idx[i] = 0;}
  sum[0] = 0;
  
  while(dp > -1){
	if(sum[dp] < max+1){
	  if(dp == maxdp-1){ 
		coeff = cons( ans[dp], coeff);
	  }else{
		dp++;
		sum[dp] = sum[dp-1];
		s[dp] = s[dp-1];
		ans[dp] = ans[dp-1];
		continue;
	  }
	}else{
	  dp--;
	  if(dp == -1){
		break;
	  }else{
		idx[dp+1] = 0;
	  }
	}
	
	if(dp < dim+1){
	  ans[dp] = ans[dp]*r*r*(2*idx[dp]+2*idx[dp+dim+1]+1)/(dim+1+2*s[dp]);
	} else {
	  ans[dp] = ans[dp]*r*r*(2*idx[dp-dim-1]+2*idx[dp]+1)/(dim+1+2*s[dp]);
	}
	s[dp+1] = 0;
	
	idx[dp]++;
	sum[dp] = sum[dp] + weight[dp];
	s[dp]++;
  }
  
  return coeff;	
}

void 
free_coeff(struct list *coeff)
{
  struct list  *tmp;

  while(coeff != NULL){
	tmp = coeff->next;
	free(coeff);
	coeff = tmp;
  }
}

struct list*
cons(double c, struct list *coeff)
{
  struct list *ans = malloc(sizeof(struct list));

  ans->c = c;
  ans->next = coeff;
  return ans;
}

double 
pow_series_dyi( double x[], double y[], double r, struct list *coeff, int i)
{
  int maxdp = 2*dim+2;
  int max = maxdeg;
  double ans[maxdp+1];
  int s[maxdp+1];
  int idx[maxdp];
  int sum[maxdp];
  int dp=0;
  int ic;

  for(ic=0; ic<maxdp; ic++){idx[ic] = ans[ic] = 0;}

  idx[0] = max / weight[0];
  sum[0] = idx[0]*weight[0];
  while(dp > -1){
	if(idx[dp] > -1){
	  if(dp == maxdp-1){ 
		ans[dp+1] = coeff->c;
		coeff = coeff->next;
	  }else{
		dp++;
		idx[dp] = (max - sum[dp-1]) /  weight[dp];
		sum[dp] = sum[dp-1] + idx[dp]*weight[dp];
		continue;
	  }
	}else {
	  dp--;
	  if(dp == -1){
		break;
	  }
	}
	
	if(dp == dim+1+i){
	  if(idx[dp] > 1){
		ans[dp] = (ans[dp] + ans[dp+1]) * y[dp -(dim +1)]*y[dp -(dim+1)] /(double)( (2*idx[dp]-1)*(2*idx[dp]-2));
	  } else if(idx[dp] == 1){
		ans[dp] = (ans[dp] + ans[dp+1]) * y[dp -(dim +1)];
	  }
	}else {
	  if(idx[dp] > 0){
		if(dp < dim+1){
		  ans[dp] = (ans[dp] + ans[dp+1]) * x[dp]/idx[dp];
		} else {
		  ans[dp] = (ans[dp] + ans[dp+1]) * y[dp -(dim +1)]*y[dp -(dim+1)] /(double)( 2*idx[dp]*(2*idx[dp]-1));
		}
	  }else if(idx[dp] == 0){
		ans[dp] = ans[dp] + ans[dp+1];
	  }
	}
	ans[dp+1] = 0;
	
	idx[dp]--;
	sum[dp] = sum[dp] - weight[dp];
  }
  
  return ans[0];
}

double 
pow_series_dyii( double x[], double y[], double r,  struct list *coeff, int i)
{
  int maxdp = 2*dim+2;
  int max = maxdeg;
  double ans[maxdp+1];
  int s[maxdp+1];
  int idx[maxdp];
  int sum[maxdp];
  int dp=0;
  int ic;

  for(ic=0; ic<maxdp; ic++){idx[ic] = ans[ic] = 0;}

  idx[0] = max / weight[0];
  sum[0] = idx[0]*weight[0];
  while(dp > -1){
	if(idx[dp] > -1){
	  if(dp == maxdp-1){ 
		ans[dp+1] = coeff->c;
		coeff = coeff->next;
	  }else{
		dp++;
		idx[dp] = (max - sum[dp-1])/ weight[dp];
		sum[dp] = sum[dp-1] + idx[dp]*weight[dp];
		continue;
	  }
	}else {
	  dp--;
	  if(dp == -1){
		break;
	  }
	}
	
	if(dp == dim+1+i){
	  if(idx[dp] > 1){
		ans[dp] = (ans[dp] + ans[dp+1]) * y[dp -(dim +1)]*y[dp -(dim+1)] /( (2*idx[dp]-2)*(2*idx[dp]-3));
	  } else if(idx[dp] == 1){
		ans[dp] = ans[dp] + ans[dp+1];
	  }
	}else {
	  if(idx[dp] > 0){
		if(dp < dim+1){
		  ans[dp] = (ans[dp] + ans[dp+1]) * x[dp]/idx[dp];
		} else {
		  ans[dp] = (ans[dp] + ans[dp+1]) * y[dp -(dim +1)]*y[dp -(dim+1)] /( 2*idx[dp]*(2*idx[dp]-1));
		}
	  }else if(idx[dp] == 0){
		ans[dp] = ans[dp] + ans[dp+1];
	  }
	}
	
	ans[dp+1] = 0;

	idx[dp]--;
	sum[dp] = sum[dp] - weight[dp];
  }
  
  return ans[0];
}
