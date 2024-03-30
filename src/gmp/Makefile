# $OpenXM: OpenXM/src/gmp/Makefile,v 1.24 2021/03/24 14:32:47 fujimoto Exp $
OpenXM_HOME=../..
PREFIX=${OpenXM_HOME}
DISTDIR=${OpenXM_HOME}/../OpenXM_dist
RM = /bin/rm
MASTER_SITE=http://www.math.kobe-u.ac.jp/pub/OpenXM/misc/
DISTNAME=gmp-6.2.1
INTELMACFILE=libgmp-intelmac-6.3.0.tar.gz
DISTFILES=${DISTNAME}.tar.bz2
WRKSRC=work/${DISTNAME}
#PATCHFILES = gmp.diff
PATCHFILES =  appleM1.diff

all : build 

fetch:
	-mkdir work
	@if [ ! -f work/.fetch_done ]; then \
		${OpenXM_HOME}/bin/oxfetch.sh ${MASTER_SITE}${DISTFILES} ${DISTDIR}; \
		${OpenXM_HOME}/bin/oxfetch.sh ${MASTER_SITE}${INTELMACFILE} ${DISTDIR}; \
	fi 
	@touch work/.fetch_done

extract: fetch
	@if [ ! -f work/.extract_done ]; then \
	    cat ${DISTDIR}/${DISTFILES} | (cd work ; tar xjvf -) ; \
	fi 
	@touch work/.extract_done

patch: extract
	@if [ ! -f work/.patch_done ]; then \
        for i in ${PATCHFILES} "" ; do \
            if [ -f "$$i" ]; then \
	            cat $$i | (cd ${WRKSRC}; patch -p1) ; \
            fi ; \
        done ; \
	fi
	@touch work/.patch_done

configure : patch
	@if [ ! -f work/.configure_done ]; then \
		prefix=`cd ${OpenXM_HOME}; pwd` ; \
		if [ -z "${TARGET_TYPE}" ]; then \
			if [  "`uname | sed -e 's/_.*$$//'`" = "CYGWIN" ]; then \
			    ${MAKE} configure-cygwin ; \
			elif [  "`uname | sed -e 's/_.*$$//'`" = "MSYS" ];  then \
			    ${MAKE} configure-msys ; \
			elif [  "`uname | sed -e 's/_.*$$//'`" = "Darwin" ] && [  "`uname -m`" = "x86_64" ]; then \
			    ${MAKE} configure-darwin-x86-64 ; \
			else \
			    ${MAKE} configure-others ; \
			fi ; \
	    else \
		    ${MAKE} configure-cross ; \
		fi ; \
	fi
	@touch work/.configure_done

configure-cygwin:
	prefix=`cd ${OpenXM_HOME}; pwd`  ; \
	(cd ${WRKSRC}; CC=${CC} ./configure  --prefix=$$prefix --enable-cxx --enable-static --disable-shared)
configure-msys:
	prefix=`cd ${OpenXM_HOME}; pwd`  ; \
	(cd ${WRKSRC}; CC=${CC} ./configure --prefix=$$prefix --enable-cxx --build=x86_64-w64-mingw32  )
configure-darwin-x86-64:
	(cd ../..; tar xzvf ../OpenXM_dist/${INTELMACFILE}; cd lib; a=`/bin/pwd`; \
	sed -e s:__LIBDIR__:$$a: libgmp.la.tmpl > libgmp.la; \
	sed -e s:__LIBDIR__:$$a: libgmpxx.la.tmpl > libgmpxx.la )
	@touch work/.build_done
	@touch work/.install_done
configure-others:
	prefix=`cd ${OpenXM_HOME}; pwd`  ; \
	(cd ${WRKSRC}; CC=${CC} ./configure --prefix=$$prefix --enable-cxx)
configure-cross:
	prefix=`cd ${OpenXM_HOME}; pwd`  ; \
	(cd ${WRKSRC}; CC=${CC} ./configure --host=${TARGET_TYPE} --prefix=$$prefix --enable-cxx)

build : configure
	@if [ ! -f work/.build_done ]; then \
		(cd ${WRKSRC} ; ${MAKE} CC=${CC}) ; \
	fi
	@touch work/.build_done

install: build
	-mkdir ${PREFIX}/bin ${PREFIX}/lib
	@if [ ! -f work/.install_done ]; then \
		(cd ${WRKSRC} ; ${MAKE} install) ; \
		for i in config.h gmp-impl.h gmp-mparam.h fib_table.h mp_bases.h fac_table.h ; do \
			cp ${WRKSRC}/$$i ${OpenXM_HOME}/include ; \
		done ; \
	fi
	@touch work/.install_done

clean:
	-${RM} -rf work

distclean: clean
