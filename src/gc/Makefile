# $OpenXM: OpenXM/src/gc/Makefile,v 1.29 2020/01/09 12:15:00 ohara Exp $
OpenXM_HOME=../..
PREFIX=${OpenXM_HOME}
DISTDIR=${OpenXM_HOME}/../OpenXM_dist
RM = /bin/rm
MASTER_SITE=http://www.math.kobe-u.ac.jp/pub/OpenXM/misc/
# Original. https://www.hboehm.info/gc/gc_source/
DISTNAME=gc-7.6.12
ATOMIC=libatomic_ops-7.6.10
DISTFILES=${DISTNAME}.tar.gz ${ATOMIC}.tar.gz
WRKSRC=work/${DISTNAME}
#PATCHFILES = ${OpenXM_HOME}/../OpenXM_contrib2/asir-gc/gc-7.diff gc-cygwin64.diff
PATCHFILES = 

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
		chmod +x ${WRKSRC}/install-sh ; \
	fi
	-(cd ${WRKSRC}; ln -sf ../${ATOMIC} libatomic_ops)
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
		if [ -z ${TARGET_TYPE} ]; then \
			if [  "`uname | sed -e 's/_.*$$//'`" = "CYGWIN" ]; then \
			  make configure-cygwin ; \
			elif [  "`uname | sed -e 's/_.*$$//'`" = "MSYS" ];  then \
			  make configure-msys ; \
			else \
			  make configure-others ; \
			fi ; \
		else \
		  make configure-cross ; \
		fi ; \
	fi
	@touch work/.configure_done

configure-cygwin:
	prefix=`cd ${OpenXM_HOME}; pwd`  ; \
	(cd ${WRKSRC}; ./configure --disable-threads --prefix=$$prefix )
configure-msys:
	prefix=`cd ${OpenXM_HOME}; pwd`  ; \
	(cd ${WRKSRC}; ./configure --disable-threads --prefix=$$prefix --build=x86_64-w64-mingw32  )
configure-others:
	prefix=`cd ${OpenXM_HOME}; pwd`  ; \
	(cd ${WRKSRC}; ./configure --disable-threads --prefix=$$prefix )
configure-cross:
	prefix=`cd ${OpenXM_HOME}; pwd`  ; \
	(cd ${WRKSRC}; ./configure --host=${TARGET_TYPE} --disable-threads --prefix=$$prefix )

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
