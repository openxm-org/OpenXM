# $OpenXM: OpenXM/src/util/Makefile,v 1.11 2003/11/16 07:14:11 takayama Exp $
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
	$(CC) -D_BSD_SOURCE -D_POSIX_C_SOURCE -c ox_pathfinder.c

test: ox_pathfinder.o test.c
	$(CC) -o test -D_BSD_SOURCE -D_POSIX_C_SOURCE test.c ox_pathfinder.o -L${OpenXM_lib} -lgc

install: all
	@if [ ! -f ./.install_done ] ; then \
	echo "Installing oxweave, oxreplace, libox_pathfinder.a, ox_pathfinder.h oxfetch.sh" ; \
	install oxweave oxreplace ${OpenXM_bin} ; \
	install oxweave ${OpenXM_ROOT}/src/kxx ; \
	cp -f libox_pathfinder.a ${OpenXM_lib}  ; \
	cp -f ox_pathfinder.h ${OpenXM_include} ; \
	install lndir.sh ${OpenXM_bin} ; \
	install oxfetch.sh ${OpenXM_bin} ; \
	touch ./.install_done ; \
	fi

clean:
	rm -f .install_done *.o oxweave oxreplace libox_pathfinder.a test 
