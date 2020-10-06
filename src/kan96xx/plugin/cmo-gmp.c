/*$OpenXM: OpenXM/src/kan96xx/plugin/cmo-gmp.c,v 1.10 2015/09/14 07:56:20 takayama Exp $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* #include <netinet/in.h> */
#include <limits.h>
#include <arpa/inet.h>
#include "datatype.h"
#include "stackm.h"
#include "extern.h"
#include "extern2.h"
#include "mathcap.h"
#include "kclass.h"

#include "gmp.h"
#include "gmp-impl.h"


#include "file2.h"
#include "cmo.h"

/* for gmp6.0.0 */
#ifndef BYTES_PER_MP_LIMB
#define BYTES_PER_MP_LIMB SIZEOF_MP_LIMB_T
#endif

extern int OxVersion;
size_t cmoOutGMPCoeff_old(mpz_srcptr x);
size_t cmoOutGMPCoeff_new(mpz_srcptr x);
size_t cmoGetGMPCoeff_old(MP_INT *x, struct cmoBuffer *cb);
size_t cmoGetGMPCoeff_new(MP_INT *x, struct cmoBuffer *cb);

size_t cmoOutGMPCoeff(mpz_srcptr x) {
  if (OxVersion >= 199907170)
    return(cmoOutGMPCoeff_new(x));
  else
    return(cmoOutGMPCoeff_old(x));
}

size_t cmoGetGMPCoeff(MP_INT *x, struct cmoBuffer *cb) {
  if (OxVersion >= 199907170)
    return(cmoGetGMPCoeff_new(x,cb));
  else
    return(cmoGetGMPCoeff_old(x,cb));
}


/* From gmp/mpz/out_raw.c and inp_raw.c */

/* mpz_out_raw -- Output a mpz_t in binary.  Use an endianess and word size
   independent format.

Copyright (C) 1995 Free Software Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Library General Public License as published by
the Free Software Foundation; either version 2 of the License, or (at your
option) any later version.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
License for more details.

You should have received a copy of the GNU Library General Public License
along with the GNU MP Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA. */

static int myfputc(int i) {
  char tmp[1];
  tmp[0] = i;
  cmoOutputToBuf(CMOPUT,tmp,1);
}
static void outRawInt32(int k)
{
  int tmp[1];
  tmp[0] = htonl((int) k);
  cmoOutputToBuf(CMOPUT,tmp,4);
}

size_t cmoOutGMPCoeff_old(mpz_srcptr x)
{
  fprintf(stderr,"cmoOutGMPCoeff_old is no longer supported.\n");
  exit(10);
  return 0;
}


/* mpz_inp_raw -- Input a mpz_t in raw, but endianess, and wordsize
   independent format (as output by mpz_out_raw).

Copyright (C) 1991, 1993, 1994, 1995 Free Software Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Library General Public License as published by
the Free Software Foundation; either version 2 of the License, or (at your
option) any later version.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
License for more details.

You should have received a copy of the GNU Library General Public License
along with the GNU MP Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA. */


static int myfgetc(struct cmoBuffer *cb)
{
  int k;
  if (cb->isStream) {
    k = fp2fgetc(cb->fp);
  }else{
    k = ((unsigned char *)(cb->buf))[cb->rpos];
    cb->rpos++;
    if (cb->rpos > cb->pos) {
      cb->rpos--;
      errorCmo(" cmo-gmp.c : myfgetc(): no data in the buffer.");
    }
  }
  return(k);
}
#if BYTES_PER_MP_LIMB == 8
static unsigned int getRawInt32(struct cmoBuffer *cb)
{
  char d[4];
  int i;
  int r;
  for (i=0; i<4; i++) {
    d[i] = myfgetc(cb);
  }
  return( ntohl(* ( (unsigned int *)d)));
}
#elif BYTES_PER_MP_LIMB == 4
static int getRawInt32(struct cmoBuffer *cb)
{
  char d[4];
  int i;
  int r;
  for (i=0; i<4; i++) {
    d[i] = myfgetc(cb);
  }
  return( ntohl(* ( (int *)d)));
}
#endif

size_t cmoGetGMPCoeff_old(MP_INT *x, struct cmoBuffer *cb)
{
  fprintf(stderr,"cmoGetGMPCoeff_old is no longer supported.\n");
  exit(10);
  return 0;
}

/*****************************************************/
/*****   new version for CMO_ZZ  *********************/
/*****************************************************/
#if BYTES_PER_MP_LIMB == 8
size_t cmoOutGMPCoeff_new(mpz_srcptr x)
{
  int i;
  mp_size_t s;
  mp_size_t xsize = ABS (x->_mp_size);
  mp_size_t xsize0;
  mp_srcptr xp = x->_mp_d;
  mp_size_t out_bytesize;
  mp_limb_t hi_limb;
  int n_bytes_in_hi_limb;
  cmoint tmp[1];
  tmp[0] = htonl(CMO_ZZ);
  cmoOutputToBuf(CMOPUT,tmp,sizeof(cmoint));

  if (CHAR_BIT != 8) {
    fprintf(stderr,"CHAR_BIT = %d\n",CHAR_BIT);
    fprintf(stderr,"cmo-gmp.c does not work on this CPU.\n");
    exit(10);
  }
  
  if (xsize == 0)
    {
      outRawInt32(0);
      return  1;
    }

  xsize0 = xsize;
  if ( (unsigned long)xp[xsize-1] < (unsigned long)0x100000000 )
    xsize = xsize0*2-1;
  else
    xsize = xsize0*2;

  if (x->_mp_size >= 0)
    outRawInt32(xsize);
  else
    outRawInt32(-xsize);

  for (s = 0; s < xsize0-1; s++)
    {
      outRawInt32(((unsigned long)xp[s])&(unsigned long)0xffffffff);
      outRawInt32(((unsigned long)xp[s])>>32);
    }
  if ( !(xsize&1) ) {
    outRawInt32(((unsigned long)xp[s])&(unsigned long)0xffffffff);
    outRawInt32(((unsigned long)xp[s])>>32);
  } else
    outRawInt32(((unsigned long)xp[s])&(unsigned long)0xffffffff);
  return ( ABS (xsize) );
}

