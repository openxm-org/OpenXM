/*
** binomial.c                                 Birk Huber, 4/99 
** -- define binomal type and main operations on binomials                  
**  
**
** TiGERS,  Toric Groebner Basis Enumeration by Reverse Search 
** copyright (c) 1999  Birk Huber
**
*/
#define BINOMIAL_H 1
#include<stdio.h>
#include<stdlib.h>
#include "utils.h"
#include "binomial.h"
extern int NT;

/*
**-------------ring defs-----------------------------------------------------
**
** Monomials in the ambiant polynomial ring will be represented by their
** exponent vectors. Before any computations can be done using monomials the
** dimension of this ring (and the exponent vectors) must be set and stored
** in the global variable "ring_N". The mapping between indeces and variable
** names for i.o. purposes is currently done by just using the first ring_N
** letters of the alphabet. 
**
** Data:
**  int  ring_N: global variable holding dimension of ambiant ring 
**  int ring_lv: global variable used by deg-rev-lex term order, i.e. specify
**               which variable to make cheepest(others ordered alphabetically)
** 
** Functions (or macros):
**  int ring_set(int n): Set ring_N to n if ring_N has not yet been set.       
**  int ring_read(File *infile): Read in a description of the ring from
**                               infile and set ring values accordingly.
** ring_getvar(FILE *): convert next variable name on FILE to variable index
** ring_putvar(File *of,int v): convert index to variable name.
**
** Note: for now a ring description is just given the number of variables.
**       the names are just the first ring_N letters of the alphabet:
**       i.e. **            name        <====>   index 
**                          'a'         <====>      1 
**                          'b'         <====>      1
**        eventually this scheme may be adjusted to allow names to be set
**        at run time.
*/
int ring_N=0; 
int ring_lv=-1;  
int *ring_weight=0;

int ring_set(int n){
  int i;
  if (ring_N!=0) { 
    fprintf(stderr,"ring_set(): can't reset dimension\n");
    return FALSE;
  }
  if ((ring_weight=(int *)(malloc(n*sizeof(int))))==0){
    fprintf(stderr,"ring_set(): malloc failed for weight vector\n");
    return FALSE;
  } 
  for(i=0;i<n;i++) ring_weight[i]=1;
  return ring_N=n;
} 

int ring_read(FILE *infile){
   int n;
   eatwhite(infile);
   if (fscanf(infile,"%d",&n)!=1){
    fprintf(stderr,"ERROR: read_ring() unable to parse ring description\n");
    return FALSE;
   }
   ring_set(n);
   return TRUE;
}

int ring_getvar(FILE *ifile){
  char c;
  c=fgetc(ifile);
  if ('a' > c || c >'a'+ring_N-1){
     fprintf(stderr,"ring_getvar(): invalid variable read\n");
     ungetc(c,ifile);
     return -1; 
  }
  return c-'a';
}

/*
**------------monomial defs-----------------------------------------------
** monomials are represented by their exponent vectors: 
** C integer vectors indexed from 0 to ring_dim-1
**
*/

/*
** void print_monomial(FILE *,int *): 
**      print exponent vector as monomial
**
*/
void print_monomial(FILE *of, int *exps){
  int i,jk=0;
  if (NT == 1) {
    fprintf(of,"[");
    for (i=0; i<ring_N; i++) {
      fprintf(of,"%d",exps[i]);
      if (i != ring_N-1) fprintf(of," , ");
    }
    fprintf(of,"]");
  }else{
    for(i=0;i<ring_N;i++){
      if (exps[i]!=0){
	if (jk!=0) fprintf(of,"*");
	else jk=1;
	ring_putvar(of,i);
	if (exps[i]>1) fprintf(of,"^%d",exps[i]);
      }
    }
  }
}

/*
** void get_monomial(FILE *,int *): 
**      Read ascii representation of monomial as power product -- convert
**      to exponent vector stored in *exps.
**
*/
void get_monomial(FILE *is,int *exps){
char c='*';
int v,e;

 for(v=0;v<ring_N;v++)exps[v]=0;
 while(c=='*'){
   eatwhite(is);
   v=ring_getvar(is);
   eatwhite(is);
   c=fgetc(is);
   if (c=='^'){
       fscanf(is,"%d",&e);
       c=fgetc(is);
   }
   else {
     e=1;
   }
   exps[v]=e;
 }
 ungetc(c,is);
}

/*
** int monomial_divides(monomial m1, monomial m2):
**     return TRUE if m1 divides m2
**
*/
int monomial_divides(monomial m1, monomial m2){
 int i;
 for(i=0;i<ring_N;i++)if (m1[i]>m2[i]) return FALSE;
 return TRUE; 
}

/*
** int monomial_rel_prime(monomial m1, monomial m2):
**     return TRUE if m1 is reletively prime to m2.
**
*/
int monomial_rel_prime(monomial m1, monomial m2){
 int i;
 for(i=0;i<ring_N;i++) if (m1[i]!=0 && m2[i]!=0) return FALSE;
 return TRUE;
}

