/* $OpenXM$ */
/*
  proj.c  部分空間へ直交射影する. 2020.09.01  (parts.c より)
*/
#include <stdio.h>
#include <stdlib.h>
#include <mpfr.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_eigen.h>
#include <gsl/gsl_permutation.h>
#include <gsl/gsl_linalg.h>
#ifdef TEST
/*
  gcc -DTEST -DTEST0 proj.c -lmpfr -lgmp -lcblas -lblas -lgsl -lm   (on FreeBSD)
  gcc -DTEST -DTEST0 proj.c -lmpfr -lgmp -lgsl -lgslcblas -lm   (on Debian)
*/
#define NN 6
#endif

#define N NN    // cc -DN=2 がエラーとなるため

#define EPS 1e-8
#define EPS2 1e-10
//int Debug=0x4|0x8;
int Debug=0;
extern int T_verbose;
#include "proj.h"   //prototypes


void myerror(char *msg) {
  fprintf(stderr,"Error: %s\n",msg);
  fprintf(stderr,"Type in enter to exit."); fflush(NULL);
  getchar();
  exit(-1);  
}
/*
 2020.03.17 parts.c
便利な関数達, include して使うのを想定.
gsl 対応も.
*/

/*
  double へ変換.
*/
int mat_get_d(double ans_d[N][N],mpfr_t ans[N][N]) {
  int i,j;
  for (i=0; i<N; i++) {
    for (j=0; j<N; j++) {
      ans_d[i][j] = mpfr_get_d(ans[i][j],MPFR_RNDD);
    }
  }
  return(0);
}
int mat_get_dvec(double ans_d[N*N],mpfr_t ans[N*N]) {
  int i;
  for (i=0; i<N*N; i++) {
    ans_d[i] = mpfr_get_d(ans[i],MPFR_RNDD);
  }
  return(0);
}
int mat_get_mdvec(double ans_d[N*N],mpfr_t ans[N][N]) {
  int i,j;
  for (i=0; i<N; i++) {
    for (j=0; j<N; j++) {
      ans_d[i*N+j] = mpfr_get_d(ans[i][j],MPFR_RNDD);
    }
  }
  return(0);
}

/*
  gsl で固有値、固有ベクトルを求める.
  gsl_eigen_nonsymmv()  ev_ak2.rr を参照. test2-ak2.rr は H^k_n
  初期値の修正は fit_init() を用いている.
*/
/* copied from GSL manual modified.
   data は n^2 の長さ.
*/
int my_gsl_eigen_nonsymmv(double *data, int n,double eigen_re[],double eigen_im[],
			  double eigen_vec_re[][N],double eigen_vec_im[][N]) {
  int nn,n0,ii,len;

  gsl_matrix_view m;
  gsl_vector_complex *eval;
  gsl_matrix_complex *evec;

  gsl_eigen_nonsymmv_workspace * w;

//  gsl_set_error_handler((gsl_error_handler_t *)myhandler);

  m
    = gsl_matrix_view_array (data, n, n);

  eval = gsl_vector_complex_alloc (n);
  evec = gsl_matrix_complex_alloc (n, n);

  w =
    gsl_eigen_nonsymmv_alloc (n);

  gsl_eigen_nonsymmv (&m.matrix, eval, evec, w);

  gsl_eigen_nonsymmv_free (w);

  /* 固有値の絶対値が大きい順に並べる */
  gsl_eigen_nonsymmv_sort (eval, evec,
                           GSL_EIGEN_SORT_ABS_DESC);

  if (N != n) fprintf(stderr,"Warning (my_gsl_eigen_nonsymmv, memory may be broken), N=%d, n=%d\n",N,n);
  /* show result on the server for debug */
  {
    int i, j;

    for (i = 0; i < n; i++)
      {
        gsl_complex eval_i
           = gsl_vector_complex_get (eval, i);
        gsl_vector_complex_view evec_i
           = gsl_matrix_complex_column (evec, i);

        if (Debug&0x1)
	  printf ("eigenvalue = %g + %gi\n",
                GSL_REAL(eval_i), GSL_IMAG(eval_i));
	eigen_re[i] = GSL_REAL(eval_i);
	eigen_im[i] = GSL_IMAG(eval_i);
        if (Debug&0x1) printf ("eigenvector = \n");
        for (j = 0; j < n; ++j)
          {
            gsl_complex z =
              gsl_vector_complex_get(&evec_i.vector, j);
            if(Debug&0x1) printf("%g + %gi\n", GSL_REAL(z), GSL_IMAG(z));
	    eigen_vec_re[i][j] = GSL_REAL(z);
	    eigen_vec_im[i][j] = GSL_IMAG(z);
          }
      }
  }

  gsl_vector_complex_free(eval);
  gsl_matrix_complex_free(evec);

  return 0;
}
void output_eigen(double eigen_re[N],
  double eigen_im[N],
  double eigen_vec_re[N][N],
  double eigen_vec_im[N][N])
{
  int i,j,n;
  n = N;
  for (i=0; i<n; i++) {
      printf("eigenvalue = %g + %gi\n",eigen_re[i],eigen_im[i]);
      printf("  eigenvector=(");
      for (j=0; j<n; j++) {
	printf("%g + %gi, ",eigen_vec_re[i][j],eigen_vec_im[i][j]);
      }
      printf(")\n");
  }
}

