/* $OpenXM: OpenXM/src/mpfr/bfsize/bfsize.c,v 1.2 2016/06/14 06:59:50 takayama Exp $ */
#include <stdio.h>
#include "gmp.h"
#include "gmp-impl.h"
#include "mpfr.h"

/* from mpfr-impl.h */
#define MPFR_PREC(x)      ((x)->_mpfr_prec)
#define MPFR_EXP(x)       ((x)->_mpfr_exp)
#define MPFR_MANT(x)      ((x)->_mpfr_d)
#define MPFR_LAST_LIMB(x) ((MPFR_PREC (x) - 1) / GMP_NUMB_BITS)
#define MPFR_LIMB_SIZE(x) (MPFR_LAST_LIMB (x) + 1)

int main(void) {
  mpfr_t x;
  int m,n,i;
  mpfr_init2(x,0xff);
  mpfr_set_d(x,3.14159,MPFR_RNDD);
  mpfr_out_str(stdout,2,0,x,MPFR_RNDD);
  printf("\n");
  printf("Dump of mpfr_t x: ");
  n = sizeof(mpfr_t);
  m = sizeof(mp_limb_t);
  for (i=0; i<n; i++) {
    if (i % m == 0) printf("|");
    printf("%02x ",((unsigned char *)(&x))[i]);
  }
  printf("\n");
  printf("sizeof(mpfr_t)=%d\n",(int) (sizeof(mpfr_t)));

  printf("sizeof(mp_limb_t)=%d\n",(int) (sizeof(mp_limb_t)));
  printf("sizeof(unsigned long)=%d\n",(int) (sizeof(unsigned long)));

  printf("GMP_NUMB_BITS=%d\n",GMP_NUMB_BITS);

  printf("MPFR_PREC(x)=%lx, ",MPFR_PREC(x));
  printf("MPFR_EXP(x)=%lx, ",MPFR_EXP(x));
  printf("MPFR_MANT(x)=%p, ",MPFR_MANT(x));


  return(0);
}


