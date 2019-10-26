/* $OpenXM: OpenXM/src/ox_gsl/call_gsl_eigen.c,v 1.1 2019/10/23 07:00:43 takayama Exp $ */
#include <stdio.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_eigen.h>
#include "ox_gsl.h"
extern int Debug;
cmo *gsl_complex_to_cmo_array(gsl_complex z) {
  cmo **cr;
  cr = (cmo **) GC_malloc(sizeof(cmo *)*2);
  cr[0] = (cmo *) new_cmo_double(GSL_REAL(z));
  cr[1] = (cmo *) new_cmo_double(GSL_IMAG(z));
  return (cmo *)new_cmo_list_array((void *)cr,2);
}

cmo *gsl_vector_complex_to_cmo_array(gsl_vector_complex * v,int n) {
  cmo **cr;
  int i;
  if (n <= 0) return NULL;
  cr = (cmo **) GC_malloc(sizeof(cmo *)*n);
  for (i=0; i<n; i++) {
    cr[i] = gsl_complex_to_cmo_array(gsl_vector_complex_get(v,i));
  }
  return (cmo *)new_cmo_list_array((void *)cr,n);
}

cmo *gsl_matrix_complex_to_cmo_array_transposed(gsl_matrix_complex *m,int row_m, int col_n) {
/* it returns the transpose of m */
  cmo **cr;
  int i;
  gsl_vector_complex_view evec_i;
  if (col_n <= 0) return NULL;
  cr = (cmo **) GC_malloc(sizeof(cmo *)*col_n);
  for (i=0; i<col_n; i++) {
    evec_i=gsl_matrix_complex_column(m,i);
    cr[i] = gsl_vector_complex_to_cmo_array(&evec_i.vector,row_m);
  }
  return (cmo *)new_cmo_list_array((void *)cr,col_n);
}

/* copied from GSL manual modified.
*/
void call_gsl_eigen_nonsymmv() {
  int argc;
  int nn,n0,n,ii,len;
  cmo *mat;
  double *data;

  gsl_matrix_view m;
  gsl_vector_complex *eval;
  gsl_matrix_complex *evec;

  gsl_eigen_nonsymmv_workspace * w;

  gsl_set_error_handler((gsl_error_handler_t *)myhandler);

  argc = get_i();
  if (argc != 1) {
    push(make_error2("call_gsl_eigen_nonsymmv accepts one arg.",NULL,0,-1));
    return ;
  }
  mat = pop();
  nn = get_length(mat);
  n0 = (int) floor(sqrt(nn));
  if (n0 <= 0) {
    push(make_error2("call_gsl_eigen_nonsymmv: give a non-empty matrix.",NULL,0,-1));
    return ;
  }
  for (n=n0-1; n<=n0+1; n++) {
    if (n*n-nn == 0) break;
  }
  if (n > n0+1) {
    push(make_error2("call_gsl_eigen_nonsymmv: matrix size must be n^2",NULL,0,-1));
    return ;
  }

  data = cmo2double_list(&len, mat);
  for (ii=0; ii<len; ii++) printf("%lg, ",data[ii]);

  m
    = gsl_matrix_view_array (data, n, n);

  eval = gsl_vector_complex_alloc (n);
  evec = gsl_matrix_complex_alloc (n, n);

  w =
    gsl_eigen_nonsymmv_alloc (n);

  gsl_eigen_nonsymmv (&m.matrix, eval, evec, w);

  gsl_eigen_nonsymmv_free (w);

  gsl_eigen_nonsymmv_sort (eval, evec,
                           GSL_EIGEN_SORT_ABS_DESC);

  /* show result on the server for debug */
  {
    int i, j;

    for (i = 0; i < n; i++)
      {
        gsl_complex eval_i
           = gsl_vector_complex_get (eval, i);
        gsl_vector_complex_view evec_i
           = gsl_matrix_complex_column (evec, i);

        printf ("eigenvalue = %g + %gi\n",
                GSL_REAL(eval_i), GSL_IMAG(eval_i));
        printf ("eigenvector = \n");
        for (j = 0; j < n; ++j)
          {
            gsl_complex z =
              gsl_vector_complex_get(&evec_i.vector, j);
            printf("%g + %gi\n", GSL_REAL(z), GSL_IMAG(z));
          }
      }
  }

  push(gsl_vector_complex_to_cmo_array(eval,n));
  push(gsl_matrix_complex_to_cmo_array_transposed(evec,n,n));

  gsl_vector_complex_free(eval);
  gsl_matrix_complex_free(evec);

  return;
}
