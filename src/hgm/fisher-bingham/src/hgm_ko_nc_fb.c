/* 
 $OpenXM$
 The original source was tkoyama-initial.c,
 which evaluates the normalizing constant of the Fisher-Bingham distribution.
 gcc hgm_ko_fb.c -lm -lgsl -lblas  on orange 
*/
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<gsl/gsl_sf_gamma.h>
#include<gsl/gsl_errno.h>
#include<gsl/gsl_matrix.h>
#include<gsl/gsl_odeiv.h>
#include<gsl/gsl_eigen.h>


#define MAXSIZE 10      /* maximum size of the matrix x  */
#define RKERROR 1e-6   /* error of runge-kutta  */
#define NEWINITIAL
#define RANGE 1000
/*#define DEBUG_INITIAL*/
#define S_FNAME 1024

struct list{
  double c;
  struct list *next;
};

struct tkoyama_params
{
  int dim;
  gsl_matrix *pfs0, *pfs1;  
};

#if SAMPLE > 0
double *fbnd(int dim, double x[MAXSIZE][MAXSIZE], double y[], int maxdeg, int weight[]);
#else
double *fbnd(int dim, double **x, double *y, int maxdeg, int *weight);
#endif

double *fbnd_diag(int dim, double x1[], double y1[], int maxdeg, int *weight);
int rk_func (double t, const double y[], double f[], void *params);
struct tkoyama_params* set_pfs(int dim, double x[], double y[]);
int free_pfs(struct tkoyama_params *params);
double *fbnd_internal(int dim, double x[], double y[], double r, int maxdeg, int *weight);
struct list *mk_coeff(int dim, double r, int maxdeg, int *weight);
void free_coeff(struct list *coeff);
struct list *cons(double c, struct list *coeff);
double fbnd_internal_dyi(int dim, double x[], double y[], double r, int maxdeg, int *weight, struct list *coeff, int i);
double fbnd_internal_dyii(int dim, double x[], double y[], double r, int maxdeg, int *weight, struct list *coeff, int i);
int set_tdev(double);
static double eps_pert = 0.0;

#if SAMPLE == 0
int main(int argc, char *argv[])
{
  int dim;                /* dimension of the sphere */
  double r = 1.0;
  double Two;
  int maxdeg = 10;
  int i,j,k,M;
  FILE *fp;
  char fname[S_FNAME];
  double **X, *Xy, *Y;
  int *weight;

  if (argc<2) {usage(); return(-1);}
  for (k=1; k<argc; k++) {
    if (strcmp(argv[k],"--1")==0) {
      Two = 0.5;
    }else if (strcmp(argv[k],"--2")==0) {
      Two = 1.0;
    }else if (strcmp(argv[k],"--help")==0) {
      usage(); return(0);
    }else if (strcmp(argv[k],"--i")==0) {
      sscanf(argv[++k],"%lg",&eps_pert);
      printf("eps_pert=%lg\n", eps_pert);
      usage(); return(-1);
    }else if (strcmp(argv[k],"--",2)==0) {
      fprintf(stderr,"Unknown option\n");
      usage(); return(-1);
    }else{
      if (k==2){
        M=caldim(argc-2);
        if (M>0){
          dim=M-1;
          printf("dim: %d\n",dim);
          X = (double **)malloc(sizeof(double *) * M); 
          Xy = (double *)malloc(sizeof(double) * M*M); 
          Y  = (double *)malloc(sizeof(double) * M);
          for (i=0; i<M; i++) {
            X[i] = Xy + i*M;
            Y[i] = 0.0;
            for (j=0; j<M; j++){
              X[i][j] = 0.0;
	    }
          }
          weight = (int *)malloc(sizeof(int) * 2*M);
          for (i=0; i<2*M; i++) weight[i]=1;
        }else{
          fprintf(stderr,"argument mismatch!\n"); usage(); return(-1);
        }
      }
      for (i=0; i<M; i++) {
	for (j=i; j<M; j++) {
	  sscanf(argv[k],"%lf",&(X[i][j]));
          if (j>i) X[i][j] *= Two;
	  X[j][i] = X[i][j];
	  k++;
	  if (k >= argc) break;
	}
	if (k>= argc) break;
      }
      for (i=0; i<M; i++) {
	sscanf(argv[k],"%lf",&(Y[i]));
	k++;
	if (k >= argc) break;
      }
    }
  }

  for (i=0; i<M; i++) {
    for (j=0; j<M; j++)
      printf(" %lf",X[i][j]);
    printf(": %lf\n",Y[i]);
  }

  double *f = fbnd(dim, X, Y, maxdeg, weight);

  free(*X); free(X);
  free(Y);
  free(weight);

  snprintf(fname,S_FNAME,"tmp-out-fb%dd.txt",dim);
  fp = fopen(fname,"w");
  if (fp == NULL) {
    fprintf(stderr,"File open error.\n"); return(-1);
  }

  for(i=0; i<2*dim+2; i++){
    printf("f[%d]=%f\n", i, f[i]);
    fprintf(fp,"%lf\n",f[i]);
  }
  fclose(fp);
  return 0;
}

