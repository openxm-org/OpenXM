# -*- mode: Makefile -*-
# $OpenXM: OpenXM/src/ox_toolkit/Makefile,v 1.1 1999/12/09 22:44:55 ohara Exp $

CFLAGS = -g -O2 -DDEBUG -I${OpenXM_HOME}/src/gmp
LIBOBJS = ox.o parse.o mysocket.o
GMPSRC = ${OpenXM_HOME}/src/gmp

all: libox.a libgmp.a bconv testclient README.jp

ox.o mysocket.o: mysocket.h
ox.o parse.o:    ox.h oxtag.h parse.h

libgmp.a:
	test -f ${GMPSRC}/libgmp.a || (cd ${GMPSRC}; ./configure; ${MAKE})
	-ln -s ${GMPSRC}/libgmp.a .

libox.a: ${LIBOBJS}
	-rm -f $@
	ar -rc $@ ${LIBOBJS}
	ranlib $@

bconv: bconv.o libox.a libgmp.a
	${CC} ${CFLAGS} -o $@ bconv.o -L. -lox -lgmp

testclient: testclient.o libox.a libgmp.a
	${CC} ${CFLAGS} -o $@ testclient.o -L. -lox -lgmp

README.jp: README
	-rm README.jp
	oxweave ja common < README > README.jp

clean:
	-rm -f *.o *~ libox.a libgmp.a README.jp bconv testclient
