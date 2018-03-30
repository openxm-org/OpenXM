MYLIB=-L../../lib -L/usr/local/lib
MYINCLUDE=-I../../include -I/usr/local/include
CFLAGS=-Wno-attributes
#CFLAGS=-Wno-unknown-attributes
all: ox_gsl

ox_gsl: ox_gsl.o call_gsl.o
	${CC} ${CFLAGS} -o ox_gsl ox_gsl.o call_gsl.o ${MYLIB} -lgsl -lox -lgmp -lgc

.c.o: ox_gsl.h
	${CC} ${CFLAGS} ${MYINCLUDE}  -c -g $<

clean:
	rm -f *.o *~ ox_gsl

# $OpenXM: OpenXM/src/ox_gsl/Makefile,v 1.2 2018/03/29 11:52:17 takayama Exp $
