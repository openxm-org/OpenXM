# $OpenXM: OpenXM/src/texmacs/texmacs/Makefile,v 1.2 2004/03/01 03:10:27 takayama Exp $
OpenXM_HOME=../../..
PREFIX=${OpenXM_HOME}
DISTDIR=${OpenXM_HOME}/../OpenXM_dist
RM = /bin/rm -rf
MASTER_CITES=ftp.math.kobe-u.ac.jp
# MASTER_CITES2 = 
DISTFILES=TeXmacs-1.0.3.3-src.tar.gz
SRCDIR=TeXmacs-1.0.3.3-src
# Use gnu make on freebsd.  On Debian type in   make  MAKE=make
MAKE=gmake
guile=`which guile-config`

all : build 
install : post-install

fetch:  check-guile
	@if test ! -f .fetch_done ; \
	then \
		${OpenXM_HOME}/bin/oxfetch.sh ftp://${MASTER_CITES}/pub/OpenXM/misc/${DISTFILES} ${DISTDIR}; \
	fi 
	@touch .fetch_done

patch : fetch
	@if test ! -f .patch_done ; \
	then \
	  $(RM) work ; \
	  mkdir work ; \
	  (cd work ; tar xzvf ../${DISTDIR}/${DISTFILES} ) ; \
	fi 
	@touch .patch_done
#	  (cd work; patch -c -p0 <../patch-aa ; patch -c -p0 <../patch-bb ; patch -c -p0 <../patch-cc) ; \

build : patch
	@if test ! -f .build_done ; \
	then \
		(cd work/${SRCDIR} ; ./configure --prefix `cd ../../../../../../OpenXM; pwd` --with-iconv`i=/usr/local; if [ -f $$i/include/iconv.h ]; then echo =$$i; fi` ; ${MAKE} ) ; \
	fi
	@touch .build_done

post-install: build
	@if test ! -f .post-install_done ; \
	then \
		(cd work/${SRCDIR} ; ${MAKE} install) ; \
	fi
	@touch .post-install_done

clean: clean-usr-local
	$(RM)  .*_done
	-(cd work/${SRCDIR} ; ${MAKE} clean)
distclean:
	$(RM)  .*_done ./.use_ports
	-(cd work/${SRCDIR} ; ${MAKE} distclean)
clean-usr-local:
	-$(RM) ${OpenXM_HOME}/share/TeXmacs*
	-$(RM) ${OpenXM_HOME}/bin/texmacs

check-guile:
	@if [ -z ${guile} ] ; \
	then \
		echo "Error ---------------------------------------------------------- " ; \
		echo "    Please install guile/scheme before installing texmacs." ; \
		echo "    cf. ftp://ftp.gnu.org/pub/gnu/guile " ; \
		echo "Hint: guile-config, portinstall , whereis , apt-get ,lib/libguile, ldconfig, ... " ; \
		echo "      http://www.texmacs.org/tmweb/download/requirements.en.html" ; \
		exit ; \
	fi

## Note on debian.
##  apt-get install libguile-dev (buile-1.6 ...?)