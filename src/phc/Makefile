# $OpenXM: OpenXM/src/phc/Makefile,v 1.5 2000/01/08 20:46:20 maekawa Exp $

PROG=phc6

SRCS=phc6.c

CC=gcc
BINDIR=../../bin

all: $(PROG)

install: all
	if test `uname` = "OSF1" ; then \
		installbsd -c -s $(PROG) $(BINDIR)/$(PROG) ; \
	else \
		install -c -s $(PROG) $(BINDIR)/$(PROG) ; \
	fi

clean:
	rm -f $(PROG)

distclean:
	rm -f $(BINDIR)/$(PROG)

$(PROG): $(SRCS)
	$(CC) -o $(PROG) $(SRCS)
