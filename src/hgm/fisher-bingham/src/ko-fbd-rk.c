/*********************************************
         ko-fbd-rk.c
         Runge-Kutta
**********************************************/
#include<stdio.h>
#include<math.h>
#include<gsl/gsl_matrix.h>
#include<gsl/gsl_odeiv.h>

double rk_error=1e-6;   /* error of runge-kutta  */
#define RANGE 1000.0
extern int dim; /* defined in ko-fbd.c */
extern int flag_verbose;

struct pfs
{
  gsl_matrix *pfs0, *pfs1;  
};

static int
rk_func (double s, const double y[], double f[], void *params)
{
  struct pfs *prms = (struct pfs*) params;
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

static struct pfs*
set_pfs(const double x[],const double y[])
{
  struct pfs *ans = (struct pfs *)malloc(sizeof(struct pfs));
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

static void
free_pfs(struct pfs *params)
{
  gsl_matrix_free(params->pfs0);
  gsl_matrix_free(params->pfs1);
  free(params);
}

void 
runge_kutta(double *g, const double *g0, const double *xdiag0, const double *ydiag0,const double r)
{
  double t,t1=1.0, dt;
  double h, rr = r*r;
  int i,ii;

  for(i=0; i<2*dim+2; i++)
    g[i] = g0[i];

  for(ii=0; ii*RANGE < rr; ii++){
    double mean = 0.0;
    for(i=0; i<2*dim*2; i++)
	mean += g[i];
    mean /= (2*dim+2);
    mean = fabs(mean);

    struct pfs *params = set_pfs(xdiag0, ydiag0);
    const gsl_odeiv_step_type *T = gsl_odeiv_step_rkf45;  
    gsl_odeiv_step * s = gsl_odeiv_step_alloc (T, 2*dim+2);
    gsl_odeiv_control *c;
    
    c = gsl_odeiv_control_y_new (mean*rk_error, 0.0); 
    gsl_odeiv_evolve * e = gsl_odeiv_evolve_alloc (2*dim+2);
    gsl_odeiv_system sys = {rk_func, NULL, 2*dim+2, params};
    t = t1;
	t1 = ( (ii+1)*RANGE < rr ) ? (ii+1)*RANGE : rr ;
    h = 1e-6;
    if (flag_verbose>2) printf("ii = %d,\t s = %f -> %f,\t rk_error=%g \n", ii,t, t1, mean*rk_error);
    int status;
    while (t < t1){
      status = gsl_odeiv_evolve_apply (e, c, s, &sys, &t, t1, &h, g);
      if (status != GSL_SUCCESS) break;
#ifdef _DEBUG_INITIAL
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
