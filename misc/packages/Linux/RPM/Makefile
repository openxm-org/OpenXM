# $OpenXM: OpenXM/misc/packages/Linux/RPM/Makefile,v 1.10 2000/03/16 09:36:13 takayama Exp $
# *If you do not like to use /usr/src/redhat area to put *rpm files, 
#  do as follows.
# 0. Generate ~/.rpmrc and set the TopDir for the work.
#    Here is my .rpmrc
#
#    TopDir: home/taka/OpenXM/misc/packages/Linux/RPM/work
#
#  [ To extract a source from *src.rpm to SOURCES dir, 
#    type in "make make-directory" and "rpm -i your.src.rpm" ]
#
# *How to build a binary package.
# 1. Make ole by make ole at  $(OpenXM_HOME)/src/kan96xx/Doc.
#    [ ole.c will be moved to somewhere else in a future. ]
# 2. Make a directory ~/tmp if you do not have.
# 3. Generate a source tar ball openxm-1.1.2.tar.gz by
#     cd $(OpenXM_HOME)/misc/packages/FreeBSD/openxm-1.1.2 ; make make-tar-ball
# 4. Type in make link-for-real-openxm
# 5. Type in make build-binary-rpm  (for test)
# REAL 5. Edit openxm.spec.in and remove the line BuildRoot.
#          Su and type in make build-binary-rpm
#         work should be removed by the super user.
# The binary package will be generated at  work/RPMS/i386.
#
# Example 1 :  debug this Makefile and openxm.spec.in
#  cd $(OpenXM_HOME)/misc/packages/FreeBSD/openxm-1.1.2 ; make fake-openxm
#  make link-for-debug
#  make build-binary-rpm
#
# *To check the file names in the package
#  rpm2cpio openxm-1.1.2-test.i386.rpm | cpio -tv | more
# or
#  rpm -qvlp openxm-1.1.2-test.i386.rpm
#
#  -ivh : Install,  -e openxm-1.1.2-test : remove, 
#  -qvl openxm-1.1.2-test : list.
#
#  The maximum rpm at http://rpm.redhat.com is the most comprehensive source
#  to use rpm to build redhat packages.

OLE_DIR=../../../../../OpenXM/src/kan96xx/Doc
OLE=${OLE_DIR}/ole -q
RPM_BUILD_BINARY_ROOT=/var/tmp/openxm-binary-root
RPM_BUILD_SHARED_ROOT=/var/tmp/openxm-shared-root

usage :
	@echo "Usage:"
	@echo "build-binary-rpm "
	@echo "build-shared-rpm "
	@echo "make-directory"
	@echo "link-for-real-openxm "

build-binary-rpm: openxm-binary-spec
	(cd work/SPECS ; rpm -bb openxm-binary.spec )

build-shared-rpm: openxm-shared-spec
	(cd work/SPECS ; rpm --buildarch noarch --buildos Linux -bb openxm-shared.spec )

## This is obsolete
build-rpm: openxm-spec
	(cd work/SPECS ; rpm -bb openxm.spec )

make-directory: clean
	mkdir work
	mkdir work/SOURCES work/SPECS work/BUILD work/RPMS 
	mkdir work/RPMS/i386 work/RPMS/noarch work/SPRMS work/RPMS/ppc work/RPMS/sparc
	mkdir ${RPM_BUILD_BINARY_ROOT}
	mkdir ${RPM_BUILD_BINARY_ROOT}/usr
	mkdir ${RPM_BUILD_BINARY_ROOT}/usr/local
	mkdir ${RPM_BUILD_BINARY_ROOT}/usr/local/bin
	mkdir ${RPM_BUILD_BINARY_ROOT}/usr/local/man
	mkdir ${RPM_BUILD_BINARY_ROOT}/usr/local/man/man1
	mkdir ${RPM_BUILD_SHARED_ROOT}
	mkdir ${RPM_BUILD_SHARED_ROOT}/usr
	mkdir ${RPM_BUILD_SHARED_ROOT}/usr/local
	mkdir ${RPM_BUILD_SHARED_ROOT}/usr/local/bin
	mkdir ${RPM_BUILD_SHARED_ROOT}/usr/local/man
	mkdir ${RPM_BUILD_SHARED_ROOT}/usr/local/man/man1


openxm-spec:  make-ole
	rm -rf work/SPECS/openxm.spec
	${OLE} <openxm.spec.in >work/SPECS/openxm.spec

openxm-binary-spec: make-ole
	rm -rf work/SPECS/openxm-binary.spec
	${OLE} <openxm-binary.spec.in >work/SPECS/openxm-binary.spec

openxm-shared-spec: make-ole
	rm -rf work/SPECS/openxm-shared.spec
	${OLE} <openxm-shared.spec.in >work/SPECS/openxm-shared.spec

make-ole :
	@if test ! -f ${OLE_DIR}/ole ; \
	then \
		(cd $(OLE_DIR) ; make ole) ;  \
	fi

clean :
	-/bin/rm -rf work ${RPM_BUILD_BINARY_ROOT} ${RPM_BUILD_SHARED_ROOT}


link-for-debug: make-directory clean
	(cd work/SOURCES ; rm -rf openxm-1.1.2.tar.gz ; ln -s $(HOME)/tmp/openxm-fake.tar.gz ./openxm-1.1.2.tar.gz)

link-for-real-openxm: make-directory clean
	(cd work/SOURCES ; rm -rf openxm-1.1.2.tar.gz ; ln -s $(HOME)/tmp/openxm-1.1.2.tar.gz ./openxm-1.1.2.tar.gz)
