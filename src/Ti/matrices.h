/*
**  matrices.h                                 4/99 
**                   
**  Definition and Minipulation of integer and double matrices and vectors.
**  vectors are simply C-vectors (with indices starting at zero) and matrices
**  are stored Numerical Recepies style -- as a vector of pointers to the
**  rows of the matrix [which are themselves just regular C vectors].
**
** TiGERS,  Toric Groebner Basis Enumeration by Reverse Search 
** copyright (c) 1999  Birk Huber
*/
#define IMref(M,i,j) ((M)[i][j])


int **imatrix_read(FILE *is,int *m, int *n);
void print_imatrix(FILE *of, char *prefix,int **M, int m, int n);
int **new_imatrix(int r, int c);
void free_imatrix(int **M);
int *new_ivector(int c);
void free_ivector(int *V);
double **new_matrix(int r, int c);
void free_matrix(double **M);
double *new_vector(int c);
void free_vector(double *V);




