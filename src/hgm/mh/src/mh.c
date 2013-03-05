/* $OpenXM: OpenXM/src/hgm/mh/src/mh.c,v 1.5 2013/02/25 12:11:23 takayama Exp $ */
#include <stdio.h>
#include "sfile.h"
#include "mh.h"
#define WSIZE 1024
extern int MH_DEBUG;
static imypower(int x,int n) {
  int a,i;
  a = 1;
  for (i=0; i<n; i++) a = a*x;
  return(a);
}

struct cWishart *new_cWishart(int rank) {
  struct cWishart *cwp;
  cwp = (struct cWishart *)mh_malloc(sizeof(struct cWishart));
  cwp->x=0;
  cwp->rank=rank;
  cwp->f = (double *)mh_malloc(sizeof(double)*rank);
  cwp->aux = cwp->aux2 = NULL;
  return(cwp);
}

struct cWishart *mh_cwishart_gen(int m,int n,double beta[],double x0,
			      int approxDeg,double h, int dp, double x,int modep[]) {
  /*
     modep[0]. Do Koev-Edelman (ignored for now).
     modep[1]. Do the HGM
     modep[2]. Return modep[2] intermediate data.
   */
  struct SFILE *fp;
  char swork[WSIZE];
  char *argv[WSIZE];
  int i,rank;
  char *comm;
  struct MH_RESULT *rp;
  struct SFILE *sfp;
  struct cWishart *cw;
  argv[0]="dummy";
  argv[1] = "--bystring";
  argv[2] = "--idata";
  fp = mh_fopen("","w",0);
  mh_fputs("%%Mg=\n",fp);
  sprintf(swork,"%d\n",m); mh_fputs(swork,fp);
  rank = imypower(2,m);
  mh_fputs("%%Beta\n",fp);
  for (i=0; i<m; i++) {
    sprintf(swork,"%lf\n",beta[i]); mh_fputs(swork,fp);
  }
  mh_fputs("%%Ng=\n",fp); /* freedom param */
  sprintf(swork,"%d\n",n); mh_fputs(swork,fp);
  mh_fputs("%%X0g=\n",fp); /* initial point */
  sprintf(swork,"%lf\n",x0); mh_fputs(swork,fp);
  mh_fputs("%%Iv\n",fp); /* initial values, dummy */
  for (i=0; i<rank; i++) mh_fputs("0.0\n",fp);
  mh_fputs("%%Ef=\n1.0\n",fp); /* Below are dummy values */
  if (h <= 0.0) {fprintf(stderr,"h<=0.0, set to 0.1\n"); h=0.1;}
  mh_fputs("%%Hg=\n",fp); 
  sprintf(swork,"%lf\n",h); mh_fputs(swork,fp);
  if (dp < 1) {fprintf(stderr,"dp<1, set to 1\n"); dp=1;}
  mh_fputs("%%Dp=\n",fp); 
  sprintf(swork,"%d\n",dp); mh_fputs(swork,fp);
  if (x <= x0) {fprintf(stderr,"x <= x0, set to x=x0+10\n"); x=x0+10;}
  mh_fputs("%%Xng=\n",fp);
  sprintf(swork,"%lf\n",x); mh_fputs(swork,fp);

  comm = (char *)mh_malloc(fp->len +1);
  mh_outstr(comm,fp->len+1,fp);
  mh_fclose(fp);
  argv[3] = comm;

  argv[4] = "--degree";
  argv[5] = (char *)mh_malloc(128);
  sprintf(argv[5],"%d",approxDeg);

  rp=jk_main(6,argv); 
  if (rp == NULL) {
    fprintf(stderr,"rp is NULL.\n"); return(NULL);
  }
  cw = new_cWishart(rank);
  cw->x = rp->x;
  cw->rank = rp->rank;
  if (rank !=  cw->rank) {
    fprintf(stderr,"Rank error.\n"); return(NULL);
  }
  for (i=0; i<cw->rank; i++) (cw->f)[i] = (rp->y)[i];  
  sfp = (rp->sfpp)[0]; 
  cw->aux = (char *) mh_malloc((sfp->len)+1); 
  mh_outstr((char *)(cw->aux),(sfp->len)+1,sfp);
  /* todo, the following line seems to cause seg fault. */
  /* deallocate the memory */
  for (i=0; i<rp->size; i++) mh_fclose((rp->sfpp)[i]);
  /* todo, mh_free_??(rp);  free Iv's */
  if (!modep[1]) return(cw);

  if (MH_DEBUG) printf("\n\n%s\n",(char *)cw->aux);
  /* This output is strange. */
  /* Starting HGM */
  argv[3] = (char *)cw->aux;
  argv[4] = "--dataf";
  argv[5] = "dummy-dataf";
  rp = mh_main(6,argv);
  if (rp == NULL) {
    fprintf(stderr,"rp is NULL in the second step.\n"); return(NULL);
  }
  cw = new_cWishart(rank);
  cw->x = rp->x;
  cw->rank = rp->rank;
  for (i=0; i<cw->rank; i++) (cw->f)[i] = (rp->y)[i];
  sfp = (rp->sfpp)[0];
  if (sfp) {
    cw->aux = (char *) mh_malloc(sfp->len+1);
    mh_outstr((char *)cw->aux,sfp->len+1,sfp);
  }

  sfp = (rp->sfpp)[1];
  if (sfp) {
    cw->aux2 = (char *) mh_malloc(sfp->len+1);
    mh_outstr((char *)cw->aux2,sfp->len+1,sfp);
  }
  /* deallocate the memory */
  for (i=0; i<rp->size; i++) mh_fclose((rp->sfpp)[i]);
  mh_freeWorkArea();
  return(cw);
}
/* Cumulative probability distribution function of the first eigenvalue of
   Wishart matrix by Series */
