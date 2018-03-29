MYLIB=-L../../lib -L/usr/local/lib
MYINCLUDE=-I../../include -I/usr/local/include
all: ox_gsl

ox_gsl: ox_gsl.o call_gsl.o
	${CC} -o ox_gsl ox_gsl.o call_gsl.o ${MYLIB} -lgsl -lox -lgmp -lgc

.c.o: ox_gsl.h
	${CC} ${MYINCLUDE}  -c -g $<

clean:
	rm -f *.o *~ ox_gsl

# $OpenXM$
