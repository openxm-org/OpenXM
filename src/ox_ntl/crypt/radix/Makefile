# $OpenXM$

CPP=g++
CC =gcc

RM =/bin/rm
CP =/bin/cp

PREFIX=$(OpenXM_HOME)

CFLAGS=-g -Wall
CPPFLAGS=$(CFLAGS) -D_WITH_NTL_ -I$(PREFIX)/include


.SUFFIXES: .cpp .o .c

SRCC=radix64.c
SRCP=

OBJC=$(SRCC:.c=.o) $(SRCP:.cpp=.o)

PROGS=radix64

all: $(PROGS)

radix64: $(OBJC)
#	$(CC) $(CPPFLAGS) $(LDFLAGS) $(OBJC) -o $@


.cpp.o:
	$(CPP) $(CPPFLAGS) -c $< -o $@

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	-$(RM) -f $(OBJC) $(PROGS)



