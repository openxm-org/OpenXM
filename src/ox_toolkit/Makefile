# -*- mode: Makefile -*-
# $OpenXM: OpenXM/src/ox_toolkit/Makefile,v 1.8 2000/01/19 06:18:51 noro Exp $

OpenXM_HOME = ../..
CFLAGS      = -g -O2 -DDEBUG -I${OpenXM_HOME}/src/gmp
LIBOBJS     = ox.o parse.o mysocket.o
GMPSRC      = ${OpenXM_HOME}/src/gmp
OXWEAVE     = ${OpenXM_HOME}/bin/oxweave
BINARIES    = bconv testclient sample1 sample2 
DOCUMENTS   = readme-eg readme-jp bnf-eg bnf-jp
X11INC      = /usr/X11R6/include
X11LIB      = /usr/X11R6/lib
CC          = gcc

all: libox.a libgmp.a ${BINARIES} ${DOCUMENTS}

all2: all ox_Xsample

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

ox_Xsample: ox_Xsample.o glib.c libox.a libgmp.a
	${CC} ${CFLAGS} -I${X11INC} -o $@ ox_Xsample.o glib.c -L. -L${X11LIB} -lox -lgmp -lX11

readme-jp: README
	${OXWEAVE} ja common < README > readme-jp

readme-eg: README
	${OXWEAVE} en common < README > readme-eg

bnf-jp: bnf.oxweave
	${OXWEAVE} ja common < bnf.oxweave > bnf-jp

bnf-eg: bnf.oxweave
	${OXWEAVE} en common < bnf.oxweave > bnf-eg

install:
	@echo "No binaries are installed."

install-for-debug:
	@echo "No binaries are installed."

install-document: ${DOCUMENTS}
	-mkdir ${OpenXM_HOME}/doc/ox_toolkit
	cp -f ${DOCUMENTS} ${OpenXM_HOME}/doc/ox_toolkit

clean-document:
	-rm -f ${DOCUMENTS}

clean-binary:
	-rm -f *.o *~ libox.a libgmp.a ${BINARIES} ox_Xsample core a.out

clean: clean-document clean-binary
