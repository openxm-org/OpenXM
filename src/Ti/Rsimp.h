/*
** Rsimp.h                                 Birk Huber, 2/99 
** -- header file for Rsimp.c with  definitions of linear programming 
** -- data structure and basic implementation of revised simplex method.
**  
**
** TiGERS,  Toric Groebner Basis Enumeration by Reverse Search 
** copyright (c) 1999  Birk Huber
**
*/
#ifndef RSIMP_H
extern int LP_MAX_N;
extern int LP_MAX_M;
extern int LP_N;
extern int LP_M;
extern double **LP_A;
extern double *LP_B;
extern double *LP_C;
extern double *LP_X;
extern int *LP_Basis;
extern int *LP_NonBasis;
extern double **LP_Q;
extern double **LP_R;
extern double *LP_t1; 
extern double *LP_t2;
#endif

#define LP_A(i,j) LP_A[j][i]
#define LP_OPT 0
#define LP_UNBD 1
#define LP_FAIL -1

void LP_get_space(int M, int N);
void LP_free_space();
void LP_print();
int Rsimp(int m, int n, double **A, double *b, double *c,
      double *x, int *basis, int *nonbasis,
      double **R, double **Q, double *t1, double *t2);






