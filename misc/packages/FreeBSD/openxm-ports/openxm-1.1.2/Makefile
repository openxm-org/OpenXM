# $OpenXM: OpenXM/misc/packages/FreeBSD/openxm-ports/openxm-1.1.2/Makefile,v 1.15 2000/03/06 03:29:21 takayama Exp $

## For debugging. 
#TAR_WORK=$(HOME)/tmp
#DISTDIR=${TAR_WORK}

## OpenXM is put under  ${PREFIX}/OpenXM and ${PREFIX}/bin, man
## The default value of PREFIX is /usr/local
## The next is for debugging.  Do not forget to mkdir tmp2/bin.
#PREFIX=$(HOME)/tmp2

################  NOTE ########################
## Before typing in make, set your path to
## set path=(/usr/local/jdk1.1.8/bin $path)
## rehash

DISTNAME=openxm-1.1.2
CATEGORIES= math
### Note
### We do not have openxm-1.1.2.tar.gz at the master site, because
### the source code distribution is not allowed for OpenXM_contrib2/asir2000.
### Put openxm-1.1.2.tar.gz under /usr/ports/distfiles

MAINTAINER= takayama@math.sci.kobe-u.ac.jp

BUILD_DEPENDS=  nkf:${PORTSDIR}/japanese/nkf \
	            platex:${PORTSDIR}/japanese/platex-euc \
                dvips:${PORTSDIR}/japanese/dvipsk-vflib \
				/usr/local/bin/perl:${PORTSDIR}/japanese/perl5 \
                jlatex2html:${PORTSDIR}/japanese/latex2html \
                makeinfo:${PORTSDIR}/japanese/texinfo \
				texi2html:${PORTSDIR}/textproc/texi2html \
                /usr/local/jdk1.1.8/bin/javac:${PORTSDIR}/java/jdk \
				bash:${PORTSDIR}/shells/bash2 \
			    autoconf:${PORTSDIR}/devel/autoconf \
#				xdvi:${PORTSDIR}/japanese/xdvik-vflib \
### Note:
### In order to make dvips work, you need to modify 
###  /usr/local/share/texmf/web2c/texmf.cnf. 
###  See /var/db/pkg/japanese/dvipsk-vflib/+DISPLAY.
### If your dvips does not work properly after this change, 
### uncomment the line 	xdvi:${PORTSDIR}/japanese/xdvik-vflib \
### However, xdvi itself does not work fine: try to generate pkfonts that 
### already exist.
### Unfortunately, japanese/vfxdvik is empty (2000, 3/4).

RUN_DEPENDS=/compat/linux/usr/i486-linux-libc5/lib/libc.so.5:${PORTSDIR}/emulators/linux_base \
		java:${PORTSDIR}/java/jdk \
##	    	/compat/linux/usr/bin/i386-redhat-linux-egcs:${PORTSDIR}/devel/linux_devtools \

			 

pre-install:
	-/bin/rm -rf ${PREFIX}/OpenXM


OpenXM_MANPAGES=${PREFIX}/OpenXM/man/man1/sm1.1 ${PREFIX}/OpenXM/man/man1/openxm.1

post-install :
# Reguild shell files to start binaries and copy it to ${PREFIX}/bin
#    Modify pkg/INSTALL, too.
	(cd ${PREFIX}/OpenXM/rc ; make clean ; make install PREFIX=${PREFIX})
#
	-cp $(OpenXM_MANPAGES) ${PREFIX}/man/man1
	-gzip ${PREFIX}/man/man1/sm1.1 ${PREFIX}/man/man1/openxm.1
	-cat pkg/MESSAGE

## see /usr/share/mk/bsd.port.mk ==> /usr/ports/Mk/bsd.port/mk
## make -d A
.include <bsd.port.mk>
