# $OpenXM$
CFLAGS	= -g -O2 -DDEBUG -I${OpenXM_HOME}/src/gmp -I../ox_toolkit
CC	= cc

all: testserv1

testserv1: testserv1.o
	${CC} ${CFLAGS} -o testserv1  testserv1.c -L../ox_toolkit -lox -lgmp -lm
