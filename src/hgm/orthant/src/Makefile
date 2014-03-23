CC = gcc
OBJ = hgm_ko_orthant.o main.o
DEBUG = -O0 -g -D_DEBUG

all: hgm_ko_orthant hgm_ko_orthant.so

hgm_ko_orthant: $(OBJ)
	$(CC) -o $@ $(OBJ) -lm -lblas -llapack

hgm_ko_orthant.so: hgm_ko_orthant.o hgm_ko_orthant_R_interface.c
	R CMD SHLIB -o hgm_ko_orthant.so hgm_ko_orthant.o hgm_ko_orthant_R_interface.c -llapack

.c.o:
	$(CC) -c -o $@ $< -Wall $(DEBUG)

.PHONY: clean

clean:
	rm -fv hgm_ko_orthant *.so *.o *~ 
