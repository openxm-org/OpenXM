# $OpenXM: OpenXM/src/asir-port/Makefile,v 1.2 2004/02/22 06:39:09 takayama Exp $
# touch for_knoppix ; make install
# to generate a distribution for knoppix. 
KNOPPIX_HOME=/home/knoppix
LIBS=alpi bfct bgk const cyclic defs.h dmul fctrdata fctrtest fff \
     gr ifplot katsura mat nf num primdec ratint robot solve sp \
	 sturm xm
# bug: md5sum is called md5 in FreeBSD.
MD5=md5sum
OS=`uname -s`
OpenXM_HOME_=../..
RSYNC=rsync -avz -e ssh

all:
install: all
	@echo "Installing scripts for portaging asir by downloading."
	-install -m 755 asir-port.sh ../../bin
	if [ -f ./for_knoppix ]; then make install-for-knoppix ; make copy-for-knoppix ; fi

# Remove files under FLL.
install-for-knoppix:
	rm -f ../../bin/asir
	ln -s ${KNOPPIX_HOME}/.asir-tmp/asir ../../bin/asir
	for i in ${LIBS} ; do \
		(cd ${OpenXM_HOME_}/lib/asir ; rm -f $$i ; ln -s ${KNOPPIX_HOME}/.asir-tmp/$$i .) ; \
	done

#The environmental variable OpenXM_HOME must be set for uninstall-for-knoppix
# Recover files under FLL.
uninstall-for-knoppix:
	rm -f ../../bin/asir
	ln -s ${OpenXM_HOME}/../OpenXM_contrib2/asir2000/asir ${OpenXM_HOME}/bin/asir
	for i in ${LIBS} ; do \
		(cd ${OpenXM_HOME}/lib/asir ; rm -f $$i ; cp ${OpenXM_HOME}/../OpenXM_contrib2/asir2000/lib/$$i .) ; \
	done

# Generating asir-Linux-85....
#      and   asirlib-Linux-c3....tar.gz  under work
# Copy md5 checksum files distinfo-? under OpenXM/lib/asir
gentar-for-knoppix:
	-mkdir work
	(cd work ; rm -rf *)
	(cd work ; cp ../${OpenXM_HOME_}/../OpenXM_contrib2/asir2000/asir . )
	(cd work ; strip asir ; gzip asir )
	(cd work ; for i in ${LIBS} ; do \
		cp ../${OpenXM_HOME_}/../OpenXM_contrib2/asir2000/lib/$$i . ; \
	done )
	(cd work ; ${MD5} asir.gz | awk '{print $$1}' >distinfo-asir)
	(cd work ; mv asir.gz "asir-${OS}-`cat distinfo-asir`.gz")
	(cd work ; tar czf asirlib.tar.gz ${LIBS})
	(cd work ; for i in ${LIBS} ; do rm -f $$i ; done )
	(cd work ; ${MD5} asirlib.tar.gz | awk '{print $$1}' >distinfo-lib)
	(cd work ; mv asirlib.tar.gz "asirlib-${OS}-`cat distinfo-lib`.tar.gz")
	(cd work ; cp distinfo-asir distinfo-lib ../${OpenXM_HOME_}/lib/asir)

copy-for-knoppix:
	if [ ! -d work ]; then make gentar-for-knoppix ; fi
	(cd work ; ${RSYNC} "asir-${OS}-`cat distinfo-asir`.gz" "asirlib-${OS}-`cat distinfo-lib`.tar.gz" ${USER}@lemon.math.kobe-u.ac.jp:/home/ftp/pub/asir/knoppix/)

clean:
	rm -rf for_knoppix work
