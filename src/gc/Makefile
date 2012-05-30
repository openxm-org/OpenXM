# $OpenXM: OpenXM/src/gc/Makefile,v 1.20 2012/05/13 09:05:46 ohara Exp $
OpenXM_HOME=../..
PREFIX=${OpenXM_HOME}
DISTDIR=${OpenXM_HOME}/../OpenXM_dist
RM = /bin/rm
MASTER_SITE=ftp://ftp.math.kobe-u.ac.jp/pub/OpenXM/misc/
# Original. http://www.hpl.hp.com/personal/Hans_Boehm/gc/gc_source/
DISTNAME=gc-7.2b
DISTFILES=${DISTNAME}.tar.gz
# WRKSRC=work/${DISTNAME}
WRKSRC=work/gc-7.2
PATCHFILES = ${OpenXM_HOME}/../OpenXM_contrib2/asir2000/gc-7.diff
#PATCHFILES = 

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
		chmod +x ${WRKSRC}/install-sh ; \
	fi
	@touch work/.extract_done

patch: extract
	@if [ ! -f work/.patch_done ]; then \
        for i in ${PATCHFILES} ; do \
	        cat $$i | (cd ${WRKSRC}; patch -p1) ; \
        done ; \
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