int almost_eq(double a, double b) {
  double diff;
  if (a == b) return 1;
  diff=fabs(a-b);
  if (diff <= EPS*(fabs(a)>fabs(b)?fabs(a):fabs(b))) return 1;
  /* a or b が 0.0 の時 */
  if ((b == 0.0) && (fabs(a)<EPS2)) return 1;
  if ((a == 0.0) && (fabs(b)<EPS2)) return 1;
  return 0;
}

/* 共役な固有値にmark.
  -1 は実数単独
Ref: 2020-09-01-parts.c.goodnotes
*/
void pair_eigen(double eigen_re[],double eigen_im[],int pair[]) {
  int i;
  for (i=0; i<N; i++) pair[i] = -1;
  for (i=0; i<N-1; i++) {
    if (almost_eq(eigen_re[i],eigen_re[i+1]) &&
	almost_eq(eigen_im[i],-eigen_im[i+1])) { // conjugate case
      pair[i] = i+1;
      pair[i+1]=i;
    }
  }
}


/* 行列の行のコピー */
void cp_mm(double m1[N][N],int row1,double m2[N][N],int row2) {
  int j;
  for (j=0; j<N; j++) m2[row2][j] = m1[row1][j];
}
/* 実標準形用の basis. 2020.09.02
   double pseudo_eigen_vec[N][N] が答え.
*/
void real_nf_basis(double eigen_vec_re[N][N], double eigen_vec_im[N][N],
		   int pair[N],double pseudo_eigen_vec[N][N]) {
  int i;
  for (i=0; i<N; i++) {
    if (pair[i] == -1) {
      cp_mm(eigen_vec_re,i,pseudo_eigen_vec,i);
    }else if (pair[i] == i+1) {
      cp_mm(eigen_vec_im,i,pseudo_eigen_vec,i);
      cp_mm(eigen_vec_re,i,pseudo_eigen_vec,i+1);
      i++;
    }else myerror("real_nf_basis");
  }
}
/* 行列の row の長さを 1 に */
void to_normal_m(double mat[N][N],int row) {
  double len;
  int i;
  len = 0;
  for (i=0; i<N; i++) len += mat[row][i]*mat[row][i];
  len = sqrt(len);
  if (len == 0.0) myerror("to_normal_m");
  for (i=0; i<N; i++) mat[row][i] = mat[row][i]/len;
}
/* row of a matrix --> vector */
void cp_mv(double mat[N][N],int row, double vec[N]) {
  int j;
  for (j=0; j<N; j++) vec[j] = mat[row][j];
}
void cp_vm(double vec[N], double mat[N][N],int row) {
  int j;
  for (j=0; j<N; j++) mat[row][j] = vec[j];
}
double inner_product_vm(double vec[N], double mat[N][N],int row) {
  int j;
  double ip;
  ip = 0;
  for (j=0; j<N; j++) ip += mat[row][j]*vec[j];
  return ip;
}
/*
  Modified Gram-Schmidt step, w <-- w - (w,p_j)p_j
*/
void gs_step(double w[N],double orth_basis[N][N],int row) {
  double ip;
  int j;
  ip = inner_product_vm(w,orth_basis,row);
  for (j=0; j<N; j++) {
    w[j] = w[j] - ip*orth_basis[row][j];
  }
}
/*
  Gram-Schmidt,  最初の m 個のみ正規直交化.
*/
int gram_schmidt(double basis[N][N],int m,double orth_basis[N][N]) {
  int i,j,r;
  double w[N];
  cp_mm(basis,0,orth_basis,0);
  to_normal_m(orth_basis,0);
  for (i=1; i<m; i++) {
    cp_mv(basis,i,w);
    for (j=0; j<i; j++) gs_step(w,orth_basis,j);
    cp_vm(w,orth_basis,i);
    to_normal_m(orth_basis,i);
    // if (r < 0) { myerror("gram_schmidt"); return r; }
  }
  return 0;
}

