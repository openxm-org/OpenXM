# $OpenXM: OpenXM/misc/packages/Makefile,v 1.35 2015/07/28 06:28:00 takayama Exp $

VERSION=1.3.3
TAG=HEAD
## TAG=RELEASE_1_3_3

### Configure below.
TAR_WORK=$(HOME)/tmp
NAME=taka
WHO=taka@orange
# It must be equal to TAR_WORK. To work as a super user. Used in build-on-pear-root.
TAR_WORK_EXPLICIT=/home/${NAME}/tmp
JDK=jdk1.1.8
FTP_AREA=/home/web/pub/OpenXM/${VERSION}
FTP_AREA_HEAD=/home/web/pub/OpenXM/Head
FTP_AREA_ASIR=/home/web/pub/asir
RCVS=cvs-OpenXM -d kerberos.math.sci.kobe-u.ac.jp:/usr/cvs
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
	@echo "OpenXM must be under the home directory. Otherwise, input like make OpenXM_HOME=/hoge/hoge/OpenXM ..."
	@echo "   (See Generic/openxm/Makefile for details) "
	@echo "1.  generate-tar-balls"
	@echo "  (update-plist)"
	@echo "  (or  generate-tar-balls-without-update-plist) "
	@echo "2.  build-on-new-orange (do not forget to set .rpmrc. See Linux/RPM/Makefile)"
	@echo "3.  build-on-pear-root (do it as a super user)"
	@echo "4.  build-on-redhat (do not forget to set .rpmrc)"
	@echo "5.  build-on-unkei"
	@echo "6.  copy-to-http-area "
	@echo "[[7]].  copy-head-tar-ball-to-http-area "
	@echo "7.b  copy-release-tar-ball-to-http-area "
	@echo "8.  copy-asir-head-tar-ball-to-http-area "
	@echo "9.  copy-asir-stable-tar-ball-to-http-area "
	@echo "10.  copy-asir-misc-tar-ball-to-http-area "

clean :
	-(cd Generic/openxm ; make clean-work)
	-(cd FreeBSD ; make clean)
	-(cd Linux/RPM ; make clean)
	/bin/rm -rf work

print:
	$(PS) Makefile | $(LPR)	
	$(PS) README | $(LPR)	
	$(PS) Generic/openxm/Makefile | $(LPR)	
	$(PS) Linux/RPM/Makefile | $(LPR)	
	$(PS) Linux/RPM/openxm-*spec.in | $(LPR)	
	$(PS) FreeBSD/openxm-ports/openxm-1.1.2/Makefile | $(LPR)	
	$(PS) FreeBSD/Makefile | $(LPR)	

clean-Generic-openxm-work:
	-(cd Generic/openxm ; make clean-work ; make fetch)

generate-tar-balls:
	-mkdir ${TAR_WORK}
	make clean
	(cd Generic/openxm ; make TAG=${TAG} VERSION=${VERSION} tar-ball)
	(cd Generic/openxm ; make  TAG=${TAG} VERSION=${VERSION} tar-ball-01)
	make update-plist
	(cd FreeBSD   ; make TAG=${TAG} VERSION=${VERSION} make-tar-ball-for-openxm-port)

update-plist:
	(cd FreeBSD ; make VERSION=${VERSION}  TAG=${TAG} update-plist )

generate-tar-balls-without-update-plist:
	-mkdir ${TAR_WORK}
	make clean
	(cd Generic/openxm ; make TAG=${TAG} VERSION=${VERSION} tar-ball)
	(cd Generic/openxm ; make  TAG=${TAG} VERSION=${VERSION} tar-ball-01)
	(cd FreeBSD   ; make TAG=${TAG} VERSION=${VERSION} make-tar-ball-for-openxm-port)

prepare-build-rpm:
	(cd Linux/RPM ; make make-directory)
	(cd Linux/RPM ; make link-for-real-openxm)