int caldim(int nv)
{
  int s=0,n=2;
  while (s<nv)
    if (nv==(s+=n++)) return(n-2);
  return(-1);
}

usage(void) {
  int i,j;
  int M=3;
  fprintf(stderr,"usage:\n");
  fprintf(stderr,"hgm_ko_nc_fb [--help] [--1 | --2] [");
  for (i=1; i<=M; i++){
    for (j=i; j<=M; j++)
      fprintf(stderr,"x%d%d ",i,j);
    fprintf(stderr,"... ");
  }
  fprintf(stderr,"... ");

  for (i=1; i<=M; i++) fprintf(stderr,"y%d ",i);
  fprintf(stderr,"... ]\n");
  fprintf(stderr,"The output data will be written to tmp-out-fbnd.txt\n");
}



#elif SAMPLE == 1
#elif SAMPLE == 2
int
main(int argc, char *argv[])
{
  int dim = 2;
  double x[MAXSIZE][MAXSIZE];
  x[0][0] =   1.1; x[0][1] = 1.2/2; x[0][2] = 1.3/2;
  x[1][0] = 1.2/2; x[1][1] =   2.2; x[1][2] = 2.3/2;
  x[2][0] = 1.3/2; x[2][1] = 2.3/2; x[2][2] =   3.3;
  double y[] = {0.1, 0.2, 0.3};
  double r = 1.0;
  int maxdeg = 20;
  int weight[] = {1,1,1,1,1,1};
  double *f = fbnd(dim, x, y, maxdeg, weight);
  int i;

  for(i=0; i<2*dim+2; i++){
	printf("f[%d]=%f\n", i, f[i]);
  }
  return 0;
}
#elif SAMPLE == 3
int
main(int argc, char *argv[])
{
  int dim = 3;
  double x[MAXSIZE][MAXSIZE];
  x[0][0] = 1.1  ; x[0][1] = 1.2/2; x[0][2] = 1.3/2; x[0][3] = 1.4/2;
  x[1][0] = 1.2/2; x[1][1] = 2.2  ; x[1][2] = 2.3/2; x[1][3] = 2.4/2;
  x[2][0] = 1.3/2; x[2][1] = 2.3/2; x[2][2] = 3.3  ; x[2][3] = 3.4/2;
  x[3][0] = 1.4/2; x[3][1] = 2.4/2; x[3][2] = 3.4/2; x[3][3] = 4.4;
  double y[] = {0.1, 0.2, 0.3, 0.4};
  double r = 1.0;
  int maxdeg = 10;
  int weight[] = {1,1, 1,1, 1,1 ,1,1};
  double *f = fbnd(dim, x, y, maxdeg, weight);
  int i;

  for(i=0; i<2*dim+2; i++){
	printf("f[%d]=%f\n", i, f[i]);
  }
  return 0;
}
#elif SAMPLE == 4
int
main(int argc, char *argv[])
{
  int dim = 4;
  double x[MAXSIZE][MAXSIZE];
  x[0][0] = 1.1  ; x[0][1] = 1.2/2; x[0][2] = 1.3/2; x[0][3] = 1.4/2; x[0][4] = 1.5/2;
  x[1][0] = 1.2/2; x[1][1] = 2.2  ; x[1][2] = 2.3/2; x[1][3] = 2.4/2; x[1][4] = 2.5/2;
  x[2][0] = 1.3/2; x[2][1] = 2.3/2; x[2][2] = 3.3  ; x[2][3] = 3.4/2; x[2][4] = 3.5/2;
  x[3][0] = 1.4/2; x[3][1] = 2.4/2; x[3][2] = 3.4/2; x[3][3] = 4.4;   x[3][4] = 4.5/2;
  x[4][0] = 1.5/2; x[4][1] = 2.5/2; x[4][2] = 3.5/2; x[4][3] = 4.5/2;   x[4][4] = 5.5;
  double y[] = {0.1, 0.2, 0.3, 0.4, 0.5};
  double r = 1.0;
  int maxdeg = 10;
  int weight[] = {1,1, 1,1, 1,1 ,1,1 ,1,1};
  double *f = fbnd(dim, x, y, maxdeg, weight);
  int i;

  for(i=0; i<2*dim+2; i++){
	printf("f[%d]=%f\n", i, f[i]);
  }
  return 0;
}

