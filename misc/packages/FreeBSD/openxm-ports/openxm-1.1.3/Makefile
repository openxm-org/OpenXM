# $OpenXM: OpenXM/misc/packages/FreeBSD/openxm-ports/openxm-1.1.3/Makefile,v 1.6 2000/10/28 16:05:58 maekawa Exp $

## The name of this directory should be changed to openxm from openxm-1.1.3
## For debugging. 
#TAR_WORK=$(HOME)/tmp
#DISTDIR=${TAR_WORK}

## OpenXM is put under  ${PREFIX}/OpenXM and ${PREFIX}/bin, man
## The default value of PREFIX is /usr/local
## The next is for debugging.  Do not forget to mkdir tmp2/bin.
#PREFIX=$(HOME)/tmp2

## How to update pkg/PLIST
# (1) uncomment #PREFIX=, #DISTDIR. 
#     Put the source tar-ball under $DISTDIR
# (2) make 
#     Double check all the files are properly genered under work/openxm-1.1.3
# (3) (cd files ; make ; ./gen-plist.shell)
# (4) Copy plist-tmp2 to pkg/PLIST and commit pkg/PLIST

################  NOTE ########################
## Before typing in make, set your path to
## set path=(/usr/local/jdk1.1.8/bin $path)
## rehash

### How to test the port for openxm  
###     cd /usr/ports/math ; tar xzvf ${TAR_WORK}/openxm-port.tar.gz 
###     cp ${TAR_WORK}/openxm-1.1.3.tar.gz /usr/ports/distfiles
###     cd /usr/ports/math/openxm ; make install
###     [Try also make package , make deinstall, pkg_delete openxm-1.1.3 , 
###      pkg_add openxm-1.1.3.tgz]

PORTNAME=openxm
PORTVERSION=1.1.3
CATEGORIES= math
MASTER_SITES= ftp://ftp.math.kobe-u.ac.jp/pub/OpenXM/1.1.3/

MAINTAINER= takayama@math.sci.kobe-u.ac.jp

.include <bsd.port.pre.mk>
BUILD_DEPENDS=  nkf:${PORTSDIR}/japanese/nkf \
	            platex:${PORTSDIR}/japanese/platex-euc \
				xdvi:${PORTSDIR}/japanese/xdvik-vflib \
                dvips:${PORTSDIR}/japanese/dvipsk-vflib \
				/usr/local/bin/perl:${PORTSDIR}/japanese/perl5 \
                jlatex2html:${PORTSDIR}/japanese/latex2html \
                jmakeinfo:${PORTSDIR}/japanese/texinfo \
				texi2html:${PORTSDIR}/textproc/texi2html \
                /usr/local/jdk1.1.8/bin/javac:${PORTSDIR}/java/jdk \
				bash:${PORTSDIR}/shells/bash2 \
			    autoconf:${PORTSDIR}/devel/autoconf \
## I could not find a package for makeinfo.
##                makeinfo:${PORTSDIR}/print/texinfo \
### Note:
### In order to make dvips work, you need to modify 
###  /usr/local/share/texmf/web2c/texmf.cnf. 
###  See /var/db/pkg/japanese/dvipsk-vflib/+DISPLAY.
### If your dvips does not work properly after this change, 
### uncomment the line 	xdvi:${PORTSDIR}/japanese/xdvik-vflib \
### However, xdvi itself does not work fine: try to generate pkfonts that 
### already exist.
### Unfortunately, japanese/vfxdvik is empty (2000, 3/4).

.if ${OSVERSION} >= 400000
BUILD_DEPENDS += bison:${PORTSDIR}/devel/bison
.endif

RUN_DEPENDS=/compat/linux/usr/i486-linux-libc5/lib/libc.so.5:${PORTSDIR}/emulators/linux_base \
		java:${PORTSDIR}/java/jdk \
##	    	/compat/linux/usr/bin/i386-redhat-linux-egcs:${PORTSDIR}/devel/linux_devtools \

			 

pre-install:
	-${RM} -rf ${PREFIX}/OpenXM


OpenXM_MANPAGES=${PREFIX}/OpenXM/man/man1/sm1.1 ${PREFIX}/OpenXM/man/man1/openxm.1

post-install :
# Reguild shell files to start binaries and copy it to ${PREFIX}/bin
#    Modify pkg/INSTALL, too.
	(cd ${PREFIX}/OpenXM/rc ; make clean ; make install PREFIX=${PREFIX})
#
	-${CP} $(OpenXM_MANPAGES) ${PREFIX}/man/man1
	-${GZIP_CMD} ${PREFIX}/man/man1/sm1.1 ${PREFIX}/man/man1/openxm.1
	-${CAT} pkg/MESSAGE

## see /usr/share/mk/bsd.port.mk ==> /usr/ports/Mk/bsd.port/mk
## make -d A
.include <bsd.port.post.mk>
