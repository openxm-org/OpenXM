# $OpenXM: OpenXM/misc/packages/FreeBSD/openxm-ports/openxm-1.1.2/Makefile,v 1.2 2000/03/03 05:13:26 takayama Exp $

## For debugging.  Put openxm-1.1.2.tar.gz at DISTDIR
#DISTDIR=$(HOME)/tmp

## OpenXM is put under  ${PREFIX}/OpenXM and ${PREFIX}/bin, man
## The next is for debugging.  Do not forget to mkdir tmp2/bin.
#PREFIX=$(HOME)/tmp2

DISTNAME=openxm-1.1.2
CATEGORIES= math
MASTER_SITES=ftp://ftp.math.kobe-u.ac.jp/pub/OpenXM/1.1.2/

MAINTAINER= takayama@math.sci.kobe-u.ac.jp

BUILD_DEPENDS=  nkf:${PORTSDIR}/japanese/nkf \
	            platex:${PORTSDIR}/japanese/platex-euc \
                dvips:${PORTSDIR}/japanese/dvipsk-vflib \
                jlatex2html:${PORTSDIR}/japanese/latex2html \
                makeinfo:${PORTSDIR}/japanese/texinfo \
                javac:${PORTSDIR}/java/jdk \
		bash:${PORTSDIR}/shells/bash2 \

RUN_DEPENDS=    

post-install :
# Reguild shell files to start binaries and copy it to ${PREFIX}/bin
#    Modify pkg/INSTALL, too.
	(cd ${PREFIX}/OpenXM/rc ; make clean ; make install PREFIX=${PREFIX})
#
	-gzip ${PREFIX}/man/man1/*
	echo "Take asir key following the instruction appearing in the starting message of asir."

## see /usr/share/mk/bsd.port.mk ==> /usr/ports/Mk/bsd.port/mk
## make -d A
.include <bsd.port.mk>
