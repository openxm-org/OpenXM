# $OpenXM: OpenXM/doc/oxlib/Makefile,v 1.5 2000/09/07 21:34:06 takayama Exp $
all : test test1 test2 test3
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

## test : test.o  
test : test.o 
	gcc -o test test.o $(ASIR_LIB) $(GC) $(EXTRALIB) -lm


.c.o :
	${CC} $(CFLAGS) -I$(INCLUDE) -c -g $<





clean :
	rm -f test test1 test2 test3 *.o *~ *.a  a.out
