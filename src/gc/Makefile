# $OpenXM: OpenXM/src/gc/Makefile,v 1.1 2003/09/02 14:07:45 takayama Exp $
OpenXM_HOME=../..
OpenXM_contrib=${OpenXM_HOME}/../OpenXM_contrib
OpenXM_lib=${OpenXM_HOME}/lib
OpenXM_include=${OpenXM_HOME}/include
GC=${OpenXM_contrib}/gc
RM=rm

all:
	@if [ ! -f ./.make_done ]; then \
		(cd ${GC} ; make) ; \
		touch ./.make_done ; \
	fi

install: all
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
