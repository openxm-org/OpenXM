/* $OpenXM$ */
#include "test1.h"
#define SEARCH_MIN_ITER 100
int main()
{
extern double g_y1;
extern double g_y2;
extern double g_x11;
extern double g_x12;
extern double g_x22;
extern double g_s1;
extern double g_s2;
extern double g_r;
extern double g_s11;
extern double g_s12;
extern double g_s22;
extern double values[N_VALUES];

	init_mat();
	g_r = 1.0;
	/* Write data here. */
	/* g_x11, g_x12, g_x22, g_y1, g_y2 */
	/* g_s11, g_s12, g_s22, g_s1, g_s2 */
/* Wind data from this11/h-mle/FB2/s1_wind.c 
   The data average s1 and s2 is defined by 
   (s1,s2) = (sum(s1_i)/N,sum(s2_i)/N)
   where (s1_i,s2_i), i=1,2, ..., N are data on the S^1 (circle).
   s11 is defined as the average of s1_i*s1_i; s11=sum(s1_i*s1_i)/N.
   s12 is defined as the average of s1_i*s2_i; s11=sum(s1_i*s2_i)/N.
   s22 is defined as the average of s2_i*s2_i; s11=sum(s2_i*s2_i)/N.

*/
g_s11 = 0.662125; g_s12 = 0.274563; g_s22 = 0.337875; g_s1 = 0.317564; g_s2 = -0.020188;
/* Initial parameter for the HGD. */
g_x11 = 0.5; g_x12 = 0.5; g_x22 = 0.15; g_y1 = 1; g_y2 = 0.15; g_r = 1.0;

	double *val;
	val = init_val(1, g_x11, g_x12, g_x22, g_y1, g_y2, g_s11, g_s12, g_s22, g_s1, g_s2);
	show_v(val, N_VALUES);
	search_min(val);
	printf("search_min :\n");
	show_v(values, N_VALUES);
	val = init_val(1, g_x11, g_x12, g_x22, g_y1, g_y2, g_s11, g_s12, g_s22, g_s1, g_s2);
	printf("init_val :\n");
	show_v(val, N_VALUES);
}

void init_mat()
{
extern gsl_matrix *a1, *b1, *c1, *e1;
extern gsl_matrix *a2, *b2, *c2, *e2;
extern gsl_matrix *a3, *b3, *c3, *e3;
extern gsl_matrix *p2, *q2, *p3, *q3, *r3;
extern gsl_matrix *db11;
extern gsl_matrix *dc11;
extern gsl_matrix *db12;
extern gsl_matrix *dc12;
extern gsl_matrix *db21;
extern gsl_matrix *dc21;
extern gsl_matrix *db22;
extern gsl_matrix *dc22;
extern gsl_matrix *dq21;
extern gsl_matrix *dq31;
extern gsl_matrix *dr31;
extern gsl_matrix *dq22;
extern gsl_matrix *dq32;
extern gsl_matrix *dr32;
extern gsl_matrix *inv_a1;
extern gsl_matrix *inv_a2;
extern gsl_matrix *inv_a3;
extern gsl_matrix *inv_p2;
extern gsl_matrix *inv_p3;
extern gsl_matrix *pf1_m;
extern gsl_matrix *pf2_m;
extern gsl_matrix *pf1_nd_m;
extern gsl_matrix *pf2_nd_m;
extern gsl_matrix *pf11_m;
extern gsl_matrix *pf12_m;
extern gsl_matrix *pf22_m;
extern gsl_matrix *pft_m;
extern gsl_vector *grad_v;

a1 = gsl_matrix_alloc(4, 4);
b1 = gsl_matrix_alloc(4, 4);
c1 = gsl_matrix_alloc(4, 1);
e1 = gsl_matrix_alloc(4, 2);
a2 = gsl_matrix_alloc(4, 4);
b2 = gsl_matrix_alloc(4, 4);
c2 = gsl_matrix_alloc(4, 1);
e2 = gsl_matrix_alloc(4, 2);
p2 = gsl_matrix_alloc(1, 1);
q2 = gsl_matrix_alloc(1, 4);
p3 = gsl_matrix_alloc(2, 2);
q3 = gsl_matrix_alloc(2, 1);
r3 = gsl_matrix_alloc(2, 4);
db11 = gsl_matrix_alloc(4, 4);
dc11 = gsl_matrix_alloc(4, 1);
db12 = gsl_matrix_alloc(4, 4);
dc12 = gsl_matrix_alloc(4, 1);
db21 = gsl_matrix_alloc(4, 4);
dc21 = gsl_matrix_alloc(4, 1);
db22 = gsl_matrix_alloc(4, 4);
dc22 = gsl_matrix_alloc(4, 1);
dq21 = gsl_matrix_alloc(1, 4);
dq31 = gsl_matrix_alloc(2, 1);
dr31 = gsl_matrix_alloc(2, 4);
dq22 = gsl_matrix_alloc(1, 4);
dq32 = gsl_matrix_alloc(2, 1);
dr32 = gsl_matrix_alloc(2, 4);
inv_a1 = gsl_matrix_alloc(4, 4);
inv_a2 = gsl_matrix_alloc(4, 4);
inv_p2 = gsl_matrix_alloc(1, 1);
inv_p3 = gsl_matrix_alloc(2, 2);
pf1_m = gsl_matrix_alloc(4, 4);
pf2_m = gsl_matrix_alloc(4, 4);
pf1_nd_m = gsl_matrix_alloc(4, 4);
pf2_nd_m = gsl_matrix_alloc(4, 4);
pf11_m = gsl_matrix_alloc(4, 4);
pf12_m = gsl_matrix_alloc(4, 4);
pf22_m = gsl_matrix_alloc(4, 4);
pft_m = gsl_matrix_alloc(N_VALUES, N_VALUES);
grad_v = gsl_vector_alloc(DIM);
}

