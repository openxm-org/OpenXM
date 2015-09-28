# $OpenXM: OpenXM/src/mpc/Makefile,v 1.2 2015/09/28 05:52:13 takayama Exp $
OpenXM_HOME=../..
PREFIX=${OpenXM_HOME}
DISTDIR=${OpenXM_HOME}/../OpenXM_dist
RM = /bin/rm
MASTER_SITE=http://www.math.kobe-u.ac.jp/pub/OpenXM/misc/
DISTNAME=mpc-1.0.3
DISTFILES=${DISTNAME}.tar.gz
WRKSRC=work/${DISTNAME}
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
	    cat ${DISTDIR}/${DISTFILES} | (cd work ; tar xzvf -) ; \
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
		if [  "`uname | sed -e 's/_.*$$//'`" = "CYGWIN" ]; then \
		  make configure-cygwin ; \
		elif [  "`uname | sed -e 's/_.*$$//'`" = "MSYS" ];  then \
		  make configure-msys ; \
		else \
		  make configure-others ; \
		fi ; \
	fi
	@touch work/.configure_done

configure-cygwin:
	prefix=`cd ${OpenXM_HOME}; pwd`  ; \
	(cd ${WRKSRC}; CC=${CC} ./configure  --prefix=$$prefix --with-gmp-lib="$$prefix/lib" --with-gmp-include="$$prefix/include"  --enable-static --disable-shared)
configure-msys:
	prefix=`cd ${OpenXM_HOME}; pwd`  ; \
	(cd ${WRKSRC}; CC=${CC} ./configure --prefix=$$prefix --with-gmp-lib="$$prefix/lib" --with-gmp-include="$$prefix/include"  --build=x86_64-w64-mingw32    --enable-static --disable-shared )
configure-others:
	prefix=`cd ${OpenXM_HOME}; pwd`  ; \
	(cd ${WRKSRC}; CC=${CC} ./configure --prefix=$$prefix --with-gmp-lib="$$prefix/lib" --with-gmp-include="$$prefix/include" )

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
