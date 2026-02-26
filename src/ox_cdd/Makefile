# $OpenXM: OpenXM/src/ox_cdd/Makefile,v 1.7 2015/02/21 06:20:36 ohara Exp $

OpenXM_HOME=../..
OpenXM_LIBDIR=../../lib
OpenXM_LIB = -L$(OpenXM_LIBDIR) -lox -lgc -lgmp -lmpfr
INCLUDES = -I$(OpenXM_HOME)/include
PREFIX=${OpenXM_HOME}
DISTDIR=${OpenXM_HOME}/../OpenXM_dist
RM = /bin/rm
MASTER_SITE=https://www.math.kobe-u.ac.jp/pub/OpenXM/misc/
MVOL_TB=MixedVol.tar.gz
CFLAGS = -g -Wall -finline-functions
MVOLINCLUDE=work/MixedVol/SRC

GMPDEF = -DGMPRATIONAL

CDD=cddlib-094d
CDDLIB_TB=$(CDD).tar.gz
CDD_HOME= work/$(CDD)

CDD_LIB = -lcdd
CDDOBJ = ox_cdd.o cddlib.o
SRESOBJ = ox_sres.o cddlib.o mvol.o
LIBS = $(CDD_LIB) $(OpenXM_LIB) -lm

CDDG_LIB = -lcddgmp
CDDOBJG = ox_cddgmp.o cddlibgmp.o
SRESOBJG = ox_sresgmp.o cddlibgmp.o mvol.o
LIBSG = $(CDDG_LIB) $(OpenXM_LIB) -lm

PROG= ox_cdd ox_sres ox_cddgmp ox_sresgmp

all: build

components : libs $(PROG)

ox_cdd: $(CDDOBJ)
	$(CC) $(CDDOBJ) $(LIBS) -o $@

ox_cddgmp: $(CDDOBJG)
	$(CXX) $(CDDOBJG) $(LIBSG) -o $@

ox_sres: $(SRESOBJ)
	$(CXX) $(SRESOBJ) $(LIBS) -o $@

ox_sresgmp: $(SRESOBJG)
	$(CXX) $(SRESOBJG) $(LIBSG) -o $@

ox_cdd.o : ox_cdd.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

ox_cddgmp.o : ox_cdd.c
	$(CC) -o ox_cddgmp.o $(CFLAGS) $(GMPDEF) $(INCLUDES) -c ox_cdd.c

ox_sres.o : ox_sres.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

ox_sresgmp.o : ox_sres.c
	$(CC) -o ox_sresgmp.o $(CFLAGS) $(GMPDEF) $(INCLUDES) -c ox_sres.c

cddlib.o : cddlib.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

cddlibgmp.o : cddlib.c
	$(CC) -o cddlibgmp.o  $(CFLAGS) $(GMPDEF) $(INCLUDES) -c cddlib.c

mvol.o : mvol.cpp
	$(CXX) $(CFLAGS) -I$(MVOLINCLUDE) -c $<

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
		${OpenXM_HOME}/bin/oxfetch.sh ${MASTER_SITE}${CDDLIB_TB} ${DISTDIR}; \
		${OpenXM_HOME}/bin/oxfetch.sh ${MASTER_SITE}${MVOL_TB} ${DISTDIR}; \
	fi 
	@touch work/.fetch_done

extract: fetch
	@if [ ! -f work/.extract_done ]; then \
	    cat ${DISTDIR}/${CDDLIB_TB} | (cd work ; tar xzvf -) ; \
	    cat ${DISTDIR}/${MVOL_TB} | (cd work ; tar xzvf -) ; \
	fi 
	@touch work/.extract_done

patch: extract
	@if [ ! -f work/.patch_done ]; then \
		cd work/${CDD}; patch -p0 < ../../cddlib.patch ; \
	fi
	@touch work/.patch_done

configure: patch
	@if [ ! -f work/.configure_done ]; then \
		prefix=`cd ${OpenXM_HOME}; pwd` ; \
		(cd work/${CDD} ; ./configure CXX=cc LDFLAGS=-L$$prefix/lib --prefix="$$prefix" ) ; \
	fi
	@touch work/.configure_done

build : configure components

install: build
	cp $(PROG) $(PROG) $(PREFIX)/bin
	-mkdir $(PREFIX)/lib/asir
	-for i in $(PROG); do \
		ln -s $(PREFIX)/bin/$$i $(PREFIX)/lib/asir ; \
	done
	@touch work/.install_done

clean:
	-$(RM) -rf work
	-$(RM) -f $(PROG) *.o
	-$(RM)  *.core

distclean: clean
