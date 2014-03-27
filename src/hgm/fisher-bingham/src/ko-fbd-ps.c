/**********************************************
ko-fbd-ps.c 
      Calculating the power series
 *********************************************/
#include<stdlib.h>
#include<math.h>
#include<gsl/gsl_sf_gamma.h> /* The gamma function is used to calclate the area of the sphere. */

int maxdeg = 10;
extern int dim; /* defined in ko-fbd.c  */
static int *weight=NULL;

struct lcoeff{
  double c;
  struct lcoeff *next;
};

static double 
pow_series_dyi(const double x[],const double y[],const double r, struct lcoeff *coeff,const int i)
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

static double 
pow_series_dyii(const double x[],const double y[],const double r,  struct lcoeff *coeff,const int i)
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

static struct lcoeff*
cons(double c, struct lcoeff *coeff)
{
  struct lcoeff *ans = malloc(sizeof(struct lcoeff));

  ans->c = c;
  ans->next = coeff;
  return ans;
}

static struct lcoeff*
mk_coeff(const double r)
{
  struct lcoeff *coeff = NULL;
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

static void 
free_coeff(struct lcoeff *coeff)
{
  struct lcoeff  *tmp;

  while(coeff != NULL){
	tmp = coeff->next;
	free(coeff);
	coeff = tmp;
  }
}

static void
set_weight(void)
{
  int i;

  weight = (int *)malloc(sizeof(int) * 2*(dim+1)); /* setting the weight vector */
  for (i=0; i<2*dim+2; i++) weight[i]=1;
}

void
pow_series(const double *x,const double *y,const double r, double *ans)
{
  double s = 2*pow(M_PI,((dim+1)/2.0)) / gsl_sf_gamma((dim+1)/2.0); /* the area of sphere */
  set_weight();
  struct lcoeff *coeff = mk_coeff(r);
  int i;

  for(i=0; i<dim+1; i++){
	ans[i] = s*pow(r,dim)*pow_series_dyi(x,y,r,coeff,i);
  }
  for(i=0; i<dim+1; i++){
	ans[i+dim+1] = s*pow(r,dim)*pow_series_dyii(x,y,r, coeff,i);
  }
  free(weight);
  free_coeff(coeff);
}
