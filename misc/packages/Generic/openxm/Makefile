# $OpenXM: OpenXM/misc/packages/Generic/openxm/Makefile,v 1.19 2000/09/21 08:06:40 takayama Exp $
# The FIRST part is a Makefile FOR COMMITTERS: it is used 
# to generate source and binary distributions of
# the OpenXM package on various operating systems.

# If you add new directory under OpenXM/lib, edit SHARED_FILES.

# If you add new files to be installed under /usr/local/bin, man, etc,
# you have to edit several files. See OpenXM/misc/packages/README.

## For committers: prepare cvs-OpenXM under your search path 
## to access to CVS servers.
RCVS=cvs-OpenXM -d kerberos.math.sci.kobe-u.ac.jp:/home/cvs
### cvs-OpenXM is the following shell script in my case.
## #!/bin/sh
## CVS_RSH=ssh
## export CVS_RSH
## cvs $*

## Configurations.
## OpenXM Version Number.
PREFIX=/usr/local
VERSION=1.1.3
TAG=HEAD
FTP_AREA=/export/home01/ftp/pub/OpenXM/${VERSION}

RELEASE=
## If you like to have a release name as a part of tar ball, then uncomment
## the next line.
##RELEASE=`uname -a`

TYPE=`uname -s`-`uname -m`

OpenXM_HOME=$(HOME)/OpenXM
TAR_WORK=$(HOME)/tmp
#TAR_WORK=${OpenXM_HOME}/misc/packages/Generic/openxm/work
INSTALL_WORK=${OpenXM_HOME}/misc/packages/Generic/openxm/work/openxm-install-dir

### Variables for the real Makefile
### Variables should be set before the first target.
srcdir = .
prefix=${PREFIX}
MAN_DIR=${prefix}/man/man1
WORK=work/openxm-${VERSION}
### End of variables for the real Makefile

### Common
SHARED_FILES = OpenXM/Copyright OpenXM/doc OpenXM/include OpenXM/man \
               OpenXM/lib/asir-contrib OpenXM/lib/java-support \
               OpenXM/lib/k097 OpenXM/lib/math OpenXM/lib/pari \
               OpenXM/lib/sm1 OpenXM/lib/asir 
