BDATE =19970304
## Kan source.  kan sources must be built up by hand.
KHOME = /home/nobuki/SSkopen
LOAD_SM1_PATH ='"/home/nobuki/SSkopen/Kan/"'
########### end of configuration.


CC = gcc
CPP = g++
MYINCLUDE = -I/usr/lib/g++-include
KAN_HOME = $(KHOME)/Kan
KAN_MSG0 = $(KAN_HOME)/msg0.o $(KAN_HOME)/msg0p.o 
KAN_MSG0P = $(KAN_HOME)/msg0p.o 
GMP_HOME = $(KHOME)/gmp
#LIBGMP = $(GMP_HOME)/libgmp.a
## This line is for buggy linux-gmp.
LIBGMP = $(GMP_HOME)/libgmp.a $(GMP_HOME)/mpn/libmpn.a
GC = $(KHOME)/gc/gc.a

all : t0

## link test with C++
t0 : t0.o $(KAN_HOME)/kanlib.a $(KAN_MSG0) $(GC) $(GMP) 
	$(CPP) $(FLAG) -I$(GMP_HOME) -I$(KAN_HOME) -o t0 t0.o $(KAN_MSG0) $(KAN_HOME)/kanlib.a $(LIBGMP) $(GC)

t0.o : t0.cc
	$(CPP) -c $(FLAG) $(MYINCLUDE) -I$(GMP_HOME) -I$(KAN_HOME) -o t0.o t0.cc

bridge.o : bridge.c
	$(CC) -c bridge.c

clean :
	/bin/rm -f *.o *~ t0



