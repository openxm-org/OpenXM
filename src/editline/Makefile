# $OpenXM$
OpenXM_HOME=../..
PREFIX=${OpenXM_HOME}
DISTDIR=${OpenXM_HOME}/../OpenXM_dist
RM = /bin/rm
MASTER_SITE=https://www.math.kobe-u.ac.jp/pub/OpenXM/misc/
# Original: https://thrysoee.dk/editline/
DISTNAME=libedit-20210910-3.1
DISTFILES=${DISTNAME}.tar.gz
WRKSRC=work/${DISTNAME}
PATCHFILES = libedit.diff

all : build

fetch:
	-mkdir work
	@if [ ! -f work/.fetch_done ]; then \
		for i in ${DISTFILES}; do ${OpenXM_HOME}/bin/oxfetch.sh ${MASTER_SITE}$$i ${DISTDIR}; done ; \
	fi
	@touch work/.fetch_done

extract: fetch
	@if [ ! -f work/.extract_done ]; then \
		for i in ${DISTFILES}; do cat ${DISTDIR}/$$i | (cd work; tar xzvf -) ; done ; \
	fi
	@touch work/.extract_done

patch: extract
	@if [ ! -f work/.patch_done ]; then \
        for i in ${PATCHFILES} ; do \
	        cat $$i | (cd ${WRKSRC}; patch -p0) ; \
        done ; \
	fi
	@touch work/.patch_done

configure : patch
	@if [ ! -f work/.configure_done ]; then \
		prefix=`cd ${OpenXM_HOME}; pwd` ; \
		(cd ${WRKSRC}; ./configure --prefix=$$prefix); \
	fi
	@touch work/.configure_done

build : configure
	@if [ ! -f work/.build_done ]; then \
		(cd ${WRKSRC}; ${MAKE}) ; \
	fi
	@touch work/.build_done

install: build
	@if [ ! -f work/.install_done ]; then \
		(cd ${WRKSRC}; ${MAKE} install-exec install-data) ; \
	fi
	@touch work/.install_done

clean:
	-$(RM) -rf work

distclean: clean
