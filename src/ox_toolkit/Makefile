# -*- mode: Makefile -*-
# $OpenXM: OpenXM/src/ox_toolkit/Makefile,v 1.4 1999/12/16 06:09:42 ohara Exp $

CFLAGS = -g -O2 -DDEBUG -I${OpenXM_HOME}/src/gmp
LIBOBJS = ox.o parse.o mysocket.o
GMPSRC = ${OpenXM_HOME}/src/gmp
OXWEAVE = ${OpenXM_HOME}/bin/oxweave
BINARIES = bconv testclient sample1 sample2

all: libox.a libgmp.a ${BINARIES} README.jp README.en

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

sample1: sample1.o libox.a libgmp.a
	${CC} ${CFLAGS} -o $@ sample1.o -L. -lox -lgmp

sample2: sample2.o libox.a libgmp.a
	${CC} ${CFLAGS} -o $@ sample2.o -L. -lox -lgmp

README.jp: README
	${OXWEAVE} ja common < README > README.jp

README.en: README
	${OXWEAVE} en common < README > README.en

clean:
	-rm -f *.o *~ libox.a libgmp.a README.jp README.en \
	       bconv testclient core a.out sample1 sample2

