# $OpenXM: OpenXM/src/ox_ntl/crypt/rsa/Makefile,v 1.3 2004/08/16 03:59:58 iwane Exp $

VPATH=.:../sha1

OpenXM_HOME ?= ${PWD}/../../../..
PREFIX=$(OpenXM_HOME)

CXX ?= g++
CC  ?= gcc

RM  ?= /bin/rm -f
CP  ?= /bin/cp


CFLAGS   += -g -Wall
CXXFLAGS += -g -Wall
CPPFLAGS  = -I$(PREFIX)/include  -I../sha1


NTL_LIBDIR=$(PREFIX)/lib/NTL
LIBDIR=$(PREFIX)/lib 

LDFLAGS=-L$(LIBDIR) -L$(NTL_LIBDIR)

.SUFFIXES: .cpp .o .c

SRCCG=gmprsa.c ../sha1/sha1.c gmprsatest.c
SRCPN=ntlrsa.cpp ntlrsatest.cpp

NTL=ntlrsatest
GMP=gmprsatest

PROGS=$(NTL) $(GMP)

OBJCN=$(SRCPN:.cpp=.o) $(SRCCN:.c=.o)
OBJCG=$(SRCPG:.cpp=.o) $(SRCCG:.c=.o)



all: $(PROGS)


$(GMP): $(OBJCG)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(OBJCG) -o $@ -lgmp


$(NTL): $(OBJCN)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(OBJCN) -o $@ -lntl -lgmp
.cpp.o:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

.c.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

clean:
	-$(RM) $(OBJCG) $(OBJCN) $(PROGS)



gmprsa.o: gmprsa.h
gmprsatest.o: gmprsa.h
ntlrsa.o: ntlrsa.h
ntlrsatest.o : ntlrsa.h