/*
** int monomial_equal(monomial m1, monomial m2):
**     return TRUE if monomials m1 and m2 have same exponents
**
*/
int monomial_equal(monomial m1, monomial m2){
   int i;
   for(i=0;i<ring_N;i++) if (m1[i]-m2[i]!=0) return FALSE;
   return TRUE;
}

/*
**int monomial_lcm(monomial m1, monomial m2, monomial m3):
**    copy least common multiple of m1 and m2 into m3.
**
*/
int monomial_lcm(monomial m1, monomial m2, monomial m3){
  int i;
  for(i=0;i<ring_N;i++){
    if (m1[i]>=m2[i]) m3[i]=m1[i];
    else m3[i]=m2[i];
  }
}

/*
**
**
*/
int monomial_stddegree(monomial m){
 int i,d=0;
 for(i=0;i<ring_N;i++) d+=m[i];
 return d;
}


/*
** int monomial_lexcomp(monomial m1, monomial m2):
**     return >0 if m1 is lexicographically greater then m2
**
*/
int monomial_lexcomp(monomial m1, monomial m2){
int i,jk;
 for(i=0;i<ring_N;i++) if ((jk=m1[i]-m2[i])!=0) break; 
 return jk; 
}

/*
** int monomial_rlexcomp(monomial m1, monomial m2):
**     return >0 if m1 is degree reverse lexicographically greater than m2
**     with variables taken in order a>b...., except that variable ring_lv is
**     taken last (if it is >=0).
**
*/
int monomial_rlexcomp(monomial m1, monomial m2){
int i,jk,d=0;
 /* compute diference of total degrees */
 for(i=0;i<ring_N;i++) d+=ring_weight[i]*(m1[i]-m2[i]);
 if (d!=0) return d;
 /* test ring_lv entree first */
 if (ring_lv>=0 && ((jk=m1[ring_lv]-m2[ring_lv])!=0)) return -1*jk;
 /* now test entrees in reverse order */
 for(i=ring_N-1;i>=0;i--) if ((jk=m1[i]-m2[i])!=0) return -1*jk;
 return 0;
}

/*
**----------binomial defs------------------------------------------------
**
**
*/

/*
** binomial binomial_new(): allocate storage for a new binomial--
**                          initialize flags and set exponents to zero;
*/
binomial binomial_new(){
   binomial m;
   int i,*E=0,*E2=0;

   if ((m=(binomial)malloc(sizeof(struct bin_tag)))==0){
    fprintf(stderr,"binomial_new(): first malloc failed\n");
    return 0;
   } 
   if ((m->E=(int *)malloc(2*ring_N*sizeof(int)))==0){   
    fprintf(stderr,"binomial_new(): seccond malloc failed\n");
    free((void*)m);
    return 0;
   } 
   m->exps1=m->E;
   m->exps2=m->E+ring_N;
   m->ff=UNKNOWN;
   m->bf=BINOMIAL;
   m->next=0;
   for(i=0;i<ring_N;i++) { m->exps1[i]=(m->exps2[i]=0);}
   return m;
}


/*
** void binomial_free(binomial m):
**      reclaim space allocated by binomial_new()
**
*/
void binomial_free(binomial m){
    if (m->E!=0)free((void *)(m->E));
    free((void *)m);
}

/*
** void binomial_read(FILE *is, binomial b):
**      read data into binomial 
**
*/
void binomial_read(FILE *is, binomial b){
  char c, d='-';
  eatwhite(is);
  c=getc(is);
  /* read in status of facet flag if present*/
  if (c=='#') b->ff=FACET;
  else if (c=='!') b->ff=NONFACET;
  else if (c=='?') b->ff=UNKNOWN;
  else ungetc(c,is);
  
  /* read in leading and trailing monomials if present*/
  eatwhite(is);
  if ((c=getc(is))=='-') d='+';
  else ungetc(c,is);
  get_monomial(is,b->exps1);
  eatwhite(is);
  if ((c=getc(is))==d){ 
    get_monomial(is,b->exps2);
    b->bf=BINOMIAL;
  }else{
    b->bf=MONOMIAL;
    ungetc(c,is);
  }
  if (d=='+'){
   if (b->bf==MONOMIAL){ 
     fprintf(stderr,"WARNING: binomial_read() monomial with negative coef\n");
   }
   binomial_flip(b);
  }
}


/*
** void binomial_print(FILE *,binomial):
**      print binomial
**
*/
void binomial_print(FILE *of, binomial b){
  if (NT == 1) {
    if (b==0) fprintf(of,"[ ]");
    else {
      /* if (b->ff==FACET) fprintf(of,"# ");
	 else if (b->ff==NONFACET) fprintf(of,"! "); */
      /*else if (b->ff==UNKNOWN) fprintf(of,"? ");*/
      fprintf(of,"[");
      print_monomial(of,b->exps1);
      if (b->bf==BINOMIAL){
	fprintf(of," , ");
	print_monomial(of,b->exps2);
	fprintf(of,"]");
      }
    }
  }else{
    if (b==0) fprintf(of,"(NULL)");
    else {
      if (b->ff==FACET) fprintf(of,"# ");
      else if (b->ff==NONFACET) fprintf(of,"! ");
      /*else if (b->ff==UNKNOWN) fprintf(of,"? ");*/
      print_monomial(of,b->exps1);
      if (b->bf==BINOMIAL){
	fprintf(of,"-");
	print_monomial(of,b->exps2);
      }
    }
  }
}

