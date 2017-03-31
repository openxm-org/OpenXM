# $OpenXM: OpenXM/src/pari/Makefile,v 1.24 2017/03/31 02:18:33 takayama Exp $

# Choose one of pari version.
# Example.
# $ make install                 ( for PARI/GP 2.0.x)
# $ make PARI_VER=2.2 install    ( for PARI/GP 2.2.x)

#PARI_VER=2.0
#PARI_VER=2.2
PARI_VER=2.3

## for PARI/GP unstable (2.2)
PARI_DEVEL_VER=pari-2.2.8.alpha
PARI_DEVEL_SUFFIX=.tar.gz

## for PARI/GP unstable (2.2)
PARI_23_VER=pari-2.3.4
PARI_23_SUFFIX=.tar.gz

all: build

fetch extract patch configure build build-doc install install-doc clean distclean:
	@if [ "${PARI_VER}" = 2.3 ]; then \
		${MAKE} -f Makefile.2.0 PARI=${PARI_23_VER} DIST_SUFFIX=${PARI_23_SUFFIX} PATCH_FILE=${PARI_DEVEL_VER}.diff $@ ; \
	elif [ "${PARI_VER}" = 2.2 ]; then \
		${MAKE} -f Makefile.2.0 PARI=${PARI_DEVEL_VER} DIST_SUFFIX=${PARI_DEVEL_SUFFIX} $@ ; \
	else \
		${MAKE} -f Makefile.2.0 $@ ; \
	fi
