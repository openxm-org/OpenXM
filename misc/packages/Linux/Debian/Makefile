# $OpenXM: OpenXM/misc/packages/Linux/Debian/Makefile,v 1.2 2002/03/02 01:00:26 takayama Exp $

VERSION=1.2.1
## configuration only for committers
WHO=taka@orange
FTP_AREA=/export/home01/ftp/pub/OpenXM/${VERSION}
TAR_WORK=$(HOME)/tmp

RM=rm

usage :
	@echo "Usage (Makefile for *.deb):"
	@echo "clean"
	@echo "build, build-noclean"
	@echo "build-main"
	@echo "copy-to-ftp-area"

clean:
	${RM} -rf work
#	(cd ../../Generic/openxm ; make clean-work )

build-noclean:
	-mkdir  work
	(cd ../../Generic/openxm ; make tar-ball VERSION=${VERSION})
	(cd work ; tar xzvf ${TAR_WORK}/openxm-${VERSION}.tar.gz)
	-(cd work/openxm-${VERSION} ; ln -s ../../debian)
	make build-main
build-main: 
## Start of build
	(cd work/openxm-${VERSION}; fakeroot debian/rules build)
	(cd work/openxm-${VERSION}; fakeroot debian/rules install)
	(cd work/openxm-${VERSION}; fakeroot dpkg-buildpackage)

build :  clean 
	make build-noclean

copy-to-ftp-area:
	-scp work/openxm-*.deb ${WHO}:${FTP_AREA}
	-scp work/openxm_*.dsc ${WHO}:${FTP_AREA}
	-scp work/openxm_*.changes ${WHO}:${FTP_AREA}

