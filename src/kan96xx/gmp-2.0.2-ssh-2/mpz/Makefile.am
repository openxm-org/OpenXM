## Process this file with automake to produce Makefile.in

# Copyright (C) 1991, 1993, 1994, 1996 Free Software Foundation, Inc.

# This file is part of the GNU MP Library.

# The GNU MP Library is free software; you can redistribute it and/or modify
# it under the terms of the GNU Library General Public License as published by
# the Free Software Foundation; either version 2 of the License, or (at your
# option) any later version.

# The GNU MP Library is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
# License for more details.

# You should have received a copy of the GNU Library General Public License
# along with the GNU MP Library; see the file COPYING.LIB.  If not, write to
# the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
# MA 02111-1307, USA.

AUTOMAKE_OPTIONS = 1.0 foreign no-dependencies

SUBDIRS = tests

noinst_LIBRARIES = libmpz.a

libmpz_a_SOURCES = init.c set.c set_ui.c set_si.c set_str.c set_d.c set_f.c \
  set_q.c iset.c iset_ui.c iset_si.c iset_str.c iset_d.c clear.c \
  get_ui.c get_si.c get_str.c get_d.c size.c sizeinbase.c \
  add.c add_ui.c sub.c sub_ui.c mul.c mul_ui.c \
  gcd.c gcd_ui.c gcdext.c sqrt.c sqrtrem.c powm.c powm_ui.c \
  cmp.c cmp_ui.c cmp_si.c mul_2exp.c fdiv_q_2exp.c fdiv_r_2exp.c \
  tdiv_q_2exp.c tdiv_r_2exp.c abs.c neg.c com.c and.c ior.c \
  inp_raw.c inp_str.c out_raw.c out_str.c \
  perfsqr.c random.c random2.c pow_ui.c ui_pow_ui.c setbit.c \
  clrbit.c fac_ui.c pprime_p.c realloc.c getlimbn.c popcount.c hamdist.c \
  cdiv_qr.c cdiv_q.c cdiv_r.c cdiv_qr_ui.c cdiv_q_ui.c cdiv_r_ui.c cdiv_ui.c \
  fdiv_qr.c fdiv_q.c fdiv_r.c fdiv_qr_ui.c fdiv_q_ui.c fdiv_r_ui.c fdiv_ui.c \
  tdiv_qr.c tdiv_q.c tdiv_r.c tdiv_qr_ui.c tdiv_q_ui.c tdiv_r_ui.c \
  mod.c divexact.c array_init.c scan0.c scan1.c \
  jacobi.c legendre.c invert.c

EXTRA_DIST = dmincl.c

INCLUDES = -I.. -I../mpn -I$(srcdir)/..
