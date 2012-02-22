# $OpenXM: OpenXM/doc/oxlib/Makefile,v 1.8 2005/11/12 02:38:01 takayama Exp $
all : install-lib
	make all-samples
all-samples : test test1 test2 test3
install-lib:
	-(cd ../../lib ; ln -s ../../OpenXM_contrib2/asir2000/libasir.a .)
##
## make   
## make OpenXM_HOME=/usr/local/OpenXM
CC=gcc
OpenXM_HOME=../../../OpenXM
LIBDIR=${OpenXM_HOME}/lib
INCDIR=${OpenXM_HOME}/include

ASIR_LIB=$(LIBDIR)/libasir.a
ASIR_LIB_PARI_X=$(LIBDIR)/libasir_pari_X.a
PARI_LIB=$(LIBDIR)/libpari.a
GC=$(LIBDIR)/libasir-gc.a
#EXTRALIB=-lnsl -lsocket
EXTRALIB=
LOPT=-L/usr/X11R6/lib -lm -lX11 -lXt -lXaw

INCLUDE=$(INCDIR)

test1 : test1.o 
	gcc -o test1 test1.o $(ASIR_LIB) $(PARI_LIB) $(GC) $(EXTRALIB) $(LOPT)

## test2 : test2.o  
test2 : test2.o 
	gcc -o test2 test2.o $(ASIR_LIB) $(PARI_LIB) $(GC) $(EXTRALIB) $(LOPT)

## test3 : test3.o  
test3 : test3.o 
	gcc -o test3 test3.o $(ASIR_LIB) $(PARI_LIB) $(GC) $(EXTRALIB) $(LOPT)

## test : test.o  
test : test.o 
	gcc -o test test.o $(ASIR_LIB) $(PARI_LIB) $(GC) $(EXTRALIB) $(LOPT)
test1-tcp-rev: test1-tcp-rev.c
	gcc -o test1-tcp-rev test1-tcp-rev.c 

.c.o :
	${CC} $(CFLAGS) -I$(INCLUDE) -c -g $<





clean :
	rm -f test test1 test2 test3 *.o *~ *.a  a.out
