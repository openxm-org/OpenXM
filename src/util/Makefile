# $OpenXM: OpenXM/src/util/Makefile,v 1.5 2003/07/21 11:36:10 takayama Exp $
OpenXM_ROOT=../../../OpenXM
OpenXM_bin=$(OpenXM_ROOT)/bin
OpenXM_lib=$(OpenXM_ROOT)/lib
OpenXM_include=$(OpenXM_ROOT)/include
CC=gcc

all : oxweave oxreplace libox_pathfinder.a

oxweave : oxweave.c
	gcc    -D_BSD_SOURCE -o oxweave -g oxweave.c
oxreplace : oxreplace.c
	gcc    -D_BSD_SOURCE -o oxreplace -g oxreplace.c
libox_pathfinder.a : ox_pathfinder.o
	ar ru libox_pathfinder.a ox_pathfinder.o
ox_pathfinder.o : ox_pathfinder.c ox_pathfinder.h
	$(CC) -D_BSD_SOURCE -c ox_pathfinder.c

install: all
	@if [ ! -f ./.install_done ] ; then \
	echo "Installing oxweave, oxreplace, libox_pathfinder.a, ox_pathfinder.h" ; \
	if [  "`uname | sed -e 's/_.*$$//'`" = "CYGWIN" ]; then \
	  cp -f oxweave.exe ${OpenXM_bin} ; \
	  cp -f oxweave.exe ${OpenXM_ROOT}/src/kxx ; \
	  cp -f oxreplace.exe ${OpenXM_bin} ; \
	else \
	  cp -f oxweave ${OpenXM_bin} ; \
	  cp -f oxweave ${OpenXM_ROOT}/src/kxx ; \
	  cp -f oxreplace ${OpenXM_bin} ; \
	fi ; \
	cp -f libox_pathfinder.a ${OpenXM_lib}  ; \
	cp -f ox_pathfinder.h ${OpenXM_include} ; \
	cp -f lndir.sh ${OpenXM_bin} ; \
	chmod 755 ${OpenXM_bin}/lndir.sh ; \
	touch ./.install_done ; \
	fi

clean:
	rm -f .install_done *.o oxweave oxreplace libox_pathfinder.a