#elif SAMPLE == 5
/* 代表的な初期点の計算 s2-mag 対策*/
int
main(int argc, char *argv[])
{
  int dim;
  double x[MAXSIZE];
  double y[MAXSIZE];
  double r = 1.0;
  int maxdeg = 10;
  int weight[MAXSIZE];
  double *f;
  int i;
  double dd;

  for(i = 0; i< 2*MAXSIZE; i++)
	weight[i] = 1;

  for(i = 0; i< MAXSIZE; i++)
	y[i] = 1.0;

  for(dim= 1; dim <= 4; dim++){
	dd = (dim+1)*dim / 2.0;
	for(i = 0; i< dim+1; i++)
	  x[i] = (i+1) / dd;
  
	f = fbnd_internal(dim, x, y, 1.0, maxdeg, weight);  

	printf("dim =%d\n", dim);
	for(i=0; i<2*dim+2; i++){
	  printf("f[%d]=%f\n", i, f[i]);
	}
  }
  return 0;
}
/*
output:
dim =1
f[0]=14.723214
f[1]=23.272780
f[2]=17.794636
f[3]=29.081037
dim =2
f[0]=9.725688
f[1]=10.958040
f[2]=12.436225
f[3]=11.414821
f[4]=12.977707
f[5]=14.866002
dim =3
f[0]=9.593480
f[1]=10.085180
f[2]=10.617169
f[3]=11.193544
f[4]=11.011918
f[5]=11.618774
f[6]=12.277555
f[7]=12.993690
dim =4
f[0]=9.528839
f[1]=9.779101
f[2]=10.040078
f[3]=10.312348
f[4]=10.596525
f[5]=10.754281
f[6]=11.056731
f[7]=11.372700
f[8]=11.702940
f[9]=12.048250
*/


#endif


