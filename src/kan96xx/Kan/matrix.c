/* matrix.c */
#include <stdio.h>
#include "datatype.h"
#include "stackm.h"
#include "extern.h"
#include "extern2.h"

#include "matrix.h"   /* ind() and ind2() are defined. */
                      /* You must use n and n2. */

struct arrayOfPOLY *aaAdd(aa,bb)
struct arrayOfPOLY *aa,*bb;
/* this function has not been tested yet. */
{
  int i;
  POLY *r;
  POLY *a;
  POLY *b;
  int size;
  struct arrayOfPOLY *ra;
  size = aa->n;
  a = aa->array; b = bb->array;
  r = (POLY *)sGC_malloc(sizeof(POLY)*size);
  if (r == (POLY *)NULL) errorMatrix("aaAdd(): no more memory");
  for (i=0; i<size; i++) {
    r[i] = ppAdd(a[i],b[i]);
  }
  ra = (struct arrayOfPOLY *)sGC_malloc(sizeof(struct arrayOfPOLY));
  if (ra == (struct arrayOfPOLY *)NULL) errorMatrix("No more memory.");
  ra->n = size; ra->array = r;
  return(ra);
}

struct matrixOfPOLY *aaMult(aa,bb)
struct matrixOfPOLY *aa,*bb;
{
  POLY *a;
  POLY *b;
  int m,n,m2,n2; /* c.f. matrix.h */
  /*  (m,n) * (m2,n2) */
  int i,j,k;
  POLY *r;
  POLY tmp;
  struct matrixOfPOLY *rmat;

  m = aa->m; n = aa->n;
  m2 = bb->m; n2 = bb->n;
  a = aa->mat; b = bb->mat;
  r = (POLY *)sGC_malloc(sizeof(POLY)*m*n2);
  if (r == (POLY *)NULL) errorMatrix("aaMult(): no more memory");
  /* we do not check n == m2. */
  for (i=0; i<m; i++) {
    for (j=0; j<n2; j++) {
      tmp = ZERO;
      for (k=0; k<n; k++) {
	tmp = ppAddv(tmp, ppMult( a[ind(i,k)], b[ind2(k,j)]));
      }
      r[ind2(i,j)] = tmp;
    }
  }
  rmat = (struct matrixOfPOLY *)sGC_malloc(sizeof(struct matrixOfPOLY));
  if (rmat == (struct matrixOfPOLY *)NULL) errorMatrix("No more memory.");
  rmat->m = m; rmat->n = n2;
  rmat->mat = r;
  return(rmat);
}
  
  
/****************  error handler ************************/
void errorMatrix(str)
char *str;
{
  fprintf(stderr,"matrix.c: %s\n",str);
  exit(10);
}

/********************************************************/

struct arrayOfPOLY *newArrayOfPOLY(size)
int size;
/* if size<=0, it returns [0,null]. */
{
  struct arrayOfPOLY *ap;
  ap = (struct arrayOfPOLY *)sGC_malloc(sizeof(struct arrayOfPOLY));
  if (ap == (struct arrayOfPOLY *)NULL) errorMatrix("No more memory.");
  if (size <=0) {
    ap->n = 0; ap->array = (POLY *)NULL;
    return(ap);
  }
  ap->n = size;
  ap->array = (POLY *)sGC_malloc(size*sizeof(POLY));
  if (ap->array == (POLY *)NULL) errorMatrix("No more memory.");
  return(ap);
}

struct matrixOfPOLY *newMatrixOfPOLY(m,n)
int m,n;
/* if size<=0, it returns [0,null]. */
{
  struct matrixOfPOLY *ap;
  ap = (struct matrixOfPOLY *)sGC_malloc(sizeof(struct matrixOfPOLY));
  if (ap == (struct matrixOfPOLY *)NULL) errorMatrix("No more memory.");
  if ((m <=0) || (n <= 0)) {
    ap->m = ap->n = 0; ap->mat = (POLY *)NULL;
    return(ap);
  }
  ap->m = m; ap->n = n;
  ap->mat = (POLY *)sGC_malloc(m*n*sizeof(POLY));
  if (ap->mat == (POLY *)NULL) errorMatrix("No more memory.");
  return(ap);
}


struct arrayOfPOLY *carrayToArrayOfPOLY(a,size)
POLY a[];
int size;
/* a[] is read only. */
{
  struct arrayOfPOLY *ans;
  int i;
  ans = newArrayOfPOLY(size);
  for (i=0; i<size; i++) {
    ans->array[i] = a[i];
  }
  return(ans);
}

  
  

    

