#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cblas.h>

#define XY (0)
#define XYT (dim*(dim+1))
#define SIGMA_MU (2*dim*(dim+1))
#define DYGI (3*dim*(dim+1))

double hgm_ko_orthant(int dimension, double *sigma, double *mu, double rk_step_size);

static int dim;
static int rank;

static void set_xy(double *x, double *y, const double *sigma, const double *mu);
static void set_initial_g(double *g, const double *x);
static void update_xy(double t, double *param);
static void cal_sigmaI_muI(int I, double *param);
static void cal_dygI(int I, const double *g, double *param);
static double get_dxgI(const int i, const int j, const int I, const double *g, const double *param);
static int function(double t, const double g[], double dg[], void *param);
static void runge_kutta(double *g, const double *x, const double *y, const double rk_step_size);
static double get_prob(double g, const double *sigma, const double *mu);

#ifdef _DEBUG
/* for debugs */
void test_dgetrf_(void);
void test_dpotri_(void);
static void test_function(double t, const double g[], double dg[], void *param);
void test_runge_kutta(void);
void test_update_xy(void);
void test_cal_sigmaI_muI(void);

static void subst(double *Pfaff, double t);

void test1(void);
void test2(void);
void test3(void);
#endif

double 
hgm_ko_orthant(int dimension, double *sigma, double *mu, double rk_step_size)
{
  dim = dimension;
  rank = 1 << dim;

  double g[rank], x[dim*dim], y[dim];
  set_xy(x,y,sigma,mu);
  set_initial_g(g, x);
  runge_kutta(g, x, y, rk_step_size);
  return get_prob(g[rank-1], sigma, mu);
}

static void 
set_xy(double *x, double *y, const double *sigma, const double *mu)
{
  int info;

  /* x <- -0.5 * (sigma)^(-1)  */
  /* y <- (sigma)^(-1) * mu  */
  cblas_dcopy(dim*dim, sigma,1, x,1);
  dpotrf_("U", &dim, x, &dim, &info);
  dpotri_("U", &dim, x, &dim, &info);
  cblas_dsymv(CblasColMajor, CblasUpper, dim, 1.0,x,dim, mu,1, 0.0,y,1);
  cblas_dscal(dim*dim, -0.5, x, 1);
}

static void 
set_initial_g(double *g, const double *x)
{
  int i,I;
  for(I=0; I<rank; I++){
    g[I] = 1.0;
    for(i=0; i<dim; i++)
      if(I&1<<i)
	g[I] *= M_PI/(-4*x[i+i*dim]);
    g[I] = sqrt(g[I]);
  }
}

static void 
update_xy(double t, double *param)
{
  cblas_dcopy(dim*(dim+1), param+XY,1,  param+XYT,1);
  cblas_dscal(dim*(dim+1), t, param+XYT, 1);
  cblas_dcopy(dim, param+XY,dim+1, param+XYT,dim+1);
}

static void 
cal_sigmaI_muI(int I, double *param)
{
  int i,j;
  for (i = 0; i < dim; i++){
    if (I & (1<<i)){
      for (j = 0; j < dim; j++){
	if (I & (1<<j)){
	  param[SIGMA_MU+j+i*dim] = param[XYT+j+i*dim];
	} else {
	  param[SIGMA_MU+j+i*dim] = 0.0;
	}
      }
    } else {
      cblas_dscal(dim, 0.0, param+SIGMA_MU+i*dim, 1);
      param[SIGMA_MU+i+i*dim] = 1.0;
    } 
  }
  for (i = 0; i < dim; i++){
    if (I & (1<<i)){
      param[SIGMA_MU+dim*dim+i] = param[XYT+dim*dim+i];
    } else {
      param[SIGMA_MU+dim*dim+i] = 0.0;
    }
  }

  int info;
  double mu[dim];
  cblas_dscal(dim*dim, -2.0, param+SIGMA_MU, 1);
  dpotrf_("U", &dim, param+SIGMA_MU, &dim, &info);
  dpotri_("U", &dim, param+SIGMA_MU, &dim, &info);
  cblas_dsymv(CblasColMajor, CblasUpper, dim, 1.0,param+SIGMA_MU,dim, param+SIGMA_MU+dim*dim,1, 0.0,mu,1);
  cblas_dcopy(dim, mu,1, param+SIGMA_MU+dim*dim,1);
}

