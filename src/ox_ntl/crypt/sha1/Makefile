# $OpenXM: OpenXM/src/ox_ntl/crypt/sha1/Makefile,v 1.2 2004/03/25 13:34:19 iwane Exp $

CPP=g++
CC =gcc

RM =/bin/rm
CP =/bin/cp

PREFIX=$(OpenXM_HOME)

CFLAGS=-g -Wall
CPPFLAGS=$(CFLAGS) -D_WITH_NTL_ -I$(PREFIX)/include


.SUFFIXES: .cpp .o .c

SRCC=sha1.c sha1test.c
SRCP=ntlsha.cpp

OBJC=$(SRCC:.c=.o)

PROGS=sha1test

all: $(PROGS)

$(PROGS): $(OBJC)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJC) -o $@ 


.cpp.o:
	$(CPP) $(CPPFLAGS) -c $< -o $@

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	-$(RM) -f $(OBJC) $(PROGS)


ntlsha.cpp: sha1.h

