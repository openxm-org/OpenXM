/*
** toric.c                                 Birk Huber, 4/99 
** -- support for computing first grobner bases of toric ideals.
** -- includes functions gset_rlex_rgb() and gset_compute_colon()
**    which once tested and working will move back to the gset.c file. 
**
**
** TiGERS,  Toric Groebner Basis Enumeration by Reverse Search 
** copyright (c) 1999  Birk Huber
**
*/
#include<stdio.h>
#include<stdlib.h>
#include "utils.h"
#include "gset.h"
#include "matrices.h"
#include "Rsimp.h"
extern int NT;



/*
** void gset_compute_colon(gset g,int lv):
**    Compute deg rev lex rgb (lv least) for colon of g with variable lv.
**    - set ring_lv=lv.
**    - convert g to rgb wrt deg_rev_lex as implemented in binomial.h.
**    - divide each binomial by highest power of var lv possible.
**    - autoreduce.
**
*/
#define min(m,n) (((m)<(n))?(m):(n))
void gset_compute_colon(gset g,int lv){
  binomial ptr;
  int lold,jk;

  /* progress report */
  if (NT == 1) {
  }else{
    fprintf(stderr,"taking colon of J with %c:\n",'a'+lv);
  }

  /* set lex last variable to be lv */
  lold=ring_lv;
  ring_lv=lv;

  /* compute grobner basis wrt to this deg rev lex term order */
  gset_rgb(g,monomial_rlexcomp);

  /* now devide each binomial by highest power of lvar which divides it */
  for(ptr=gset_first(g); ptr!=0; ptr=binomial_next(ptr)){
    jk=min(binomial_lead(ptr)[lv],binomial_trail(ptr)[lv]);
    binomial_lead(ptr)[lv]-=jk;
    binomial_trail(ptr)[lv]-=jk;
  }
  
  /* result is a grobner basis, reduce it to make it an rgb */
  gset_autoreduce(g);

  /* restore original value of lvar*/
  ring_lv=lold;
}

/*
** gset gset_toric_ideal(int **M,int m, int n):
**  Given an mxn integer matrix M -- compute an rgb for the toric ideal I_M.
**  Uses repeated colon computations.
*/
gset gset_toric_ideal(int **M,int m, int n){
   int i,j,jk,crk;
   binomial b;
   int **V,**S,**new_imatrix();
   gset g;
 
   /* set ring dimension*/
   if (ring_N!=n){ 
    fprintf(stderr,"ERROR gset_toric_ideal():");
    fprintf(stderr," matrix dimensions incompatible with ring\n");
    return 0;
   }
   /* set wieghting of A */
   for(i=0;i<n;i++){
    ring_weight[i]=0;
    for(j=0;j<m;j++) ring_weight[i]+=M[j][i];
   }

   /* reserve space for S and V */
   S=new_imatrix(n,m);
   V=new_imatrix(n,n);

   /* copy transpose of M to S*/
   for(i=0;i<n;i++)for(j=0;j<m;j++) IMref(S,i,j)=IMref(M,j,i);

   /* compute hermite normal form of M,S,V and corank of M*/
   crk=ihermite(S,V,m,n);

   /* create new gset and read off equations corresponding to rows of V*/
   g=gset_new();
   for(i=crk;i<n;i++){
     b=binomial_new();
       for(j=0;j<n;j++){
         jk=IMref(V,i,j);
         if (jk>0){ 
             binomial_lead(b)[j]=jk; binomial_trail(b)[j]=0;}
         else {binomial_trail(b)[j]=-1*jk; binomial_lead(b)[j]=0;}
	 }
     gset_insert(g,b);
   }

   /* compute successive colon ideals */
   for(i=0;i<n;i++)gset_compute_colon(g,i);
   
   /*unset weighting */
   for(i=0;i<n;i++) ring_weight[i]=1;

   /* free space used by matrices */
   free_imatrix(M);
   free_imatrix(S);
   free_imatrix(V);

   /* make sure result is the lex gbasis */
   gset_rgb(g,monomial_lexcomp);   

return g;
}












