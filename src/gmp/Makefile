# $OpenXM: OpenXM/src/gmp/Makefile,v 1.7 2004/03/25 01:52:18 takayama Exp $
OpenXM_HOME=../..
PREFIX=${OpenXM_HOME}
DISTDIR=${OpenXM_HOME}/../OpenXM_dist
RM = /bin/rm -rf
MASTER_SITE=ftp://ftp.math.kobe-u.ac.jp/pub/OpenXM/misc/
DISTFILES=gmp-4.1.2.tar.gz
GMP=gmp-4.1.2

all : build 

fetch:
	@if [ ! -f .fetch_done ]; then \
		${OpenXM_HOME}/bin/oxfetch.sh ${MASTER_SITE}${DISTFILES} ${DISTDIR}; \
	fi 
	@touch .fetch_done

extract: fetch
	-mkdir work
	@if [ ! -f .extract_done ]; then \
	    cat ${DISTDIR}/${DISTFILES} | (cd work ; tar xzvf -) ; \
	fi 
	@touch .extract_done

patch: extract
	@touch .patch_done

build : patch
	@if test ! -f .build_done ; then \
		(cd work/${GMP} ; ./configure --prefix `cd ../..; cd ${OpenXM_HOME}; pwd`; ${MAKE}) ; \
	fi
	@touch .build_done

install: build
	-mkdir ${PREFIX}/bin ${PREFIX}/lib
	@if [ ! -f .install_done ]; then \
		(cd work/${GMP} ; ${MAKE} install) ; \
		for i in config.h gmp-impl.h gmp-mparam.h; do \
			cp work/${GMP}/$$i ${OpenXM_HOME}/include ; \
		done ; \
	fi
	@touch .install_done

clean:
	-$(RM) ./.*_done
	-$(RM) -rf work

distclean: clean
