# $OpenXM: OpenXM/misc/packages/Makefile,v 1.13 2000/03/11 10:32:27 takayama Exp $

VERSION=1.1.2
TAG=HEAD
## TAG=RELEASE_1_1_2

TAR_WORK=$(HOME)/tmp
NAME=taka
TAR_WORK_EXPLICIT=/home/${NAME}/tmp
JDK=jdk1.1.8
FTP_AREA=/export/home01/ftp/pub/OpenXM/${VERSION}
## TAG for the CVS server.

# Modify below for priting.
PS=a2ps-j
LPR=lpr -Pdousan

usage :
	@echo "Targets "
	@echo "  clean "
	@echo "  print "
	@echo "Do not forget to put java and javac in your search path." 
	@echo "Put your rcvs script by the name cvs-OpenXM under your search path."
	@echo "   (See Generic/openxm/Makefile for details) "
	@echo "1.  generate-tar-balls"
	@echo "  (update-plist)"
	@echo "2.  build-on-orange (do not forget to set .rpmrc. See Linux/RPM/Makefile)"
	@echo "3.  build-on-pear (do it as a super user)"
	@echo "4.  build-on-redhat (do not forget to set .rpmrc)"
	@echo "5.  build-on-unkei"
	@echo "6.  copy-to-ftp-area "

clean :
	-(cd Generic/openxm ; make clean-work)
	-(cd FreeBSD ; make clean)
	-(cd Linux/RPM ; make clean)

print:
	$(PS) Makefile | $(LPR)	
	$(PS) Generic/openxm/Makefile | $(LPR)	
	$(PS) Linux/RPM/Makefile | $(LPR)	
	$(PS) Linux/RPM/openxm-*spec.in | $(LPR)	
	$(PS) FreeBSD/openxm-ports/openxm-1.1.2/Makefile | $(LPR)	
	$(PS) FreeBSD/Makefile | $(LPR)	


generate-tar-balls:
	-mkdir ${TAR_WORK}
	make clean
	(cd Generic/openxm ; make TAG=${TAG} VERSION=${VERSION} tar-ball)
	(cd Generic/openxm ; make  TAG=${TAG} VERSION=${VERSION} tar-ball-01)
	make update-plist
	(cd FreeBSD   ; make TAG=${TAG} VERSION=${VERSION} make-tar-ball-for-openxm-port)
	(cd Linux/RPM ; make make-directory)
	(cd Linux/RPM ; make link-for-real-openxm)

update-plist:
	(cd FreeBSD ; make VERSION=${VERSION}  TAG=${TAG} update-plist )

build-on-orange:
	(cd Generic/openxm ; make  TAG=${TAG} VERSION=${VERSION} binary-tar-ball)
	(cd Generic/openxm ; make  TAG=${TAG} VERSION=${VERSION} shared-tar-ball)
	(cd Generic/openxm ; make  TAG=${TAG} VERSION=${VERSION} clean-work)
	(cd Linux/RPM ; make  TAG=${TAG} VERSION=${VERSION} build-shared-rpm)
	(cp Linux/RPM/work/RPMS/i386/*.rpm ${TAR_WORK})

# You should be a super user.
build-on-pear:
	cp ${TAR_WORK_EXPLICIT}/openxm-${VERSION}.tar.gz /usr/ports/distfiles
	(cd /usr/ports/math ; /bin/rm -rf openxm ; tar xzvf ${TAR_WORK_EXPLICIT}/openxm-port.tar.gz)
	(PATH=/usr/local/${JDK}/bin:$(PATH) ; export PATH ; cd /usr/ports/math/openxm ; make install ; make package ; make deinstall ; cp openxm-${VERSION}.tgz ${TAR_WORK_EXPLICIT} ; chown ${NAME} ${TAR_WORK_EXPLICIT}/openxm-${VERSION}.tgz)

build-on-redhat:
	(cd Generic/openxm ; make clean-for-make)
	(cd Generic/openxm ; make  TAG=${TAG} VERSION=${VERSION} binary-tar-ball)
	(cd Linux/RPM ; make build-binary-rpm)
	(cp Linux/RPM/work/RPMS/i386/*.rpm ${TAR_WORK})

build-on-unkei:
	(cd Generic/openxm ; make clean-for-make)
	(cd Generic/openxm ; make  TAG=${TAG} VERSION=${VERSION} binary-tar-ball)

copy-to-ftp-area:
	-mkdir ${FTP_AREA}
	cp ${TAR_WORK}/openxm*rpm ${FTP_AREA}
	cp ${TAR_WORK}/openxm01-${VERSION}.tar.gz ${FTP_AREA}
	cp ${TAR_WORK}/OpenXM-*.tgz ${FTP_AREA}
	cp ${TAR_WORK}/openxm-${VERSION}.tgz ${FTP_AREA}
	ls -l ${FTP_AREA}