#if SAMPLE > 0
double*
fbnd(int dim, double x[MAXSIZE][MAXSIZE], double y[], int maxdeg, int weight[])
#else
double *fbnd(int dim, double **x, double *y, int maxdeg, int *weight)
#endif
{
  gsl_eigen_symmv_workspace *w = gsl_eigen_symmv_alloc( dim+1);
  gsl_matrix *A = gsl_matrix_alloc(dim+1, dim+1);
  gsl_vector *eval = gsl_vector_alloc(dim+1);
  gsl_matrix *evec = gsl_matrix_alloc(dim+1, dim+1);
  double xdiag[dim+1], ydiag[dim+1];
  double *f;
  double h[dim+1][dim+1];
  double *ans = (double *) malloc((2*dim+2)*sizeof(double));

  int i,j,k;

  for(i=0; i<dim+1; i++){
	for(j=0; j<dim+1; j++){
	  gsl_matrix_set(A, i, j, x[i][j]);
	}
  }

  gsl_eigen_symmv(A, eval, evec, w);
  gsl_eigen_symmv_free(w);
  gsl_eigen_symmv_sort(eval,evec, GSL_EIGEN_SORT_VAL_DESC);

  for(i=0; i<dim+1; i++){ xdiag[i] = gsl_vector_get(eval, i);}
  for(i=0; i<dim+1; i++){
	ydiag[i] = 0.0;
	for(j=0; j<dim+1; j++){
	  ydiag[i] += gsl_matrix_get(evec, j, i)*y[j]; /*	yd = q * y (q = p') */
	}
  }
  f = fbnd_diag(dim, xdiag,ydiag, maxdeg, weight);

  for(i=0; i<2*dim+2; i++){ ans[i] =0.0;}
  for(i=0; i<dim+1; i++){ ans[0] += f[i+dim+1];}
  for(i=0; i<dim+1; i++){
	for(j=0; j<dim+1; j++){
	  ans[i+1] += gsl_matrix_get(evec, i, j)*f[j];
	}
  }

  for(i=0; i<dim+1; i++){
	for(j = i+1; j<dim+1; j++){
	  h[i][j] = -(ydiag[i]*f[j]-ydiag[j]*f[i])/(xdiag[i]-xdiag[j]);
	}
  }
  
  for(i=0; i<dim; i++){
	for(j=0; j<dim+1; j++){
	  ans[i+dim+2] += gsl_matrix_get(evec, i,j)*gsl_matrix_get(evec, i,j)*f[j+dim+1];
	}
	for(j=0; j<dim+1; j++){
	  for(k = j+1; k<dim+1; k++){
		ans[i+dim+2] += gsl_matrix_get(evec, i,j)*gsl_matrix_get(evec, i,k)*h[j][k];
	  }
	}
  }	

  printf("return of the function fbnd:\n[");
  for(i = 0; i< 2*dim+1; i++){
	printf("\t %g,", ans[i]);
  }
  printf("\t %g]\n\n", ans[i]);

  gsl_matrix_free(A);
  gsl_matrix_free(evec);
  gsl_vector_free(eval);
  free(f);

  return ans;
}

int 
set_tdev(double r)
{
  double rr = r*r;

  if(rr > RANGE){
    return (int)(rr/RANGE) ;
  } else {
    return 1;
  }
}

