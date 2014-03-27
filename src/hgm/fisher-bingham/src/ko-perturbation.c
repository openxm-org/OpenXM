#include <stdio.h>
#include <stdlib.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

static int init=1;
static const gsl_rng_type *T;
static gsl_rng * r;
static int length;
static double eps;

double
*perturbation(const double v[], double err[])
{
  if(init){
    fprintf(stderr, "perturbation: erorr! Forget perturbation_init?\n");
    exit(1);
  }

  int i;
  double *vp = (double *) malloc(sizeof(double) * length);
  for(i=0; i<length; i++){
    err[i]= gsl_ran_gaussian (r, eps);
    vp[i] = v[i]+err[i];
  }
  return vp;
}

void 
perturbation_init(int l,  double e)
{
  if(init){
    gsl_rng_env_setup();     
    T = gsl_rng_default;
    r = gsl_rng_alloc (T);
    init = 0;
  } 
  length = l;
  eps = e;
}

void
perturbation_free(void)
{
  gsl_rng_free (r);
}
