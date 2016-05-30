/* $OpenXM: OpenXM/src/hgm/mh/src/mh-r.c,v 1.9 2016/02/13 05:55:09 takayama Exp $
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