# set path=(/usr/local/jdk1.1.8/bin $path) ; rehash
# build-on-new-orange should depend on clean-Generic-openxm-work in a future.
build-on-new-orange: prepare-build-rpm
	(cd Generic/openxm ; make  TAG=${TAG} VERSION=${VERSION} binary-tar-ball)
	(cd Generic/openxm ; make  TAG=${TAG} VERSION=${VERSION} shared-tar-ball)
	(cd Generic/openxm ; make  TAG=${TAG} VERSION=${VERSION} clean-work)
	(cd Linux/RPM ; make  TAG=${TAG} VERSION=${VERSION} build-shared-rpm)
	-(cp Linux/RPM/work/RPMS/i386/*.rpm ${TAR_WORK})
	-(cp Linux/RPM/work/RPMS/noarch/*.rpm ${TAR_WORK})

# You should be a super user.
build-on-pear-root:
	cp ${TAR_WORK_EXPLICIT}/openxm-${VERSION}.tar.gz /usr/ports/distfiles
	(cd /usr/ports/math ; /bin/rm -rf openxm ; tar xzvf ${TAR_WORK_EXPLICIT}/openxm-port.tar.gz)
	(PATH=/usr/local/${JDK}/bin:$(PATH) ; export PATH ; cd /usr/ports/math/openxm ; make install ; make package ; make deinstall ; cp openxm-${VERSION}.tgz ${TAR_WORK_EXPLICIT} ; chown ${NAME} ${TAR_WORK_EXPLICIT}/openxm-${VERSION}.tgz)

build-on-redhat: prepare-build-rpm clean-Generic-openxm-work
	(cd Generic/openxm ; make TAG=${TAG} VERSION=${VERSION} TAR_WORK=${TAR_WORK} tar-ball)
	(cd Generic/openxm ; make  TAG=${TAG} VERSION=${VERSION} binary-tar-ball)
	(cd Linux/RPM ; make build-binary-rpm)
	(cp Linux/RPM/work/RPMS/i386/*.rpm ${TAR_WORK})

# set path=(/usr/ccs/bin /usr/local/bin $path); rehash
# setenv OpenXM_HOME  xxx-yyy  
# (Note for taka). On unkei, cd Solaris/OpenXM/misc/packages, 
#                  setenv OpenXM_HOME $HOME/Solaris/OpenXM 
#                  and make OpenXM_HOME=$OpenXM_HOME build-on-unkei
build-on-unkei: clean-Generic-openxm-work
	(cd Generic/openxm ; make  TAG=${TAG} VERSION=${VERSION} OpenXM_HOME=${OpenXM_HOME} binary-tar-ball)

copy-to-http-area:
	-mkdir ${FTP_AREA}
	-cp ${TAR_WORK}/openxm*rpm ${FTP_AREA}
	-cp ${TAR_WORK}/openxm-${VERSION}.tar.gz ${FTP_AREA}
	-cp ${TAR_WORK}/OpenXM-*.tgz ${FTP_AREA}
	-cp ${TAR_WORK}/openxm-${VERSION}.tgz ${FTP_AREA}
	-cp ${TAR_WORK}/openxm-port.tar.gz ${FTP_AREA}
	ls -l ${FTP_AREA}

copy-release-tar-ball-to-http-area:
	-mkdir ${TAR_WORK}
	-mkdir work
	(cd work ; /bin/rm -rf *)
	(cd work ; ${RCVS} checkout OpenXM OpenXM_contrib2)
	(cd work ; tar czvf ${TAR_WORK}/openxm-${VERSION}.tar.gz OpenXM OpenXM_contrib2)
	/bin/rm -rf work
	-scp ${TAR_WORK}/openxm-${VERSION}.tar.gz ${WHO}:${FTP_AREA}

copy-head-tar-ball-to-http-area:
	-mkdir ${TAR_WORK}
	-mkdir work
	(cd work ; /bin/rm -rf *)
	(cd work ; ${RCVS} checkout OpenXM  OpenXM_contrib2)
	(cd work ; tar czvf ${TAR_WORK}/openxm-head.tar.gz OpenXM OpenXM_contrib2)
	/bin/rm -rf work
	cp ${TAR_WORK}/openxm-head.tar.gz ${FTP_AREA_HEAD}

copy-asir-head-tar-ball-to-http-area:
	-mkdir ${TAR_WORK}
	-mkdir work
	(cd work ; /bin/rm -rf *)
	(cd work ; ${RCVS} export -rHEAD OpenXM_contrib/pari OpenXM_contrib2/asir2000)
	(cd work/OpenXM_contrib ; tar czvf ${TAR_WORK}/pari.tar.gz pari)
	(cd work/OpenXM_contrib2 ; tar czvf ${TAR_WORK}/asir-head.tar.gz asir2000)
	/bin/rm -rf work
	-scp ${TAR_WORK}/pari.tar.gz ${WHO}:${FTP_AREA_ASIR}
	-scp ${TAR_WORK}/asir-head.tar.gz ${WHO}:${FTP_AREA_ASIR}

copy-asir-stable-tar-ball-to-http-area:
	-mkdir ${TAR_WORK}
	-mkdir work
	(cd work ; /bin/rm -rf *)
	(cd work ; ${RCVS} export -rSTABLE_1_1_3 OpenXM_contrib2/asir2000)
	(cd work/OpenXM_contrib2 ; tar czvf ${TAR_WORK}/asir-stable.tar.gz asir2000)
	/bin/rm -rf work
	-scp ${TAR_WORK}/asir-stable.tar.gz ${WHO}:${FTP_AREA_ASIR}

copy-asir-misc-tar-ball-to-http-area:
	-mkdir ${TAR_WORK}
	-mkdir work
	(cd work ; /bin/rm -rf *)
	(cd work ; ${RCVS} export -rHEAD OpenXM_contrib2/windows)
	(cd work/OpenXM_contrib2 ; tar czvf ${TAR_WORK}/asir-head-windows.tar.gz windows)
	(cd work ; ${RCVS} export -rHEAD OpenXM/src/asir-doc)
	(cd work/OpenXM/src ; tar czvf ${TAR_WORK}/asir-doc.tar.gz asir-doc)
	/bin/rm -rf work
	-scp ${TAR_WORK}/asir-head-windows.tar.gz ${WHO}:${FTP_AREA_ASIR}
	-scp ${TAR_WORK}/asir-doc.tar.gz ${WHO}:${FTP_AREA_ASIR}