size_t cmoGetGMPCoeff_new(MP_INT *x, struct cmoBuffer *cb)
{
  int i;
  mp_size_t s;
  mp_size_t xsize;
  mp_size_t xsize0;
  mp_ptr xp;
  unsigned int c;
  mp_limb_t x_limb;
  mp_size_t in_bytesize;
  int neg_flag;

  if (CHAR_BIT != 8) {
    fprintf(stderr,"CHAR_BIT = %d\n",CHAR_BIT);
    fprintf(stderr,"cmo-gmp.c does not work on this CPU.\n");
    exit(10);
  }

  /* Read 4-byte size */

  xsize = (int)getRawInt32(cb);
  neg_flag = xsize < 0;
  xsize = ABS(xsize);
  
  if (xsize == 0)
    {
      x->_mp_size = 0;
      return 1;         /* we've read 4 bytes */
    }

  xsize0 = (xsize+1)/2;
  if (x->_mp_alloc < xsize0)
    _mpz_realloc (x, xsize0);
  xp = x->_mp_d;

  for (i=0; i<xsize0-1; i++) {
    xp[i] = ((unsigned long)getRawInt32(cb))
      |(((unsigned long)getRawInt32(cb))<<32);
  }
  if ( !(xsize&1) )
    xp[i] = ((unsigned long)getRawInt32(cb))
      |(((unsigned long)getRawInt32(cb))<<32);
  else
    xp[i] = (unsigned long)getRawInt32(cb);

  MPN_NORMALIZE (xp, xsize0);
  x->_mp_size = neg_flag ? -xsize0 : xsize0;
  return( xsize0 ); 
}
#elif BYTES_PER_MP_LIMB == 4
size_t cmoOutGMPCoeff_new(mpz_srcptr x)
{
  int i;
  mp_size_t s;
  mp_size_t xsize = ABS (x->_mp_size);
  mp_srcptr xp = x->_mp_d;
  mp_size_t out_bytesize;
  mp_limb_t hi_limb;
  int n_bytes_in_hi_limb;
  cmoint tmp[1];
  tmp[0] = htonl(CMO_ZZ);
  cmoOutputToBuf(CMOPUT,tmp,sizeof(cmoint));

  if (BYTES_PER_MP_LIMB != 4) {
    fprintf(stderr,"BYTES_PER_MP_LIMB = %d\n",BYTES_PER_MP_LIMB);
    fprintf(stderr,"cmo-gmp.c does not work on this CPU.\n");
    fprintf(stderr,"Read the GMP source code and rewrite cmo-gmp.c.\n");
    exit(10);
  }
  if (CHAR_BIT != 8) {
    fprintf(stderr,"CHAR_BIT = %d\n",CHAR_BIT);
    fprintf(stderr,"cmo-gmp.c does not work on this CPU.\n");
    exit(10);
  }
  
  if (xsize == 0)
    {
      outRawInt32(0);
      return  1;
    }

  if (x->_mp_size >= 0)
    outRawInt32(xsize);
  else
    outRawInt32(-xsize);

  
  /* Output from the least significant limb to the most significant limb */
  /* We assume that the size of limb is 4. */

  for (s = 0; s < xsize; s++)
    {
      outRawInt32((int) xp[s]);
    }
  return ( ABS (xsize) );
}

size_t cmoGetGMPCoeff_new(MP_INT *x, struct cmoBuffer *cb)
{
  int i;
  mp_size_t s;
  mp_size_t xsize;
  mp_ptr xp;
  unsigned int c;
  mp_limb_t x_limb;
  mp_size_t in_bytesize;
  int neg_flag;

  if (BYTES_PER_MP_LIMB != 4) {
    fprintf(stderr,"BYTES_PER_MP_LIMB = %d\n",BYTES_PER_MP_LIMB);
    fprintf(stderr,"cmo-gmp.c does not work on this CPU.\n");
    fprintf(stderr,"Read the GMP source code and rewrite cmo-gmp.c.\n");
    exit(10);
  }
  if (CHAR_BIT != 8) {
    fprintf(stderr,"CHAR_BIT = %d\n",CHAR_BIT);
    fprintf(stderr,"cmo-gmp.c does not work on this CPU.\n");
    exit(10);
  }

  /* Read 4-byte size */

  xsize = getRawInt32(cb);
  neg_flag = xsize < 0;
  xsize = ABS(xsize);
  
  if (xsize == 0)
    {
      x->_mp_size = 0;
      return 1;         /* we've read 4 bytes */
    }

  if (x->_mp_alloc < xsize)
    _mpz_realloc (x, xsize);
  xp = x->_mp_d;

  for (i=0; i<xsize; i++) {
    xp[i] = getRawInt32(cb);
  }

  MPN_NORMALIZE (xp, xsize);
  x->_mp_size = neg_flag ? -xsize : xsize;
  return( xsize ); 
}
#endif /* BYTES_PER_MP_LIMB */
