MYLIB=../../lib
MYINCLUDE=../../include
all: ox_gsl

ox_gsl: ox_gsl.o
	${CC} -o ox_gsl ox_gsl.o -L${MYLIB} -lox -lgmp -lgc

.c.o:
	${CC} -I${MYINCLUDE}  -c -g $<

clean:
	rm -f *.o *~ ox_gsl

