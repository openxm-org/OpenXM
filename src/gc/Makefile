# $OpenXM: OpenXM/src/gc/Makefile,v 1.7 2004/03/25 01:52:18 takayama Exp $
OpenXM_HOME=../..
PREFIX=${OpenXM_HOME}
DISTDIR=${OpenXM_HOME}/../OpenXM_dist
RM = /bin/rm
MASTER_SITE=ftp://ftp.math.kobe-u.ac.jp/pub/OpenXM/misc/
DISTFILES=gc6.2.tar.gz
PATCH_INTERIX = ${OpenXM_HOME}/../OpenXM_contrib2/asir2000/gc_interix.diff

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
	@if [ ! -f .patch_done ]; then \
	    cat ${PATCH_INTERIX} | (cd work/gc6.2 ; patch -p0) ; \
	fi
	@touch .patch_done

build : patch
	@if [ ! -f .build_done ]; then \
		(cd work/gc6.2 ; ./configure --prefix `cd ../..; cd ${OpenXM_HOME}; pwd`; ${MAKE}) ; \
	fi
	@touch .build_done

install: build
	@if [ ! -f .install_done ]; then \
		(cd work/gc6.2 ; ${MAKE} install) ; \
	fi
	@touch .install_done

clean:
	-$(RM) ./.*_done
	-$(RM) -rf work

distclean: clean

