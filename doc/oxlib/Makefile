# $OpenXM: OpenXM/doc/oxlib/Makefile,v 1.3 2000/03/16 07:34:37 noro Exp $
all : test1 test2 test3
##
## make   
## make OpenXM_HOME=/usr/local/OpenXM
CC=gcc
OpenXM_HOME=../../../OpenXM
LIBDIR=${OpenXM_HOME}/lib
INCDIR=${OpenXM_HOME}/include

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
