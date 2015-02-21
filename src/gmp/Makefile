# $OpenXM: OpenXM/src/gmp/Makefile,v 1.19 2012/02/18 05:11:51 takayama Exp $
OpenXM_HOME=../..
PREFIX=${OpenXM_HOME}
DISTDIR=${OpenXM_HOME}/../OpenXM_dist
RM = /bin/rm
MASTER_SITE=http://www.math.kobe-u.ac.jp/pub/OpenXM/misc/
DISTNAME=gmp-5.0.4
DISTFILES=${DISTNAME}.tar.bz2
WRKSRC=work/${DISTNAME}
#PATCHFILES = gmp.diff
PATCHFILES = 

all : build 

fetch:
	-mkdir work
	@if [ ! -f work/.fetch_done ]; then \
		${OpenXM_HOME}/bin/oxfetch.sh ${MASTER_SITE}${DISTFILES} ${DISTDIR}; \
	fi 
	@touch work/.fetch_done

extract: fetch
	@if [ ! -f work/.extract_done ]; then \
	    cat ${DISTDIR}/${DISTFILES} | (cd work ; tar xjvf -) ; \
	fi 
	@touch work/.extract_done

patch: extract
	@if [ ! -f work/.patch_done ]; then \
        for i in ${PATCHFILES} "" ; do \
            if [ -f "$$i" ]; then \
	            cat $$i | (cd ${WRKSRC}; patch -p0) ; \
            fi ; \
        done ; \
	fi
	@touch work/.patch_done

configure: patch
	@if [ ! -f work/.configure_done ]; then \
		prefix=`cd ${OpenXM_HOME}; pwd` ; \
		(cd ${WRKSRC} ; CC=${CC} ./configure --prefix="$$prefix" --enable-cxx) ; \
	fi
	@touch work/.configure_done

build : configure
	@if [ ! -f work/.build_done ]; then \
		(cd ${WRKSRC} ; ${MAKE} CC=${CC}) ; \
	fi
	@touch work/.build_done

install: build
	-mkdir ${PREFIX}/bin ${PREFIX}/lib
	@if [ ! -f work/.install_done ]; then \
		(cd ${WRKSRC} ; ${MAKE} install) ; \
		for i in config.h gmp-impl.h gmp-mparam.h fib_table.h mp_bases.h; do \
			cp ${WRKSRC}/$$i ${OpenXM_HOME}/include ; \
		done ; \
	fi
	@touch work/.install_done

clean:
	-${RM} -rf work

distclean: clean
