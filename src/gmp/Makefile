# $OpenXM: OpenXM/src/gmp/Makefile,v 1.5 2004/02/23 05:56:32 takayama Exp $
OpenXM_HOME=../..
OpenXM_contrib=${OpenXM_HOME}/../OpenXM_contrib
OpenXM_lib=${OpenXM_HOME}/lib
OpenXM_include=${OpenXM_HOME}/include
GMP=${OpenXM_contrib}/gmp
RM=rm
all:
	@if [ ! -f ./.use_ports ]; then \
		make all-l ; \
	else \
		make -f Makefile-ports all ; \
	fi
install:
	@if [ ! -f ./.use_ports ]; then \
		make install-l ; \
	else \
		make -f Makefile-ports install ; \
	fi
all-l: configure-l
	-@if [ ! -f ./.make_done ]; then \
		(cd ${GMP} ; make) ; \
		touch ./.make_done ; \
	fi
install-l: install-exec-l
install-exec-l: all-l
	if [ ! -f ./.install_done ]; then \
		(cd ${GMP}; make install-exec) ; \
		rm -f ${OpenXM_include}/config.h ${OpenXM_include}/gmp-impl.h ${OpenXM_include}/gmp-mparam.h ; \
		cp ${GMP}/gmp.h ${OpenXM_include} ; \
		touch ./.install_done ; \
	fi
configure:
	@if [ ! -f ./.use_ports ]; then \
		make configure-l ; \
	fi
configure-l:
	if [ ! -f ./.configure_done ]; then \
		(cd ${GMP} ; ./configure --prefix=`pwd`/../../OpenXM ) ; \
		touch ./.configure_done ; \
	fi
clean: 
	@if [ -f ./.make_done ]; then \
		(cd ${GMP} ; make clean) \
	fi
	${RM} -f ./.*done
distclean: 
	-@if [ -f ./.make_done ]; then \
		(cd ${GMP} ; make distclean) \
	fi
	${RM} -f ./.*done
