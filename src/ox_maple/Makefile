# $OpenXM: OpenXM/src/ox_maple/Makefile,v 1.2 2004/06/26 13:19:01 ohara Exp $
all:   libsimple.so  libsimple2.so

libsimple.so: simple.c
	gcc -fpic -fPIC -I../../include -c simple.c
	gcc -shared -Wl,-soname,libsimple.so -o libsimple.so simple.o ../ox_toolkit/libox.so

libsimple2.so: simple2.c
	gcc -fpic -fPIC -I../../include -c simple2.c
	gcc -shared -Wl,-soname,libsimple2.so -o libsimple2.so simple2.o ../ox_toolkit/libox.so

clean:
	rm -f *~ *.o libsimple.so libsimple2.so
	rm -f mwrap_*