static void 
cal_dygI(int I, const double *g, double *param)
{
  double *delVI = param + DYGI;
  double *sigmaI = param + SIGMA_MU;
  double *muI = param + SIGMA_MU + dim*dim;
  int i,j;
  double *p = delVI + I*dim;
  for(i=0; i<dim; i++){
    *p = 0.0;
    if(I&1<<i){
      *p += muI[i] * g[I];
      for(j=0; j<i; j++)
	if(I&1<<j)
	  *p += sigmaI[j+i*dim] * g[I&~(1<<j)];
      for(j=i; j<dim; j++)
	if(I&1<<j)
	  *p += sigmaI[i+j*dim] * g[I&~(1<<j)];
    }
    p++;
  }
  return;
}

static double 
get_dxgI(const int p, const int q,const int I, const double *g, const double *param)
{
  double result;
  int i;
  const double *delVI = param + DYGI;
  const double *sigmaI = param + SIGMA_MU;
  const double *muI = param + SIGMA_MU + dim*dim;

  result = sigmaI[q+p*dim] * g[I];
  result += muI[p] * delVI[q+I*dim];
  for(i=0; i<p; i++)
    if((I&1<<i) && i!=q)
      result += sigmaI[i+p*dim] * delVI[q+ (I&~(1<<i))*dim];
  for(i=p; i<dim; i++)
    if((I&1<<i) && i!=q)
      result += sigmaI[p+i*dim] * delVI[q+ (I&~(1<<i))*dim];
  if(p!=q)
    result *= 2.0;
  return result;
}

static int
function(double t, const double g[], double dg[], void *param)
{
  update_xy(t, (double *) param);
  int I, i, j;
  double *delVI = (double *)param + DYGI;
  double *x = (double *)param;
  double *y = (double *)param + dim*dim;

  dg[0] = 0.0;
  for (I = 1; I < rank; I++){
    dg[I] = 0.0;
    cal_sigmaI_muI(I, (double *)param);
    cal_dygI(I, g, (double *)param);
    for(i=0; i<dim; i++){
      if(I&1<<i){
	dg[I] += delVI[i+I*dim] * y[i];
	for(j=0; j<i; j++) /* X0[i][i] == X1[i][i] */
	  if(I&1<<j)
	    dg[I] += get_dxgI(i,j,I,g, (double *)param) * x[j+i*dim];
      }
    }
  }
  return 1;
}

static void 
runge_kutta(double *g, const double *x, const double *y, const double rk_step_size)
{
  /* g'(t) = F(t, g(t)) */
  double dg[rank], k1[rank],k2[rank], k3[rank], k4[rank];
  double t = 0.0; /* initial point */
  double h = rk_step_size;
  double h2 = 0.5*h;
  double param[3*dim*(dim+1)+rank*dim];
  cblas_dcopy(dim*dim, x,1, param,1);
  cblas_dcopy(dim, y,1, param+dim*dim,1);

  while (t < 1.0){
    /* k1 <- F(t, g) , dg += k1 */
    function(t, g, k1, param);
    cblas_dcopy(rank, k1,1, dg,1);

    /* k2 <- F(t+0.5*h,g+0.5*h*k1), dg += 2*k2 */
    cblas_dscal(rank, h2, k1, 1);
    cblas_daxpy(rank, 1.0, g,1, k1,1);
    function(t+h2, k1, k2, param);
    cblas_daxpy(rank, 2.0, k2,1, dg,1);

    /* k3 <- F(t+0.5*h,g+0.5*h*k2), dg += 2*k3 */
    cblas_dscal(rank, h2, k2, 1);
    cblas_daxpy(rank, 1.0, g,1, k2,1);
    function(t+h2, k2, k3, param);
    cblas_daxpy(rank, 2.0, k3,1, dg,1);

    /* k4 <- F(t+h, g+h*k3), dg += k4 */
    cblas_dscal(rank, h, k3, 1);
    cblas_daxpy(rank, 1.0, g,1, k3,1);
    function(t+h2, k3, k4, param);
    cblas_daxpy(rank, 1.0, k4,1, dg,1);

    /* g += h * dg /6 */
    cblas_daxpy(rank, h/6.0, dg,1, g,1);
    t += h;
    //    fprintf(stderr, "%lf %lf %lf %lf\n", g[0], g[1], g[2], g[3]);
  }
  return;
}

