# $OpenXM$

CXX ?= g++
CC  ?= gcc

RM ?= /bin/rm
CP ?= /bin/cp

OpenXM_HOME ?= ${PWD}/../../../..
PREFIX=$(OpenXM_HOME)

CFLAGS   += -g -Wall -DHAVE_CONFIG_H
CXXFLAGS += -g -Wall -D_WITH_NTL_
CPPFLAGS  = -I$(PREFIX)/include -I../..

.SUFFIXES: .cpp .o .c

SRCC=cast5.c casttest.c
SRCP=

OBJC=$(SRCC:.c=.o)

PROGS=cast128

all: $(PROGS)

$(PROGS): $(OBJC)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $(OBJC) -o $@


.cpp.o:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

.c.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

clean:
	-$(RM) -f $(OBJC) $(PROGS)


