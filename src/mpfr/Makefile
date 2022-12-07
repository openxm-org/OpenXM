# $OpenXM: OpenXM/src/mpfr/Makefile,v 1.5 2015/09/28 12:30:57 takayama Exp $
OpenXM_HOME=../..
PREFIX=${OpenXM_HOME}
DISTDIR=${OpenXM_HOME}/../OpenXM_dist
RM = /bin/rm
MASTER_SITE=http://www.math.kobe-u.ac.jp/pub/OpenXM/misc/
DISTNAME=mpfr-3.1.3
DISTFILES=${DISTNAME}.tar.bz2
WRKSRC=work/${DISTNAME}
#PATCHFILES = mpfr.diff
PATCHFILES = 

all : build 

fetch:
	-mkdir work
	@if [ ! -f work/.fetch_done ]; then \
		${OpenXM_HOME}/bin/oxfetch.sh ${MASTER_SITE}${DISTFILES} ${DISTDIR}; \
	fi 
	@touch work/.fetch_done

extract: fetch
	@if [ ! -f work/.extract_done ]; then \
	    cat ${DISTDIR}/${DISTFILES} | (cd work ; tar xjvf -) ; \
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
	(cd ${WRKSRC}; CC=${CC} ./configure --prefix=$$prefix --with-gmp-lib="$$prefix/lib" --with-gmp-include="$$prefix/include" --disable-thread-safe --enable-static --disable-shared)
configure-msys:
	prefix=`cd ${OpenXM_HOME}; pwd`  ; \
	(cd ${WRKSRC}; CC=${CC} ./configure --prefix=$$prefix --with-gmp-lib="$$prefix/lib" --with-gmp-include="$$prefix/include" --disable-thread-safe --build=x86_64-w64-mingw32 --disable-thread-safe --enable-static --disable-shared )
configure-others:
	prefix=`cd ${OpenXM_HOME}; pwd`  ; \
	(cd ${WRKSRC}; CC=${CC} ./configure --prefix=$$prefix --with-gmp-lib="$$prefix/lib" --with-gmp-include="$$prefix/include" --disable-thread-safe)
configure-cross:
	prefix=`cd ${OpenXM_HOME}; pwd`  ; \
	(cd ${WRKSRC}; CC=${CC} ./configure --host=${TARGET_TYPE} --prefix=$$prefix --with-gmp-lib="$$prefix/lib" --with-gmp-include="$$prefix/include" --disable-thread-safe)

build : configure
	@if [ ! -f work/.build_done ]; then \
		(cd ${WRKSRC} ; ${MAKE} CC=${CC}) ; \
	fi
	@touch work/.build_done

install: build
	-mkdir ${PREFIX}/bin ${PREFIX}/lib
	@if [ ! -f work/.install_done ]; then \
		(cd ${WRKSRC} ; ${MAKE} install) ; \
	fi
	@touch work/.install_done

clean:
	-${RM} -rf work

distclean: clean