/* 2020.09.02 13:23 
  Lower<= Re Lambda <=Upper
  となる部分空間への射影行列を求める.
*/
int projection_mat(double Upper,double Lower,double eigen_re[N],double eigen_im[N],double eigen_vec_re[N][N],double eigen_vec_im[N][N],double proj_mat[N][N]) {
  int pair[N];
  double pseudo_eigen_vec[N][N];
  double basis[N][N];
  double orth_basis[N][N];
  int i,j,k,m;
  pair_eigen(eigen_re,eigen_im,pair);
  real_nf_basis(eigen_vec_re,eigen_vec_im,pair,pseudo_eigen_vec);

  for (i=0,m=0; i<N; i++) {
    if ((Lower <= eigen_re[i]) && (eigen_re[i] <= Upper)) {
      if (pair[i] < 0) {
	cp_mm(pseudo_eigen_vec,i,basis,m); m++;
      }else if (pair[i] == i+1) {
	cp_mm(pseudo_eigen_vec,i,basis,m); m++;i++;
	cp_mm(pseudo_eigen_vec,i,basis,m); m++;
      }else {
	myerror("projection_mat");
      }
    }
  }
  if (m == 0) return m;
  gram_schmidt(basis,m,orth_basis);

  for (i=0; i<N; i++) {
    for (j=0; j<N; j++) {
      proj_mat[i][j] = 0;
      for (k=0; k<m; k++) {
	proj_mat[i][j] += orth_basis[k][i]*orth_basis[k][j];
      }
    }
  }
  if (Debug & 0x4) check_projection(Upper,Lower,eigen_re,eigen_im,proj_mat,pseudo_eigen_vec,basis,m);
  return m;
}

void output_mat(double mat[N][N]) {
  int i,j;
  for (i=0; i<N; i++) {
    for (j=0; j<N; j++) printf("%5.5f  ",mat[i][j]);
    printf("\n");
  }
}
void output_vec(double *vec,int n) {
  int i;
  for (i=0; i<n; i++) {
    printf("%g  ",vec[i]);
  }
}

/* 2020.09.02
   テストのための函数
*/
void mat_dup(double m1[N][N],double m2[N][N]) {
  int i,j;
  for (i=0; i<N; i++) for (j=0; j<N; j++) m2[i][j] = m1[i][j];
}
int mat_inverse(double mat[N][N],double inv_mat[N][N]) {
  double data[N*N];
  double data2[N*N];
  int i,j;
  gsl_matrix_view a;
  gsl_matrix_view b;
  int sig;
  gsl_permutation *p;
  for (i=0; i<N; i++) for (j=0; j<N; j++) data[i*N+j] = mat[i][j];
  a=gsl_matrix_view_array(data,N,N);
  for (i=0; i<N; i++) for (j=0; j<N; j++) data2[i*N+j] = 0;
  b=gsl_matrix_view_array(data2,N,N);
  p = gsl_permutation_alloc(N);
  gsl_linalg_LU_decomp(&a.matrix,p,&sig);
  gsl_linalg_LU_invert(&a.matrix,p,&b.matrix);
  for (i=0; i<N; i++) for (j=0; j<N; j++) inv_mat[i][j] = gsl_matrix_get(&b.matrix,i,j);
  return sig;
}

int mat_prod(double m1[N][N],double m2[N][N],double m3[N][N]) {
  int i,j,k;
  double tt[N][N];
  for (i=0; i<N; i++) {
    for (j=0; j<N; j++) {
      tt[i][j] = 0;
      for (k=0; k<N; k++) {
	tt[i][j] += m1[i][k]*m2[k][j];
      }
    }
  }
  for (i=0; i<N; i++) cp_mm(tt,i,m3,i);
  return 0;
}
int mat_transpose(double mat[N][N],double tmat[N][N]) {
  int i,j;
  for (i=0; i<N; i++) {
    for (j=0; j<N; j++) {
      tmat[i][j] = mat[j][i];
    }
  }
  return 0;
}

