# $OpenXM: OpenXM/src/util/Makefile,v 1.1 2002/04/05 01:12:09 takayama Exp $
OpenXM_ROOT=../../../OpenXM
OpenXM_bin=$(OpenXM_ROOT)/bin
OpenXM_lib=$(OpenXM_ROOT)/lib
CC=gcc

all : oxweave

oxweave : oxweave.c
	gcc    -D_BSD_SOURCE -o oxweave -g oxweave.c


install: all
	if [  "`uname | sed -e 's/_.*$$//'`" = "CYGWIN" ]; then \
	  cp -f oxweave.exe ${OpenXM_bin} ; \
	  cp -f oxweave.exe ${OpenXM_ROOT}/src/kxx ; \
	else \
	  cp -f oxweave ${OpenXM_bin} ; \
	  cp -f oxweave ${OpenXM_ROOT}/src/kxx ; \
	fi