void set_abce_1(double x11, double x12, double x22, double y1, double y2, double r)
{
extern gsl_matrix *a1, *b1, *c1, *e1;
gsl_matrix_set(a1, 0, 0, 1);
gsl_matrix_set(a1, 1, 1, 1);
gsl_matrix_set(a1, 2, 1, 1*x12);
gsl_matrix_set(a1, 2, 2, (-2)*x11 + 2*x22);
gsl_matrix_set(a1, 3, 3, 1);
gsl_matrix_set(b1, 0, 1, 1);
gsl_matrix_set(b1, 1, 3, 1);
gsl_matrix_set(b1, 2, 0, 1*x12*r*r);
gsl_matrix_set(b1, 2, 1, (-1)*y2);
gsl_matrix_set(b1, 2, 2, 1*y1);
gsl_matrix_set(b1, 2, 3, (-1)*x12);
gsl_matrix_set(e1, 3, 0, 1);
}

void set_abce_2(double x11, double x12, double x22, double y1, double y2, double r)
{
extern gsl_matrix *a2, *b2, *c2, *e2;
gsl_matrix_set(a2, 0, 0, 1);
gsl_matrix_set(a2, 1, 1, 2*x11 + (-2)*x22);
gsl_matrix_set(a2, 1, 2, 1*x12);
gsl_matrix_set(a2, 2, 2, 1);
gsl_matrix_set(a2, 3, 3, (-2)*x11 + 2*x22);
gsl_matrix_set(b2, 0, 2, 1);
gsl_matrix_set(b2, 1, 1, 1*y2);
gsl_matrix_set(b2, 1, 2, (-1)*y1);
gsl_matrix_set(b2, 1, 3, 1*x12);
gsl_matrix_set(b2, 2, 0, 1*r*r);
gsl_matrix_set(b2, 2, 3, (-1));
gsl_matrix_set(b2, 3, 1, 1*x12*r*r);
gsl_matrix_set(b2, 3, 2, 1);
gsl_matrix_set(b2, 3, 3, (-1)*y2);
gsl_matrix_set(c2, 3, 0, 1*y1);
gsl_matrix_set(e2, 3, 0, (-2)*x12);
}

