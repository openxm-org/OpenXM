# $OpenXM: OpenXM/src/gmp/Makefile,v 1.1 2003/09/02 14:24:43 takayama Exp $
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
	${RM} -f ./.*done
	@if [ -f ./.make_done ]; then \
		(cd ${GMP} ; make clean) \
	fi
distclean: 
	${RM} -f ./.*done
	-@if [ -f ./.make_done ]; then \
		(cd ${GMP} ; make distclean) \
	fi