static double 
get_prob(double g, const double *sigma0, const double *mu)
{
  double C = 1.0;
  C *= 1.0 / pow(2.0*M_PI, 0.5*dim);

  double sigma[dim*dim];
  int info;
  cblas_dcopy(dim*dim, sigma0,1, sigma,1);
  dpotrf_("U", &dim, sigma, &dim, &info);

  double det_sigma = 1.0;
  int i;
  for (i = 0; i < dim; i++)
    det_sigma *= sigma[i+i*dim];
  C *= 1.0 / det_sigma;

  dpotri_("U", &dim, sigma, &dim, &info);
  double tmp[dim];
  cblas_dsymv(CblasColMajor, CblasUpper, dim, 1.0,sigma,dim, mu,1, 0.0,tmp,1);
  double tmp2 = exp(-0.5 * cblas_ddot(dim, mu, 1, tmp, 1));
  C *= tmp2;
  return C*g;
}

#ifdef _DEBUG
void test_dgetrf_(void)
{
  //  double a[4] = { 1.0, 0.5, 1.0, 0.5};
  double a[4] = { 1.0, 2.0, 3.0, 4.0};
  int n = 2;
  int ipiv[n+1];
  int info;

  dgetrf_(&n, &n, a, &n, ipiv, &info);
  printf("a=[%lf %lf %lf %lf]\n", a[0], a[1], a[2], a[3]);
  printf("ipiv=[%d %d %d]\n", ipiv[0], ipiv[1], ipiv[2]);
  printf("info=%d\n", info);
}

void test_dpotri_(void)
{
  double a[4] = { 1.0, 0.5, 0.5, 1.0};
  //double a[4] = { 1.0, 0.0, 0.0, 1.0};
  char s[] = "U"; /* or "L" */
  int n = 2;
  int info;

  printf("a=[%lf %lf %lf %lf]\n", a[0], a[1], a[2], a[3]);
  dpotrf_(s, &n, a, &n, &info);
  printf("a=[%lf %lf %lf %lf]\n", a[0], a[1], a[2], a[3]);
  printf("info=%d\n", info);
  dpotri_(s, &n, a, &n, &info);
  printf("a=[%lf %lf %lf %lf]\n", a[0], a[1], a[2], a[3]);
  printf("info=%d\n", info);
}

static void 
test_function(double t, const double g[], double dg[], void *param)
{
  dg[0] = g[0];
  dg[1] = -g[1];
}

void 
test_runge_kutta(void)
{
  /* for g(t) = [exp(t), exp(-t)], t = 0.0 -> 1.0 */
  /* g'(t) = F(t, g(t)) */
  double g[2] = {1.0, 1.0}; /* initial value */
  rank = 2;
  double dg[rank], k1[rank],k2[rank], k3[rank], k4[rank];
  double t = 0.0; /* initial point */
  double h = 0.0001;
  double h2 = 0.5*h;

  while (t < 1.0){
    /* k1 <- F(t, g) , dg += k1 */
    test_function(t, g, k1, NULL);
    cblas_dcopy(rank, k1,1, dg,1);

    /* k2 <- F(t+0.5*h,g+0.5*h*k1), dg += 2*k2 */
    cblas_dscal(rank, h2, k1, 1);
    cblas_daxpy(rank, 1.0, g,1, k1,1);
    test_function(t+h2, k1, k2, NULL);
    cblas_daxpy(rank, 2.0, k2,1, dg,1);

    /* k3 <- Pfaff(t+h/2) * (phi + h/2*k2), dphi += 2*k3 */
    cblas_dscal(rank, h2, k2, 1);
    cblas_daxpy(rank, 1.0, g,1, k2,1);
    test_function(t+h2, k2, k3, NULL);
    cblas_daxpy(rank, 2.0, k3,1, dg,1);

    /* k3 <- Pfaff(t+h) * (phi + h*k3), dphi += k4 */
    cblas_dscal(rank, h, k3, 1);
    cblas_daxpy(rank, 1.0, g,1, k3,1);
    test_function(t+h2, k3, k4, NULL);
    cblas_daxpy(rank, 1.0, k4,1, dg,1);

    /* phi += h * dphi /6 */
    cblas_daxpy(rank, h/6.0, dg,1, g,1);
    t += h;
  }
  printf("g(1)=[%lf %lf]\n", g[0], g[1]);
}

