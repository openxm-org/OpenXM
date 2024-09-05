# $OpenXM: OpenXM/src/asir-port/Makefile,v 1.48 2022/03/04 00:08:41 takayama Exp $
# touch for_knoppix ; make install
# to generate a distribution for knoppix.
#### Do not add extra spaces after the VERSION.
VERSION=1.3.3-9

KNOPPIX_HOME=/home/knoppix
LIBS=alpi bfct bgk const cyclic defs.h dmul fctrdata fctrtest fff \
     gr ifplot katsura mat nf num primdec ratint robot solve sp \
	 sturm xm
# bug: md5sum is called md5 in FreeBSD.
MD5=md5sum
OS=`dpkg --print-architecture`
OpenXM_HOME_=../..
RSYNC=rsync -avz -e ssh
#FTP_DIR=/home/ftp/pub/OpenXM/head/knoppix
FTP_DIR=/home/web/pub/OpenXM/head/knoppix
OpenXM_HOME=../../../OpenXM

all:
install: all
	@echo "Installing scripts for portaging asir by downloading."
	-install -m 755 asir-port.sh ../../bin
	-install -m 755 asir-install.sh ../../bin
	if [ -f ./for_knoppix ]; then make install-for-knoppix ; make gentar-for-knoppix ; echo "do make copy-for-knoppix by hand" ; fi

# Remove files under FLL.
install-for-knoppix:
	rm -f ../../bin/asir 
	ln -s ${KNOPPIX_HOME}/.asir-tmp/asir ../../bin/asir
	for i in ${LIBS} ; do \
		(cd ${OpenXM_HOME_}/lib/asir ; rm -f $$i ; ln -s ${KNOPPIX_HOME}/.asir-tmp/$$i .) ; \
	done
	echo -n ${VERSION} >${OpenXM_HOME}/lib/version.txt

#The environmental variable OpenXM_HOME must be set for uninstall-for-knoppix
# Recover files under FLL.
uninstall-for-knoppix:
	rm -f ../../bin/asir
	(cd ../../bin ; ln -s ../../OpenXM_contrib2/asir2018/asir .)
	for i in ${LIBS} ; do \
		(cd ${OpenXM_HOME}/lib/asir ; rm -f $$i ; cp ../../../OpenXM_contrib2/asir2018/lib/$$i .) ; \
	done

# Generating asir-Linux-...
#      and   asirlib-Linux-....tar.gz  under work
# Copy md5 checksum files distinfo-*.md5 under OpenXM/lib/asir
gentar-for-knoppix:
	-mkdir work
	(cd work ; rm -rf *)
	(cd work ; cp ../${OpenXM_HOME_}/../OpenXM_contrib2/asir2018/.libs/asir . )
	(cd work ; strip asir ; gzip asir )
	(cd work ; for i in ${LIBS} ; do \
		cp ../${OpenXM_HOME_}/../OpenXM_contrib2/asir2018/lib/$$i . ; \
	done )
	(cd work ; mv asir.gz "asir-${OS}-${VERSION}.gz")
	(cd work ; ${MD5} "asir-${OS}-${VERSION}.gz" | awk '{print $$1}' >distinfo-asir.md5)
	(cd work ; tar czf asirlib.tar.gz ${LIBS})
	(cd work ; for i in ${LIBS} ; do rm -f $$i ; done )
	(cd work ; mv asirlib.tar.gz "asirlib-${OS}-${VERSION}.tar.gz")
	(cd work ; ${MD5} "asirlib-${OS}-${VERSION}.tar.gz" | awk '{print $$1}' >distinfo-asirlib.md5)
	(cd work ; cp distinfo-asir.md5 distinfo-asirlib.md5 ../${OpenXM_HOME}/lib/asir)

copy-for-knoppix:
	if [ ! -d work ]; then make gentar-for-knoppix ; fi
	(cd work ; ${RSYNC} "asir-${OS}-${VERSION}.gz" "asirlib-${OS}-${VERSION}.tar.gz" ${USER}@lemon.math.kobe-u.ac.jp:${FTP_DIR})

clean:
	rm -rf for_knoppix work

