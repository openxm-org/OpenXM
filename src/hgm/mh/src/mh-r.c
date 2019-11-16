/* $OpenXM: OpenXM/src/hgm/mh/src/mh-r.c,v 1.11 2017/04/01 12:01:25 takayama Exp $
 R interface module
*/

#include <stdio.h>
#include "sfile.h"
#include "mh.h"
#define WSIZE 1024

int Rmh_cwishart_gen(int *mp,int *np,double *beta,double *x0p,
                     int *approxDegp,double *hp, int *dpp, double *xp,
                     int *modep,int *rankp,
                     int *automaticp,double *assigned_series_errorp,int *verbosep,
                     double *xy) {
  struct cWishart *cw;
  int rank;
  int i;
  reset_SAR_warning(1);
  rank = *rankp;
  cw = mh_cwishart_gen(*mp,*np,beta,*x0p,*approxDegp,*hp,*dpp,*xp,modep,
                       *automaticp,*assigned_series_errorp,*verbosep);
  xy[0] = cw->x;
  for (i=1; i<=rank; i++) xy[i] = (cw->f)[i-1];

  if ((modep[2] > 0) && cw->aux) {
    struct SFILE *sfp3;
    char *s3;
    char str[1024];
    double x;
    s3 = (char *)cw->aux;
    sfp3 = mh_fopen(s3,"r",0);
    for (i=cw->rank+1; i<modep[2]+(cw->rank)+1; i++) xy[i] = 0.0;
    for (i=cw->rank+1; i<modep[2]+(cw->rank)+1; i++) {
      if (!mh_fgets(str,1024,sfp3)) break;
      sscanf(str,"%lg",&x); 
      xy[i] = x;
    }
  }
  return(0);
}

int Rmh_set_strategy(int *m,double *err,double *ans) {
  mh_set_strategy(*m,err);
  ans[0] = 0.0;
  return(0);
}

int Rmh_pFq_gen(int *mp,
                int *pp, double *a,
                int *qp, double *b,
                int *ef_typep,
                double *beta,double *x0p,
                int *approxDegp,double *hp, int *dpp, double *xp,
                int *modep,int *rankp,
                int *automaticp,double *assigned_series_errorp,int *verbosep,
                double *xy) {
  struct cWishart *cw;
  int rank;
  int i;
  reset_SAR_warning(1);
  rank = *rankp;
  cw = mh_pFq_gen(*mp,
                  *pp,a,
                  *qp,b,
                  *ef_typep,
                  beta,*x0p,*approxDegp,*hp,*dpp,*xp,modep,
                  *automaticp,*assigned_series_errorp,*verbosep);
  xy[0] = cw->x;
  for (i=1; i<=rank; i++) xy[i] = (cw->f)[i-1];

  if ((modep[2] > 0) && cw->aux) {
    struct SFILE *sfp3;
    char *s3;
    char str[1024];
    double x;
    s3 = (char *)cw->aux;
    sfp3 = mh_fopen(s3,"r",0);
    for (i=cw->rank+1; i<modep[2]+(cw->rank)+1; i++) xy[i] = 0.0;
    for (i=cw->rank+1; i<modep[2]+(cw->rank)+1; i++) {
      if (!mh_fgets(str,1024,sfp3)) break;
      sscanf(str,"%lg",&x); 
      xy[i] = x;
    }
  }
  return(0);
}

void so3_main(double *in1,double *in2,double *in3,double *t0p,int *quiet,int *deg,int *log,double *out);
void hgm_ko_orthant(int *, double *, double *, double *retv);
static const R_CMethodDef CallEntries[] = {
  {"Rmh_cwishart_gen", (DL_FUNC) &Rmh_cwishart_gen, 14},
  {"Rmh_set_strategy", (DL_FUNC) &Rmh_set_strategy, 3},
  {"Rmh_pFq_gen", (DL_FUNC) &Rmh_pFq_gen,18},
  {"so3_main", (DL_FUNC) &so3_main,8},
  {"hgm_ko_orthant", (DL_FUNC) &hgm_ko_orthant,4},
  {NULL}
};
  /* Sample.
  {"ggdmc_getAccumulatorMatrix", (DL_FUNC) &ggdmc_getAccumulatorMatrix, 4},
  {"ggdmc_ddmc", (DL_FUNC) &ggdmc_ddmc, 4},
  */
void R_init_hgm(DllInfo *dll) {
  R_registerRoutines(dll, CallEntries, NULL, NULL, NULL);
  R_useDynamicSymbols(dll, FALSE);
}