void set_pqr(double x11, double x12, double x22, double y1, double y2, double r)
{
extern gsl_matrix *p2, *q2, *p3, *q3, *r3;
gsl_matrix_set(p2, 0, 0, (-2)*x11 + 2*x22);
gsl_matrix_set(q2, 0, 0, (-1)*x12*r*r);
gsl_matrix_set(q2, 0, 1, 1*y2);
gsl_matrix_set(q2, 0, 2, (-1)*y1);
gsl_matrix_set(q2, 0, 3, 2*x12);
gsl_matrix_set(p3, 0, 0, 2*x11 + (-2)*x22);
gsl_matrix_set(p3, 0, 1, 2*x12);
gsl_matrix_set(p3, 1, 0, 2*x12);
gsl_matrix_set(p3, 1, 1, (-2)*x11 + 2*x22);
gsl_matrix_set(q3, 0, 0, 1*y2);
gsl_matrix_set(q3, 1, 0, (-1)*y1);
gsl_matrix_set(r3, 0, 0, (-1)*y1*r*r);
gsl_matrix_set(r3, 0, 1, (-2)*x11*r*r + 2*r*r*x22 + 1);
gsl_matrix_set(r3, 0, 2, (-1)*x12*r*r);
gsl_matrix_set(r3, 0, 3, 1*y1);
gsl_matrix_set(r3, 1, 1, (-1)*x12*r*r);
gsl_matrix_set(r3, 1, 2, (-1));
gsl_matrix_set(r3, 1, 3, 1*y2);
}

void set_dbcqr(double x11, double x12, double x22, double y1, double y2, double r)
{
extern gsl_matrix *db11;
extern gsl_matrix *dc11;
extern gsl_matrix *db12;
extern gsl_matrix *dc12;
extern gsl_matrix *db21;
extern gsl_matrix *dc21;
extern gsl_matrix *db22;
extern gsl_matrix *dc22;
extern gsl_matrix *dq21;
extern gsl_matrix *dq31;
extern gsl_matrix *dr31;
extern gsl_matrix *dq22;
extern gsl_matrix *dq32;
extern gsl_matrix *dr32;
gsl_matrix_set(db11, 2, 2, 1);
gsl_matrix_set(db12, 2, 1, (-1));
gsl_matrix_set(db21, 1, 2, (-1));
gsl_matrix_set(dc21, 3, 0, 1);
gsl_matrix_set(db22, 1, 1, 1);
gsl_matrix_set(db22, 3, 3, (-1));
gsl_matrix_set(dq21, 0, 2, (-1));
gsl_matrix_set(dq31, 1, 0, (-1));
gsl_matrix_set(dr31, 0, 0, (-1)*r*r);
gsl_matrix_set(dr31, 0, 3, 1);
gsl_matrix_set(dq22, 0, 1, 1);
gsl_matrix_set(dq32, 0, 0, 1);
gsl_matrix_set(dr32, 1, 3, 1);
}

void pf_all(double x11, double x12, double x22, double y1, double y2, double r)
{
extern gsl_matrix *a1, *inv_a1, *b1, *c1, *e1;
extern gsl_matrix *a2, *inv_a2, *b2, *c2, *e2;
extern gsl_matrix *p2, *inv_p2, *q2;
extern gsl_matrix *p3, *inv_p3, *q3, *r3;
extern gsl_matrix *pf1_m;
extern gsl_matrix *pf1_nd_m;
extern gsl_matrix *pf2_m;
extern gsl_matrix *pf2_nd_m;
extern double g_s1;
extern double g_s2;

extern gsl_matrix *db11;
extern gsl_matrix *dc11;
extern gsl_matrix *db12;
extern gsl_matrix *dc12;
extern gsl_matrix *db21;
extern gsl_matrix *dc21;
extern gsl_matrix *db22;
extern gsl_matrix *dc22;
extern gsl_matrix *dq21;
extern gsl_matrix *dq31;
extern gsl_matrix *dr31;
extern gsl_matrix *dq22;
extern gsl_matrix *dq32;
extern gsl_matrix *dr32;

extern gsl_matrix *pf11_m;
extern gsl_matrix *pf12_m;
extern gsl_matrix *pf22_m;
extern double g_s11;
extern double g_s12;
extern double g_s22;
gsl_matrix *t1;
gsl_matrix *t2;
gsl_matrix *t3;
gsl_matrix *t4;
gsl_matrix *t5;
gsl_matrix *t6;

static double o_y1;
static double o_y2;
static double o_x11;
static double o_x12;
static double o_x22;
if (o_y1 == y1 && o_y2 == y2 && o_x11 == x11 &&o_x12 == x12 &&o_x22 == x22) 
	return;
o_y1 = y1;
o_y2 = y2;
o_x11 = x11;
o_x12 = x12;
o_x22 = x22;

set_pqr(x11, x12, x22, y1, y2, r);
invmat(p2, inv_p2);
invmat(p3, inv_p3);

set_abce_1(x11, x12, x22, y1, y2, r);
invmat(a1, inv_a1);
t1 = gsl_matrix_alloc(inv_p2->size1, q2->size2);
gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, inv_p2, q2, 0.0, t1);
t2 = gsl_matrix_alloc(c1->size1, t1->size2);
gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, c1, t1, 0.0, t2);
t3 = gsl_matrix_alloc(q3->size1, t1->size2);
gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, q3, t1, 0.0, t3);
gsl_matrix_sub(t3, r3);
t4 = gsl_matrix_alloc(inv_p3->size1, t3->size2);
gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, inv_p3, t3, 0.0, t4);
t5 = gsl_matrix_alloc(e1->size1, t4->size2);
gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, e1, t4, 0.0, t5);
gsl_matrix_add(t5, b1);
gsl_matrix_sub(t5, t2);
gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, inv_a1, t5, 0.0, t2);
gsl_matrix_memcpy(pf1_nd_m, t2);
gsl_matrix_memcpy(pf1_m, t2);

