OpenXM_HOME=../../..
OpenXM_LIBDIR=-L$(OpenXM_HOME)/lib
LIBS = -lmpfr  -lgmp
INCLUDES = -I$(OpenXM_HOME)/include
CFLAGS = -g -Wall -finline-functions -D_POSIX_C_SOURCE


all: bfsize

bfsize : bfsize.c
	$(CC) $(CFLAGS) $(INCLUDES) $(OpenXM_LIBDIR) $< $(LIBS) -o bfsize
clean:
	rm -f bfsize.exe bfsize *~ a.out a.exe
