# $OpenXM: OpenXM/src/gc/Makefile,v 1.2 2003/09/14 01:19:57 takayama Exp $
OpenXM_HOME=../..
OpenXM_contrib=${OpenXM_HOME}/../OpenXM_contrib
OpenXM_lib=${OpenXM_HOME}/lib
OpenXM_include=${OpenXM_HOME}/include
GC=${OpenXM_contrib}/gc
RM=rm

all:
	@if [ ! -f ./.use_ports ]; then \
		make all-l ; \
	else \
		make -f Makefile-ports all ;  \
	fi
install:
	@if [ ! -f ./.use_ports ]; then \
		make install-l ; \
	else \
		make -f Makefile-ports install ; \
	fi

all-l:
	@if [ ! -f ./.make_done ]; then \
		(cd ${GC} ; make) ; \
		touch ./.make_done ; \
	fi

install-l: all-l
	if [ ! -f ./.install_done ]; then \
		cp ${GC}/gc.a ${OpenXM_lib}/libgc.a ; \
		if test ! -d ${OpenXM_include}/gc ; then mkdir -p ${OpenXM_include}/gc ; fi ; \
		cp ${GC}/include/gc.h ${GC}/include/gc_cpp.h ${OpenXM_include}/gc ; \
		touch ./.install_done ; \
	fi
clean: 
	${RM} -f ./.*done
	@if [ -f ./.make_done ]; then \
		(cd ${GC} ; make clean) \
	fi
distclean: 
	${RM} -f ./.*done
	-@if [ -f ./.make_done ]; then \
		(cd ${GC} ; make distclean) \
	fi
