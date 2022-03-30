VERSION=1.3.3-7
OS=`dpkg --print-architecture`
OpenXMBinaryTGZ=openxm-binary-${OS}-${VERSION}.tar.gz
AsirBinaryTGZ=asir-binary-${OS}-${VERSION}.tar.gz
DISTDIR=${HOME}/OpenXM_tmp
#MASTER_SITE=ftp://ftp.math.kobe-u.ac.jp/pub/OpenXM/head/knoppix
MASTER_SITE=http://www.math.kobe-u.ac.jp/pub/OpenXM/head/knoppix

install-small:
	mkdir -p ../bin
	install ${OpenXM_HOME}/src/util/oxfetch.sh ../bin

install-medium:
	mkdir -p ${DISTDIR}
	${OpenXM_HOME}/bin/oxfetch.sh ${MASTER_SITE}/${AsirBinaryTGZ} ${DISTDIR}
	(cd ${OpenXM_HOME}/.. ; tar xzvf ${DISTDIR}/${AsirBinaryTGZ})

install-big:
	mkdir -p ${DISTDIR}
	${OpenXM_HOME}/bin/oxfetch.sh ${MASTER_SITE}/${OpenXMBinaryTGZ} ${DISTDIR}
	(cd ${OpenXM_HOME}/.. ; tar xzvf ${DISTDIR}/${OpenXMBinaryTGZ})

clean-big:
#	rm -rf ${DISTDIR}
# It is removed in openxm.postinst

test:
	(OpenXM_HOME=`pwd`/../../OpenXM ./openxm-install.sh)
clean-test:
	(cd .. ; rm -rf Copyright bin doc include lib man rc )
	make install-small
