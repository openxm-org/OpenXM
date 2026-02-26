# $OpenXM: OpenXM/src/phcpack/Makefile,v 1.1 2005/08/15 06:17:22 ohara Exp $
OpenXM_HOME=../..
PREFIX=${OpenXM_HOME}
DISTDIR=${OpenXM_HOME}/../OpenXM_dist
RM = /bin/rm
MASTER_SITE=https://www.math.kobe-u.ac.jp/pub/OpenXM/misc/
# Release 2.3 of PHCpack. http://www2.math.uic.edu/~jan/download.html
DISTNAME=PHCv2_3p
DISTFILES=${DISTNAME}.tar.gz
WRKSRC=work/${DISTNAME}/Objects
PATCHFILES = ${DISTNAME}.diff

all : build

fetch:
	-mkdir work
	@if [ ! -f work/.fetch_done ]; then \
		${OpenXM_HOME}/bin/oxfetch.sh ${MASTER_SITE}${DISTFILES} ${DISTDIR}; \
	fi
	@touch work/.fetch_done

extract: fetch
	@if [ ! -f work/.extract_done ]; then \
	    cat ${DISTDIR}/${DISTFILES} | (cd work; tar xzvf -) ; \
	fi
	@touch work/.extract_done

patch: extract
	@if [ ! -f work/.patch_done ]; then \
	    cat ${PATCHFILES} | (cd ${WRKSRC}; patch -p0) ; \
	fi
	@touch work/.patch_done

build : patch
	@if [ ! -f work/.build_done ]; then \
		(cd ${WRKSRC}; ${MAKE}) ; \
	fi
	@touch work/.build_done

install: build
	@if [ ! -f work/.install_done ]; then \
		(cd ${WRKSRC}; ${MAKE} install) ; \
	fi
	@touch work/.install_done

clean:
	-$(RM) -rf work

distclean: clean
