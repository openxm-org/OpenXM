/* $OpenXM: OpenXM/src/hgm/mh/src/mh-r.c,v 1.2 2013/03/01 05:26:25 takayama Exp $
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
  cw = mh_cwishart_gen(*mp,*np,beta,*x0p,*approxDegp,*hp,*dpp,*xp,*modep);
  xy[0] = cw->x;
  for (i=1; i<=rank; i++) xy[i] = (cw->f)[i-1];
}
