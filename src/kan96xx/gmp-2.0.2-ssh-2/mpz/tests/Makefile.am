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

INCLUDES = -I../.. -I$(srcdir)/../.. -I../../mpn -I$(srcdir)/../../mpn
LDADD = ../../libgmp.a

TESTS = t-mul t-tdiv t-tdiv_ui t-fdiv t-fdiv_ui t-gcd t-gcd2 \
 dive t-sqrtrem convert io logic t-powm t-powm_ui reuse

EXTRA_PROGRAMS = t-mul t-tdiv t-tdiv_ui t-fdiv t-fdiv_ui t-gcd t-gcd2 \
 dive t-sqrtrem convert io logic t-powm t-powm_ui reuse

t_mul_SOURCES = t-mul.c
t_tdiv_SOURCES = t-tdiv.c
t_tdiv_ui_SOURCES = t-tdiv_ui.c
t_fdiv_SOURCES = t-fdiv.c
t_fdiv_ui_SOURCES = t-fdiv_ui.c
t_gcd_SOURCES = t-gcd.c
t_gcd2_SOURCES = t-gcd2.c
dive_SOURCES = dive.c
t_sqrtrem_SOURCES = t-sqrtrem.c
convert_SOURCES = convert.c
io_SOURCES = io.c
logic_SOURCES = logic.c
t_powm_SOURCES = t-powm.c
t_powm_ui_SOURCES = t-powm_ui.c
reuse_SOURCES = reuse.c

EXTRA_DIST = io-binary.c

CLEANFILES = $(EXTRA_PROGRAMS)