/*
** void binomial_copy(binomial src,binomial dest):
**      copy source binomials exponents and flags to those of dest binomial
**
*/
void binomial_copy(binomial src,binomial dest){
 int i;
 
 for(i=0;i<ring_N;i++){
    binomial_lead(dest)[i]=binomial_lead(src)[i];
    binomial_trail(dest)[i]=binomial_trail(src)[i];
 }
 binomial_next(dest)=0;
 dest->bf=src->bf;
}

/*
** void binomial_flip(binomial b):
**      interchange leading and trailing terms of binomial
**
*/
void binomial_flip(binomial b){
   int *tmp;
   tmp=binomial_lead(b);
   binomial_lead(b)=binomial_trail(b);
   binomial_trail(b)=tmp;
}   

/*
** binomial monomial_spair(binomial b,monomial m):
**   form spair between marked binomial and marked monomial
**   and reduce result by binomial as many times as possible
**
*/
binomial monomial_spair(binomial b,monomial m){
  binomial S = 0;
  int i,tmp;
        
  S=binomial_new();
 
  /* form monomial spair i.e. trailing term times factor required to */
  /* bump lead term up to monomial -- also set trail terms to zero  */  
  for(i=0;i<ring_N;i++){
    (binomial_lead(S))[i]=(binomial_trail(b))[i];
    (binomial_trail(S))[i]=0;
    if ((tmp=(m[i]-(binomial_lead(b))[i]))>0)((binomial_lead(S))[i])+=tmp;
  }

  /*reduce S by b as many times as possible*/
  while(monomial_divides(binomial_lead(b),binomial_lead(S))==TRUE){
    for(i=0;i<ring_N;i++){
      tmp=(binomial_lead(S))[i]-(binomial_lead(b))[i];
      (binomial_lead(S))[i]=(binomial_trail(b))[i]+tmp;
    }

  }
  return S;
}

/* 
** int binomial_spair(binomial b1, binomial b2):
**     replace b1, by spair of b1 and b2 -- the spair is taken with respect
**     to the markings of b1 and b2, but it's marking is not meaningfull.
**     returns TRUE if result is equivalent to zero monomial.
**
*/
int binomial_spair(binomial b1, binomial b2){
   int i,jk,jk2=TRUE;
   for(i=0;i<ring_N;i++){
     jk=binomial_lead(b1)[i]-binomial_lead(b2)[i];
     if (binomial_lead(b1)[i]!=0 && binomial_lead(b2)!=0) jk2=FALSE;
     binomial_lead(b1)[i]=binomial_trail(b2)[i];
     if (jk>0) binomial_lead(b1)[i]+=jk;
     else binomial_trail(b1)[i]-=jk; 
   }
   jk=monomial_equal(binomial_lead(b1),binomial_trail(b1));
   if (jk==TRUE || jk2==TRUE) return TRUE;
   else return FALSE;
}


/*
**void binomial_bumpto(binomial b1, binomial b2):
**     assuming that the leading term of b1 divides that of b2 
**     multiply both sides of b1 so that the leading terms of b1 
**     and b2 are equal
**
*/  
void binomial_bumpto(binomial b1, binomial b2){
  int i,tmp;
  for(i=0;i<ring_N;i++){
    tmp=(binomial_lead(b2))[i]-(binomial_lead(b1))[i];
    (binomial_lead(b1))[i]+=tmp; 
    (binomial_trail(b1))[i]+=tmp;
  }
}
 
/*
** void reducetrail(binomial b1, binomial b2):
**      assuming that lead(b2) divides trail(b1) change b1 into binomial 
**      resulting from bumping b2 so that lead(b2)=trail(b1) and adding 
**      result to b1.
**
*/
void reducetrail(binomial b1, binomial b2){
      int i;
      for( i=0;i<ring_N;i++){
          binomial_trail(b1)[i]+=binomial_trail(b2)[i]-binomial_lead(b2)[i];
       }
}

/*
** int binomial_compair(binomial b1,binomial b2):
**     return TRUE if the lead term of b1 is lexicographically greater than 
**     that of b2 or if they tie return true if the trail term b1 is greater
**     or equal to that of b2 otherwise return false
*/
int binomial_compair(binomial b1,binomial b2){
 int tmp;

 tmp=monomial_lexcomp(binomial_lead(b1),binomial_lead(b2));
 if (tmp>0) return TRUE;
 else if (tmp<0) return FALSE;
 else {
   tmp=monomial_lexcomp(binomial_trail(b1),binomial_trail(b2));
   if (tmp>=0) return TRUE;
   else return FALSE;
 }
}