double*
fbnd_diag(int dim, double x1[], double y1[], int maxdeg, int *weight)
{
  int i, status;
  double sum, r;
  double x[dim+1], y[dim+1];
  double *g, tmp;

  sum = 0.0;
  for(i=0; i<dim+1; i++){
	sum += fabs(x1[i]) + y1[i]*y1[i];
  }
  r = sum;
  for(i=0; i<dim+1; i++){
	x[i] = x1[i]/(r*r); 
	y[i] = y1[i]/r;
  }

  g = fbnd_internal(dim, x, y, 1.0, maxdeg, weight);

  tmp = exp(-x[0]);
  for(i=0; i<2*dim+2; i++){
	g[i] = g[i]*tmp; 
  }

  char sss[10];
  double t,t1, dt;
  double h;
  double rkerror2;
  int ii;

  int tdev;
  tdev = set_tdev(r);
  dt = (r*r-1.0)/tdev;

  printf("r*r = %f\n", r*r);
  /*  scanf("%s", sss);*/
  for(ii =0; ii < tdev; ii++){
	struct tkoyama_params *params = set_pfs(dim, x, y);
	
	const gsl_odeiv_step_type *T = gsl_odeiv_step_rkf45;  
	gsl_odeiv_step * s = gsl_odeiv_step_alloc (T, 2*dim+2);
	gsl_odeiv_control * c;
	if(ii == 0 ){
	  c = gsl_odeiv_control_y_new (RKERROR, 0.0); 
	}else{
	  /*	  printf("g[0] = %f\n", g[0]);*/
	  rkerror2 = fabs(g[0])/100000;
	  c = gsl_odeiv_control_y_new (rkerror2, 0.0); 
	}
	gsl_odeiv_evolve * e = gsl_odeiv_evolve_alloc (2*dim+2);
	gsl_odeiv_system sys = {rk_func, NULL, 2*dim+2, params};

	t = 1.0+ ii*dt;   t1 = 1.0+(ii+1)*dt;
	h = 1e-6;
	
	printf("ii = %d,\t s = %f -> %f \n", ii,t, t1);
	while (t < t1){
	  status = gsl_odeiv_evolve_apply (e, c, s, &sys, &t, t1, &h, g);
	  if (status != GSL_SUCCESS) break;
#ifdef DEBUG_INITIAL
	  printf("s = %f [", t);
	  for(i = 0; i< 2*dim+1; i++)
	  	printf("%g, ", g[i]);
	  printf("%g]\n ", g[i]);
	  /*
        printf("[%f,%f,%f,%f,%f,%f,%f,%f,%f]\n", g_x11 + t * params[0], g_x12 + t * params[1], g_x13 + t * params[2], g_x22 + t * params[3], g_x23 + t * params[4], g_x33 + t * params[5], g_y1 + t * params[6], g_y2 + t * params[7], g_y3 + t * params[8]);
	  */
#endif
	}
	
	gsl_odeiv_evolve_free (e);
	gsl_odeiv_control_free (c);
	gsl_odeiv_step_free (s);
	free_pfs(params);
	/*	if(ii%1 == 0){
	  printf("enter");
	  scanf("%s", sss);
	  }*/
  }

  tmp = exp(r*r*x[0]);
  for(i=0; i<2*dim+2; i++){g[i] *=  tmp;}

  tmp = pow(r, dim+1);
  for(i=0; i<dim+1; i++){g[i] /= tmp;  }
  tmp *= r;
  for(i=0; i<dim+1; i++){g[i+dim+1] /= tmp;  }

  return g;
}

int
rk_func (double s, const double y[], double f[], void *params)
{
  struct tkoyama_params *prms = (struct tkoyama_params*) params;
  int dim = prms->dim;
  gsl_matrix *pfs0 = prms->pfs0;
  gsl_matrix *pfs1 = prms->pfs1;
  int i, j;

  for (i = 0; i < 2*dim+2; i++) {
		f[i] = 0.0;
		for (j = 0; j < 2*dim+2; j++)
			f[i] += (gsl_matrix_get(pfs0, i, j)+ gsl_matrix_get(pfs1, i, j)/(2*s) )* y[j];
	}

  return GSL_SUCCESS;

}

struct tkoyama_params*
set_pfs(int dim, double x[], double y[])
{
  struct tkoyama_params *ans = (struct tkoyama_params *)malloc(sizeof(struct tkoyama_params));
  gsl_matrix *pfs0, *pfs1;
  int i,j;

  pfs0 = gsl_matrix_alloc(2*dim+2, 2*dim+2);
  pfs1 = gsl_matrix_alloc(2*dim+2, 2*dim+2);

  gsl_matrix_set_zero(pfs0);
  gsl_matrix_set_zero(pfs1);

  for(i=1; i<dim+1; i++){
	gsl_matrix_set(pfs0,       i,       i, x[i]-x[0]);
	gsl_matrix_set(pfs0, i+dim+1, i+dim+1, x[i]-x[0]);
  }
  for(i=0; i<dim+1; i++){
	gsl_matrix_set(pfs0, i+dim+1,       i, y[i]/2);
	gsl_matrix_set(pfs1,       i,       i,      1);
	gsl_matrix_set(pfs1, i+dim+1, i+dim+1,      2);
  }
  for(i=0; i<dim+1; i++){
	  for(j=0; j<dim+1; j++){
		gsl_matrix_set(pfs1, i, j+dim+1, y[i]);
		if(i!=j) gsl_matrix_set(pfs1, i+dim+1, j+dim+1, 1);
	  }
  }

  ans->dim = dim;
  ans->pfs0 = pfs0;
  ans->pfs1 = pfs1;

  return ans;
}

