# $OpenXM: OpenXM/src/phc/Makefile,v 1.3 2000/01/07 20:41:51 maekawa Exp $

PROG=phc6

SRCS=phc6.c

CC=gcc
BINDIR=../../bin

all: $(PROG)

install: all
	install -c -s $(PROG) $(BINDIR)/$(PROG)

clean:
	rm -f $(PROG)

$(PROG): $(SRCS)
	$(CC) -o $(PROG) $(SRCS)
