# Main Make File For Spin 0.0

# specify compiler and compiler flags
# note to use profiling need -pg option for both compiling and linking
CC = gcc
CFLAGS = -O3

Lib_Dirs = 
Libs = -lm 

.C.o:
	$(CC) -c $(CFLAGS) $*.C 

all:  tigers
	cp -f tigers ../../bin 

objA = utime.o  exsearch.o call.o gset.o binomial.o utils.o Rsimp.o tigers.o matrices.o toric.o Ihermite.o
tigers: $(objA) ;
	$(CC)  $(LDFLAGS)  $(objA)  -o tigers $(Lib_Dirs) $(Libs)


clean:
	/bin/rm -rf *.o *~ tigers

