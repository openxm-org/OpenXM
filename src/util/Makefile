# $OpenXM: OpenXM/src/util/Makefile,v 1.2 2002/04/05 01:28:47 takayama Exp $
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
	cp -f lndir.sh ${OpenXM_bin}
	chmod 755 ${OpenXM_bin}/lndir.sh
