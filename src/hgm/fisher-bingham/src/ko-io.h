#ifndef _KO_IO_H_
#define _KO_IO_H_

#ifndef __GSL_MATRIX_H__
#include <gsl/gsl_matrix.h>
#endif

extern int input_from_file(char *fname, double ***Xp, double **Yp);
extern int input_from_command(const int argc, char *argv[],const int optind, double ***X, double **Y);
extern void free_xy(double **X, double *Y);
extern void print_vec(int n, int length, ...);
extern void print_gsl_vector(gsl_vector *v, char *str);
extern void print_gsl_matrix(gsl_matrix *m, char *str);

extern double Two; 
extern int flag_diag;

#endif
