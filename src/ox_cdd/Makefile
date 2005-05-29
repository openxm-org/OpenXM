# $OpenXM: OpenXM/src/ox_cdd/Makefile,v 1.3 2005/05/25 06:43:44 noro Exp $

OpenXM_HOME=../..
OpenXM_LIBDIR=../../lib
OpenXM_LIB = -L$(OpenXM_LIBDIR) -lox -lgc -lgmp
INCLUDES = -I$(OpenXM_HOME)/include
PREFIX=${OpenXM_HOME}
DISTDIR=${OpenXM_HOME}/../OpenXM_dist
RM = /bin/rm
MASTER_SITE=ftp://ftp.math.kobe-u.ac.jp/pub/OpenXM/misc/
CDDLIB_TB=cddlib-093d.tar.gz
MVOL_TB=MixedVol.tar.gz
CFLAGS = -g -Wall -finline-functions
MVOLINCLUDE=work/MixedVol/SRC

GMPDEF = -DGMPRATIONAL

CDD=cddlib-093d
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
	g++ $(CDDOBJG) $(LIBSG) -o $@

ox_sres: $(SRESOBJ)
	g++ $(SRESOBJ) $(LIBS) -o $@

ox_sresgmp: $(SRESOBJG)
	g++ $(SRESOBJG) $(LIBSG) -o $@

ox_cdd.o : ox_cdd.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

ox_cddgmp.o : ox_cdd.c
	$(CC) -o ox_cddgmp.o $(CFLAGS) $(GMPDEF) $(INCLUDES) -c $<

ox_sres.o : ox_sres.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

ox_sresgmp.o : ox_sres.c
	$(CC) -o ox_sresgmp.o $(CFLAGS) $(GMPDEF) $(INCLUDES) -c $<

cddlib.o : cddlib.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

cddlibgmp.o : cddlib.c
	$(CC) -o cddlibgmp.o  $(CFLAGS) $(GMPDEF) $(INCLUDES) -c $<

mvol.o : mvol.cpp
	g++ $(CFLAGS) -I$(MVOLINCLUDE) -c $<

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
		cd work/${CDD}; patch < ../../cddlib.patch ; \
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