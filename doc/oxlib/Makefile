# $OpenXM$
all : test1 test2

ASIR_SOURCE=../../OpenXM_contrib2/asir2000
A=$(ASIR_SOURCE)
P=$(HOME)/OpenXM/lib
INCLUDE=$(P)/../doc/OpenXM-web

ASIR_LIB=${P}/libasir.a
ASIR_LIB_PARI_X=$(P)/libasir_pari_X.a
GC=$(P)/libasir-gc.a

## test1 : test1.o  
test1 : test1.o 
	gcc -o test1 test1.o $(ASIR_LIB) $(GC) -lm

## test2 : test2.o  
test2 : test2.o 
	gcc -o test2 test2.o $(ASIR_LIB) $(GC) -lm

.c.o :
	$(CC) $(CFLAGS) -I$(INCLUDE) -c -g $<





clean :
	rm -f test1 test2  *.o *~ *.a 