gsl_matrix_set_identity(t2);
gsl_matrix_scale(t2, -g_s1);
gsl_matrix_add(pf1_m, t2);

gsl_matrix_free(t1);
gsl_matrix_free(t2);
gsl_matrix_free(t3);
gsl_matrix_free(t4);
gsl_matrix_free(t5);

set_abce_2(x11, x12, x22, y1, y2, r);
invmat(a2, inv_a2);
t1 = gsl_matrix_alloc(inv_p2->size1, q2->size2);
gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, inv_p2, q2, 0.0, t1);
t2 = gsl_matrix_alloc(c2->size1, t1->size2);
gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, c2, t1, 0.0, t2);
t3 = gsl_matrix_alloc(q3->size1, t1->size2);
gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, q3, t1, 0.0, t3);
gsl_matrix_sub(t3, r3);
t4 = gsl_matrix_alloc(inv_p3->size1, t3->size2);
gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, inv_p3, t3, 0.0, t4);
t5 = gsl_matrix_alloc(e2->size1, t4->size2);
gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, e2, t4, 0.0, t5);
gsl_matrix_add(t5, b2);
gsl_matrix_sub(t5, t2);
gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, inv_a2, t5, 0.0, t2);
gsl_matrix_memcpy(pf2_nd_m, t2);
gsl_matrix_memcpy(pf2_m, t2);

gsl_matrix_set_identity(t2);
gsl_matrix_scale(t2, -g_s2);
gsl_matrix_add(pf2_m, t2);

gsl_matrix_free(t1);
gsl_matrix_free(t2);
gsl_matrix_free(t3);
gsl_matrix_free(t4);
gsl_matrix_free(t5);

set_dbcqr(x11, x12, x22, y1, y2, r);
t1 = gsl_matrix_alloc(inv_p2->size1, q2->size2);
gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, inv_p2, q2, 0.0, t1);
t2 = gsl_matrix_alloc(inv_p2->size1, dq21->size2);
gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, inv_p2, dq21, 0.0, t2);
t3 = gsl_matrix_alloc(dq31->size1, t1->size2);
gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, dq31, t1, 0.0, t3);
t4 = gsl_matrix_alloc(q3->size1, t2->size2);
gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, q3, t2, 0.0, t4);
gsl_matrix_add(t3, t4);
gsl_matrix_sub(t3, dr31);
gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, inv_p3, t3, 0.0, t4);
t5 = gsl_matrix_alloc(e1->size1, t4->size2);
gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, e1, t4, 0.0, t5);
gsl_matrix_add(t5, db11); 
t6 = gsl_matrix_alloc(dc11->size1, t1->size2);
gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, dc11, t1, 0.0, t6);
gsl_matrix_sub(t5, t6); 
gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, c1, t2, 0.0, t6);
gsl_matrix_sub(t5, t6); 
gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, inv_a1, t5, 0.0, t6);
gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, pf1_nd_m, pf1_nd_m, 0.0, t5);
gsl_matrix_add(t5, t6); 
gsl_matrix_memcpy(pf11_m, t5);

gsl_matrix_set_identity(t5);
gsl_matrix_scale(t5, -g_s11);
gsl_matrix_add(pf11_m, t5);
gsl_matrix_free(t1);
gsl_matrix_free(t2);
gsl_matrix_free(t3);
gsl_matrix_free(t4);
gsl_matrix_free(t5);
gsl_matrix_free(t6);

