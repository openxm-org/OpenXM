# $OpenXM: OpenXM/src/ox_ntl/crypt/rsa/Makefile,v 1.1 2004/05/16 15:04:54 iwane Exp $

OpenXM_HOME ?= ${PWD}/../../../..
PREFIX=$(OpenXM_HOME)

CXX ?= g++
CC  ?= gcc

RM  ?= /bin/rm
CP  ?= /bin/cp


CFLAGS   += -g -Wall
CXXFLAGS += -g -Wall
CPPFLAGS  = -I$(PREFIX)/include 


NTL_LIBDIR=$(PREFIX)/lib/NTL
LIBDIR=$(PREFIX)/lib 

LDFLAGS=-L$(LIBDIR) -L$(NTL_LIBDIR)

.SUFFIXES: .cpp .o .c

SRCC=
SRCP=ntlrsa.cpp ntlrsatest.cpp

PROG=rsa
OBJC=$(SRCP:.cpp=.o)

all: rsa


$(PROG): $(OBJC)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(OBJC) -o $@ -lntl -lgmp


.cpp.o:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

.c.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

clean:
	-$(RM) -f $(OBJC) $(PROG)


