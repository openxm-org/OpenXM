# $OpenXM: OpenXM/doc/oxlib/Makefile,v 1.2 2000/02/09 06:08:58 takayama Exp $
all : test1 test2 test3
CC=gcc
LIBDIR=../../lib
INCDIR=../../include

ASIR_LIB=$(LIBDIR)/libasir.a
ASIR_LIB_PARI_X=$(LIBDIR)/libasir_pari_X.a
GC=$(LIBDIR)/libasir-gc.a
#EXTRALIB=-lnsl -lsocket
EXTRALIB=

INCLUDE=$(INCDIR)

## test1 : test1.o  
test1 : test1.o 
	gcc -o test1 test1.o $(ASIR_LIB) $(GC) $(EXTRALIB) -lm

## test2 : test2.o  
test2 : test2.o 
	gcc -o test2 test2.o $(ASIR_LIB) $(GC) $(EXTRALIB) -lm

## test3 : test3.o  
test3 : test3.o 
	gcc -o test3 test3.o $(ASIR_LIB) $(GC) $(EXTRALIB) -lm

.c.o :
	${CC} $(CFLAGS) -I$(INCLUDE) -c -g $<





clean :
	rm -f test1 test2 test3 *.o *~ *.a 
