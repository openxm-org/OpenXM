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

TESTS = reuse t-add t-conv t-dm2exp t-muldiv t-sqrt t-sub

EXTRA_PROGRAMS = reuse t-add t-conv t-dm2exp t-muldiv t-sqrt t-sub

reuse_SOURCES = reuse.c
t_add_SOURCES = t-add.c ref.c
t_conv_SOURCES = t-conv.c
t_dm2exp_SOURCES = t-dm2exp.c
t_muldiv_SOURCES = t-muldiv.c
t_sqrt_SOURCES = t-sqrt.c
t_sub_SOURCES = t-sub.c ref.c

CLEANFILES = $(EXTRA_PROGRAMS)
