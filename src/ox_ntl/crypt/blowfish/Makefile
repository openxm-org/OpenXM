# $OpenXM: OpenXM/src/ox_ntl/crypt/blowfish/Makefile,v 1.1 2004/07/22 12:12:05 iwane Exp $

CXX ?= g++
CC  ?= gcc

RM ?= /bin/rm -f
CP ?= /bin/cp

OpenXM_HOME ?= ${PWD}/../../../..
PREFIX=$(OpenXM_HOME)

CFLAGS   += -g -Wall
CXXFLAGS += -g -Wall -D_WITH_NTL_
CPPFLAGS  = 

.SUFFIXES: .cpp .o .c

SRCC=blowfish.c bftest.c
SRCP=

OBJC=$(SRCC:.c=.o)

PROGS=blowfish

all: $(PROGS)

$(PROGS): $(OBJC)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $(OBJC) -o $@


.cpp.o:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

.c.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

clean:
	-$(RM) $(OBJC) $(PROGS)


blowfish.o: blowfish.h bf_tbl.h
bftest.o: blowfish.h

