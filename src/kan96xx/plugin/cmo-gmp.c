#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
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

extern int OxVersion;
size_t cmoOutGMPCoeff_old(mpz_srcptr x);
size_t cmoOutGMPCoeff_new(mpz_srcptr x);
size_t
cmoOutGMPCoeff(mpz_srcptr x) {
  if (OxVersion >= 199907170)
    return(cmoOutGMPCoeff_new(x));
  else
    return(cmoOutGMPCoeff_old(x));
}

cmoGetGMPCoeff(MP_INT *x, struct cmoBuffer *cb) {
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
static outRawInt32(int k)
{
  int tmp[1];
  tmp[0] = htonl((int) k);
  cmoOutputToBuf(CMOPUT,tmp,4);
}

size_t
cmoOutGMPCoeff_old(mpz_srcptr x)
{
  int i;
  mp_size_t s;
  mp_size_t xsize = ABS (x->_mp_size);
  mp_srcptr xp = x->_mp_d;
  mp_size_t out_bytesize;
  mp_limb_t hi_limb;
  int n_bytes_in_hi_limb;
  cmoint tmp[1];
  tmp[0] = htonl(CMO_ZZ_OLD);
  cmoOutputToBuf(CMOPUT,tmp,sizeof(cmoint));
  
  if (xsize == 0)
    {
      for (i = 4 - 1; i >= 0; i--)
	myfputc (0);
      return  4;
    }

  hi_limb = xp[xsize - 1];
  for (i = BYTES_PER_MP_LIMB - 1; i > 0; i--)
    {
      if ((hi_limb >> i * BITS_PER_CHAR) != 0)
	break;
    }
  n_bytes_in_hi_limb = i + 1;
  out_bytesize = BYTES_PER_MP_LIMB * (xsize - 1) + n_bytes_in_hi_limb;
  if (x->_mp_size < 0)
    out_bytesize = -out_bytesize;

  /* Make the size 4 bytes on all machines, to make the format portable.  */
  for (i = 4 - 1; i >= 0; i--)
    myfputc ((out_bytesize >> (i * BITS_PER_CHAR)) % (1 << BITS_PER_CHAR));

  /* Output from the most significant limb to the least significant limb,
     with each limb also output in decreasing significance order.  */

  /* Output the most significant limb separately, since we will only
     output some of its bytes.  */
  for (i = n_bytes_in_hi_limb - 1; i >= 0; i--)
    myfputc ((hi_limb >> (i * BITS_PER_CHAR)) % (1 << BITS_PER_CHAR));

  /* Output the remaining limbs.  */
  for (s = xsize - 2; s >= 0; s--)
    {
      mp_limb_t x_limb;

      x_limb = xp[s];
      for (i = BYTES_PER_MP_LIMB - 1; i >= 0; i--)
	myfputc ((x_limb >> (i * BITS_PER_CHAR)) % (1 << BITS_PER_CHAR));
    }
  return ( ABS (out_bytesize) + 4);
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
    return(k);
  }
}
static int getRawInt32(struct cmoBuffer *cb)
{
  char d[4];
  int i;
  for (i=0; i<4; i++) {
    d[i] = myfgetc(cb);
  }
  return(ntohl(* ( (int *)d)));
}

cmoGetGMPCoeff_old(MP_INT *x, struct cmoBuffer *cb)
{
  int i;
  mp_size_t s;
  mp_size_t xsize;
  mp_ptr xp;
  unsigned int c;
  mp_limb_t x_limb;
  mp_size_t in_bytesize;
  int neg_flag;

  /* Read 4-byte size */
  in_bytesize = 0;
  for (i = 4 - 1; i >= 0; i--)
    {
      c = myfgetc (cb);
      in_bytesize = (in_bytesize << BITS_PER_CHAR) | c;
    }

  /* Size is stored as a 32 bit word; sign extend in_bytesize for non-32 bit
     machines.  */
  if (sizeof (mp_size_t) > 4)
    in_bytesize |= (-(in_bytesize < 0)) << 31;

  neg_flag = in_bytesize < 0;
  in_bytesize = ABS (in_bytesize);
  xsize = (in_bytesize + BYTES_PER_MP_LIMB - 1) / BYTES_PER_MP_LIMB;

  if (xsize == 0)
    {
      x->_mp_size = 0;
      return 4;			/* we've read 4 bytes */
    }

  if (x->_mp_alloc < xsize)
    _mpz_realloc (x, xsize);
  xp = x->_mp_d;

  x_limb = 0;
  for (i = (in_bytesize - 1) % BYTES_PER_MP_LIMB; i >= 0; i--)
    {
      c = myfgetc (cb);
      x_limb = (x_limb << BITS_PER_CHAR) | c;
    }
  xp[xsize - 1] = x_limb;

  for (s = xsize - 2; s >= 0; s--)
    {
      x_limb = 0;
      for (i = BYTES_PER_MP_LIMB - 1; i >= 0; i--)
	{
	  c = myfgetc (cb);
	  x_limb = (x_limb << BITS_PER_CHAR) | c;
	}
      xp[s] = x_limb;
    }

  if (c == EOF)
    return 0;			/* error */

  MPN_NORMALIZE (xp, xsize);
  x->_mp_size = neg_flag ? -xsize : xsize;
  return in_bytesize + 4;
}

/*****************************************************/
/*****   new version for CMO_ZZ  *********************/
/*****************************************************/
size_t
cmoOutGMPCoeff_new(mpz_srcptr x)
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
  if (BITS_PER_CHAR != 8) {
    fprintf(stderr,"BITS_PER_CHAR = %d\n",BITS_PER_CHAR);
    fprintf(stderr,"cmo-gmp.c does not work on this CPU.\n");
    fprintf(stderr,"Read the GMP source code and rewrite cmo-gmp.c.\n");
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

cmoGetGMPCoeff_new(MP_INT *x, struct cmoBuffer *cb)
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
  if (BITS_PER_CHAR != 8) {
    fprintf(stderr,"BITS_PER_CHAR = %d\n",BITS_PER_CHAR);
    fprintf(stderr,"cmo-gmp.c does not work on this CPU.\n");
    fprintf(stderr,"Read the GMP source code and rewrite cmo-gmp.c.\n");
    exit(10);
  }

  /* Read 4-byte size */

  xsize = getRawInt32(cb);
  neg_flag = xsize < 0;
  xsize = ABS(xsize);
  
  if (xsize == 0)
    {
      x->_mp_size = 0;
      return 1;			/* we've read 4 bytes */
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
