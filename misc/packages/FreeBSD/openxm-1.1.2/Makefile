# $OpenXM: OpenXM/misc/packages/FreeBSD/openxm-1.1.2/Makefile,v 1.9 2000/03/06 04:29:01 takayama Exp $

### NOTE for committers.
### How to generate openxm-1.1.2.tar.gz (the tar ball of source code)?
### How to generate openxm-port.tar.gz  (the tar ball of the port for openxm)?
###     make make-tar-ball
### Then, the two tar balls are generated under ${TAR_WORK}.
### The default is ${HOME}/tmp
###
### How to test the port for openxm  
###     cd /usr/ports/math ; tar xzvf ${TAR_WORK}/openxm-port.tar.gz 
###     cp ${TAR_WORK}/openxm-1.1.2.tar.gz /usr/ports/distfiles
###     cd /usr/ports/math/openxm ; make install
###     [Try also make package , pkg_delete openxm-1.1.2 , 
###      pkg_add openxm-1.1.2.tgz]

# Modify this. It's for committers.
OpenXM_TOP=$(HOME)
## For committers: prepare cvs-OpenXM under your search path 
## to access to CVS servers.
RCVS=cvs-OpenXM -d kerberos.math.sci.kobe-u.ac.jp:/usr/cvs
### cvs-OpenXM is the following shell script
## #!/bin/sh
## CVS_RSH=ssh
## export CVS_RSH
## cvs $*
#

TAR_WORK=$(HOME)/tmp

srcdir = .
prefix=${PREFIX}
MAN_DIR=${prefix}/man/man1

OpenXM_BINARIES=gnuplot4ox gnuplot_x11 k0 ox ../lib/asir/asir ox_sm1 oxlog oxweave sm1 tigers

usage:
	@echo "Targets are all, install, clean, distclean"
	@echo "You need to specify the PREFIX to install,"
	@echo "e.g., make install PREFIX=/usr/local"
	@echo "For committers: make-tar-ball, make-tar-ball-from-local-OpenXM"
	@echo "                generate-symbolic-links, clean-this-directory"
	@echo " make-tar-ball : create openxm-1.1.2.tar.gz under $(TAR_WORK)"
	@echo " fake-openxm : create a fake OpenXM for a test."

configure :
	(cd $(srcdir)/OpenXM/src ;	make configure )

all : configure
#	(cd $(srcdir)/OpenXM/src ;	make ;	make install)
	(cd $(srcdir)/OpenXM/src ;	make binary-dist)

install : all
	@echo "Installation phase with PREFIX= "
	@echo ${prefix}
	tar cf - OpenXM/bin OpenXM/doc OpenXM/lib OpenXM/rc OpenXM/Copyright OpenXM/include OpenXM/man  OpenXM/misc | (cd ${prefix} ; tar xf - )
	-for i in $(OpenXM_BINARIES); do strip ${prefix}/OpenXM/bin/$$i ; done
	(cd ${prefix}/OpenXM/rc ; make ; make install PREFIX=${prefix})

install.man : install
	-cp ${prefix}/OpenXM/man/man1/sm1.1 ${prefix}/OpenXM/man/man1/openxm.1 ${MAN_DIR}
	-gzip ${MAN_DIR}/sm1.1 ${MAN_DIR}/openxm.1 

clean :
	cd $(srcdir)/OpenXM/src ; make clean

distclean :
	cd $(srcdir)/OpenXM/src ; make distclean

clean-installed-openxm:
	/bin/rm -rf ${prefix}/OpenXM

#### The rests are for committers.
make-tar-ball :
	rm -rf OpenXM OpenXM_contrib OpenXM_contrib2
	$(RCVS) export -rHEAD OpenXM OpenXM_contrib OpenXM_contrib2
	rm -f *~
	echo "Creating a tar ball under $(TAR_WORK)"
	(cd .. ; tar czf ${TAR_WORK}/openxm-1.1.2.tar.gz openxm-1.1.2)
	rm -rf openxm
	mv OpenXM/misc/packages/FreeBSD/openxm-ports/openxm-1.1.2 openxm
	tar czf ${TAR_WORK}/openxm-port.tar.gz openxm

make-tar-ball-for-openxm-port:
	rm -rf openxm 
	rm -rf OpenXM/misc/packages/FreeBSD
	$(RCVS) export -rHEAD OpenXM/misc/packages/FreeBSD
	mv OpenXM/misc/packages/FreeBSD/openxm-ports/openxm-1.1.2 openxm
	tar czf ${TAR_WORK}/openxm-port.tar.gz openxm

make-tar-ball-from-local-OpenXM :
	rm -rf OpenXM OpenXM_contrib OpenXM_contrib2
	(cd $(OpenXM_TOP) ; tar cf - OpenXM OpenXM_contrib OpenXM_contrib2) | tar xf -
	(cd OpenXM/src  ; make distclean )
	rm -f *~
	echo "It is ready to tar this directory."

# These are used for the test.
generate-symbolic-links :
	rm -rf OpenXM OpenXM_contrib OpenXM_contrib2
	ln -s $(OpenXM_TOP)/OpenXM OpenXM
	ln -s $(OpenXM_TOP)/OpenXM OpenXM_Contrib
	ln -s $(OpenXM_TOP)/OpenXM OpenXM_Contrib2

# Make a fake distribution for a test of RPM etc.
fake-openxm: clean-this-directory
	mkdir OpenXM
	mkdir OpenXM/bin OpenXM/lib OpenXM/doc OpenXM/man OpenXM/include OpenXM/rc
	mkdir OpenXM/Copyright OpenXM/misc
	touch OpenXM/bin/hoge-bin OpenXM/lib/hoge-lib OpenXM/doc/hoge-doc
	(cd OpenXM/bin ; ln -s ../lib/hoge-lib hoge-lib-link)
	mkdir OpenXM/src
	echo "configure:" >OpenXM/src/Makefile
	echo "	echo configure" >>OpenXM/src/Makefile
	echo "binary-dist:" >>OpenXM/src/Makefile
	echo "	echo binary-dist" >>OpenXM/src/Makefile
	echo "install:" >OpenXM/rc/Makefile 
	echo "Creating a fake tar ball at ${TAR_WORK}"
	(cd .. ; tar czf ${TAR_WORK}/openxm-fake.tar.gz openxm-1.1.2)


clean-this-directory:
	/bin/rm -rf OpenXM OpenXM_contrib OpenXM_contrib2 *~ openxm