void 
test_update_xy(void)
{
  dim = 2;
  double params[12] = {1.0,2.0,3.0,4.0, 5.0,6.0, 0,0,0,0, 0,0};
  update_xy(0.5, params);
  int i;
  for ( i = 0; i < 12; i++)
    printf("%lf ", params[i]);
  printf("\n");
}

void 
test_cal_sigmaI_muI(void)
{
  dim = 2;
  double param[18] = {0,0,0,0,0,0, -0.5,0,0,-0.5,1,0, 0,0,0,0,0,0};
  //double param[18] = {0,0,0,0,0,0, 2,1,1,5,1,2, 0,0,0,0,0,0};
  //double param[18] = {0,0,0,0,0,0, 1,0.5,0.5,1,1,2, 0,0,0,0,0,0};
  cal_sigmaI_muI(3, param);
  printf("%lf %lf\n%lf %lf\n%lf %lf\n\n", param[12],param[14],param[13],param[15],param[16],param[17]);
  cal_sigmaI_muI(2, param);
  printf("%lf %lf\n%lf %lf\n%lf %lf\n\n", param[12],param[14],param[13],param[15],param[16],param[17]);
  cal_sigmaI_muI(1, param);
  printf("%lf %lf\n%lf %lf\n%lf %lf\n\n", param[12],param[14],param[13],param[15],param[16],param[17]);
  return;
}

void 
subst(double Pfaff[], double t)
{
  Pfaff[24] = -0.5 * t; /* 3 + 3*7 */
  Pfaff[40] = -t;       /* 5 + 5*7 */
  Pfaff[48] = -t;       /* 6 + 6*7 */
}


#define TEST1rank 7


