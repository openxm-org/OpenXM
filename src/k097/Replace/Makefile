CC = gcc
KAN_HOME = /home/nobuki/SSkan/Kan
GMP_HOME = /home/nobuki/SSkan/gmp
LIBGMP = $(GMP_HOME)/libgmp.a
GC = /home/nobuki/SSkan/gc3.2/gc.a
a.out : colide.c
	$(CC) -I$(GMP_HOME) -I$(KAN_HOME) -o a.out colide.c $(KAN_HOME)/kanlib.a $(LIBGMP) $(GC)

colide2 : colide.c colide2.c
	$(CC) -c colide2.c
	ar ru colide2.a colide2.o
	ranlib colide2.a
	$(CC) -I$(GMP_HOME) -I$(KAN_HOME) -o colide2 colide.c colide2.a $(KAN_HOME)/kanlib.a $(LIBGMP) $(GC)

########## cleaning
clean:
	/bin/rm *~ a.out colide2.a colide2 

