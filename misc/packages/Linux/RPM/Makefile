# $OpenXM: OpenXM/misc/packages/Linux/RPM/Makefile,v 1.1 2000/03/09 03:09:21 takayama Exp $
# *Preparation for debugging this Makefile and openxm.spec.in
# 0. Generate ~/.rpmrc and set the TopDir for the work.
#    Here is my .rpmrc
#
#    TopDir: home/taka/OpenXM/misc/packages/Linux/RPM/work
#
#  [ To extract a source from *src.rpm to SOURCES dir, 
#    type in "make make-directory" and "rpm -i your.src.rpm" ]
# *Preparations to use this redhat-package builder for OpenXM
#  and make a binary distribution.
# 0. You must be a root to execute 5 and later.
#    rm TopDir: in your ~/.rpmrc
#
# *How to build a binary package.
# 1. Make ole by make ole at  $(OpenXM_HOME)/src/kan96xx/Doc.
#    [ ole.c will be moved to somewhere else in a future. ]
# 2. Make a directory ~/tmp if you do not have.
# 3. Generate a source tar ball openxm-1.1.2.tar.gz by
#     cd $(OpenXM_HOME)/misc/packages/FreeBSD/openxm-1.1.2 ; make make-tar-ball
# 4. Type in make link-for-real-openxm
# 5. Type in make build-binary-rpm
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

OLE=$(HOME)/OpenXM/src/kan96xx/Doc/ole -q
RPM_BUILD_BINARY_ROOT=/var/tmp/openxm-root

usage :

build-binary-rpm: openxm-spec
	(cd work/SPECS ; rpm -bb openxm.spec )

make-directory: clean
	mkdir work
	mkdir work/SOURCES work/SPECS work/BUILD work/RPMS 
	mkdir work/RPMS/i386 work/SPRMS
	mkdir ${RPM_BUILD_BINARY_ROOT}
	mkdir ${RPM_BUILD_BINARY_ROOT}/usr
	mkdir ${RPM_BUILD_BINARY_ROOT}/usr/local
	mkdir ${RPM_BUILD_BINARY_ROOT}/usr/local/bin
	mkdir ${RPM_BUILD_BINARY_ROOT}/usr/local/man
	mkdir ${RPM_BUILD_BINARY_ROOT}/usr/local/man/man1


openxm-spec:
	rm -rf work/SPECS/openxm.spec
	${OLE} <openxm.spec.in >work/SPECS/openxm.spec

clean :
	-/bin/rm -rf work ${RPM_BUILD_BINARY_ROOT}


link-for-debug: make-directory clean
	(cd work/SOURCES ; rm -rf openxm-1.1.2.tar.gz ; ln -s $(HOME)/tmp/openxm-fake.tar.gz ./openxm-1.1.2.tar.gz)

link-for-real-openxm: make-directory clean
	(cd work/SOURCES ; rm -rf openxm-1.1.2.tar.gz ; ln -s $(HOME)/tmp/openxm-1.1.2.tar.gz ./openxm-1.1.2.tar.gz)
