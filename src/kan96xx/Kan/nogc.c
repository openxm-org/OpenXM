/* $OpenXM: OpenXM/src/kan96xx/Kan/nogc.c,v 1.4 2004/02/27 09:46:46 takayama Exp $ */
#include <stdio.h>
static long Total = 0;
unsigned int GC_version=0;

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

GC_version(void) { return(GC_version); }


  
