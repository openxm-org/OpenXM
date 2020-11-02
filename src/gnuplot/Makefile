# $OpenXM: OpenXM/src/gnuplot/Makefile,v 1.20 2015/02/21 06:20:36 ohara Exp $

OpenXM_HOME = ../..
MASTER_SITE = http://www.math.kobe-u.ac.jp/pub/OpenXM/misc/
DISTNAME    = gnuplot-5.4.0
DISTFILES   = ${DISTNAME}.tar.gz
DISTDIR     = ${OpenXM_HOME}/../OpenXM_dist
WRKSRC      = work/${DISTNAME}
#PATCH_FILES = gnuplot-v4.diff
#PATCH_FILES = gnuplot4_interix.diff
PATCH_FILES =
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
		for i in ${PATCH_FILES} "" ; do if [ -f "$$i" ]; then cat $$i | (cd ${WRKSRC}; patch -p1); fi done;\
	fi
	@touch work/.patch_done

configure: patch
	if [ ! -f work/.configure_done ]; then \
		prefix=`cd ${OpenXM_HOME}; pwd` ; \
		(cd ${WRKSRC} ; ./configure --with-x --prefix="$$prefix" --without-pdf) ; \
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
		mkdir -p $${prefix}/libexec/gnuplot/5.4; \
		(cd ${WRKSRC}/src; \
		 ${INSTALL_PROGRAM} gnuplot     $${prefix}/bin/gnuplot ; \
		 ${INSTALL_PROGRAM} gnuplot_x11 $${prefix}/libexec/gnuplot/5.4) ; \
	fi
	@touch work/.install_done

clean:
	-rm -rf work

distclean: clean
	-rm -f ${OpenXM_HOME}/bin/gnuplot ${OpenXM_HOME}/bin/gnuplot_x11
