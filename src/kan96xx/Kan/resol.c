/* $OpenXM: OpenXM/src/kan96xx/Kan/resol.c,v 1.4 2000/06/26 11:15:04 takayama Exp $ */
/* resol.c */
#include <stdio.h>
#include "datatype.h"
#include "extern2.h"
#include "gradedset.h"

#define lcomp_tower(f) ((f)==NULL? -1 : (f)->m->x[(f)->m->ringp->nn])
/* returns leading component number of POLY f, which is equal to i of ee^i */
static void shellForMonomialSyz(struct monomialSyz **p,int size);
static struct arrayOfMonomialSyz schreyerSkelton0(struct arrayOfPOLY g,int i);
static struct arrayOfMonomialSyz putMonomialSyz(struct arrayOfMonomialSyz a,
						struct monomialSyz *s);

static int RemoveRedundantInSchreyerSkelton = 1;

struct monomialSyz *newMonomialSyz(void)
{
  struct monomialSyz *s;
  s = (struct monomialSyz *)sGC_malloc(sizeof(struct monomialSyz));
  if (s == NULL) errorGradedSet("newMonomialSyz(): no memory.");
  s->i = s->j = -1;
  s->deleted = 0;
  s->a = POLYNULL;  s->b = POLYNULL;
  return(s);
}

struct arrayOfMonomialSyz enlargeArrayOfMonomialSyz(struct arrayOfMonomialSyz pp)
{
  struct monomialSyz **newp;
  int i;
  int newlimit;
  if (pp.limit <= 0) newlimit = 10; else newlimit = (pp.limit)*2;
  newp = (struct monomialSyz **) sGC_malloc(sizeof(struct monomialSyz *)*newlimit);
  if (newp == NULL) errorGradedSet("enlargeArrayOfMonomialSyz(): no memory.");
  for (i=0; i< pp.size; i++) {
    newp[i] = (pp.p)[i];
  }
  for (i=pp.size ; i<newlimit; i++) newp[i] = NULL;
  pp.limit = newlimit;
  pp.p = newp;
  return(pp);
}


static struct arrayOfMonomialSyz schreyerSkelton0(struct arrayOfPOLY g,int i)
/* return value will be changed by the next call of this function. */
{
  int m,j,k,flag;
  static int s_ij_size = 0;
  static struct monomialSyz **s_ij = NULL;
  struct monomialSyz *s;
  struct spValue sv;
  struct arrayOfMonomialSyz ans;
  extern int RemoveRedundantInSchreyerSkelton;
  /* It was 1. */

  m = g.n;
  if (m > s_ij_size) {
    s_ij_size = m+1;
    s_ij = (struct monomialSyz **)sGC_malloc(sizeof(struct monomialSyz *)*
					    s_ij_size);
    if (s_ij == NULL) errorGradedSet("schreyerSkelton(): no memory");
  }
  for (j=i+1; j<m; j++) {
    s_ij[j-i-1] = s = newMonomialSyz();
    s->i = i; s->j = j;
    if ((*isSameComponent)((g.array)[i],(g.array)[j])) {
      s->deleted = 0;
      sv = (*sp)((g.array)[i],(g.array)[j]);
      s->a = sv.a; s->b = sv.b;
    }else{
      s->deleted = 1;
    }
  }
  shellForMonomialSyz(s_ij,m-i-1);
  if (RemoveRedundantInSchreyerSkelton) {
	do {
	  flag = 0;
      for (j=0; j<m-i-1;j++) {
		if (s_ij[j]->deleted != 1) {
		  for (k=0; k<m-i-1;k++) {
			if ((j != k) && (s_ij[k]->deleted != 1)) {
			  if ((*isReducible)(s_ij[k]->a,s_ij[j]->a)) {
				s_ij[k]->deleted = 1;
				flag = 1;
			  }
			}
		  }
		}
	  }
	}while (flag);
  }
  ans.size = m-i-1;
  ans.limit = s_ij_size;
  ans.p = s_ij;
  return(ans);
}

static struct arrayOfMonomialSyz putMonomialSyz(struct arrayOfMonomialSyz a,
						struct monomialSyz *s)
{
  if (a.limit <= a.size) {
    a = enlargeArrayOfMonomialSyz(a);
  }
  (a.p)[a.size] = s;
  a.size = a.size+1;
  return(a);
}
  
    
  
struct arrayOfMonomialSyz schreyerSkelton(struct arrayOfPOLY g)
{
  int i,k,m;
  struct arrayOfMonomialSyz ans;
  struct arrayOfMonomialSyz ipart;
  ans.size = 0;
  ans.limit = 0;
  ans.p = NULL;

  m = g.n;
  for (i=0;  i<m; i++ ) {
    ipart = schreyerSkelton0(g,i);
    for (k=0; k< ipart.size; k++) {
      if ((ipart.p)[k]->deleted != 1) {
        ans = putMonomialSyz(ans,(ipart.p)[k]);
      }
    }
  }
  shellForMonomialSyz(ans.p, ans.size);
  return(ans);
}

static void shellForMonomialSyz(struct monomialSyz **p,int n)
{
  int gap, i, j, r;
  struct monomialSyz *temp;
  for (gap = n/2; gap > 0; gap /= 2) {
    for (i = gap; i < n; i++) {
      for (j = i - gap ; j >= 0; j -= gap) {
	r = (*mmLarger)(p[j+gap]->a, p[j]->a);
	if ( r >= 1) break;
	temp = p[j];
	p[j] = p[j+gap];
	p[j+gap] = temp;
      }
    }
  }
}
