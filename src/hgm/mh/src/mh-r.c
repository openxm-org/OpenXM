/* $OpenXM: OpenXM/src/hgm/mh/src/mh-r.c,v 1.5 2013/03/05 07:03:37 takayama Exp $
 R interface module
*/

#include <stdio.h>
#include "sfile.h"
#include "mh.h"
#define WSIZE 1024

int Rmh_cwishart_gen(int *mp,int *np,double *beta,double *x0p,
		int *approxDegp,double *hp, int *dpp, double *xp,
		 int *modep,int *rankp,double *xy) {
  struct cWishart *cw;
  int rank;
  int i;
  rank = *rankp;
  cw = mh_cwishart_gen(*mp,*np,beta,*x0p,*approxDegp,*hp,*dpp,*xp,modep);
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
