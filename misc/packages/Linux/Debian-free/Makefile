# $OpenXM$

## You need the following packages to build *.deb
## apt-get install fakeroot
## apt-get install debhelper
## ( apt-get install dh-make )

## How to  debug?
## ex1.
##    cd work/openxm-head ; fakeroot make -f debian/rules build

VERSION=head
## configuration only for committers
WHO=$(USER)@orange2
FTP_AREA=/home/ftp/pub/OpenXM/${VERSION}
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
# dpkg-buildpackage reads info from debian/control
	(cd work/openxm-${VERSION}; fakeroot dpkg-buildpackage -b -B)

build :  clean 
	make build-noclean

copy-to-ftp-area:
	-scp work/openxm-*.deb ${WHO}:${FTP_AREA}
	-scp work/openxm_*.dsc ${WHO}:${FTP_AREA}
	-scp work/openxm_*.changes ${WHO}:${FTP_AREA}

