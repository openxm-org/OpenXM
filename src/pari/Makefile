# $OpenXM: OpenXM/src/pari/Makefile,v 1.27 2020/08/26 06:03:31 noro Exp $

OpenXM_HOME=../..
PREFIX=${OpenXM_HOME}
DISTDIR=${OpenXM_HOME}/../OpenXM_dist
RM = /bin/rm
RANLIB=ranlib
PARI=pari-2.11.4
PATCH_FILE=${PARI}.diff
MASTER_SITE=http://www.math.kobe-u.ac.jp/pub/OpenXM/misc/
DIST_SUFFIX=.tar.gz
DISTFILES=${PARI}${DIST_SUFFIX}
GALFILES=galdata.tgz galpol.tgz

all: build 

fetch:
	-mkdir work
	@if [ ! -f work/.fetch_done ]; then \
	    for i in ${DISTFILES} ${GALFILES}; do \
	        ${OpenXM_HOME}/bin/oxfetch.sh ${MASTER_SITE}"$$i" ${DISTDIR}; \
	    done; \
	fi 
	@touch work/.fetch_done

extract: fetch
	@if [ ! -f work/.extract_done ]; then \
	    cat ${DISTDIR}/${DISTFILES} | (cd work ; tar xzvf -) ; \
	    for i in ${GALFILES}; do \
	        cat ${DISTDIR}/"$$i" | (cd work/${PARI} ; tar xzvf -) ; \
	    done; \
	fi 
	@touch work/.extract_done

patch: extract
	@if [ ! -f work/.patch_done ]; then \
	    if [ -f "${PATCH_FILE}" ]; then \
	        cat ${PATCH_FILE} | (cd work/${PARI} ; patch -p1 ) ; \
	    fi \
	fi 
	@touch work/.patch_done

configure: patch
	@if [ ! -f work/.configure_done ]; then \
		prefix=`cd ${OpenXM_HOME}; pwd` ; \
		(cd work/${PARI} ; ./Configure --prefix="$$prefix" ) ; \
	fi
	@touch work/.configure_done

build: configure
	@if [ ! -f work/.build_done ]; then \
		d=`cd work/${PARI} ; config/objdir` ; \
		(cd work/${PARI}/$$d ; ${MAKE} lib-sta; ${MAKE} gp) ; \
	fi
	@touch work/.build_done

build-doc: configure
	@if [ ! -f work/.build_doc_done ]; then \
		(cd work/${PARI} ; ${MAKE} doc) ; \
	fi
	@touch work/.build_doc_done

install: build
	-mkdir ${PREFIX}/bin ${PREFIX}/lib
	@if [ ! -f work/.install_done ]; then \
		d=`cd work/${PARI} ; config/objdir` ; \
		(cd work/${PARI}/$$d ; ${MAKE} install-lib-sta install-include install-lib-dyn install-data) ; \
		${RANLIB} ${PREFIX}/lib/libpari.a ; \
	fi
	@touch work/.install_done

install-doc: build-doc
	-mkdir ${PREFIX}/doc/pari
	@if [ ! -f work/.install_doc_done ]; then \
		for i in users.dvi tutorial.dvi; do \
			cp work/${PARI}/doc/$$i ${PREDIX}/doc/pari ; \
		done ; \
	fi
	@touch work/.install_doc_done

clean:
	-${RM} -rf work

distclean: clean
