# $OpenXM: OpenXM/src/tigers/Makefile,v 1.5 2005/01/18 16:24:22 ohara Exp $
OpenXM_HOME=../..
PREFIX=${OpenXM_HOME}
DISTDIR=${OpenXM_HOME}/../OpenXM_dist
RM = /bin/rm
RANLIB=ranlib
INSTALL_PROGRAM=install -c -s
DISTNAME=TiGERS_0.9
PATCH_FILE=tigers.patch
MASTER_SITE=https://www.math.kobe-u.ac.jp/pub/OpenXM/misc/
DIST_SUFFIX=.taz
DISTFILES=${DISTNAME}${DIST_SUFFIX}

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
	    if [ -f "${PATCH_FILE}" ]; then \
	        for i in NOTE.txt README.change memo.txt; do \
	            touch work/${DISTNAME}/$$i ; \
	        done ; \
	        cat ${PATCH_FILE} | (cd work/${DISTNAME} ; patch -p1 -f) ; \
	    fi \
	fi 
	@touch work/.patch_done

build: patch
	@if [ ! -f work/.build_done ]; then \
		(cd work/${DISTNAME}; ${MAKE} all) ; \
	fi
	@touch work/.build_done

install: build
	-mkdir ${PREFIX}/bin
	@if [ ! -f work/.install_done ]; then \
		bindir=$$(cd ${PREFIX}/bin; pwd); \
		(cd work/${DISTNAME}; ${INSTALL_PROGRAM} tigers $${bindir}); \
	fi
	@touch work/.install_done

clean:
	-${RM} -rf work

distclean: clean
