/*********************************************
file name: io.c
Description: Functions for input and output 
             the data. 
**********************************************/
#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>

double Two = 0.5;
int flag_diag = 0;

/*
int input_from_file(char *fname, double ***Xp, double **Yp)
 Reading the values of dim,X, and Y from a file.
*/
int 
input_from_file(char *fname, double ***Xp, double **Yp)
{
  FILE *fp;
  int dim, M, i, j;
  double **X, *Xy, *Y;

  fp = fopen(fname,"r");
  if (fp == NULL) {
    fprintf(stderr,"%s:File open error.\n", fname); 
    return(-1);
  }

  fscanf(fp,"%d", &dim);
#ifdef _SPHERE
  M = dim+1;
#else
  M = dim;
#endif
  if (M>0){
    X = (double **)malloc(sizeof(double *) * M); /* memory allocation */
    Xy = (double *)malloc(sizeof(double) * M*M); 
    Y  = (double *)malloc(sizeof(double) * M);
    for (i=0; i<M; i++) { /* initializing  */
      X[i] = Xy + i*M;
      Y[i] = 0.0;
      for (j=0; j<M; j++){
	X[i][j] = 0.0;
      }
    }
  }else{
    return -1;
  }

  if(flag_diag){
    for(i=0; i<M; i++)
	fscanf(fp, "%lf", &(X[i][i]));
    for(i=0; i<M; i++) /* X <- (X + X^T)/2 */
      for(j=i+1; j<M; j++)
	X[i][j] = X[j][i] = 0.0;
  }else{
    for(i=0; i<M; i++)
      for(j=0; j<M; j++)
	fscanf(fp, "%lf", &(X[i][j]));
    for(i=0; i<M; i++) /* X <- (X + X^T)/2 */
      for(j=i+1; j<M; j++)
	X[i][j] = X[j][i] = (X[i][j]+X[j][i])/2.0 * Two;
  }
  for(i=0; i<M; i++)
    fscanf(fp, "%lf", &(Y[i]));

  fclose(fp); 

  *Xp = X, *Yp = Y;
  return dim;
}


/*
Calclate the dimension.
This function is called by input_from_command only.
*/
static int caldim(int nv)
{
  int s=0,n=2;
  if(flag_diag){
    if(nv > 0 && nv%2 == 0 )
      return (nv/2);
  }else{
    while (s<nv)
      if (nv==(s+=n++)) return(n-2);
  }
  return(-1);
}

/*
int input_from_command(const int argc,char *argv[],const int optind, double ***Xp, double **Yp)
 Reading the values of X and Y from 
 the command-line arguments. 
 This function have to be written after
 "getopt loop".
 The variable "getind" is definde the header file
 "getopt.h".
*/
int 
input_from_command(const int argc,char *argv[],const int optind, double ***Xp, double **Yp)
{
  double **X, *Xy, *Y;
  int M = caldim(argc - optind);
  int dim;
  int i,j,k;

  if (M>0){
    dim=M-1;
    X = (double **)malloc(sizeof(double *) * M); /* memory allocation */
    Xy = (double *)malloc(sizeof(double) * M*M); 
    Y  = (double *)malloc(sizeof(double) * M);
    for (i=0; i<M; i++) { /* initializing  */
      X[i] = Xy + i*M;
      Y[i] = 0.0;
      for (j=0; j<M; j++){
	X[i][j] = 0.0;
      }
    }
  }else{
    return -1;
  }

  k = optind;
  if(flag_diag){
    for (i=0; i<M; i++){  /* reading X[][] from argv[] */
      sscanf(argv[k++],"%lf",&(X[i][i]));
      if (k>= argc) break;
    }
    for (i=0; i<M; i++)
      for(j=i+1; j<M; j++)
	X[i][j] = X[j][i] = 0.0;
  }else{
    for (i=0; i<M; i++) { /* reading X[][] from argv[] */
      for (j=i; j<M; j++) {
	sscanf(argv[k],"%lf",&(X[i][j]));
	if (j>i) X[i][j] *= Two;
	X[j][i] = X[i][j];
	k++;
	if (k >= argc) break;
      }
      if (k>= argc) break;
    }
  }
  for (i=0; i<M; i++) { /* reading Y[] from argv[]*/
    sscanf(argv[k],"%lf",&(Y[i]));
    k++;
    if (k >= argc) break;
  }
  *Xp = X; *Yp = Y; 
  return dim;
}

/*
void free_xy(double **X, double Y)
Description:
 Free the memory allocated by the function
 input_from_file or input_from_command.
*/
void 
free_xy(double **X, double *Y)
{
  free(*X); free(X);
  free(Y);
}

/*
void print_vec(int n, int length, char *name, double v[], ...);
Description:
 This function is for printing vectors which 
 has the same length.
 The variable "n" means the number of vectors 
 and "length" means the length of vectors.
example:
 print_vec(4,10,
  "foo:\n", foo
  "bar:\n", bar,
  "baz:\n", baz,
  "qux:\n", qux
 );
*/
void 
print_vec(int n, int length, ...)
{
  va_list ap;
  char *s;
  double *v;
  int i,j;

  va_start(ap, length);
  for(i= 0; i<n; i++){
    s = va_arg(ap, char*);
    printf("%s",s);
    v = va_arg(ap, double*);
    for(j=0; j<length;j++)
      printf("%10lg ", v[j]);
    printf("\n");
  }
  va_end(ap);
}

/*
void print_gsl_vector(gsl_vector *v, char *str)
Description:
 This function is for printing gsl_vector with 
 name.
*/
void 
print_gsl_vector(gsl_vector *v, char *str)
{
  int length = v->size;
  int i,j;

  printf("%s\n\t", str);
  for(i=0; i<length; i++){
    printf("%10.6f ", gsl_vector_get(v, i));
  }
  putchar('\n');
}



/*
void print_gsl_matrix(gsl_matrix *m, char *str)
Description:
 This function is for printing gsl_matrix with 
 name.
*/
void 
print_gsl_matrix(gsl_matrix *m, char *str)
{
  int nrow = m->size1, ncol = m->size2;
  int i,j;

  printf("%s\n", str);
  for(i=0; i<nrow; i++){
    putchar('\t');
    for(j=0; j<ncol; j++){
      printf("%10.6f ", gsl_matrix_get(m, i, j));
    }
    putchar('\n');
  }
}