t1 = gsl_matrix_alloc(inv_p2->size1, q2->size2);
gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, inv_p2, q2, 0.0, t1);
t2 = gsl_matrix_alloc(inv_p2->size1, dq22->size2);
gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, inv_p2, dq22, 0.0, t2);
t3 = gsl_matrix_alloc(dq32->size1, t1->size2);
gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, dq32, t1, 0.0, t3);
t4 = gsl_matrix_alloc(q3->size1, t2->size2);
gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, q3, t2, 0.0, t4);
gsl_matrix_add(t3, t4);
gsl_matrix_sub(t3, dr32);
gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, inv_p3, t3, 0.0, t4);
t5 = gsl_matrix_alloc(e1->size1, t4->size2);
gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, e1, t4, 0.0, t5);
gsl_matrix_add(t5, db12); 
t6 = gsl_matrix_alloc(dc12->size1, t1->size2);
gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, dc12, t1, 0.0, t6);
gsl_matrix_sub(t5, t6); 
gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, c1, t2, 0.0, t6);
gsl_matrix_sub(t5, t6); 
gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, inv_a1, t5, 0.0, t6);
gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, pf1_nd_m, pf2_nd_m, 0.0, t5);
gsl_matrix_add(t5, t6); 
gsl_matrix_memcpy(pf12_m, t5);

gsl_matrix_set_identity(t5);
gsl_matrix_scale(t5, -g_s12);
gsl_matrix_add(pf12_m, t5);
gsl_matrix_free(t1);
gsl_matrix_free(t2);
gsl_matrix_free(t3);
gsl_matrix_free(t4);
gsl_matrix_free(t5);
gsl_matrix_free(t6);

t1 = gsl_matrix_alloc(inv_p2->size1, q2->size2);
gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, inv_p2, q2, 0.0, t1);
t2 = gsl_matrix_alloc(inv_p2->size1, dq22->size2);
gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, inv_p2, dq22, 0.0, t2);
t3 = gsl_matrix_alloc(dq32->size1, t1->size2);
gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, dq32, t1, 0.0, t3);
t4 = gsl_matrix_alloc(q3->size1, t2->size2);
gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, q3, t2, 0.0, t4);
gsl_matrix_add(t3, t4);
gsl_matrix_sub(t3, dr32);
gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, inv_p3, t3, 0.0, t4);
t5 = gsl_matrix_alloc(e2->size1, t4->size2);
gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, e2, t4, 0.0, t5);
gsl_matrix_add(t5, db22); 
t6 = gsl_matrix_alloc(dc22->size1, t1->size2);
gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, dc22, t1, 0.0, t6);
gsl_matrix_sub(t5, t6); 
gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, c2, t2, 0.0, t6);
gsl_matrix_sub(t5, t6); 
gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, inv_a2, t5, 0.0, t6);
gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, pf2_nd_m, pf2_nd_m, 0.0, t5);
gsl_matrix_add(t5, t6); 
gsl_matrix_memcpy(pf22_m, t5);

gsl_matrix_set_identity(t5);
gsl_matrix_scale(t5, -g_s22);
gsl_matrix_add(pf22_m, t5);
gsl_matrix_free(t1);
gsl_matrix_free(t2);
gsl_matrix_free(t3);
gsl_matrix_free(t4);
gsl_matrix_free(t5);
gsl_matrix_free(t6);

}

void invmat(gsl_matrix *m, gsl_matrix *invm)
{
	int n = m->size1;
	int s;
	gsl_permutation *p = gsl_permutation_alloc(n);
	gsl_matrix *old_m = gsl_matrix_alloc(m->size1, m->size2);
	gsl_matrix_memcpy(old_m, m);

	gsl_linalg_LU_decomp(m, p, &s);
	gsl_linalg_LU_invert(m, p, invm);

	gsl_permutation_free(p);
	gsl_matrix_memcpy(m, old_m);
	gsl_matrix_free(old_m);
}

