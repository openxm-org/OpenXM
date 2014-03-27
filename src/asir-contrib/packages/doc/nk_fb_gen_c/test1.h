/* $OpenXM$ */
#include <stdio.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_odeiv.h>
#include <gsl/gsl_multimin.h>

#define N_VALUES 4
#define DIM 5
#define ODEIV_STEP_TYPE gsl_odeiv_step_rkf45
#define MOVE_T_SUCCESS 1
#define MOVE_T_FAIL    0
/* #define MULTIMIN_FDFMINIMIZER_TYPE gsl_multimin_fdfminimizer_conjugate_fr */
#define MULTIMIN_FDFMINIMIZER_TYPE gsl_multimin_fdfminimizer_steepest_descent
#define MAXSIZE 10

gsl_matrix *a1, *b1, *c1, *e1;
gsl_matrix *a2, *b2, *c2, *e2;
gsl_matrix *p2, *q2, *p3, *q3, *r3;
gsl_matrix *db11;
gsl_matrix *dc11;
gsl_matrix *db12;
gsl_matrix *dc12;
gsl_matrix *db21;
gsl_matrix *dc21;
gsl_matrix *db22;
gsl_matrix *dc22;
gsl_matrix *dq21;
gsl_matrix *dq31;
gsl_matrix *dr31;
gsl_matrix *dq22;
gsl_matrix *dq32;
gsl_matrix *dr32;
gsl_matrix *inv_a1;
gsl_matrix *inv_a2;
gsl_matrix *inv_p2;
gsl_matrix *inv_p3;
gsl_matrix *pf1_m;
gsl_matrix *pf2_m;
gsl_matrix *pf1_nd_m;
gsl_matrix *pf2_nd_m;
gsl_matrix *pf11_m;
gsl_matrix *pf12_m;
gsl_matrix *pf22_m;
gsl_matrix *pft_m;
gsl_vector *grad_v;
double g_y1;
double g_y2;
double g_x11;
double g_x12;
double g_x22;
double g_r;
double g_s1;
double g_s2;
double g_s11;
double g_s12;
double g_s22;
double values[N_VALUES];
double move_t_points[DIM];

void init_mat();
void set_abce_1(double x11, double x12, double x22, double y1, double y2, double r);
void set_abce_2(double x11, double x12, double x22, double y1, double y2, double r);
void set_pqr(double x11, double x12, double x22, double y1, double y2, double r);
void set_dbcqr(double x11, double x12, double x22, double y1, double y2, double r);
void pf1(double x11, double x12, double x22, double y1, double y2, double r);
void pf1_no_diag_shift(double x11, double x12, double x22, double y1, double y2, double r);
void pf2(double x11, double x12, double x22, double y1, double y2, double r);
void pf2_no_diag_shift(double x11, double x12, double x22, double y1, double y2, double r);
void pf11(double x11, double x12, double x22, double y1, double y2, double r);
void pf12(double x11, double x12, double x22, double y1, double y2, double r);
void pf21(double x11, double x12, double x22, double y1, double y2, double r);
void pf22(double x11, double x12, double x22, double y1, double y2, double r);
void pf_all(double x11, double x12, double x22, double y1, double y2, double r);
void invmat(gsl_matrix *m, gsl_matrix *invm);
void gsl_matrix_show(gsl_matrix *mat);
int move_t(double x11, double x12, double x22, double y1, double y2, double xx11, double xx12, double xx22, double yy1, double yy2, double *val);
int sys_t(double t, const double *y, double *val, double *params);
double *fbnd(int dim, double x[MAXSIZE][MAXSIZE], double y[], int maxdeg, int weight[]);
double my_f(const gsl_vector *v, void *params);
void my_df(const gsl_vector *v, void *param, gsl_vector *df);
void my_fdf(const gsl_vector *x, void *params, double *f, gsl_vector *df);
void gsl_vector_show(gsl_vector *mat);
void show_v(double *v, int n);
void grad(double x11, double x12, double x22, double y1, double y2, double r, double *val);
double *init_val(int dim, double x11, double x12, double x22, double y1, double y2, double s11, double s12, double s22, double s1, double s2);
void search_min(double *val);
