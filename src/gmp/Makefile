# $OpenXM: OpenXM/src/gmp/Makefile,v 1.9 2004/03/25 04:54:36 takayama Exp $
OpenXM_HOME=../..
PREFIX=${OpenXM_HOME}
DISTDIR=${OpenXM_HOME}/../OpenXM_dist
RM = /bin/rm
MASTER_SITE=ftp://ftp.math.kobe-u.ac.jp/pub/OpenXM/misc/
DISTFILES=gmp-4.1.2.tar.gz
GMP=gmp-4.1.2

all : build 

fetch:
	-mkdir work
	@if [ ! -f work/.fetch_done ]; then \
		${OpenXM_HOME}/bin/oxfetch.sh ${MASTER_SITE}${DISTFILES} ${DISTDIR}; \
	fi 
	@touch work/.fetch_done

extract: fetch
	@if [ ! -f work/.extract_done ]; then \
	    cat ${DISTDIR}/${DISTFILES} | (cd work ; tar xzvf -) ; \
	fi 
	@touch work/.extract_done

patch: extract
	@touch work/.patch_done

configure: patch
	@if [ ! -f work/.configure_done ]; then \
		(cd work/${GMP} ; ./configure --prefix `cd ../..; cd ${OpenXM_HOME}; pwd`) ; \
	fi
	@touch work/.configure_done

build : configure
	@if [ ! -f work/.build_done ]; then \
		(cd work/${GMP} ; ${MAKE}) ; \
	fi
	@touch work/.build_done

install: build
	-mkdir ${PREFIX}/bin ${PREFIX}/lib
	@if [ ! -f work/.install_done ]; then \
		(cd work/${GMP} ; ${MAKE} install) ; \
		for i in config.h gmp-impl.h gmp-mparam.h; do \
			cp work/${GMP}/$$i ${OpenXM_HOME}/include ; \
		done ; \
	fi
	@touch work/.install_done

clean:
	-$(RM) -rf work

distclean: clean
