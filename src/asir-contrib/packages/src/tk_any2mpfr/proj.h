/* $OpenXM$ */
void myerror(char *msg);
void output_eigen(double eigen_re[N], double eigen_im[N],  double eigen_vec_re[N][N],  double eigen_vec_im[N][N]);
void pair_eigen(double eigen_re[],double eigen_im[],int pair[]);
void cp_mm(double m1[N][N],int row1,double m2[N][N],int row2);
void real_nf_basis(double eigen_vec_re[N][N], double eigen_vec_im[N][N],int pair[N],double pseudo_eigen_vec[N][N]);
void to_normal_m(double mat[N][N],int row);
void cp_mv(double mat[N][N],int row, double vec[N]);
void cp_vm(double vec[N], double mat[N][N],int row);
void gs_step(double w[N],double orth_basis[N][N],int row);
void output_mat(double mat[N][N]);
void output_vec(double vec[],int n);

int gram_schmidt(double basis[N][N],int m,double orth_basis[N][N]);
int projection_mat(double Upper,double Lower,double eigen_re[N],double eigen_im[N],double eigen_vec_re[N][N],double eigen_vec_im[N][N],double proj_mat[N][N]);

int mat_get_d(double ans_d[N][N],mpfr_t ans[N][N]);
int mat_get_mdvec(double ans_d[N*N],mpfr_t ans[N][N]);
int mat_get_dvec(double ans_d[N*N],mpfr_t ans[N*N]);
int almost_eq(double a, double b);
int gram_schmidt(double basis[N][N],int m,double orth_basis[N][N]);

int projection_matrix_to_subspace_of(int n_prone, double data[],double proj_mat[N][N]);

/* for check */
void mat_dup(double m1[N][N],double m2[N][N]);
int mat_transpose(double mat[N][N],double tmat[N][N]);
void check_projection(double upper,double lower,double eigen_re[N],double eigen_im[N],double proj_mat[N][N],double pseudo_eigen_vec[N][N],double basis[N][N],int m);
int mat_inverse(double mat[N][N],double inv_mat[N][N]);
int mat_prod(double m1[N][N],double m2[N][N],double m3[N][N]);
void mat_linear_transformation(double aa[N][N],double w[N],double tw[N]);
void vec_scalar_multiplication(double s,double vec[N],double vec_new[N]);
void cp_vmatm(double data[N*N],double mat[N][N]);

int test0(double data[]);
int test1();
int test2();
int main();

