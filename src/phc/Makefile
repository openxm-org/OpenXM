# $OpenXM$

PROG=phc6

SRCS=phc6.c

CC=gcc
BINDIR=../../bin

all: $(PROG)

install: all
	install -c -s $(PROG) $(BINDIR)/$(PROG)

clean:
	rm -f $(PROG) $(BINDIR)/$(PROG)

$(PROG): $(SRCS)
	$(CC) -o $(PROG) $(SRCS)