/*
  aa*w --> tw
*/
void mat_linear_transformation(double aa[N][N],double w[N],double tw[N]) {
  int i,j;
  for (i=0; i<N; i++) {
    tw[i]=0;
    for (j=0; j<N; j++) {
      tw[i] += aa[i][j]*w[j];
    }
  }
}

/* proj_mat が 射影行列であるか調べる */
void check_projection(double upper,double lower,double eigen_re[N],double eigen_im[N],double proj_mat[N][N],double pseudo_eigen_vec[N][N],double basis[N][N],int m) {
  int i,j;
  double tt[N][N];
  double ww[N];
  double tw[N];
  double ip;
  /* Step 1. P^2=P */
  mat_prod(proj_mat,proj_mat,tt);
  for (i=0; i<N; i++) for (j=0; j<N; j++) if (!almost_eq(proj_mat[i][j],tt[i][j])) myerror("check_projection step 1.");
  printf("check_projection step 1 ==> OK\n");
  /* Step 2. Pw=w for w \in basis */
  for (i=0; i<m; i++) {
    cp_mv(basis,i,ww);
    mat_linear_transformation(proj_mat,ww,tw);
    for (i=0; i<N; i++) if (!almost_eq(ww[i],tw[i])) myerror("check_projection step 2.");
  }
  printf("check_projection step 2 ==> OK\n");
  /* Step 3. v \not\in basis (Pw-w,basis)=0 */
  for (i=0; i<N; i++) {
    if ((lower <=eigen_re[i]) && (eigen_re[i] <= upper)) continue;
    cp_mv(pseudo_eigen_vec,i,ww);
    mat_linear_transformation(proj_mat,ww,tw);
    for (j=0; j<N; j++) tw[j]=tw[j]-ww[j];
    for (j=0; j<m; j++) if (!almost_eq(ip=inner_product_vm(tw,basis,j),0.0)) {
	printf("i,j=%d,%d; ip=%g; ",i,j,ip); myerror("check_projection Step 3.");
      }
  }
  printf("check_projection step 3 ==> OK\n");
}

void mysort(double eigen_re[],double sorted[]) {
  int size,i,j;
  double tmp;
  for (i=0; i<N; i++) sorted[i] = eigen_re[i];
  size = N;
  /* bubble sort */
  for (j=size-1; j>0; j--) {
    for (i=0; i<j; i++) {
      if (sorted[i] < sorted[i+1]) {
        tmp = sorted[i+1];
        sorted[i+1] = sorted[i];
        sorted[i] = tmp;
      }
    }
  }
  printf("Sort result: ");for (i=0; i<N; i++) printf("%g ",sorted[i]); printf("\n");
}

/*
  上から n_prune 個の固有値 (Re で比較)を除いたものに対応する固有空間への
  射影行列を求める.
  gsl の固有値計算で data は書き換えられるので注意.
*/
int projection_matrix_to_subspace_of(int n_prune, double data[],double proj_mat[N][N]) {
  double eigen_re[N];
  double eigen_im[N];
  double eigen_vec_re[N][N];
  double eigen_vec_im[N][N];
  double pseudo_eigen_vec[N][N];
  double orth_basis[N][N];
  int n,i,j,m;
  int pair[N];
  double aa[N][N];
  double tt[N][N];
  double sorted[N];
  double upper,lower;
  for (i=0; i<N; i++) for (j=0; j<N; j++) aa[i][j] = data[i*N+j];
  n = N;
  my_gsl_eigen_nonsymmv(data, n,eigen_re,eigen_im,eigen_vec_re,eigen_vec_im);

  /* 取り出す固有値の範囲 */
  mysort(eigen_re,sorted);
  lower=sorted[N-1]-1.0;
  if (n_prune==0) upper=sorted[0]+1.0;
//  else upper=(sorted[n_prune]+sorted[n_prune-1])/2;
  else upper=sorted[n_prune];

  if ((Debug&0x8) || T_verbose) {
    output_eigen(eigen_re,eigen_im,eigen_vec_re,eigen_vec_im);
    printf("---  ---  ---\n");
  }
  pair_eigen(eigen_re,eigen_im,pair);
  if (Debug&0x8) {
    for (i=0; i<N; i++) printf("pair[%d]=%d, ",i,pair[i]); printf("\n");
  }
  real_nf_basis(eigen_vec_re,eigen_vec_im,pair,pseudo_eigen_vec);
  if (Debug&0x8) output_mat(pseudo_eigen_vec);
  if (Debug&0x2) {
    /* block 三角になってるか調べる */
    double inv[N][N];
    double pp[N][N];
    mat_transpose(pseudo_eigen_vec,pp); // 固有vecは横ベクトル
    mat_inverse(pp,inv);
    output_mat(inv);
    mat_prod(inv,aa,tt);
    mat_prod(tt,pp,tt);
    printf("Block diagonal?\n");
    output_mat(tt);
  }
  if (Debug&0x8) {
    printf("---  ---  ---\n");
  }
  gram_schmidt(pseudo_eigen_vec,N,orth_basis);
  if (Debug&0x8) {
    output_mat(orth_basis);
    printf("---  ---  ---\n");
  }
  m=projection_mat(upper,lower,eigen_re,eigen_im,eigen_vec_re,eigen_vec_im,proj_mat);
  if (Debug&0x8) {
    printf("m=%d\n",m);
    output_mat(proj_mat);
    printf("---------------\n");
  }
  return(m);
}

