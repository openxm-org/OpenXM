# $OpenXM: OpenXM/src/util/Makefile,v 1.3 2003/03/06 10:36:04 ohara Exp $
OpenXM_ROOT=../../../OpenXM
OpenXM_bin=$(OpenXM_ROOT)/bin
OpenXM_lib=$(OpenXM_ROOT)/lib
CC=gcc

all : oxweave oxreplace

oxweave : oxweave.c
	gcc    -D_BSD_SOURCE -o oxweave -g oxweave.c
oxreplace : oxreplace.c
	gcc    -D_BSD_SOURCE -o oxreplace -g oxreplace.c


install: all
	if [  "`uname | sed -e 's/_.*$$//'`" = "CYGWIN" ]; then \
	  cp -f oxweave.exe ${OpenXM_bin} ; \
	  cp -f oxweave.exe ${OpenXM_ROOT}/src/kxx ; \
	  cp -f oxreplace.exe ${OpenXM_bin} ; \
	else \
	  cp -f oxweave ${OpenXM_bin} ; \
	  cp -f oxweave ${OpenXM_ROOT}/src/kxx ; \
	  cp -f oxreplace ${OpenXM_bin} ; \
	fi
	cp -f lndir.sh ${OpenXM_bin}
	chmod 755 ${OpenXM_bin}/lndir.sh
