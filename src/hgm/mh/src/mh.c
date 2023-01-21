/* $OpenXM: OpenXM/src/hgm/mh/src/mh.c,v 1.18 2016/02/15 07:42:07 takayama Exp $ */
#include <stdio.h>
#include <string.h>
#include "sfile.h"
#include "mh.h"
#define WSIZE 1024
int MH_DEBUG2=0;
extern int MH_DEBUG;
extern int M_show_autosteps;
static int imypower(int x,int n) {
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
				 int approxDeg,double h, int dp, double x,int modep[],
                               int automatic,double assigned_series_error,
                               int verbose)
{
  /*
     modep[0]. Do Koev-Edelman (ignored for now).
     modep[1]. Do the HGM
     modep[2]. Return modep[2] intermediate data.
   */
  struct SFILE *fp;
  char swork[WSIZE];
  char *argv[WSIZE];
  int argc;
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
  snprintf(swork,WSIZE,"%d\n",m); mh_fputs(swork,fp);
  rank = imypower(2,m);
  mh_fputs("%%Beta\n",fp);
  for (i=0; i<m; i++) {
    snprintf(swork,WSIZE,"%lf\n",beta[i]); mh_fputs(swork,fp);
  }
  mh_fputs("%%Ng=\n",fp); /* freedom param */
  snprintf(swork,WSIZE,"%d\n",n); mh_fputs(swork,fp);
  mh_fputs("%%X0g=\n",fp); /* initial point */
  snprintf(swork,WSIZE,"%lf\n",x0); mh_fputs(swork,fp);
  mh_fputs("%%Iv\n",fp); /* initial values, dummy */
  for (i=0; i<rank; i++) mh_fputs("0.0\n",fp);
  mh_fputs("%%Ef=\n1.0\n",fp); /* Below are dummy values */
  if (h <= 0.0) {oxprintfe("h<=0.0, set to 0.1\n"); h=0.1;}
  mh_fputs("%%Hg=\n",fp); 
  snprintf(swork,WSIZE,"%lf\n",h); mh_fputs(swork,fp);
  mh_fputs("%%Dp=\n",fp); 
  snprintf(swork,WSIZE,"%d\n",dp); mh_fputs(swork,fp);
  if (x <= x0) {oxprintfe("x <= x0, set to x=x0+10\n"); x=x0+10;}
  mh_fputs("%%Xng=\n",fp);
  snprintf(swork,WSIZE,"%lf\n",x); mh_fputs(swork,fp);

  snprintf(swork,WSIZE,"%%automatic=%d\n",automatic); mh_fputs(swork,fp);
  snprintf(swork,WSIZE,"%%assigned_series_error=%lg\n",assigned_series_error); mh_fputs(swork,fp);
  snprintf(swork,WSIZE,"%%show_autosteps=%d\n",verbose); mh_fputs(swork,fp);

  comm = (char *)mh_malloc(fp->len +1);
  mh_outstr(comm,fp->len+1,fp);
  mh_fclose(fp);
  argv[3] = comm;

  argv[4] = "--degree";
  argv[5] = (char *)mh_malloc(128);
  snprintf(argv[5],128,"%d",approxDeg);

  rp=jk_main(6,argv); 
  if (rp == NULL) {
    oxprintfe("rp is NULL.\n"); return(NULL);
  }
  cw = new_cWishart(rank);
  cw->x = rp->x;
  cw->rank = rp->rank;
  if (rank !=  cw->rank) {
    oxprintfe("Rank error.\n"); return(NULL);
  }
  for (i=0; i<cw->rank; i++) (cw->f)[i] = (rp->y)[i];  
  sfp = (rp->sfpp)[0]; 
  cw->aux = (char *) mh_malloc((sfp->len)+1); 
  mh_outstr((char *)(cw->aux),(sfp->len)+1,sfp);
  /* todo, the following line seems to cause seg fault. */
  /* deallocate the memory */
  //for (i=0; i<rp->size; i++) mh_fclose((rp->sfpp)[i]);
  /* todo, mh_free_??(rp);  free Iv's */
  if (!modep[1]) return(cw);

  if (MH_DEBUG) oxprintf("\n\n%s\n",(char *)cw->aux);
  /* This output is strange. */
  /* Starting HGM */
  argv[3] = (char *)cw->aux;
  argv[4] = "--dataf";
  argv[5] = "dummy-dataf";
  argc=6;
  if (verbose) {
    argv[6] = "--verbose"; ++argc;
  }
  rp = mh_main(argc,argv);
  if (rp == NULL) {
    oxprintfe("rp is NULL in the second step.\n"); return(NULL);
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
                               int approxDeg,double h, int dp, double x,
                               int automatic,double assigned_series_error,
                               int verbose)
{
  int modep[]={1,0,0};
  return(mh_cwishart_gen(m,n,beta,x0,approxDeg,h,dp,x,modep,automatic,assigned_series_error,verbose));
}

/* Cumulative probability distribution function of the first eigenvalue of
   Wishart matrix by HGM */
struct cWishart *mh_cwishart_hgm(int m,int n,double beta[],double x0,
                                 int approxDeg, double h, int dp , double x,
                               int automatic,double assigned_series_error,
                               int verbose)
{
  int modep[]={1,1,0};
  return(mh_cwishart_gen(m,n,beta,x0,approxDeg,h,dp,x,modep,automatic,assigned_series_error,verbose));
}

struct cWishart *mh_pFq_gen(int m,
                            int p, double a[],
                            int q, double b[],
                            int ef_type,
                            double beta[],double x0,
                            int approxDeg,double h, int dp, double x,int modep[],
                               int automatic,double assigned_series_error,
                            int verbose) {
  /*
     modep[0]. Do Koev-Edelman (ignored for now).
     modep[1]. Do the HGM
     modep[2]. Return modep[2] intermediate data.
   */
  struct SFILE *fp;
  char swork[WSIZE];
  char *argv[WSIZE];
  int argc;
  int i,rank;
  char *comm;
  struct MH_RESULT *rp;
  struct SFILE *sfp;
  struct cWishart *cw;
  argv[0]="dummy";
  argv[1] = "--bystring";
  argv[2] = "--idata";
  fp = mh_fopen("","w",0);
  mh_fputs("%!version2.0\n",fp);
  mh_fputs("%Mg=\n",fp);
  snprintf(swork,WSIZE,"%d\n",m); mh_fputs(swork,fp);
  rank = imypower(q+1,m);

  snprintf(swork,WSIZE,"%%p_pFq=%d ",p); mh_fputs(swork,fp);
  for (i=0; i<p; i++) {
    snprintf(swork,WSIZE,"%lg ",a[i]); mh_fputs(swork,fp);
  }
  mh_fputs("\n",fp);

  snprintf(swork,WSIZE,"%%q_pFq=%d ",q); mh_fputs(swork,fp);
  for (i=0; i<q; i++) {
    snprintf(swork,WSIZE,"%lg ",b[i]); mh_fputs(swork,fp);
  }
  mh_fputs("\n",fp);

  snprintf(swork,WSIZE,"%%ef_type=%d\n",ef_type); mh_fputs(swork,fp);

  mh_fputs("%Beta=\n",fp);
  for (i=0; i<m; i++) {
    snprintf(swork,WSIZE,"%lf\n",beta[i]); mh_fputs(swork,fp);
  }
  mh_fputs("%X0g=\n",fp); /* initial point */
  snprintf(swork,WSIZE,"%lf\n",x0); mh_fputs(swork,fp);
  if (h <= 0.0) {oxprintfe("h<=0.0, set to 0.1\n"); h=0.1;}
  mh_fputs("%Hg=\n",fp); 
  snprintf(swork,WSIZE,"%lf\n",h); mh_fputs(swork,fp);
  mh_fputs("%Dp=\n",fp); 
  snprintf(swork,WSIZE,"%d\n",dp); mh_fputs(swork,fp);
  if (x <= x0) {oxprintfe("x <= x0, set to x=x0+10\n"); x=x0+10;}
  mh_fputs("%Xng=\n",fp);
  snprintf(swork,WSIZE,"%lf\n",x); mh_fputs(swork,fp);

  snprintf(swork,WSIZE,"%%automatic=%d\n",automatic); mh_fputs(swork,fp);
  snprintf(swork,WSIZE,"%%assigned_series_error=%lg\n",assigned_series_error); mh_fputs(swork,fp);
  snprintf(swork,WSIZE,"%%show_autosteps=%d\n",verbose); mh_fputs(swork,fp);

  comm = (char *)mh_malloc(fp->len +1);
  mh_outstr(comm,fp->len+1,fp);
  mh_fclose(fp);
  argv[3] = comm;
  if (MH_DEBUG2) oxprintf("comm=\n%s",comm); 

  argv[4] = "--degree";
  argv[5] = (char *)mh_malloc(128);
  snprintf(argv[5],128,"%d",approxDeg);

  rp=jk_main(6,argv); 
  if (rp == NULL) {
    oxprintfe("rp is NULL.\n"); return(NULL);
  }
  cw = new_cWishart(rank);
  cw->x = rp->x;
  cw->rank = rp->rank;
  if (rank !=  cw->rank) {
    oxprintfe("Rank error.\n"); return(NULL);
  }
  for (i=0; i<cw->rank; i++) (cw->f)[i] = (rp->y)[i];  
  sfp = (rp->sfpp)[0]; 
  cw->aux = (char *) mh_malloc((sfp->len)+1); 
  mh_outstr((char *)(cw->aux),(sfp->len)+1,sfp);
  /* todo, the following line seems to cause seg fault. */
  /* deallocate the memory */
  //for (i=0; i<rp->size; i++) mh_fclose((rp->sfpp)[i]);
  /* todo, mh_free_??(rp);  free Iv's */
  if (!modep[1]) return(cw);

  if (MH_DEBUG2) oxprintf("\n\n%s\n",(char *)cw->aux);
  /* This output is strange. */
  /* Starting HGM */
  argv[3] = (char *)cw->aux;
  argv[4] = "--dataf";
  argv[5] = "dummy-dataf";
  argc=6;
  if (verbose) {
    argv[6] = "--verbose"; ++argc;
  }
  rp = mh_main(argc,argv);
  if (rp == NULL) {
    oxprintfe("rp is NULL in the second step.\n"); return(NULL);
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


#ifdef STANDALONE
int main(int argc,char *argv[]) {
  double beta[5]={1.0,2.0,3.0,4.0,5.0};
  struct cWishart *cw;
  struct SFILE *sfp;
  char *s;
  char str[1024];
  double x;
  int i,show;
  int strategy=1;
  double err[2]={-1.0,-1.0};
  int verbose=0;
  int testnew=0;
  show=1;
  for (i=1; i<argc; i++) {
	if (strcmp(argv[i],"--strategy")==0) {
	  i++; sscanf(argv[i],"%d",&strategy);
	}else if (strcmp(argv[i],"--abserr")==0) {
	  i++; sscanf(argv[i],"%lg",&(err[0]));
	}else if (strcmp(argv[i],"--relerr")==0) {
	  i++; sscanf(argv[i],"%lg",&(err[1]));
	}else if (strcmp(argv[i],"--quiet")==0) {
	  show=0;
	}else if (strcmp(argv[i],"--verbose")==0) {
	  verbose=1;
	}else if (strcmp(argv[i],"--testnew")==0) {
	  testnew=1;
	}else{
	  oxprintfe("Unknown option.\n"); return(-1);
	}
  }
  if (testnew) {
    int tmodep[] = {1,1,0};
    double a[] = {2.0,7.5};
    double b[] = {4.5};
    double beta[] = {1,2,4};
    mh_set_strategy(strategy,err);
    /* Testdata/new-2016-02-04-4-in.txt, Hashiguchi note 2016.02.03 */
    cw=mh_pFq_gen(3, /* m */
                  2,a,
                  1,b,
                  2, /* ef_type */
                  beta,0.1,
                  8,0.001,1, 4,tmodep,
                  1,1e-20,verbose);
    if (cw != NULL) {
      oxprintf("x=%lf, y=%lf\n",cw->x,(cw->f)[0]);
      /* oxprintf("%s",(char *)cw->aux); */
    }
    return(0);
  }
  mh_set_strategy(strategy,err);
  cw=mh_cwishart_hgm(3,5,beta,0.3,7,  0.01,1,10, 1,1e-7,verbose);
  if (cw != NULL) {
    oxprintf("x=%lf, y=%lf\n",cw->x,(cw->f)[0]);
    /* oxprintf("%s",(char *)cw->aux); */
  }
  cw=mh_cwishart_hgm(4,5,beta,0.3,7,  0.01,1,10, 1,1e-7,verbose);
  if (cw != NULL) {
    oxprintf("x=%lf, y=%lf\n",cw->x,(cw->f)[0]);
    s = (char *)cw->aux;
    /* oxprintf("%s",(char *)cw->aux); */
    if (show) {
      sfp = mh_fopen(s,"r",0);
      while (mh_fgets(str,1024,sfp)) {
        sscanf(str,"%lg",&x); oxprintf("%lg\n",x);
      }
      mh_fclose(sfp);
    }
  }
  return(0);
}
int main1(void) {
  double beta[5]={1.0,2.0,3.0,4.0,5.0};
  struct cWishart *cw;
  int verbose=1;
  cw=mh_cwishart_s(3,5,beta,0.3,7,  0,0,0, 1,1e-7,verbose);
  if (cw != NULL) {
    oxprintf("%s",(char *)cw->aux);
  }
  cw=mh_cwishart_s(4,5,beta,0.3,7,  0,0,0, 1,1e-7,verbose);
  if (cw != NULL) {
    oxprintf("%s",(char *)cw->aux);
  }
  cw=mh_cwishart_s(5,5,beta,0.3,7,  0,0,0, 1,1e-7,verbose);
  if (cw != NULL) {
    oxprintf("%s",(char *)cw->aux);
  }
  return(0);
}
#endif

