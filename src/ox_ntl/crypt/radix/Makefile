# $OpenXM: OpenXM/src/ox_ntl/crypt/radix/Makefile,v 1.1 2004/01/12 14:52:18 iwane Exp $

CXX ?= g++
CC  ?= gcc

RM  ?= /bin/rm
CP  ?= /bin/cp

OpenXM_HOME ?= ${PWD}/../../../..
PREFIX=$(OpenXM_HOME)

CFLAGS   +=-g -Wall
CXXFLAGS +=-g -Wall -D_WITH_NTL_
CPPFLAGS  =-I$(PREFIX)/include


.SUFFIXES: .cpp .o .c

SRCC=radix64.c
SRCP=

OBJC=$(SRCC:.c=.o) $(SRCP:.cpp=.o)

PROGS=radix64

all: $(PROGS)

radix64: $(OBJC)
#	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJC) -o $@


.cpp.o:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

.c.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

clean:
	-$(RM) -f $(OBJC) $(PROGS)



