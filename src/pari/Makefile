# $OpenXM: OpenXM/src/pari/Makefile,v 1.19 2004/12/07 13:08:45 ohara Exp $

PARI_VER=2.0
#PARI_VER=2.2

all: build

fetch extract patch configure build build-doc install install-doc clean distclean:
	${MAKE} -f Makefile.${PARI_VER} $@
