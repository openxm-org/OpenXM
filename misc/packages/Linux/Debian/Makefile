# $OpenXM$

VERSION=1.1.3
## configuration only for committers
WHO=taka@orange
FTP_AREA=/export/home01/ftp/pub/OpenXM/${VERSION}
TAR_WORK=$(HOME)/tmp

RM=rm

usage :
	@echo "Usage (Makefile for *.deb):"
	@echo "clean"
	@echo "build-noclean"
	@echo "build"
	@echo "copy-to-ftp-area"

clean:
	${RM} -rf work
#	(cd ../../Generic/openxm ; make clean-work )

build-noclean:
	-mkdir  work
	(cd ../../Generic/openxm ; make tar-ball )
	(cd work ; tar xzvf ${TAR_WORK}/openxm-${VERSION}.tar.gz)
	-(cd work/openxm-${VERSION} ; ln -s ../../debian)
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

