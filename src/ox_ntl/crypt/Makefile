# $OpenXM: OpenXM/src/ox_ntl/crypt/Makefile,v 1.3 2004/07/04 02:31:51 iwane Exp $


SUBDIRS = radix sha1 rsa des
OpenXM_HOME ?= ${PWD}/../../..

echo:
	@echo "'make compile': compile"
	@echo "'make clean'  : delete object file"

compile:
	@for _sub in ${SUBDIRS}; do (echo "(cd $${_sub}; make)"; cd $${_sub} && ${MAKE} OpenXM_HOME=${OpenXM_HOME}) || exit 1; done

clean:
	@for _sub in ${SUBDIRS}; do (echo "(cd $${_sub}; make $@)"; cd $${_sub} && ${MAKE} OpenXM_HOME=${OpenXM_HOME} $@) || exit 1; done
	
