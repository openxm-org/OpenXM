# $OpenXM: OpenXM/src/gnuplot/Makefile,v 1.12 2004/04/14 03:04:04 ohara Exp $

OpenXM_HOME = ../..
MASTER_SITE = ftp://ftp.math.kobe-u.ac.jp/pub/OpenXM/misc/
DISTNAME    = gnuplot-4.0.0
DISTFILES   = ${DISTNAME}.tar.gz
DISTDIR     = ${OpenXM_HOME}/../OpenXM_dist
WRKSRC      = work/${DISTNAME}
PATCH_FILES = gnuplot-v4.diff
INSTALL_PROGRAM = install -c -s

all: build

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
	@if [ ! -f work/.patch_done ]; then \
		for i in ${PATCH_FILES}; do cat $$i | (cd ${WRKSRC}; patch -p1); done;\
	fi
	@touch work/.patch_done

configure: patch
	if [ ! -f work/.configure_done ]; then \
		prefix=`cd ${OpenXM_HOME}; pwd` ; \
		(cd ${WRKSRC} ; ./configure --with-x --prefix="$$prefix" ) ; \
	fi
	@touch work/.configure_done

build: configure
	@if [ ! -f work/.build_done ]; then \
	    (cd ${WRKSRC}/src; ${MAKE}) ; \
	fi
	@touch work/.build_done

install: build
	@if [ ! -f work/.install_done ]; then \
		prefix=`cd ${OpenXM_HOME}; pwd`; \
		mkdir -p $${prefix}/libexec/gnuplot/4.0; \
		(cd ${WRKSRC}/src; \
		 ${INSTALL_PROGRAM} gnuplot     $${prefix}/bin/gnuplot4ox ; \
		 ${INSTALL_PROGRAM} gnuplot_x11 $${prefix}/libexec/gnuplot/4.0) ; \
	fi
	@touch work/.install_done

clean:
	-rm -rf work

distclean: clean
	-rm -f ${OpenXM_HOME}/bin/gnuplot4ox ${OpenXM_HOME}/bin/gnuplot_x11
