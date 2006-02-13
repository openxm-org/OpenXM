# Do source dot.bashrc before doing something.

VERSION=1.2.3-6
# This VERSION must be compatible with debian/control ver. 
#                                OpenXM/download/Makefile
#                                work/OpenXM/src/asir-port/Makefile

MD5=md5sum
OS=`uname -s`
RSYNC=rsync -avz -e ssh
FTP_DIR=/home/ftp/pub/OpenXM/head/knoppix
TM_OX=work/OpenXM/src/kxx/init-openxm.scm

build-package: fetch  make-tar-ball-2  make-tar-ball-3
	fakeroot debian/rules build
	fakeroot debian/rules install
	fakeroot dpkg-buildpackage

build-deb:
	fakeroot debian/rules build
	fakeroot debian/rules install
	fakeroot dpkg-buildpackage

fetch:
	if [ ! -f .fetch_done ]; then \
	  rm -rf work ; \
	  mkdir  work ; \
	  (cd work ; cvs -d kerberos.math.kobe-u.ac.jp:/home/cvsroot/openxm checkout OpenXM OpenXM_contrib2) ; \
	fi
	touch .fetch_done

build: fetch
	if [ ! -f .build_done ]; then \
	  touch work/OpenXM/src/asir-port/for_knoppix ;\
	  (cd work/OpenXM/src ;  make ; make install) ;\
	  (cd work/OpenXM/src ; make install-document ) ; \
	fi
	touch .build_done

## Knoppix step 1.  Install asir-port.sh under OpenXM/bin
## Knoppix step2 work 2.  Remove FLL files and replace them to a symbolic link
## to /home/knoppix/.asir-tmp/...
build-knoppix: build
	if [ ! -f .build-knoppix_done ]; then \
	  (cd work/OpenXM/src/asir-port ; make install ) ; \
	  (cd work/OpenXM/src/asir-port ; make install-for-knoppix ) ; \
	fi
	touch .build-knoppix_done

## Generate asir-Linux-${VERSION}.gz, ... under work.
## Copy them to the ftp area.
make-tar-ball-1: build-knoppix
	if [ ! -f .make-tar-ball-1_done ]; then \
	  (cd work/OpenXM/src/asir-port ; make gentar-for-knoppix ) ; \
	  (cd work/OpenXM/src/asir-port ; make copy-for-knoppix ) ; \
	fi
	touch .make-tar-ball-1_done

# Do strip.
# Generate openxm-binary-${VERSION}.tar.gz under work
# Compute MD5 and copy it to OpenXM/download
BINARY_FILES=d0 fep k0 ox ox100start ox_k0 ox_sm1 ox_texmacs oxc \
    oxgentexi oxlog oxreplace oxweave phc6 polymake2tfb sm1 tigers
EXCLUDE_FILES=--exclude CVS --exclude .keep_me \
              --exclude-from ../exclude-files.txt
FILES=OpenXM/Copyright OpenXM/bin OpenXM/include \
      OpenXM/lib OpenXM/man OpenXM/rc
      
make-tar-ball-2: make-tar-ball-1
	if [ ! -f .make-tar-ball-2_done ]; then \
	  (cd work/OpenXM/bin ; for i in ${BINARY_FILES} ; do strip $$i ; done ) ;\
	  (cd work/OpenXM/rc ; make clean ) ;\
	  (cd work; tar czvf openxm-binary-${OS}-${VERSION}.tar.gz ${EXCLUDE_FILES} ${FILES}) ; \
	  (cd work ; ${MD5} "openxm-binary-${OS}-${VERSION}.tar.gz" | awk '{print $$1}' >distinfo-openxm-binary.md5) ;\
	  (cp work/distinfo-openxm-binary.md5 OpenXM/download) ; \
	  (cp work/OpenXM/lib/version.txt OpenXM/download) ; \
	fi
	touch .make-tar-ball-2_done

##cf.misc-2004/A2/knoppix-2005/Makefile. There is no fix on .asirrc this time.
make-tar-ball-3:
	if [ ! -f .make-tar-ball-3_done ]; then \
	  rm -rf .TeXmacs ;\
	  mkdir -p .TeXmacs/plugins/ox/progs ;\
	  mkdir -p .TeXmacs/progs ;\
	  cp ${TM_OX} .TeXmacs/plugins/ox/progs/init-ox.scm ;\
	  touch .TeXmacs/progs/my-init-texmacs.scm ;\
	  tar czvf work/ox-texmacs-${OS}-${VERSION}.tar.gz ./.TeXmacs  ;\
	fi
	touch .make-tar-ball-3_done
 
copy-tar-ball-and-asir: make-tar-ball-2 make-tar-ball-3
	(cd work/OpenXM/src/asir-port ; make copy-for-knoppix)
	(cd work ;  ${RSYNC} "openxm-binary-${OS}-${VERSION}.tar.gz" ${USER}@lemon.math.kobe-u.ac.jp:${FTP_DIR})
	(cd work ;  ${RSYNC} "ox-texmacs-${OS}-${VERSION}.tar.gz" ${USER}@lemon.math.kobe-u.ac.jp:${FTP_DIR})
	(cp ../openxm*deb . ;  ${RSYNC} "openxm_${VERSION}_i386.deb" ${USER}@lemon.math.kobe-u.ac.jp:${FTP_DIR})

clean: clean-tilde
	rm -rf debian/tmp build-stamp work .*done openxm*.deb
clean-tilde:
	rm -rf *~