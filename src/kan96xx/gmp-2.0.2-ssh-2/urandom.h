/* urandom.h -- define urandom returning a full unsigned long random value.

Copyright (C) 1995, 1996 Free Software Foundation, Inc.

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

#if BITS_PER_MP_LIMB == 32 && !defined (__URANDOM)
long random ();
static inline unsigned long
urandom ()
{
  /* random() returns 31 bits, we want 32.  */
  return random () ^ (random () << 1);
}
#define __URANDOM
#endif

#if BITS_PER_MP_LIMB == 64 && !defined (__URANDOM)
long random ();
#ifdef _LONG_LONG_LIMB
static inline unsigned long long
urandom ()
{
  /* random() returns 31 bits, we want 64.  */
  return (unsigned long long) random () ^
    ((unsigned long long) random () << 31) ^
    ((unsigned long long) random () << 62);
}
#else
static inline unsigned long
urandom ()
{
  /* random() returns 31 bits, we want 64.  */
  return random () ^ (random () << 31) ^ (random () << 62);
}
#endif /* _LONG_LONG_LIMB */
#define __URANDOM
#endif
