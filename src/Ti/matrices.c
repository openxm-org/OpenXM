/*
**  matrices.c                                 4/99 
**                   
**  Definition and Minipulation of integer and double matrices and vectors.
**  vectors are simply C-vectors (with indices starting at zero) and matrices
**  are stored Numerical Recepies style -- as a vector of pointers to the
**  rows of the matrix [which are themselves just regular C vectors].
**
** TiGERS,  Toric Groebner Basis Enumeration by Reverse Search 
** copyright (c) 1999  Birk Huber
**
*/
#include<stdio.h>
#include<stdlib.h>
#include "utils.h"
#include "matrices.h"

/*
** Integer Matrices:
**   new_imatrix(int r, int c) -- reserve storage for a rxc matrix of integers
**   free_imatrix(*M)          -- free storage allocated with new_imatrix()
**
**   integer matrix is given by the integers r,c and a vector M of rows
** 
**  example  r=4, c=3                             
**           _____________________________
**      M=  | row0 | row1 | row 2| row 3 |        pointers to rows
**           ---|--------|-------|---- --|
**              |        |        \       \
**              _____________________________________
**             |e0|e1|e2|e3|e4|e5|e6|e7|e8|e9|e10|e11|  vector of elements
**              -------------------------------------
**      
*/
int **new_imatrix(int r, int c){
    int **tmp=0;
    int i,m=r,n=c;
    /* get space for pointers to rows*/
    if ((tmp=(int **)malloc(m*sizeof(int *)))==0){ 
      fprintf(stderr,"memory failure 1 in new_imatrix()\n");
      abort();
    }
    /* get space for all entrees -- store in pointer to first row*/ 
    if ((tmp[0]=(int *)malloc(n*m*sizeof(int)))==0){
        fprintf(stderr,"memory failure 2 in new_imatrix()\n");
        free((void *)tmp);
        abort();
    }
    /* set remaining row pointers into entry space*/
    for(i=1;i<m;i++) tmp[i]=tmp[i-1]+n;
    return tmp;
    }


void free_imatrix(int **M){
  if (M!=0){
    if (M[0]!=0) free((void *)M[0]);
    free((void *)M);
  }
}

/*
** Integer Matrices: IO routines
**
** imatrix_read(FILE *is, int *m, int *n) 
**   -- read in description of imatrix from is, create matrix and fill it.
**   -- format: { m n : entry_1 .... entry_mn } where m=number of rows
**                                                    n=number of columns
**  e.g. 
**     { 2 4 : 1 0 3 5 0 1 8 9 } describes the matrix [ 1, 0, 3, 5]
**                                                    [ 0, 1, 8, 9] 
** print_imatrix(FILE *of, char *prefix,int **M, int m, int n)
**   -- copy prefex string to output file of, then write ascii representation 
**      of matrix with m-rows, n-cols and entrees in M to of.
**
*/
int **imatrix_read(FILE *is,int *m, int *n){
  char c;
  int i,j;
  int **M;

  /* find openning brace */  
  eatwhite(is); 
  c=getc(is);
  if (c!='{'){ 
    fprintf(stderr,"ERROR: expecting '{' in imatrix_read()\n");
    return 0;
  } 
  /* read in matrix dimensions and initialize matrix */
  fscanf(is," %d %d :",m,n);
  M=new_imatrix(*m,*n);

  /* read in matrix entrees */
  for(i=0;i<*m;i++){
    for(j=0;j<*n;j++){
      fscanf(is," %d",&(IMref(M,i,j)));
    }
  }

  /* find closing brace */
  eatwhite(is);
  c=getc(is);
  if (c!='}'){ 
    fprintf(stderr,"ERROR: expecting '}' in imatrix_read()\n");
    return 0;
  } 

return M;
}

void print_imatrix(FILE *of, char *prefix,int **M, int m, int n){
  int i,j;
  fprintf(of,"{ %d %d:\n",m,n);
  for(i=0;i<m;i++){
   fprintf(of,"%s %d",prefix,IMref(M,i,0));
   for(j=1;j<n;j++) fprintf(of,", %d",IMref(M,i,j));
   fprintf(of,"\n");
  }
  fprintf(of,"%s}\n",prefix);
}

/*
** Integer Vectors:
** new_ivector(int n)    -- reserve storage for a c-vector of n integers
** free_ivector(int *M)  -- free storage allocated by new_ivector()
**
*/
int  *new_ivector(int n){
       int *tmp=0;
       tmp=(int *)malloc(n*sizeof(int));
       if (tmp==0){ fprintf(stderr,"memory failure in new_ivector()\n");
                  abort();
       }
       return tmp;
}

void free_ivector(int *M){ if (M!=0)free((void *)M);}


/*
** Double Matrices
**   new_matrix(int r, int c) -- reserve storage for a rxc matrix of doubles
**   free_matrix(*M)          -- free storage allocated with new_matrix()
**
**   (aside from the data types of the entries -- double matrices are exactly
**    as described above for integer matrices)
**
*/
double **new_matrix(int r, int c){
    double **tmp=0;
    int i,m=c,n=r;
    if ((tmp=(double **)malloc(m*sizeof(double *)))==0){
      fprintf(stderr,"memory failure 1 in new_matrix()\n");
      abort();
    }
    if ((tmp[0]=(double *)malloc(n*m*sizeof(double)))==0){
      fprintf(stderr,"memory failure 2 in new_matrix()\n");
      free((void *)tmp);
      abort();
    }
    for(i=1;i<m;i++) tmp[i]=tmp[i-1]+n;
    return tmp;
}

void free_matrix(double **M){
    if (M!=0){
      if (M[0]!=0) free((void *)M[0]);
      free((void *)M);
    }
}

/*
** Double Vectors: 
** new_vector(int n)    -- reserve storage for a c-vector of n doubles
** free_vector(int *M)  -- free storage allocated by new_vector()
**
*/
double *new_vector(int n){
       double *tmp=0;
       tmp=(double *)malloc(n*sizeof(double));
       if (tmp==0){ fprintf(stderr,"memory failure in new_vector\n");
                  abort();
       }
       return tmp;
}

void free_vector(double *M){ if (M!=0)free((void *)M);}







