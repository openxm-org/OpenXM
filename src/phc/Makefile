# $OpenXM: OpenXM/src/phc/Makefile,v 1.4 2000/01/07 21:49:13 maekawa Exp $

PROG=phc6

SRCS=phc6.c

CC=gcc
BINDIR=../../bin

all: $(PROG)

install: all
	install -c -s $(PROG) $(BINDIR)/$(PROG)

clean:
	rm -f $(PROG)

distclean:
	rm -f $(BINDIR)/$(PROG)

$(PROG): $(SRCS)
	$(CC) -o $(PROG) $(SRCS)
