# $OpenXM: OpenXM/src/ox_ntl/crypt/Makefile,v 1.2 2004/05/17 14:53:39 iwane Exp $


SUBDIRS = radix sha1 rsa
OpenXM_HOME ?= ${PWD}/../../..

echo:
	@echo "'make compile': compile"
	@echo "'make clean'  : delete object file"

compile:
	@for _sub in ${SUBDIRS}; do (echo "(cd $${_sub}; make)"; cd $${_sub} && ${MAKE} OpenXM_HOME=${OpenXM_HOME}) || exit 1; done

clean:
	@for _sub in ${SUBDIRS}; do (echo "(cd $${_sub}; make $@)"; cd $${_sub} && ${MAKE} OpenXM_HOME=${OpenXM_HOME} $@) || exit 1; done
	