struct cWishart *mh_cwishart_s(int m,int n,double beta[],double x0,
			       int approxDeg,double h, int dp, double x) {
  int modep[]={1,0,0};
  return(mh_cwishart_gen(m,n,beta,x0,approxDeg,h,dp,x,modep));
}

/* Cumulative probability distribution function of the first eigenvalue of
   Wishart matrix by HGM */
struct cWishart *mh_cwishart_hgm(int m,int n,double beta[],double x0,
				 int approxDeg, double h, int dp , double x)
{
  int modep[]={1,1,0};
  return(mh_cwishart_gen(m,n,beta,x0,approxDeg,h,dp,x,modep));
}

#ifdef STANDALONE
main() {
  double beta[5]={1.0,2.0,3.0,4.0,5.0};
  struct cWishart *cw;
  cw=mh_cwishart_hgm(3,5,beta,0.3,7,  0.01,1,10);
  if (cw != NULL) {
    printf("x=%lf, y=%lf\n",cw->x,(cw->f)[0]);
    /* printf("%s",(char *)cw->aux); */
  } 
  cw=mh_cwishart_hgm(4,5,beta,0.3,7,  0.01,1,10);
  if (cw != NULL) {
    printf("x=%lf, y=%lf\n",cw->x,(cw->f)[0]);
    /* printf("%s",(char *)cw->aux); */
  }
}
main1() {
  double beta[5]={1.0,2.0,3.0,4.0,5.0};
  struct cWishart *cw;
  cw=mh_cwishart_s(3,5,beta,0.3,7,  0,0,0);
  if (cw != NULL) {
    printf("%s",(char *)cw->aux);
  }
  cw=mh_cwishart_s(4,5,beta,0.3,7,  0,0,0);
  if (cw != NULL) {
    printf("%s",(char *)cw->aux);
  }
  cw=mh_cwishart_s(5,5,beta,0.3,7,  0,0,0);
  if (cw != NULL) {
    printf("%s",(char *)cw->aux);
  }
}
#endif
