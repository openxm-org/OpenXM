/* $OpenXM: OpenXM/src/hgm/mh/src/mh.h,v 1.12 2016/02/13 02:18:59 takayama Exp $ */
#include "oxprint.h"
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
                               int approxDeg,double h, int dp, double x,
                               int automatic,double assigned_series_error,
                               int verbose);
/* Cumulative probability distribution function of the first eigenvalue of
   Wishart matrix by HGM */
struct cWishart *mh_cwishart_hgm(int m,int n,double beta[],double x0,
                                 int approxDeg, double h, int dp, double x,
                               int automatic,double assigned_series_error,
                               int verbose);
struct cWishart *mh_cwishart_gen(int m,int n,double beta[],double x0,
                                 int approxDeg,double h, int dp, double x,int modep[],
                               int automatic,double assigned_series_error,
                               int verbose);
struct cWishart *new_cWishart(int rank);
struct cWishart *mh_pFq_gen(int m,
			    int p, double a[],
			    int q, double b[],
			    int ef_type,
			    double beta[],double x0,
                            int approxDeg,double h, int dp, double x,int modep[],
                               int automatic,double assigned_series_error,
                               int verbose);

int mh_usage(void);
void mh_freeWorkArea(void);
void mh_rf_ef_type_1(double x, double *f, int rank_not_used, double *val, int n_not_used);
void mh_rf_ef_type_2(double x, double *f, int rank_not_used, double *val, int n_not_used);
void mh_set_strategy(int s,double err[2]);
void mh_setA(double a[],int alen);
void mh_setB(double b[],int blen);

#define STRATEGY_DEFAULT 1
/* Four significant digits for checking MH_abserr. cf. M_assigned_series_error */
#define SIGDIGIT_DEFAULT ((M_ASSIGNED_SERIES_ERROR_DEFAULT)*10.0)

typedef void (*MH_RF)(double x, double *f, int rank_not_used, double *val, int n_not_used);

#define MH_SMAX  4096
