# $OpenXM$

CFLAGS=-O2 -Wall
#LIBS=-lsocket -lnsl

all: bin/tm_asir

bin/tm_asir: src/tm_asir.c
	-mkdir bin
	${CC} ${CFLAGS} -o bin/tm_asir src/tm_asir.c ${LIBS}
	strip bin/tm_asir

clean:
	-rm -f bin/tm_asir *~ src/*~ progs/*~
