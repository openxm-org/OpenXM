# $OpenXM: OpenXM/misc/packages/FreeBSD/Makefile,v 1.12 2000/09/23 07:28:13 takayama Exp $
VERSION=1.1.3
TAG=HEAD
TAR_WORK=$(HOME)/tmp

RCVS=cvs-OpenXM -d kerberos.math.sci.kobe-u.ac.jp:/home/cvs

usage:
	@echo "Targets:  "
	@echo "  clean "
	@echo "  update-plist "

all :

clean:
	-/bin/rm -rf openxm-ports/openxm-1.1.3/work openxm-ports/openxm-1.1.3/openxm-1.1.3.tgz work
##  tmp2 might be used to debug openxm-1.1.3 for ports.
	-/bin/rm -rf $(HOME)/tmp2/OpenXM $(HOME)/tmp2/bin
	-mkdir $(HOME)/tmp2
	-mkdir $(HOME)/tmp2/bin
	-mkdir $(HOME)/tmp2/man
	-mkdir $(HOME)/tmp2/man/man1

## You need to put the source tar ball under $(HOME)/tmp
update-plist :
	-mkdir $(HOME)/tmp2 $(HOME)/tmp2/bin $(HOME)/tmp2/man $(HOME)/tmp2/man/man1
	(cd openxm-ports/openxm-${VERSION} ; make TAR_WORK=$(HOME)/tmp DISTDIR=$(HOME)/tmp PREFIX=$(HOME)/tmp2)
	(cd openxm-ports/openxm-${VERSION}/files ; make ; ./gen-plist.shell)
	(cd openxm-ports/openxm-${VERSION}/pkg ; ${RCVS} update)
	(cd openxm-ports/openxm-${VERSION}/files ; cp plist-tmp2 ../pkg/PLIST)
	(cd openxm-ports/openxm-${VERSION}/pkg ; ${RCVS} commit -m 'AutomaticUpdate' PLIST)


make-tar-ball-for-openxm-port:
	rm -rf work
	mkdir work
	(cd work ; $(RCVS) export -r${TAG} OpenXM/misc/packages/FreeBSD)
	(cd work ; mv OpenXM/misc/packages/FreeBSD/openxm-ports/openxm-1.1.3 openxm)
	(cd work ; tar czf ${TAR_WORK}/openxm-port.tar.gz openxm)

