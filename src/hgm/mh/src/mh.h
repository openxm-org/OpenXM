/* $OpenXM: OpenXM/src/hgm/mh/src/mh.h,v 1.6 2013/03/07 05:23:31 takayama Exp $ */
struct cWishart {
  double x;
  double rank;
  double *f;  /* f[0],...,f[rank-1] */
  void *aux;
  void *aux2;
};

/* Cumulative probability distribution function of the first eigenvalue of
   Wishart matrix by Series */
struct cWishart *mh_cwishart_s(int m,int n,double beta[],double x0,
                               int approxDeg,double h, int dp, double x);
/* Cumulative probability distribution function of the first eigenvalue of
   Wishart matrix by HGM */
struct cWishart *mh_cwishart_hgm(int m,int n,double beta[],double x0,
                                 int approxDeg, double h, int dp, double x);
struct cWishart *mh_cwishart_gen(int m,int n,double beta[],double x0,
                                 int approxDeg,double h, int dp, double x,int modep[]); 
struct cWishart *new_cWishart(int rank);

int mh_usage(void);
void mh_freeWorkArea(void);
void mh_rf(double x, double *f, int rank_not_used, double *val, int n_not_used);
void mh_set_strategy(int s,double err[2]);

#define STRATEGY_DEFAULT 0
