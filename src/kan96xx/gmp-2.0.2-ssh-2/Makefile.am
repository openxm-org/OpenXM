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

AUTOMAKE_OPTIONS = 1.0 foreign dist-zip no-dependencies

SUBDIRS = mpn mpz mpf mpq mpbsd demos

lib_LIBRARIES = libgmp.a libmp.a
info_TEXINFOS = gmp.texi
include_HEADERS = gmp.h mp.h
noinst_HEADERS = gmp-impl.h longlong.h stack-alloc.h urandom.h

libgmp_a_SOURCES = memory.c mp_set_fns.c mp_clz_tab.c version.c stack-alloc.c \
  mp_bpl.c extract-double.c insert-double.c
libgmp_a_LIBADD = @LIBOBJS@

libmp_a_SOURCES = memory.c mp_set_fns.c mp_clz_tab.c version.c stack-alloc.c \
  mp_bpl.c extract-double.c insert-double.c

EXTRA_DIST = PROJECTS SPEED cre-mparam.c make.bat config.h.in acconfig.h

INCLUDES = -I. -I$(srcdir) -Impn -I$(srcdir)/mpn

libgmp.a: $(libgmp_a_OBJECTS) $(libgmp_a_LIBADD) all-recursive
	-rm -rf tmpdirgmp
	-mkdir tmpdirgmp
	for i in mpn mpz mpf mpq; \
	  do \
	    mkdir tmpdirgmp/$$i; \
	    ( cd tmpdirgmp/$$i; $(AR) x ../../$$i/lib$$i.a ); \
	  done
	cd tmpdirgmp; for i in */*.o; do mv $$i `echo $$i | sed 's@/@_@g'`;done
	cp $(libgmp_a_OBJECTS) $(libgmp_a_LIBADD) tmpdirgmp
	cd tmpdirgmp; $(AR) cru $@ *.o
	$(RANLIB) tmpdirgmp/$@
	-rm -f $@
	mv tmpdirgmp/$@ .
	-rm -rf tmpdirgmp

# There appear to be strange problems with ar and parallel makes; I've added
# dummy depend on libgmp.a here to serialize these.
libmp.a: $(libmp_a_OBJECTS) $(libmp_a_LIBADD) all-recursive libgmp.a
	-rm -rf tmpdirmp
	-mkdir tmpdirmp
	for i in mpn mpbsd; \
	  do \
	    mkdir tmpdirmp/$$i; \
	    ( cd tmpdirmp/$$i; $(AR) x ../../$$i/lib$$i.a ); \
	  done
	cp $(libmp_a_OBJECTS) $(libmp_a_LIBADD) tmpdirmp
	cd tmpdirmp; for i in */*.o; do mv $$i `echo $$i | sed 's@/@_@g'`; done
	cd tmpdirmp; $(AR) cru $@ *.o
	$(RANLIB) tmpdirmp/$@
	-rm -f $@
	mv tmpdirmp/$@ .
	-rm -rf tmpdirmp


## Code to copy mpn assembly code directories into the distribution.  This
## should really be in the mpn Makefile.am, but there is a bug in automake-1.0
## which makes that difficult to do.  So i've put this here for now.

ASMDIRS = a29k alpha clipper cray generic hppa i960 m68k m88k mips2 mips3 \
  msdos ns32k power powerpc32 powerpc64 pyr sh sparc32 sparc64 vax \
  x86 z8000 z8000x

# Copy assembly directories into the distribution.
dist-hook:
	for dir in $(ASMDIRS); do                     \
	  (cd $(srcdir)/mpn && tar cf - $$dir) |      \
             (cd $(distdir)/mpn && tar xpf -);        \
	done
