/*
** Rsimp.c                                 Birk Huber, 4/99 
** -- definitions of linear programming data structure and 
** -- basic implementation of revised simplex method.
**  
**
** TiGERS,  Toric Groebner Basis Enumeration by Reverse Search 
** copyright (c) 1999  Birk Huber
**
*/
#include <stdio.h>
#include <math.h>
#define RSIMP_H 1
#include "matrices.h"
#include "Rsimp.h"
#define verbose 0
#define zero_tol RS_zt

int LP_MAX_N=0,LP_MAX_M=0;
int LP_N=0, LP_M=0;
double **LP_A=0;
double *LP_B=0;
double *LP_C=0;
double *LP_X=0;
int *LP_Basis=0;
int *LP_NonBasis=0;
double **LP_Q=0;
double **LP_R=0;
double *LP_t1=0; 
double *LP_t2=0;
double RS_zt=0.0000001;


#define basis0(j) (basis[j])
#define nonbasis0(j) (nonbasis[j])
#define AN0(i,j) (A[nonbasis0(j)][i])
#define AB0(i,j) (A[basis0(j)][i])
#define CB0(i)   (c[basis0(i)])
#define CN0(i)   (c[nonbasis0(i)])
#define XB0(i)   (x[basis0(i)])
#define XN0(i)   (x[nonbasis0(i)])
#define Y0(i)    (t1[i])
#define W0(i)    (t2[i])
#define D0(i)    (t2[i])
#define R0(i,j)  (R[j][i])
#define Q0(i,j)  (Q[i][j])


/*
** void LP_free_space():
**       deallocate space for LP data structures.
**       set all LP globals to 0.
**
*/
void LP_free_space(){
  free_matrix(LP_A);
  free_matrix(LP_Q);
  free_matrix(LP_R);
  free_vector(LP_B);
  free_vector(LP_C);
  free_vector(LP_X);
  free_ivector(LP_Basis);
  free_ivector(LP_NonBasis);
  free_vector(LP_t1);
  free_vector(LP_t2);
  LP_M=LP_N=LP_MAX_M=LP_N=0;
  LP_A=LP_Q=LP_R=0;
  LP_B =LP_C=LP_X=LP_t1=LP_t2=0;
  LP_Basis=LP_NonBasis = 0;
}

/*
** void LP_get_space(int M, int N):
**      allocate space for LP data structures.
**
*/
void LP_get_space(int M, int N){
  if (M>LP_MAX_M || N>LP_MAX_N){
    LP_free_space();
    LP_A=new_matrix(M,N);
    LP_Q=new_matrix(M,M);
    LP_R=new_matrix(M,N);
    LP_B = new_vector(M);
    LP_C = new_vector(N);
    LP_X = new_vector(N);
    LP_Basis=new_ivector(M);
    LP_NonBasis = new_ivector(N-M);
    LP_t1=new_vector(N);
    LP_t2=new_vector(N);
    LP_MAX_M=M;
    LP_MAX_N=N;
  }
  LP_M=M; 
  LP_N=N;
}

/*
** void Print_LP():
**     print LP data structures to stdout.
**
*/
void Print_LP(){
int i,j;
 fprintf(stdout,"LP_C \n ");
 for(i=0;i<LP_N;i++)fprintf(stdout," %f ;", LP_C[i] );
 fprintf(stdout,"\nLP_A  | LP_B\n");
 for(j=0;j<LP_M;j++) {
    for(i=0;i<LP_N;i++) fprintf(stdout," %f ",LP_A(j,i));
    fprintf(stdout,"%f \n",LP_B[j]);
  }
  fprintf(stdout, "LP_X \n ");
  for(i=0;i<LP_N;i++) fprintf(stdout," %f ",LP_X[i]);
  fprintf(stdout, "\nLP_Basis \n ");
  for(i=0;i<LP_M;i++) fprintf(stdout," %d ", LP_Basis[i]);
  fprintf(stdout,"\nLP_LP_NonBasis \n ");
  for(i=0;i<LP_N-LP_M;i++) fprintf(stdout," %d ",LP_NonBasis[i]);
  fprintf(stdout,"\n\n");
}

/*
**
**int Rsimp(m,n,A,b,c,x,basis,nonbasis,R,Q,t1,t2):
**     revised simplex method (Using Bland's rule) 
**     and a qr factorization to solve the linear equations
**
**      Adapted from algorithms presented in 
**             Linear Approximations and Extensions                  
**             (theory and algorithms)
**             Fang & Puthenpura
**             Prentice Hall, Engelwood Cliffs NJ (1993)
**      and 
**            Linear Programming
**            Chvatal 
**            Freeman and Company, New York, 1983
** 
**      (developed first in Octave, many thanks to the author)
** 
**
**  Solve the problem 
**       minimize C'x, 
**       subject to A*x=b,  x>=0
**       for x,c,b n-vectors, and A an m,n matrix with full row rank 
** 
** Assumptions:
**    A mxn matrix with full row rank.
**    b an m matrix. 
**    c an n-vector.
**    x an n-vector holding a basic feasible solution, 
**    basis m-vector holding indices of the basic variables in x
**    nonbasis n-m vector holding the indices not appearing in x.
** 
**  Returns: 
**      LP_FAIL if algorithm doesn't terminate.
**      LP_UNBD if problem is unbounded
**      LP_OPT  if optimum found
**  efects:
**    A,b,c unchanged.
**    x basis, nonbasis, hold info describing last basic feasible 
**                       solution.
**    Q,R hold qrdecomp of last basis matrix.
**    t1,t2 undefined.
**
**
*/ 

