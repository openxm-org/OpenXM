# $OpenXM: OpenXM/misc/packages/FreeBSD/openxm-1.1.2/Makefile,v 1.2 2000/03/03 05:13:26 takayama Exp $

# Modify this.
OpenXM_TOP=$(HOME)
# Modify this.  It's for committters.
RCVS=mycvs -d kerberos.math.kobe-u.ac.jp:/usr/cvs

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
	@echo " make-tar-ball : create openxm-1.1.2.tar.gz under ~/tmp"

configure :
	(cd $(srcdir)/OpenXM/src ;	make configure )

all : configure
#	(cd $(srcdir)/OpenXM/src ;	make ;	make install)
	(cd $(srcdir)/OpenXM/src ;	make binary-dist)

install : all
	tar cf - OpenXM/bin OpenXM/doc OpenXM/lib OpenXM/rc OpenXM/Copyright OpenXM/include OpenXM/man  OpenXM/misc | (cd ${prefix} ; tar xf - )
	-for i in $(OpenXM_BINARIES); do strip ${prefix}/OpenXM/bin/$$i ; done
	(cd ${prefix}/OpenXM/rc ; make ; make install PREFIX=${prefix})

install.man : install
	-cp ${prefix}/OpenXM/man/man1/* ${MAN_DIR}

clean :
	cd $(srcdir)/OpenXM/src ; make clean

distclean :
	cd $(srcdir)/OpenXM/src ; make distclean


#### The rests are for committers.
make-tar-ball :
	rm -rf OpenXM OpenXM_contrib OpenXM_contrib2
	$(RCVS) export -rHEAD OpenXM OpenXM_contrib OpenXM_contrib2
	rm -f *~
	echo "Creating a tar ball under ~/tmp/"
	(cd .. ; tar czf $(HOME)/tmp/openxm-1.1.2.tar.gz openxm-1.1.2)
	rm -rf openxm
	mv OpenXM/misc/packages/FreeBSD/openxm-ports/openxm-1.1.2 openxm
	tar czf $(HOME)/tmp/openxm-port.tar.gz openxm

make-tar-ball-from-local-OpenXM :
	rm -rf OpenXM OpenXM_contrib OpenXM_contrib2
	(cd $(OpenXM_TOP) ; tar cf - OpenXM OpenXM_contrib OpenXM_contrib2) | tar xf -
	(cd OpenXM/src  ; make distclean )
	rm -f *~
	echo "It is ready to tar this directory."

# This is used for the test.
generate-symbolic-links :
	rm -rf OpenXM OpenXM_contrib OpenXM_contrib2
	ln -s $(OpenXM_TOP)/OpenXM OpenXM
	ln -s $(OpenXM_TOP)/OpenXM OpenXM_Contrib
	ln -s $(OpenXM_TOP)/OpenXM OpenXM_Contrib2



clean-this-directory:
	/bin/rm -rf OpenXM OpenXM_contrib OpenXM_contrib2 *~
