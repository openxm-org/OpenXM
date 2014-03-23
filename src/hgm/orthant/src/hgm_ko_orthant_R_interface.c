#include <R.h>
#include <Rinternals.h>

extern double hgm_ko_orthant(int dimension, double *sigma, double *mu, double rk_step_size);

void hgm_ko_orthant_R(int *dim, double *x, double *y, double *rk_step_size, double *result)
{
  *result = hgm_ko_orthant(*dim, x, y, *rk_step_size);
  return;
}

