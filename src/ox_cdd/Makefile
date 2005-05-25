# $OpenXM: OpenXM/src/ox_cdd/Makefile,v 1.1 2005/05/25 04:42:20 noro Exp $

OpenXM_HOME=../..
OpenXM_LIBDIR=../../lib
OpenXM_LIB = -L$(OpenXM_LIBDIR) -lox -lgc -lgmp
INCLUDES = -I$(OpenXM_HOME)/include
PREFIX=${OpenXM_HOME}
DISTDIR=${OpenXM_HOME}/../OpenXM_dist
RM = /bin/rm
MASTER_SITE=ftp://ftp.math.kobe-u.ac.jp/pub/OpenXM/misc/
DISTFILES=cddlib-093d.tar.gz
CFLAGS = -g -Wall -finline-functions

GMPDEF = -DGMPRATIONAL

CDD=cddlib-093d
CDD_HOME= work/$(CDD)

CDD_LIB = -lcdd
OBJ = ox_cdd.o cddlib.o
PROG = ox_cdd

CDDGMP_LIB = -lcddgmp
OBJGMP = ox_cddgmp.o cddlibgmp.o
PROGGMP = ox_cddgmp

LIBS = $(CDD_LIB) $(OpenXM_LIB) -lm
LIBSGMP = $(CDDGMP_LIB) $(OpenXM_LIB) -lm

all: build

components : libs $(PROG) $(PROGGMP)

$(PROG): $(OBJ)
	$(CC) $(OBJ) $(LIBS) -o $@

$(PROGGMP): $(OBJGMP)
	g++ $(OBJGMP) $(LIBSGMP) -o $@

ox_cdd.o : ox_cdd.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

ox_cddgmp.o : ox_cdd.c
	$(CC) -o ox_cddgmp.o $(CFLAGS) $(GMPDEF) $(INCLUDES) -c $<

cddlib.o : cddlib.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

cddlibgmp.o : cddlib.c
	$(CC) -o cddlibgmp.o  $(CFLAGS) $(GMPDEF) $(INCLUDES) -c $<

libs : ox gc libgmp cdd

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

cdd :
	-mkdir ${PREFIX}/bin ${PREFIX}/lib
	@if [ ! -f work/.install_done ]; then \
		(cd work/${CDD} ; ${MAKE} install) ; \
	fi

clean:

distclean: clean
	rm -f $(OpenXM_HOME)/lib/libox.a
	rm -f $(OpenXM_HOME)/lib/libgc.a
	rm -f $(CDD_HOME)/lib-src/libcdd.a

fetch:
	-mkdir work
	@if [ ! -f work/.fetch_done ]; then \
		${OpenXM_HOME}/bin/oxfetch.sh ${MASTER_SITE}${DISTFILES} ${DISTDIR}; \
	fi 
	@touch work/.fetch_done

extract: fetch
	@if [ ! -f work/.extract_done ]; then \
	    cat ${DISTDIR}/${DISTFILES} | (cd work ; tar xzvf -) ; \
	fi 
	@touch work/.extract_done

patch: extract
	@touch work/.patch_done

configure: patch
	@if [ ! -f work/.configure_done ]; then \
		prefix=`cd ${OpenXM_HOME}; pwd` ; \
		(cd work/${CDD} ; ./configure CXX=cc --prefix="$$prefix" ) ; \
	fi
	@touch work/.configure_done

build : configure components

install: 
	cp $(PROG) $(PROGGMP) $(PREFIX)/bin
	-mkdir $(PREFIX)/lib/asir
	-ln -s $(PREFIX)/bin/$(PROG) $(PREFIX)/bin/$(PROGGMP) $(PREFIX)/lib/asir
	@touch work/.install_done

clean:
	-$(RM) -rf work
	-$(RM) -f $(PROG) $(PROGGMP) $(OBJ) $(OBJGMP)
	-$(RM)  *.core

distclean: clean