BINARY_FILES = OpenXM/bin OpenXM/lib/*.a OpenXM/lib/*.so* OpenXM/lib/*.a.* \
               OpenXM/rc
MAN_PAGES    = openxm.1 sm1.1

EXECUTABLES=gnuplot4ox gnuplot_x11 k0 ox ox_sm1 oxlog oxweave sm1 tigers ox_math

usage : usage-of-real-make
	@echo "--------------------------------------------------"
	@echo "Usage for building distributions (for COMMITTERS):"
	@echo "Targets are "
	@echo "  fetch  :   get the sources from the cvs server under $(WORK)"
	@echo "  tar-ball : generate openxm-${VERSION}.tar.gz under $(TAR_WORK)"
	@echo "  tar-ball-01 : generate openxm01-${VERSION}.tar.gz under $(TAR_WORK)"
	@echo "  binary-tar-ball : generate OpenXM-binary-OS-CPU.tgz under $(TAR_WORK)"
	@echo "  shared-tar-ball : generate OpenXM-shared.tgz under $(TAR_WORK)"
	@echo  "  install-tar-to-ftp-area : copy openxm-${VERSION}.tar.gz to ${FTP_AREA}"
	@echo "  clean-work "
	@echo "  clean-for-make "
	@echo "--------------------------------------------------"
	@echo -n "Type is "
	@echo $(TYPE)
	@echo -n "INSTALL_WORK is "
	@echo  $(INSTALL_WORK)
	@echo -n "TAR_WORK is "
	@echo  $(TAR_WORK)
	@echo -n "FTP_AREA is "
	@echo  $(FTP_AREA)
	@echo " "

### The SECOND part is a Makefile to build binaries from source.
### It only calls the Makefile at OpenXM/src.
### Start of Makefile  to work in the directory work/openxm-$(VERSION)
### Original version is in misc/packages/FreeBSD/openxm-1.1.2
### Absolute path should be used for TAR_WORK

usage-of-real-make:
	@echo "Usage for building OpenXM binaries (main Makefile is at OpenXM/src):"
	@echo "Targets are "
	@echo "   configure, all, install, clean, distclean"
	@echo "You need to specify the PREFIX to install, e.g., make install PREFIX=/usr/local"

configure :
	@if [ ! -f .configure_done ] ; then \
		(cd $(srcdir)/OpenXM/src ;	make configure ) ; \
	fi
	@touch .configure_done

all : configure
	@if [ ! -f .all_done ] ; then \
		(cd $(srcdir)/OpenXM/src ;	make binary-dist) ; \
	fi
	@touch .all_done

all-ideal : configure
	@if [ ! -f .all_done ] ; then \
		(cd $(srcdir)/OpenXM/src ;	make ;	make install) ; \
	fi
	@touch .all_done

install : install-binary-files install-shared-files
	make install-shell-scripts PREFIX=${prefix}

install-shell-scripts:
# Install shell scripts to ${prefix}/bin, which is typically /usr/local/bin
	(cd ${prefix}/OpenXM/rc ; make ; make install PREFIX=${prefix})


install-binary-files:  all
	@echo "install-binary-files with PREFIX= "
	@echo ${prefix}
	tar cf - ${BINARY_FILES} | (cd ${prefix} ; tar xf -)
	(cd ${prefix}/OpenXM/bin ; make clean-for-install)
	-for i in $(EXECUTABLES); do strip ${prefix}/OpenXM/bin/$$i ; done

install-shared-files:  all
	@echo "install-binary-files with PREFIX= "
	@echo ${prefix}
	tar cf - ${SHARED_FILES} | (cd ${prefix} ; tar xf -)

install-man-pages-for-rpm:
	-(cd ${prefix}/OpenXM/man/man1 ; cp $(MAN_PAGES) ${PREFIX}/man/man1)

install.man : install
	-(cd ${prefix}/OpenXM/man/man1 ; cp ${MAN_PAGES} ${MAN_DIR})
	-gzip ${MAN_DIR}/sm1.1 ${MAN_DIR}/openxm.1 

clean :
	-(cd $(srcdir)/OpenXM/src ; make clean)
	rm -f .*_done*

distclean :
	-(cd $(srcdir)/OpenXM/src ; make distclean)
	rm -f .*_done*

clean-installed-openxm:
	/bin/rm -rf ${prefix}/OpenXM

########### end of the real Makefile



fetch :
	@if [ ! -f ./.fetch_done ]; then \
		/bin/rm -rf work ; mkdir work ; mkdir ${WORK} ; \
		( cd ${WORK} ; ${RCVS} export -r${TAG} OpenXM OpenXM_contrib OpenXM_contrib2) ; \
		cp Makefile ${WORK}/Makefile ; \
	fi
	@touch ./.fetch_done

build-tar-work:
	@if [ ! -d ${TAR_WORK} ]; then \
		mkdir ${TAR_WORK} ; \
	fi

build-ftp-area:
	@if [ ! -d ${FTP_AREA} ]; then \
		mkdir ${FTP_AREA} ; \
	fi

tar-ball: fetch build-tar-work
	@if [ ! -f ./.tar-ball_done ]; then \
		cp Makefile ${WORK}/Makefile ; \
		(cd ${WORK}/OpenXM/src ; make generate-source-distribution) ; \
		(cd work ; tar czvf ${TAR_WORK}/openxm-${VERSION}.tar.gz openxm-${VERSION}) ; \
	fi
	touch ./.tar-ball_done

tar-ball-01: fetch build-tar-work
	@if [ ! -f ./.tar-ball-01_done ]; then \
		cp Makefile ${WORK}/Makefile  ; \
		(cd ${WORK}/OpenXM/src ; make generate-source-distribution) ; \
		(cd work ; tar czvf ${TAR_WORK}/openxm01-${VERSION}.tar.gz openxm-${VERSION}/OpenXM openxm-${VERSION}/OpenXM_contrib ) ; \
	fi
	@touch ./.tar-ball-01_done

binary-tar-ball : fetch
	cp Makefile ${WORK}/Makefile 
	rm -rf ${INSTALL_WORK}
	mkdir ${INSTALL_WORK} ${INSTALL_WORK}/bin ${INSTALL_WORK}/man 
	mkdir ${INSTALL_WORK}/man/man1
	(cd ${WORK} ; make install-binary-files PREFIX=${INSTALL_WORK})
	(cd ${INSTALL_WORK} ; tar czf ${TAR_WORK}/OpenXM-binary-$(TYPE)$(RELEASE).tgz ${BINARY_FILES})

shared-tar-ball : fetch
	cp Makefile ${WORK}/Makefile 
	rm -rf ${INSTALL_WORK}
	mkdir ${INSTALL_WORK} ${INSTALL_WORK}/bin ${INSTALL_WORK}/man 
	mkdir ${INSTALL_WORK}/man/man1
	(cd ${WORK} ; make install-shared-files PREFIX=${INSTALL_WORK})
	(cd ${INSTALL_WORK} ; tar czf ${TAR_WORK}/OpenXM-shared.tgz ${SHARED_FILES})

install-tar-to-ftp-area: build-ftp-area tar-ball
	cp ${TAR_WORK}/openxm-${VERSION}.tar.gz ${FTP_AREA}

clean-work :
	/bin/rm -rf .*_done *~
	/bin/rm -rf work work-fake

clean-for-make:
	(cd ${WORK} ; make distclean)
	rm -f .configure_done .all_done
