# $OpenXM: OpenXM/src/Macaulay2/ports/Debian/Macaulay2/Makefile,v 1.1 2001/01/06 08:19:52 takayama Exp $
# Makefile of M2 for Debian/potato.
#  OpenXM_HOME must be properly set.  If you want to install under /usr/local
#   type in  make PREFIX=/usr/local DISTDIR=/tmp
# make should be gmake (gmake --version)
# wget should be installed.  (dpkg install wget)
PREFIX=${OpenXM_HOME}
DISTDIR=${OpenXM_HOME}/../OpenXM_dist
RM = /bin/rm -rf
MASTER_CITES=ftp.math.kobe-u.ac.jp
# MASTER_CITES2 = www.math.cornell.edu/~htsai/Dmods/Dmods.tar
DISTFILES=Macaulay2-0.8.60-porting-src.tar.gz
DISTFILES2=Dmods.tar

all : build post-install

pre-fetch:
	-@if test ! -f .pre-fetch_done ; \
	then \
	  mkdir ${DISTDIR} ; \
	fi 
	@touch .pre-fetch_done


fetch: pre-fetch
	@if test ! -f .fetch_done ; \
	then \
	  (cd ${DISTDIR}; wget --passive-ftp ftp://$(MASTER_CITES)/pub/misc/$(DISTFILES)) ; \
	  (cd ${DISTDIR}; wget --passive-ftp ftp://$(MASTER_CITES)/pub/misc/$(DISTFILES2)) ; \
	fi 
	@touch .fetch_done

patch : fetch
	@if test ! -f .patch_done ; \
	then \
	  $(RM) work ; \
	  mkdir work ; \
	  (cd work; tar xzvf ${DISTDIR}/$(DISTFILES)) ; \
	  (cd work; patch -c -p0 <../patch-aa ; patch -c -p0 <../patch-bb ; patch -c -p0 <../patch-cc) ; \
	fi 
	@touch .patch_done

build : patch
	@if test ! -f .build_done ; \
	then \
		(cd work ; make ) ; \
	fi
	@touch .build_done

post-install: build
	-mkdir ${PREFIX}/lib ${PREFIX}/bin
	cat work/Macaulay2-0.8.60-i*-*inux*.tar.gz | (cd ${PREFIX}/lib ; tar xzvf - )
	(cd ${PREFIX}/lib/Macaulay2; ./setup)
	-(cd ${PREFIX}/lib/Macaulay2/m2 ; tar xvf ${DISTDIR}/Dmods.tar)
	cp ${PREFIX}/lib/Macaulay2/bin/M2  ${PREFIX}/bin

clean:
	$(RM) work .*_done