int
free_pfs(struct tkoyama_params *params)
{
  gsl_matrix_free(params->pfs0);
  gsl_matrix_free(params->pfs1);
  free(params);

  return 1;
}


double*
fbnd_internal(int dim, double x[], double y[], double r, int maxdeg, int *weight)
{
  double *ans = (double*) malloc((2*dim+2)*sizeof(double));
  double s = 2*pow(M_PI,((dim+1)/2.0)) / gsl_sf_gamma((dim+1)/2.0);
  struct list *coeff = mk_coeff(dim,r,maxdeg, weight);;
  int i;

#ifdef DEBUG_INITIAL
  printf("fbnd_interal:runge-kutta initial value \n");
#endif
  for(i=0; i<dim+1; i++){
	ans[i] = s*pow(r,dim)*fbnd_internal_dyi(dim,x,y,r,maxdeg, weight,coeff,i);
#ifdef DEBUG_INITIAL
	printf("ans[%2d] = %g \n", i, ans[i]);
#endif
  }

  for(i=0; i<dim+1; i++){
	ans[i+dim+1] = s*pow(r,dim)*fbnd_internal_dyii(dim,x,y,r,maxdeg, weight,coeff,i);
#ifdef DEBUG_INITIAL
	printf("ans[%2d] = %g \n", i+dim+1 , ans[i+dim+1]);
#endif
  }
#ifdef DEBUG_INITIAL
  printf("\n");
#endif

  free_coeff(coeff);
  return ans;
}


struct list*
mk_coeff(int dim, double r, int maxdeg, int *weight)
{
  struct list *coeff = NULL;
  int maxdp = 2*dim+2;
  int max = maxdeg;
  double ans[maxdp+1];
  int s[maxdp+1];
  int idx[maxdp];
  int sum[maxdp];
  int dp=0;
  int i;

  ans[0] = 1;
  s[0] = 0;
  for(i=0; i<maxdp; i++){idx[i] = 0;}
  sum[0] = 0;
  
  while(dp > -1){
	if(sum[dp] < max+1){
	  if(dp == maxdp-1){ 
		coeff = cons( ans[dp], coeff);
	  }else{
		dp++;
		sum[dp] = sum[dp-1];
		s[dp] = s[dp-1];
		ans[dp] = ans[dp-1];
		continue;
	  }
	}else{
	  dp--;
	  if(dp == -1){
		break;
	  }else{
		idx[dp+1] = 0;
	  }
	}
	
	if(dp < dim+1){
	  ans[dp] = ans[dp]*r*r*(2*idx[dp]+2*idx[dp+dim+1]+1)/(dim+1+2*s[dp]);
	} else {
	  ans[dp] = ans[dp]*r*r*(2*idx[dp-dim-1]+2*idx[dp]+1)/(dim+1+2*s[dp]);
	}
	s[dp+1] = 0;
	
	idx[dp]++;
	sum[dp] = sum[dp] + weight[dp];
	s[dp]++;
  }
  
  return coeff;	
}

void 
free_coeff(struct list *coeff)
{
  struct list  *tmp;

  while(coeff != NULL){
	tmp = coeff->next;
	free(coeff);
	coeff = tmp;
  }
}

struct list*
cons(double c, struct list *coeff)
{
  struct list *ans = malloc(sizeof(struct list));

  ans->c = c;
  ans->next = coeff;
  return ans;
}

