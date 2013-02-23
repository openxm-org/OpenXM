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
struct cWishart *new_cWishart(int rank);