# $OpenXM$

# OpenXM_HOME=${PWD}/../../..

CPP=g++
CC =gcc

RM =/bin/rm
CP =/bin/cp

CFLAGS=-I$(PREFIX)/include -g -Wall
CPPFLAGS=$(CFLAGS)

PREFIX=$(OpenXM_HOME)

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
	$(CPP) $(CPPFLAGS) $(LDFLAGS) $(OBJC) -o $@ -lntl -lgmp


.cpp.o:
	$(CPP) $(CPPFLAGS) -c $< -o $@

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	-$(RM) -f $(OBJC) $(PROG)


