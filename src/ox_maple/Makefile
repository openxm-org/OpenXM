# $OpenXM$
all:   libsimple.so

libsimple.so: simple.c
	gcc -fPIC -I/private/taka/OpenXM/include -c simple.c
	gcc -shared -Wl,-soname,libsimple.so -o libsimple.so simple.o ../ox_toolkit/libox.so

clean:
	rm -f *~ *.o libsimple.so
	rm -f mwrap_*
