# $OpenXM: OpenXM/src/gmp/Makefile,v 1.2 2003/09/14 01:19:57 takayama Exp $
OpenXM_HOME=../..
OpenXM_contrib=${OpenXM_HOME}/../OpenXM_contrib
OpenXM_lib=${OpenXM_HOME}/lib
OpenXM_include=${OpenXM_HOME}/include
GMP=${OpenXM_contrib}/gmp
RM=rm

all: configure
	-@if [ ! -f ./.make_done ]; then \
		(cd ${GMP} ; make) ; \
		touch ./.make_done ; \
	fi
install: install-exec
install-exec: all
	if [ ! -f ./.install_done ]; then \
		(cd ${GMP}; make install-exec) ; \
		cp ${GMP}/gmp.h ${OpenXM_include} ; \
		touch ./.install_done ; \
	fi
configure:
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
