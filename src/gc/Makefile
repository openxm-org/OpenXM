# $OpenXM: OpenXM/src/gc/Makefile,v 1.11 2004/04/21 11:32:07 takayama Exp $
OpenXM_HOME=../..
PREFIX=${OpenXM_HOME}
DISTDIR=${OpenXM_HOME}/../OpenXM_dist
RM = /bin/rm
MASTER_SITE=ftp://ftp.math.kobe-u.ac.jp/pub/OpenXM/misc/
DISTNAME=gc6.3
DISTFILES=${DISTNAME}.tar.gz
WRKSRC=work/${DISTNAME}
PATCH_INTERIX = ${OpenXM_HOME}/../OpenXM_contrib2/asir2000/gc_interix.diff

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
	    cat ${PATCH_INTERIX} | (cd ${WRKSRC}; patch -p0) ; \
	fi
	@touch work/.patch_done

configure : patch
	@if [ ! -f work/.configure_done ]; then \
		prefix=`cd ${OpenXM_HOME}; pwd` ; \
		(cd ${WRKSRC}; ./configure --disable-threads --prefix="$$prefix" ) ; \
	fi
	@touch work/.configure_done

build : configure
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
