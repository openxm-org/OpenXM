# $OpenXM: OpenXM/src/pcc/Makefile,v 1.4 2015/02/21 06:20:36 ohara Exp $
OpenXM_HOME=../..
PREFIX=${OpenXM_HOME}
DISTDIR=${OpenXM_HOME}/../OpenXM_dist
RM = /bin/rm
MASTER_SITE=https://www.math.kobe-u.ac.jp/pub/OpenXM/misc/
DISTNAME=pcc-20210323
DISTFILES=${DISTNAME}.tgz
WRKSRC=work/${DISTNAME}
BUILDDIR=${WRKSRC}/cpp
PATCHFILES = pcc.diff

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
	@if [ ! -f work/.patch_done ]; then \
        for i in ${PATCHFILES} "" ; do \
            if [ -f "$$i" ]; then \
	            cat $$i | (cd ${WRKSRC}; patch -p0) ; \
            fi ; \
        done ; \
		(cd work/${DISTNAME}; mv -f cc/cpp cpp; rm -rf cc); \
	fi
	@touch work/.patch_done

configure: patch
	@if [ ! -f work/.configure_done ]; then \
		prefix=`cd ${OpenXM_HOME}; pwd` ; \
		(cd ${WRKSRC} ; ./configure --prefix="$$prefix" --libexecdir="$$prefix"/bin ) ; \
	fi
	@touch work/.configure_done

build : configure
	@if [ ! -f work/.build_done ]; then \
		(cd ${BUILDDIR} ; ${MAKE}) ; \
	fi
	@touch work/.build_done

install: build
	-mkdir ${PREFIX}/bin ${PREFIX}/lib
	@if [ ! -f work/.install_done ]; then \
		(cd ${BUILDDIR} ; ${MAKE} install) ; \
	fi
	@touch work/.install_done

clean:
	-${RM} -rf work

distclean: clean
