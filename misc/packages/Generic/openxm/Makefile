# $OpenXM$
# This is a Makefile to generate source and binary distributions of
# the OpenXM package on various plathome.

## For committers: prepare mycvs under your search path 
## to access to CVS servers.
RCVS=mycvs -d kerberos.math.kobe-u.ac.jp:/usr/cvs
TYPE=`uname -s`-`uname -m`

RELEASE=
## If you like to have a release name as a part of tar ball, then uncomment
## the next line.
##RELEASE=`uname -a`

usage :
	@echo "Targets are "
	@echo "  fetch  :   get the sources from the cvs server under work"
	@echo "  make-tar-ball : generate OpenXM-source.tgz under work"
	@echo "  make-tar-ball-01 : generate OpenXM-source-01.tgz under work"
	@echo "  make-binary-tar-ball : generate OpenXM-binary-OS-CPU.tgz under work"
	@echo "  make-binary-tar-ball-using-binary-FreeBSD : generate OpenXM-binary-OS-CPU.tgz under work"
	@echo "   by using binary for FreeBSD because document generation might fails on other operating systems."
	@echo -n "Type is "
	@echo $(TYPE)

fetch :
	@if [ ! -f ./.fetch_done ]; then \
		/bin/rm -rf work ; mkdir work ; \
		( cd work ; $(RCVS) export -rHEAD OpenXM OpenXM_contrib OpenXM_contrib2) \
	fi
	@touch ./.fetch_done

make-tar-ball: fetch
	(cd work/OpenXM/src ; make generate-source-distribution)
	(cd work ; tar czvf OpenXM-source.tgz OpenXM OpenXM_Contrib OpenXM_contrib2 )

make-tar-ball-01: fetch
	@if [ ! -f ./.make-tar-ball-01_done ]; then \
		(cd work/OpenXM/src ; make generate-source-distribution) ; \
		(cd work ; tar czvf OpenXM-source-01.tgz OpenXM OpenXM_Contrib ) ; \
	fi
	@touch ./.make-tar-ball-01_done

make-binary-tar-ball : make-tar-ball-01
	(cd work/OpenXM/src ; make binary-dist )
	(cd work ; tar czf work/OpenXM-binary-$(TYPE)$(RELEASE).tgz OpenXM/Copyright OpenXM/bin OpenXM/doc OpenXM/include OpenXM/lib OpenXM/man OpenXM/rc OpenXM/install.txt)
	(cd work/OpenXM/src ; make distclean )

make-binary-tar-ball-using-binary-FreeBSD :
	@if [ ! -f ./work/OpenXM-binary-FreeBSD-i386.tgz ]; then \
		echo "Generate work/OpenXM-binary-FreeBSD-i386.tgz first" ; \
		exit \
	fi
	/bin/rm -rf work/binary-$(TYPE)
	mkdir work/binary-$(TYPE)
	(cd work/binary-$(TYPE) ; tar xzf ../OpenXM-binary-FreeBSD-i386.tgz )
	(cd work/binary-$(TYPE) ; /bin/rm -rf OpenXM/bin )
	(cd work/OpenXM/src ; make binary-dist )
	(cd work ; tar czf - OpenXM/bin OpenXM/lib OpenXM/rc ) | (cd work/binary-$(TYPE) ; tar xzf - )
	(cd work ; tar czf work/OpenXM-binary-$(TYPE)$(RELEASE).tgz OpenXM/Copyright OpenXM/bin OpenXM/doc OpenXM/include OpenXM/lib OpenXM/man OpenXM/rc OpenXM/install.txt)
	(cd work/OpenXM/src ; make distclean )

clean :
	/bin/rm -rf .*_done
	/bin/rm -rf work
