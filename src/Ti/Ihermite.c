/*
** Ihermite.c                                 Birk Huber, 2/99 
**   -- compute hermite normal form of an integer matrix
**  
**
** TiGERS,  Toric Groebner Basis Enumeration by Reverse Search 
** copyright (c) 1999  Birk Huber
**
*/
#include<stdio.h>
#include<stdlib.h>
#include "utils.h"


/*
**
** Imatrix_hermite:
**    Input: S an n by m Imatrix.
**   Output: True 
**
**   Sidefects: U points to a unitary nxn matrix
**              S gets replaced by its hermite normal form (U*S)
**            
**  Method:
**    U = Inxn
**    c = 1
**    while (c<=n) do
**      find mc>=c such that abs(S(mc,c)) has minimum non-zero value
**      if (mc!=c) then  switch rows mc and c (in S, and U)
**      if (S(c,c)<0) then multiply row c by -1
**      if (S(c,c)!=0) then 
**        for i in c+1:n  add S(i,c)/S(c,c) times row c to row i;
**      end(if)
**      if S(i,c)==0 for all i in c+1 ... n set c=c+1
**    end(while)
**
*/
#define Mref(U,i,j) U[(i)-1][(j)-1]
#define Mrows(U) m
#define Mcols(U) n



int ihermite(int **S,int **U,int m, int n){
  int c=1,mc,i,j,done,sign;
  int t=0,mv=0, mn,crk=0;
 
  if (m>n) mn=n; else mn=m;

  /* Initialize U to nxn identity */
  for(i=1;i<=n;i++){
    for(j=1;j<=n;j++){
      if (i==j) Mref(U,i,j)=1;
      else Mref(U,i,j)=0;
    }     
  }
 
  while(c<=mn){
    /* find minimum entry in col c */
    mv=Mref(S,c,c);
    if (mv<0) mv*=-1;
    mc=c;
    for(i=c+1;i<=n;i++){
      t=Mref(S,i,c);
      if (t<0) t*=-1; 
      if(mv==0 || (mv>t && t!=0)){
           mv=t;
           mc=i;
      }
    }

    /* if nescesary pivot to put min in row c and multiply by+-1
       to ensure diagonal entry is positive */
    if (mc!=c||Mref(S,mc,c)<0){
      if (Mref(S,mc,c)<0) sign=-1;
      else sign=+1;
      for(j=c;j<=m;j++){
        t=Mref(S,mc,j);
        Mref(S,mc,j)=Mref(S,c,j);
        Mref(S,c,j)=sign*t;
      }
      for(j=1;j<=n;j++){
        t=Mref(U,mc,j);
        Mref(U,mc,j)=Mref(U,c,j);
        Mref(U,c,j)=sign*t;
      }
    }

    /* if collumb is not zero do a reduction step */
    done=TRUE;
    if (Mref(S,c,c)!=0){
      crk=c;
      for(i=c+1;i<=n;i++){
        t=Mref(S,i,c)/Mref(S,c,c);
        for(j=c;j<=m;j++){
           Mref(S,i,j)-=t*Mref(S,c,j);
        }
        for(j=1;j<=n;j++){
           Mref(U,i,j)-=t*Mref(U,c,j);
        }
        if (Mref(S,i,c)!=0) done=FALSE;
      }
    }
    /* if all entrees of col c bellow row c are zero go tonext col */
    if (done==TRUE) c++;
  }
  return crk;
 }








