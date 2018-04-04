MYLIB=-L../../lib -L/usr/local/lib
MYINCLUDE=-I../../include -I/usr/local/include
CFLAGS=-Wno-attributes
GSL_LIBS=-lgsl -lcblas -lblas -lm
#GSL_LIBS=-lgsl    #  MacOS X
BSD_OPT=-L/usr/local/lib/gcc49 -Wl,-rpath=/usr/local/lib/gcc49
#CFLAGS=-Wno-unknown-attributes
all: ox_gsl

ox_gsl: ox_gsl.o call_gsl.o ox_eval.o
	${CC} ${CFLAGS} -o ox_gsl ox_gsl.o call_gsl.o ox_eval.o ${BSD_OPT} ${MYLIB} ${GSL_LIBS} -lox  -lmpfr -lgmp -lgc

.c.o: ox_gsl.h
	${CC} ${CFLAGS} ${MYINCLUDE}  -c -g $<

clean:
	rm -f *.o *~ ox_gsl

# $OpenXM: OpenXM/src/ox_gsl/Makefile,v 1.3 2018/03/30 04:43:16 takayama Exp $
