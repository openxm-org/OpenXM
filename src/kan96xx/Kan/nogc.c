#include <stdio.h>
static long Total = 0;

void *GC_malloc(n)
int n;
{
  void *p;
  p = (void *)calloc(n,1);  /* clear the memory */

  Total += n;
  if (p == (void *)NULL) {
    fprintf(stderr,"\nNo more memory. We have allocated %ld bytes of the memory.\n",Total);
    return(p);
  }
  return(p);
}

void *GC_realloc(void *p,int n) {
  void *p;
  p = (void *)realloc(p,n);

  Total += n;
  if (p == (void *)NULL) {
    fprintf(stderr,"\nNo more memory. We have allocated %ld bytes of the memory.\n",Total);
    return(p);
  }
  return(p);
}

GC_free(void *p) {  ; }

  
