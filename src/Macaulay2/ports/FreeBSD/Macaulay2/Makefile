# $OpenXM$

## Comment out DISTDIR and PREFIX for FreeBSD ports
## /usr/ports/math/Macaulay2
# For debugging. 
#TAR_WORK=$(HOME)/tmp2
#DISTDIR=$(HOME)/tmp2
#PREFIX=$(HOME)/tmp2
## For OpenXM
DISTDIR=${OpenXM_HOME}/../OpenXM_dist
PREFIX=${OpenXM_HOME}
##### End of configuration

PORTNAME=Macaulay2
PORTVERSION=0.8.60
DISTFILES=Macaulay2-0.8.60-porting-src.tar.gz
CATEGORIES=math lang
## Real MASTER_SITES (uiuc) has not been ready.
## MASTER_SITES= ftp://ftp.math.uiuc.edu/Macaulay2/
MASTER_SITES= ftp://ftp.math.kobe-u.ac.jp/pub/misc/
WRKSRC=work
USE_GMAKE=yes
MAINTAINER= takayama@math.kobe-u.ac.jp
.include <bsd.port.pre.mk>
.if ${OSVERSION} < 400000
.error "This port is tested only on FreeBSD 4.x and gcc version 2.95.2. Macaulay2 requires gcc version 2.75 or later."
.endif

BUILD_DEPENDS=  gmake:${PORTSDIR}/devel/gmake 

#RUN_DEPENDS=

install: all post-install

post-install :
	cat work/Macaulay2-0.8.60-i386-FreeBSD*.tar.gz | (cd ${PREFIX}/lib ; tar xzvf - )
	(cd ${PREFIX}/lib/Macaulay2; ./setup)
	cp ${PREFIX}/lib/Macaulay2/bin/M2  ${PREFIX}/bin

mybackup:
	cp Makefile $(HOME)/that/OpenXM/M2
	cp -r patches $(HOME)/that/OpenXM/M2

generate-diff:
#	diff -c $(HOME)/tmp2/M2.org/Macaulay2/d/configure work/Macaulay2/d/configure >> patches/patch-cc

.include <bsd.port.post.mk>
