# -*- mode: Makefile -*-
# $OpenXM$

CFLAGS = -g -O2 -DDEBUG -I${OpenXM_HOME}/src/gmp
LIBOBJS = ox.o parse.o mysocket.o

all: libox.a

ox.o mysocket.o: mysocket.h
ox.o parse.o:    ox.h oxtag.h parse.h


libox.a: ${LIBOBJS}
	-rm -f $@
	ar -rc $@ ${LIBOBJS}
	ranlib $@

clean:
	-rm -f *.o *~ libox.a