double 
fbnd_internal_dyi(int dim, double x[], double y[], double r, int maxdeg, int *weight, struct list *coeff, int i)
{
  int maxdp = 2*dim+2;
  int max = maxdeg;
  double ans[maxdp+1];
  int s[maxdp+1];
  int idx[maxdp];
  int sum[maxdp];
  int dp=0;
  int ic;

  for(ic=0; ic<maxdp; ic++){idx[ic] = ans[ic] = 0;}

  idx[0] = max / weight[0];
  sum[0] = idx[0]*weight[0];
  while(dp > -1){
	if(idx[dp] > -1){
	  if(dp == maxdp-1){ 
		ans[dp+1] = coeff->c;
		coeff = coeff->next;
	  }else{
		dp++;
		idx[dp] = (max - sum[dp-1]) /  weight[dp];
		sum[dp] = sum[dp-1] + idx[dp]*weight[dp];
		continue;
	  }
	}else {
	  dp--;
	  if(dp == -1){
		break;
	  }
	}
	
	if(dp == dim+1+i){
	  if(idx[dp] > 1){
		ans[dp] = (ans[dp] + ans[dp+1]) * y[dp -(dim +1)]*y[dp -(dim+1)] /(double)( (2*idx[dp]-1)*(2*idx[dp]-2));
	  } else if(idx[dp] == 1){
		ans[dp] = (ans[dp] + ans[dp+1]) * y[dp -(dim +1)];
	  }
	}else {
	  if(idx[dp] > 0){
		if(dp < dim+1){
		  ans[dp] = (ans[dp] + ans[dp+1]) * x[dp]/idx[dp];
		} else {
		  ans[dp] = (ans[dp] + ans[dp+1]) * y[dp -(dim +1)]*y[dp -(dim+1)] /(double)( 2*idx[dp]*(2*idx[dp]-1));
		}
	  }else if(idx[dp] == 0){
		ans[dp] = ans[dp] + ans[dp+1];
	  }
	}
	ans[dp+1] = 0;
	
	idx[dp]--;
	sum[dp] = sum[dp] - weight[dp];
  }
  
  return ans[0];
}

double 
fbnd_internal_dyii(int dim, double x[], double y[], double r, int maxdeg, int *weight, struct list *coeff, int i)
{
  int maxdp = 2*dim+2;
  int max = maxdeg;
  double ans[maxdp+1];
  int s[maxdp+1];
  int idx[maxdp];
  int sum[maxdp];
  int dp=0;
  int ic;

  for(ic=0; ic<maxdp; ic++){idx[ic] = ans[ic] = 0;}

  idx[0] = max / weight[0];
  sum[0] = idx[0]*weight[0];
  while(dp > -1){
	if(idx[dp] > -1){
	  if(dp == maxdp-1){ 
		ans[dp+1] = coeff->c;
		coeff = coeff->next;
	  }else{
		dp++;
		idx[dp] = (max - sum[dp-1])/ weight[dp];
		sum[dp] = sum[dp-1] + idx[dp]*weight[dp];
		continue;
	  }
	}else {
	  dp--;
	  if(dp == -1){
		break;
	  }
	}
	
	if(dp == dim+1+i){
	  if(idx[dp] > 1){
		ans[dp] = (ans[dp] + ans[dp+1]) * y[dp -(dim +1)]*y[dp -(dim+1)] /( (2*idx[dp]-2)*(2*idx[dp]-3));
	  } else if(idx[dp] == 1){
		ans[dp] = ans[dp] + ans[dp+1];
	  }
	}else {
	  if(idx[dp] > 0){
		if(dp < dim+1){
		  ans[dp] = (ans[dp] + ans[dp+1]) * x[dp]/idx[dp];
		} else {
		  ans[dp] = (ans[dp] + ans[dp+1]) * y[dp -(dim +1)]*y[dp -(dim+1)] /( 2*idx[dp]*(2*idx[dp]-1));
		}
	  }else if(idx[dp] == 0){
		ans[dp] = ans[dp] + ans[dp+1];
	  }
	}
	
	ans[dp+1] = 0;

	idx[dp]--;
	sum[dp] = sum[dp] - weight[dp];
  }
  
  return ans[0];
}

