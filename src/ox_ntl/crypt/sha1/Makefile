# $OpenXM: OpenXM/src/ox_ntl/crypt/sha1/Makefile,v 1.4 2004/07/04 02:31:51 iwane Exp $

CXX ?= g++
CC  ?= gcc

RM ?= /bin/rm -f
CP ?= /bin/cp

OpenXM_HOME ?= ${PWD}/../../../..
PREFIX=$(OpenXM_HOME)

CFLAGS   += -g -Wall
CXXFLAGS += -g -Wall -D_WITH_NTL_
CPPFLAGS  = -I$(PREFIX)/include


.SUFFIXES: .cpp .o .c

SRCC=sha1.c sha1test.c
SRCP=ntlsha.cpp

OBJC=$(SRCC:.c=.o)

PROGS=sha1test

all: $(PROGS)

$(PROGS): $(OBJC)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJC) -o $@ 


.cpp.o:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

.c.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

clean:
	-$(RM) $(OBJC) $(PROGS)

ntlsha.cpp: sha1.h

