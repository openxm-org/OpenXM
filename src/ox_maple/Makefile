# $OpenXM$

PROG=ox_maple

CC = gcc
RM ?= /bin/rm
MAKE ?= make


CFLAGS += -Wall -g -O2 -I$(OpenXM_HOME)/include -I$(OpenXM_HOME)/src/ox_ntl -I$(MAPLE)/extern/include  


LIBS_MPL= -L$(MAPLE)/bin.IBM_INTEL_LINUX -lmaplec
LIBS= -loxsv -lox -lgmp -lgc 
LDFLAGS= -L$(OpenXM_HOME)/lib 


DEFS += -D__GMP_FALE_H__

.SUFFIXES: .o .c


SRCC= m2c.c c2m.c oxmaples.c

OBJC = $(SRCC:.c=.o) 

all:: $(PROG) libsimple.so  libsimple2.so

$(PROG): $(OBJC) main.o 
	$(CC) $(OBJC) main.o -o $@ $(LIBS_MPL) $(LDFLAGS) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) -c $< $(DEFS)


clean:
	-$(RM) -f $(OBJC)  main.o local.o
	-$(RM) -f $(PROG)
	-$(RM) -f simple.o simple2.o libsimple.so libsimple2.so
	

#----------------------------------------------
libsimple.so: simple.c
	gcc -fpic -fPIC -I../../include -c simple.c
	gcc -shared -Wl,-soname,libsimple.so -o libsimple.so simple.o -L../../lib -lox -lgmp -lgc

libsimple2.so: simple2.c
	gcc -fpic -fPIC -I../../include -c simple2.c
	gcc -shared -Wl,-soname,libsimple2.so -o libsimple2.so simple2.o -L../../lib -lox -lgmp -lgc


