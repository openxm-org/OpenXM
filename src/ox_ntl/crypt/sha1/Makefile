# $OpenXM: OpenXM/src/ox_ntl/crypt/sha1/Makefile,v 1.1 2004/01/12 13:16:28 iwane Exp $

CPP=g++
CC =gcc

RM =/bin/rm
CP =/bin/cp

PREFIX=$(OpenXM_HOME)

CFLAGS=-g -Wall
CPPFLAGS=$(CFLAGS) -D_WITH_NTL_ -I$(PREFIX)/include


.SUFFIXES: .cpp .o .c

SRCC=sha1.c
SRCP=ntlsha.cpp

OBJC=$(SRCC:.c=.o) $(SRCP:.cpp=.o)

PROGS=sha1

all: $(PROGS)

sha1: $(OBJC)
#	$(CPP) $(CPPFLAGS) $(LDFLAGS) $(OBJC) -o $@


.cpp.o:
	$(CPP) $(CPPFLAGS) -c $< -o $@

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	-$(RM) -f $(OBJC) $(PROGS)


ntlsha.cpp: sha1.h