int move_t(double x11, double x12, double x22, double y1, double y2, double xx11, double xx12, double xx22, double yy1, double yy2, double *val)
{
extern double g_x11;
extern double g_x12;
extern double g_x22;
extern double g_y1;
extern double g_y2;
extern double move_t_points[DIM];

double params[DIM];
params[0] = xx11 - x11;
params[1] = xx12 - x12;
params[2] = xx22 - x22;
params[3] = yy1 - y1;
params[4] = yy2 - y2;

const gsl_odeiv_step_type *T = ODEIV_STEP_TYPE;
gsl_odeiv_step *s = gsl_odeiv_step_alloc(T, N_VALUES);
gsl_odeiv_control *c = gsl_odeiv_control_y_new(1e-6, 0.0);
gsl_odeiv_evolve *e = gsl_odeiv_evolve_alloc(N_VALUES);
gsl_odeiv_system sys = {sys_t, NULL, N_VALUES, params};

double t = 0.0;
double h = 1e-6;
double hmin = 1e-6;

#ifdef DEBUG_RK
printf("Runge-Kutta: [%f,%f,%f,%f,%f] -> [%f,%f,%f,%f,%f]\n",x11, x12, x22, y1, y2, xx11, xx12, xx22, yy1, yy2);
#endif
while (t < 1.0) {
	int status = gsl_odeiv_evolve_apply(e, c, s, &sys, &t, 1.0, &h, val);
	if (status != GSL_SUCCESS) break;
	if (h < hmin) {
		printf("move_t : h is too small\n");
		move_t_points[0] = g_x11 + t * params[0];
		move_t_points[1] = g_x12 + t * params[1];
		move_t_points[2] = g_x22 + t * params[2];
		move_t_points[3] = g_y1 + t * params[3];
		move_t_points[4] = g_y2 + t * params[4];
		gsl_odeiv_evolve_free(e);
		gsl_odeiv_control_free(c);
		gsl_odeiv_step_free(s);
		return MOVE_T_FAIL;
	}
#ifdef DEBUG_RK
	printf("t = %f [%f,%f,%f,%f] : ", t, val[0],val[1],val[2],val[3]);
	printf("[%f,%f,%f,%f,%f]\n", g_x11 + t * params[0], g_x12 + t * params[1], g_x22 + t * params[2], g_y1 + t * params[3], g_y2 + t * params[4]);
#endif
}
gsl_odeiv_evolve_free(e);
gsl_odeiv_control_free(c);
gsl_odeiv_step_free(s);
return MOVE_T_SUCCESS;
}

int sys_t(double t, const double *y, double *val, double *params)
{
extern double g_x11;
extern double g_x12;
extern double g_x22;
extern double g_y1;
extern double g_y2;
extern double g_r;
extern gsl_matrix *pf11_m;
extern gsl_matrix *pf12_m;
extern gsl_matrix *pf22_m;
extern gsl_matrix *pf1_m;
extern gsl_matrix *pf2_m;
extern gsl_matrix *pft_m;

int i, j;
double n_x11, n_x12, n_x22, n_y1, n_y2, n_r;

gsl_matrix *t_m = gsl_matrix_alloc(pf11_m->size1, pf11_m->size2);

n_x11 = g_x11 + t * params[0];
n_x12 = g_x12 + t * params[1];
n_x22 = g_x22 + t * params[2];
n_y1 = g_y1 + t * params[3];
n_y2 = g_y2 + t * params[4];
n_r = g_r;
gsl_matrix_set_zero(pft_m);
pf_all(n_x11, n_x12, n_x22, n_y1, n_y2, n_r);
gsl_matrix_memcpy(t_m, pf11_m);
gsl_matrix_scale(t_m, params[0]);
gsl_matrix_add(pft_m, t_m);
gsl_matrix_memcpy(t_m, pf12_m);
gsl_matrix_scale(t_m, params[1]);
gsl_matrix_add(pft_m, t_m);
gsl_matrix_memcpy(t_m, pf22_m);
gsl_matrix_scale(t_m, params[2]);
gsl_matrix_add(pft_m, t_m);
gsl_matrix_memcpy(t_m, pf1_m);
gsl_matrix_scale(t_m, params[3]);
gsl_matrix_add(pft_m, t_m);
gsl_matrix_memcpy(t_m, pf2_m);
gsl_matrix_scale(t_m, params[4]);
gsl_matrix_add(pft_m, t_m);
for (i = 0; i < N_VALUES; i++) {
	val[i] = 0.0;
	for (j = 0; j < N_VALUES; j++)
		val[i] += gsl_matrix_get(pft_m, i, j) * y[j];
}
gsl_matrix_free(t_m);
return GSL_SUCCESS;
}

