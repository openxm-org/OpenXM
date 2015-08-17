# $OpenXM: OpenXM/src/ox_pari/Makefile,v 1.2 2015/08/06 09:15:32 noro Exp $

OpenXM_HOME=../..
OpenXM_LIBDIR=../../lib
OpenXM_LIB = -L$(OpenXM_LIBDIR) -lgmp -lox -lgc
INCLUDES = -I$(OpenXM_HOME)/include
PREFIX=${OpenXM_HOME}
DISTDIR=${OpenXM_HOME}/../OpenXM_dist
RM = /bin/rm
MASTER_SITE=http://www.math.kobe-u.ac.jp/pub/OpenXM/misc/
CFLAGS = -g -Wall -finline-functions -D_POSIX_C_SOURCE

PARI_LIB = -lmpfr -lpari

PARIOBJ = ox_pari.o
LIBS = $(OpenXM_LIB) $(PARI_LIB) -lm

PROG= ox_pari

all: build

components : libs $(PROG)

ox_pari: $(PARIOBJ)
	$(CC) $(PARIOBJ) $(LIBS) -o $@

ox_pari.o : ox_pari.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

libs : ox gc libgmp


ox : 
	-@if test -f $(OpenXM_HOME)/lib/libox.a ; then \
		true ; \
	else \
		( cd $(OpenXM_HOME)/src/ ; make install-oxtoolkit ) ; \
	fi

gc :
	-@if test -f $(OpenXM_HOME)/lib/libgc.a ; then \
		true ; \
	else \
		( cd $(OpenXM_HOME)/src/ ; make install-gc ) ; \
	fi

libgmp : 
	-@if test -f $(OpenXM_HOME)/lib/libgmp.a ; then \
		true ; \
	else \
		( cd $(OpenXM_HOME)/src/ ; make install-gmp ) ; \
	fi

clean:

distclean: clean
	rm -f $(OpenXM_HOME)/lib/libox.a
	rm -f $(OpenXM_HOME)/lib/libgc.a

build : components

install: build
	cp $(PROG) $(PREFIX)/bin
	-mkdir $(PREFIX)/lib/asir
	-for i in $(PROG); do \
		ln -s $(PREFIX)/bin/$$i $(PREFIX)/lib/asir ; \
	done

clean:
	-$(RM) -f $(PROG) *.o
	-$(RM)  *.core

distclean: clean
