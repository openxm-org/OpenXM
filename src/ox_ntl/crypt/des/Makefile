# $OpenXM: OpenXM/src/ox_ntl/crypt/des/Makefile,v 1.1 2004/07/11 00:32:17 iwane Exp $

OpenXM_HOME ?= ${PWD}/../../../..
PREFIX=$(OpenXM_HOME)

PROG=des

CC ?= gcc

RM ?= /bin/rm -f


CFLAGS   += -Wall -g -DHAVE_CONFIG_H
CPPFLAGS  = -I$(PREFIX)/include -I../..


SRCS=block.c des.c destest.c

OBJC=$(SRCS:.c=.o)

all:: $(PROG)

des: $(OBJC)
	$(CC) $(CFLAGS) $(OBJC) -o $@


.c.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

clean:
	-$(RM) $(OBJC) $(PROG)


