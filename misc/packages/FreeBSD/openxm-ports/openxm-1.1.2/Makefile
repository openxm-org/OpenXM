# $OpenXM: OpenXM/misc/packages/FreeBSD/openxm-ports/openxm-1.1.2/Makefile,v 1.3 2000/03/04 01:35:18 takayama Exp $

## For debugging.  Put openxm-1.1.2.tar.gz at DISTDIR
#DISTDIR=$(HOME)/tmp

## OpenXM is put under  ${PREFIX}/OpenXM and ${PREFIX}/bin, man
## The next is for debugging.  Do not forget to mkdir tmp2/bin.
#PREFIX=$(HOME)/tmp2

################  NOTE ########################
## Before typing in make, set your path to
## set path=(/usr/local/jdk1.1.8/bin $path)
## rehash

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
### If your dvips does not work properly, add the next one. 
### However, xdvi itself does not work fine: try to generate pkfonts that 
### already exist.
### Unfortunately, japanese/vfxdvik is empty (2000, 3/4).
				xdvi:${PORTSDIR}/japanese/xdvik-vflib \

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
