# $OpenXM: OpenXM/src/ox_maple/Makefile,v 1.3 2004/07/02 13:10:43 takayama Exp $
all:   libsimple.so  libsimple2.so

libsimple.so: simple.c
	gcc -fpic -fPIC -I../../include -c simple.c
	gcc -shared -Wl,-soname,libsimple.so -o libsimple.so simple.o -L../../lib -lox -lgmp -lgc

libsimple2.so: simple2.c
	gcc -fpic -fPIC -I../../include -c simple2.c
	gcc -shared -Wl,-soname,libsimple2.so -o libsimple2.so simple2.o -L../../lib -lox -lgmp -lgc

clean:
	rm -f *~ *.o libsimple.so libsimple2.so
	rm -f mwrap_*