void vec_scalar_multiplication(double s,double vec[N],double vec_new[N]) {
  int i;
  for (i=0; i<N; i++) vec_new[i] = s*vec[i];
}
/* vector rep of a mat --> mat */
void cp_vmatm(double data[N*N],double mat[N][N]) {
  int i,j;
  for (i=0; i<N; i++) {
    for (j=0; j<N; j++) {
      mat[i][j] = data[i*N+j];
    }
  }
}

int test0(double data[]) {
  double eigen_re[N];
  double eigen_im[N];
  double eigen_vec_re[N][N];
  double eigen_vec_im[N][N];
  double pseudo_eigen_vec[N][N];
  double orth_basis[N][N];
  double proj_mat[N][N];
  int n,i,j,m;
  int pair[N];
  double aa[N][N];
  double tt[N][N];
  double upper,lower;
  upper=3.0;
  lower=0.0;
  for (i=0; i<N; i++) for (j=0; j<N; j++) aa[i][j] = data[i*N+j];
  n = N;
  my_gsl_eigen_nonsymmv(data, n,eigen_re,eigen_im,eigen_vec_re,eigen_vec_im);
  output_eigen(eigen_re,eigen_im,eigen_vec_re,eigen_vec_im);
  printf("---  ---  ---\n");
  pair_eigen(eigen_re,eigen_im,pair);
  for (i=0; i<N; i++) printf("pair[%d]=%d, ",i,pair[i]); printf("\n");
  real_nf_basis(eigen_vec_re,eigen_vec_im,pair,pseudo_eigen_vec);
  output_mat(pseudo_eigen_vec);
  if (Debug&0x2) {
    /* block 三角になってるか調べる */
    double inv[N][N];
    double pp[N][N];
    mat_transpose(pseudo_eigen_vec,pp); // 固有vecは横ベクトル
    mat_inverse(pp,inv);
    output_mat(inv);
    mat_prod(inv,aa,tt);
    mat_prod(tt,pp,tt);
    printf("Block diagonal?\n");
    output_mat(tt);
  }
  printf("---  ---  ---\n");
  gram_schmidt(pseudo_eigen_vec,N,orth_basis);
  output_mat(orth_basis);
  printf("---  ---  ---\n");
  m=projection_mat(upper,lower,eigen_re,eigen_im,eigen_vec_re,eigen_vec_im,proj_mat);
  printf("m=%d\n",m);
  output_mat(proj_mat);
  printf("---------------\n");
  return(0);
}

int test1() {
  double data[N*N];
  int i;
  for (i=0; i<N*N; i++) data[i] = i+1;
  test0(data);
  return 0;
}
int test2() {
  double data[N*N];
  double small=1e-2;
//  double small=0;
  int i,j;
  for (i=0; i<N*N; i++) data[i] = 0;
  for (i=7; i<N; i++) data[i*N+i] = i+1;
  if (N>=6) {
    data[0*N+1] = 1; data[1*N+0] = -1;
    data[2*N+3] = 2; data[3*N+2] = -2;
    data[4*N+4] = 4; data[5*N+5] = 0.1;
  }
  if (N>=6) {
    for (i=0; i<N; i++) {
      for (j=0; j<N; j++) {
	data[i*N+j] = data[i*N+j]+small*(i+j);
      }
    }
  }
  test0(data);
  return 0;
}


#ifdef TEST0
int main() {
  test1();
  test2();
  return(0);
}
#endif