double *init_val(int dim, double x11, double x12, double x22, double y1, double y2, double s11, double s12, double s22, double s1, double s2)
{
double xxx[MAXSIZE][MAXSIZE];
xxx[0][0] = x11;
xxx[0][1] = x12/2;
xxx[1][0] = x12/2;
xxx[1][1] = x22;
double y[] = {y1, y2};
double exp_part = exp(-(s11 * x11 + s12 * x12 + s22 * x22 + s1 * y1 + s2 * y2));
int maxdeg = 10;
int weight[] = {1, 1, 1, 1};
double *val = fbnd(dim, xxx, y, maxdeg, weight);
int i;
for(i = 0; i < 2*dim+2; i++)
	val[i] = exp_part * val[i];
return val;
}

void search_min(double *val)
{
extern double g_x11;
extern double g_x12;
extern double g_x22;
extern double g_y1;
extern double g_y2;
extern double values[N_VALUES];
extern gsl_vector *grad_v;

size_t iter = 0;
int status;
const gsl_multimin_fdfminimizer_type *T;
gsl_multimin_fdfminimizer *s;
int move_t_status;
double t_x11;
double t_x12;
double t_x22;
double t_y1;
double t_y2;

int j;
for (j = 0; j < N_VALUES; j++)
	values[j] = val[j];
gsl_vector *x;
gsl_multimin_function_fdf my_func;
my_func.n = 5;
my_func.f = &my_f;
my_func.df = &my_df;
my_func.fdf = &my_fdf;
my_func.params = NULL;
x = gsl_vector_alloc(5);
gsl_vector_set(x, 0, g_x11);
gsl_vector_set(x, 1, g_x12);
gsl_vector_set(x, 2, g_x22);
gsl_vector_set(x, 3, g_y1);
gsl_vector_set(x, 4, g_y2);
T = MULTIMIN_FDFMINIMIZER_TYPE;
s = gsl_multimin_fdfminimizer_alloc(T, DIM);
gsl_multimin_fdfminimizer_set(s, &my_func, x, 0.01, 1e-4);
do {
	iter++;
	status = gsl_multimin_fdfminimizer_iterate(s);
	if (status) {
		if (status == GSL_ENOPROG)
			printf("GSL_ENOPROG : gsl_multimin_fdminimizer_iterate\n");
		break;
	}
	status = gsl_multimin_test_gradient(s->gradient, 1e-3);
	if (status == GSL_SUCCESS) printf("Minimum found\n");
	t_x11 = gsl_vector_get(s->x, 0);
	t_x12 = gsl_vector_get(s->x, 1);
	t_x22 = gsl_vector_get(s->x, 2);
	t_y1 = gsl_vector_get(s->x, 3);
	t_y2 = gsl_vector_get(s->x, 4);
	printf("%d, %g, %g, %g, %g, %g, %g\n", iter, t_x11, t_x12, t_x22, t_y1, t_y2, s->f);
	move_t_status = move_t(g_x11, g_x12, g_x22, g_y1, g_y2, t_x11, t_x12, t_x22, t_y1, t_y2, values);
	if (move_t_status == MOVE_T_SUCCESS) {
		g_x11 = t_x11;
		g_x12 = t_x12;
		g_x22 = t_x22;
		g_y1 = t_y1;
		g_y2 = t_y2;
	} else {
		printf("move_t failed\n");
		g_x11 = move_t_points[0];
		g_x12 = move_t_points[1];
		g_x22 = move_t_points[2];
		g_y1 = move_t_points[3];
		g_y2 = move_t_points[4];
	}
	printf("points = [%g, %g, %g, %g, %g]\n", g_x11, g_x12, g_x22, g_y1, g_y2);
	printf("values = [%g, %g, %g, %g]\n", values[0], values[1], values[2], values[3]);
	printf("grad : "); gsl_vector_show(grad_v);
	printf("norm(grad) : %f\n", gsl_blas_dnrm2(grad_v));
} while (status == GSL_CONTINUE && iter < SEARCH_MIN_ITER);
gsl_multimin_fdfminimizer_free(s);
gsl_vector_free(x);
}

