# $OpenXM: OpenXM/src/texmacs/texmacs/Makefile,v 1.4 2004/12/15 11:58:31 ohara Exp $
OpenXM_HOME=../../..
PREFIX=${OpenXM_HOME}
DISTDIR=${OpenXM_HOME}/../OpenXM_dist
RM = /bin/rm
MASTER_CITES=http://www.math.kobe-u.ac.jp/pub/OpenXM/misc/
# MASTER_CITES2 = 
DISTNAME=TeXmacs-1.0.4.5-src
DISTFILES=${DISTNAME}.tar.gz
WRKSRC=work/${DISTNAME}
PATCH_FILES=TeXmacs.diff
# Use gnu make on freebsd.  On Debian type in   make  MAKE=make
MAKE=gmake
guile=$$(which guile-config)

all: build 

fetch:  check-guile
	-mkdir work
	@if [ ! -f work/.fetch_done ]; then \
		${OpenXM_HOME}/bin/oxfetch.sh ${MASTER_CITES}${DISTFILES} ${DISTDIR}; \
	fi 
	@touch work/.fetch_done

extract: fetch
	@if [ ! -f work/.extract_done ]; then \
	    cat ${DISTDIR}/${DISTFILES} | (cd work; tar xzvf -) ; \
	fi 
	@touch work/.extract_done

patch: extract
	@if [ ! -f work/.patch_done ]; then \
		for i in ${PATCH_FILES} ; do \
			cat $$i | (cd ${WRKSRC}; patch -p1) ; \
		done ; \
	fi 
	@touch work/.patch_done

configure: patch
	@if [ ! -f work/.configure_done ]; then \
		prefix=$$(cd ../../../../OpenXM; pwd) ;\
		with_iconv=--with-iconv$$(i=/usr/local; if [ -f $$i/include/iconv.h ]; then echo =$$i; fi) ;\
		(cd ${WRKSRC}; ./configure --prefix=$${prefix} $${with_iconv}) ; \
	fi
	@touch work/.configure_done

build: configure
	@if [ ! -f work/.build_done ]; then \
		(cd ${WRKSRC}; ${MAKE}) ; \
	fi
	@touch work/.build_done

install: build
	@if [ ! -f work/.install_done ]; then \
		(cd ${WRKSRC}; ${MAKE} install) ; \
	fi
	@touch work/.post-install_done

clean:
	-${RM} -rf work

distclean: clean
	-${RM} -rf ${OpenXM_HOME}/share/TeXmacs* ${OpenXM_HOME}/bin/texmacs

check-guile:
	@if [ ! -x "${guile}" ]; then \
		echo "Error ----------------------------------------------------------" ; \
		echo "    Please install guile/scheme before installing texmacs." ; \
		echo "    cf. ftp://ftp.gnu.org/pub/gnu/guile " ; \
		echo "Hint: guile-config, portinstall, whereis, apt-get, lib/libguile, ldconfig, ..." ; \
		echo "      http://www.texmacs.org/tmweb/download/requirements.en.html" ; \
	fi

## Note on debian.
##  apt-get install libguile-dev (buile-1.6 ...?)