void test1()
{
  double phi[TEST1rank] = {0, 0, 0, 0, 1, 1, 1};
  double Pfaff[TEST1rank*TEST1rank] = {
    0, 0, 0, 1, 0,       0,       0,
    0, 0, 0, 0, 0,       0,       1,
    0, 0, 0, 0, 0,       1,       0,
    0, 0, 0, 0, 0, 1.0/2.0, 1.0/2.0,
    0, 0, 0, 0, 0,       0,       0,
    0, 0, 0, 0, 0,       0,       0,
    0, 0, 0, 0, 0,       0,       0
  };
  double t = 0.0;
  double h = 0.001;
  double h2 = 0.5*h;

  double k1[TEST1rank],k2[TEST1rank],k3[TEST1rank],k4[TEST1rank],dphi[TEST1rank];

  int i;

  while (t < 10){
    /* k1 <- Pfaff(t) * phi , dphi += k1 */
    subst(Pfaff,t); 
    cblas_dgemv(CblasRowMajor, CblasNoTrans,TEST1rank,TEST1rank, 1.0,Pfaff,TEST1rank, phi,1, 0.0,k1,1);
    cblas_dcopy(TEST1rank, k1,1, dphi,1);

    /* k2 <- Pfaff(t+h/2) * (phi + h/2*k1), dphi += 2*k2 */
    subst(Pfaff,t+h2); 
    cblas_dscal(TEST1rank, h2, k1, 1);
    cblas_daxpy(TEST1rank, 1.0, phi,1, k1,1);
    cblas_dgemv(CblasRowMajor, CblasNoTrans,TEST1rank,TEST1rank, 1.0,Pfaff,TEST1rank, k1,1, 0.0,k2,1);
    cblas_daxpy(TEST1rank, 2.0, k2,1, dphi,1);

    /* k3 <- Pfaff(t+h/2) * (phi + h/2*k2), dphi += 2*k3 */
    cblas_dscal(TEST1rank, h2, k2, 1);
    cblas_daxpy(TEST1rank, 1.0, phi,1, k2,1);
    cblas_dgemv(CblasRowMajor, CblasNoTrans,TEST1rank,TEST1rank, 1.0,Pfaff,TEST1rank, k2,1, 0.0,k3,1);
    cblas_daxpy(TEST1rank, 2.0, k3,1, dphi,1);

    /* k3 <- Pfaff(t+h) * (phi + h*k3), dphi += k4 */
    subst(Pfaff,t+h); 
    cblas_dscal(TEST1rank, h, k3, 1);
    cblas_daxpy(TEST1rank, 1.0, phi,1, k3,1);
    cblas_dgemv(CblasRowMajor, CblasNoTrans,TEST1rank,TEST1rank, 1.0,Pfaff,TEST1rank, k3,1, 0.0,k4,1);    
    cblas_daxpy(TEST1rank, 1.0, k4,1, dphi,1);

    /* phi += h * dphi /6 */
    cblas_daxpy(TEST1rank, h/6.0, dphi,1, phi,1);
    t += h;
  }
  
  phi[0] *= 1.0 / (2*M_PI);
  phi[1] *= 1.0 / sqrt(2*M_PI);
  phi[2] *= 1.0 / sqrt(2*M_PI);
  phi[3] *= 1.0 / sqrt(2*M_PI);

  for (i = 0; i < TEST1rank; i++)
    printf("%lf ", phi[i]);
  printf("\n");


  return;
}

void 
test2(void)
{
  /*
    g:	   1.0000000000    1.2533141373    1.2533141373    1.5707963268 
    probability=0.250000000
    double sigma[4] = { 1.0, 0.0, 0.0, 1.0};
    double mu[2] = {0.0, 0.0};
    OK!

    g:	   1.0000000000    3.4770518132    1.2533141373    4.3578381936 
    probability=0.420672373
    double sigma[4] = { 1.0, 0.0, 0.0, 1.0};
    double mu[2] = {1.0, 0.0};
    OK!

    result = 0.707860982 
    g:	   1.0000000000    3.4770518117    3.4770518117   12.0898893056 
    double sigma[4] = { 1.0, 0.0, 0.0, 1.0};
    double mu[2] = {1.0, 1.0};
    OK!

    g:	   1.0000000000    2.5481580894    1.0854018818    5.6544969464 
    probability=0.630283928
  */
  double sigma[4] = { 1.0, 0.5, 0.5, 1.0};
  double mu[2] = {1.0, 0.5};

  dim = 2;
  rank = 1 << dim;
  printf("dim=%d, rank=%d\n", dim, rank);

  double x[dim*dim], y[dim];
  set_xy(x,y,sigma,mu);
  printf("x=[%lf %lf]\n", x[0], x[2]);
  printf("  [%lf %lf]\n",  0.0, x[3]);
  printf("y=[%lf %lf]\n", y[0], y[1]);

  double g[rank];
  set_initial_g(g, x);
  printf("g=[%lf %lf %lf %lf]\n", g[0], g[1], g[2], g[3]);

  runge_kutta(g, x, y, 1e-03);
  printf("g=[%lf %lf %lf %lf]\n", g[0], g[1], g[2], g[3]);

  double result = get_prob(g[rank-1], sigma, mu);
  printf("prob=%lf\n", result);
  return ;
}

void 
test3(void)
{
  /*
    g:	   1.0000000000    2.5481580894    1.0854018818    5.6544969464 
    probability=0.630283928
  */
  double sigma[4] = { 1.0, 0.5, 0.5, 1.0};
  double mu[2] = {1.0, 0.5};

  //set_rk_step_size(1e-6);
  double prob = hgm_ko_orthant(2, sigma, mu, 1e-3);
  printf("%lf\n", prob);
}
#endif