double my_f(const gsl_vector *v, void *params)
{
extern double values[N_VALUES];
extern double g_x11, g_x12, g_x22, g_y1, g_y2;
double o_x11, o_x12, o_x22, o_y1, o_y2;
double n_x11, n_x12, n_x22, n_y1, n_y2;
double val[N_VALUES];
int i;
for (i = 0; i < N_VALUES; i++)
	val[i] = values[i];
o_x11 = g_x11;
o_x12 = g_x12;
o_x22 = g_x22;
o_y1 = g_y1;
o_y2 = g_y2;
n_x11 = gsl_vector_get(v, 0);
n_x12 = gsl_vector_get(v, 1);
n_x22 = gsl_vector_get(v, 2);
n_y1 = gsl_vector_get(v, 3);
n_y2 = gsl_vector_get(v, 4);
#ifdef DEBUG_RK
printf("call my_f([%g, %g, %g, %g, %g])\n", n_x11, n_x12, n_x22, n_y1, n_y2);
#endif
move_t(g_x11, g_x12, g_x22, g_y1, g_y2, n_x11, n_x12, n_x22, n_y1, n_y2, val);
g_x11 = o_x11;
g_x12 = o_x12;
g_x22 = o_x22;
g_y1 = o_y1;
g_y2 = o_y2;
return val[0];
}

void my_df(const gsl_vector *v, void *param, gsl_vector *df)
{
extern double g_x11, g_x12, g_x22, g_y1, g_y2;
extern double values[N_VALUES];
extern gsl_vector *grad_v;

double x11, x12, x22, y1, y2;
x11 = gsl_vector_get(v, 0);
x12 = gsl_vector_get(v, 1);
x22 = gsl_vector_get(v, 2);
y1 = gsl_vector_get(v, 3);
y2 = gsl_vector_get(v, 4);
#ifdef DEBUG_RK
printf("call my_df([%g, %g, %g, %g, %g])\n", x11, x12, x22, y1, y2);
#endif

/* 点 v での関数値を計算 */
double o_x11, o_x12, o_x22, o_y1, o_y2;
double val[N_VALUES];
int i;

/* move_all で更新されてしまうので、 g_* をコピー  */
for (i = 0; i < N_VALUES; i++)
	val[i] = values[i];
o_x11 =g_x11;
o_x12 =g_x12;
o_x22 =g_x22;
o_y1 = g_y1;
o_y2 = g_y2;

move_t(g_x11, g_x12, g_x22, g_y1, g_y2, gsl_vector_get(v, 0), gsl_vector_get(v, 1), gsl_vector_get(v, 2), gsl_vector_get(v, 3), gsl_vector_get(v, 4), val);
/* g_* を元の値に戻す */
g_x11 =o_x11;
g_x12 =o_x12;
g_x22 =o_x22;
g_y1 = o_y1;
g_y2 = o_y2;
grad(x11, x12, x22, y1, y2, 1, val);
for (i = 0; i < DIM; i++)
	gsl_vector_set(df, i, gsl_vector_get(grad_v, i));
}

void my_fdf(const gsl_vector *x, void *params, double *f, gsl_vector *df)
{
*f = my_f(x, params);
my_df(x, params, df);
}

void grad(double x11, double x12, double x22, double y1, double y2, double r, double *val)
{
extern gsl_vector *grad_v;
extern gsl_matrix *pf11_m, *pf12_m, *pf22_m, *pf1_m, *pf2_m;
int i;
double t;

for (i = 0, t = 0; i < N_VALUES; i++)
	t += gsl_matrix_get(pf11_m, 0, i) * val[i];
gsl_vector_set(grad_v, 0, t);

for (i = 0, t = 0; i < N_VALUES; i++)
	t += gsl_matrix_get(pf12_m, 0, i) * val[i];
gsl_vector_set(grad_v, 1, t);

for (i = 0, t = 0; i < N_VALUES; i++)
	t += gsl_matrix_get(pf22_m, 0, i) * val[i];
gsl_vector_set(grad_v, 2, t);

for (i = 0, t = 0; i < N_VALUES; i++)
	t += gsl_matrix_get(pf1_m, 0, i) * val[i];
gsl_vector_set(grad_v, 3, t);

for (i = 0, t = 0; i < N_VALUES; i++)
	t += gsl_matrix_get(pf2_m, 0, i) * val[i];
gsl_vector_set(grad_v, 4, t);

}

void gsl_vector_show(gsl_vector *v)
{
int i;
for (i = 0; i < v->size; i++)
printf("%f ", gsl_vector_get(v, i));
printf("\n");
}

void show_v(double *v, int n)
{
int i;
printf("[");
for (i = 0; i < n; i++) {
printf("%g", v[i]);
if (i < n - 1)
printf(", ");
}
printf("]\n");
}
