# $OpenXM: OpenXM/src/ox_maple/Makefile,v 1.1 2004/06/21 12:46:04 takayama Exp $
all:   libsimple.so

libsimple.so: simple.c
	gcc -fpic -fPIC -I../../include -c simple.c
	gcc -shared -Wl,-soname,libsimple.so -o libsimple.so simple.o ../ox_toolkit/libox.so

clean:
	rm -f *~ *.o libsimple.so
	rm -f mwrap_*