int Rsimp(int m, int n, double **A, double *b, double *c,
      double *x, int *basis, int *nonbasis,
      double **R, double **Q, double *t1, double *t2){
 int i,j,k,l,q,qv;
 int max_steps=20;
 double r,a,at;
 int ll;
 void GQR(int,int,double**,double**);
 max_steps=4*n;

 for(k=0; k<=max_steps;k++){
   /* 
   ++ Step 0) load new basis matrix and factor it
   */
   for(i=0;i<m;i++)for(j=0;j<m;j++)R0(i,j)=AB0(i,j);
   GQR(m,m,Q,R);
 
   /* 
   ++ Step 1) solving system  B'*w=c(basis) 
   ++      a) forward solve R'*y=c(basis)   
   */
   for(i=0;i<m;i++){
     Y0(i)=0.0;
     for(j=0;j<i;j++)Y0(i)+=R0(j,i)*Y0(j);
     if (R0(i,i)!=0.0) Y0(i)=(CB0(i)-Y0(i))/R0(i,i);
     else {
       printf("Warning Singular Matrix Found\n");
       return LP_FAIL;
     }
   }
   /*
   ++      b) find w=Q*y
   ++         note: B'*w=(Q*R)'*Q*y= R'*(Q'*Q)*y=R'*y=c(basis) 
   */
   for(i=0;i<m;i++){
     W0(i)=0.0;
     for(j=0;j<m;j++)W0(i)+=Q0(i,j)*Y0(j);
   }

   /*
   ++ Step 2)find entering variable, 
   ++ (use lexicographically first variable with negative reduced cost)
   */
   q=n;
   for(i=0;i<n-m;i++){
     /* calculate reduced cost */
     r=CN0(i);
     for(j=0;j<m;j++) r-=W0(j)*AN0(j,i);      
     if (r<-zero_tol && (q==n || nonbasis0(i)<nonbasis0(q))) q=i; 
   }
     
   /*
   ++ if ratios were all nonnegative current solution is optimal
   */
   if (q==n){           
     if (verbose>0) printf("optimal solution found in %d iterations\n",k);
     return LP_OPT;
   }
   /* 
   ++ Step 3)Calculate translation direction for q entering
   ++        by solving system  B*d=-A(:,nonbasis(q));
   ++   a) let y=-Q'*A(:,nonbasis(q));
   */
   for(i=0;i<m;i++){
      Y0(i)=0.0;
      for(j=0;j<m;j++) Y0(i)-=Q0(j,i)*AN0(j,q);
   }
  
   /*
   ++  b) back solve Rd=y  (d=R\y)
   ++     note B*d= Q*R*d=Q*y=Q*-Q'*A(:nonbasis(q))=-A(:,nonbasis(q)) 
   */
   for(i=m-1;i>=0;i--){
     D0(i)=0.0;
     for(j=m-1;j>=i+1;j--)D0(i)+=R0(i,j)*D0(j);
     if (R0(i,i)!=0.0) D0(i)=(Y0(i)-D0(i))/R0(i,i);
     else {
       printf("Warning Singular Matrix Found\n");
       return LP_FAIL;
     } 
   }
   /*
   ++ Step 4 Choose leaving variable 
   ++     (first variable to become negative, by moving in direction D)
   ++     (if none become negative, then objective function unbounded)
   */
   a=0;
   l=-1;
   for(i=0;i<m;i++){
     if (D0(i)<-zero_tol){
        at=-1*XB0(i)/D0(i);
        if (l==-1 || at<a){ a=at; l=i;}
     }
   }
   if (l==-1){
     if (verbose>0){
         printf("Objective function Unbounded (%d iterations)\n",k);
     }
     return LP_UNBD;
   }
   /*
   ++ Step 5) Update solution and basis data
   */
   XN0(q)=a;        
   for(j=0;j<m;j++) XB0(j)+=a*D0(j);
   XB0(l)=0.0;             /* enforce strict zeroness of nonbasis variables */
   qv=nonbasis0(q);
   nonbasis0(q)=basis0(l);
   basis0(l)=qv;
 }
 if (verbose>=0){ 
   printf("Simplex Algorithm did not Terminate in %d iterations\n",k);
 }
 return LP_FAIL;
}


/*
**void GQR(int r, int c, double **Q, double **R):
**  Use givens rotations on R to bring it into triangular form.
**  Store orthogonal matrix needed to bring R to triangular form in Q.
**  Assume R is an rxc matrix and Q is rxr.
**  (Question: is r>=c needed ?) 
**
*/
void GQR(int r, int c, double **Q, double **R){
     int i,j,k;
     double s,s1,s2;
     double t1,t2;

     for(i=0;i<r;i++){
        for(k=0;k<r;k++)Q0(i,k)=0.0;
        Q0(i,i)=1.0;
     }

     for (i=0;i<c;i++)
       for (k=i+1;k<r;k++)
         /* performing givens rotations to zero A[k][i] */
         if (R0(k,i)!=0){
           s=sqrt(R0(i,i)*R0(i,i)+R0(k,i)*R0(k,i));
           s1=R0(i,i)/s;
           s2=R0(k,i)/s;
           for(j=0;j<c;j++) {
             t1=R0(i,j);
             t2=R0(k,j);
             R0(i,j)=s1*t1+s2*t2;
             R0(k,j)=-s2*t1+s1*t2;
           }
           /* actually doing givens row rotations on  Q */
           for(j=0;j<r;j++){
             t1=Q0(j,i);
             t2=Q0(j,k);
             Q0(j,i)=s1*t1+s2*t2;
             Q0(j,k)=-s2*t1+s1*t2;
	   }
        }
  
}






