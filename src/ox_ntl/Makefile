# $OpenXM: OpenXM/src/ox_ntl/Makefile,v 1.8 2003/11/15 09:06:20 iwane Exp $
#
#
#
#
# wget should be installed for install `NTL'
#

PROG=ox_ntl

OpenXM_HOME=${PWD}/../..

CPP=g++
CC =gcc

RM =/bin/rm
CP =/bin/cp

CFLAGS=-I$(PREFIX)/include -g
CPPFLAGS=$(CFLAGS)

# WITH_GMP        <== for ox_toolkit --      include "gmp.h"
# __GMP_FALE_H__  <==                -- dont include "gmp_fake.h"
DEFS=-DWITH_GMP=1 -D__GMP_FAKE_H__

PREFIX=$(OpenXM_HOME)

MASTER_CITES=ftp.math.sci.kobe-u.ac.jp

NTL_LIBDIR=$(PREFIX)/lib/NTL
LIBDIR=$(PREFIX)/lib 

DISTDIR=${OpenXM_HOME}/../OpenXM_dist
DISTFILEDIR=ntl-5.3.1
DISTFILE=ntl-5.3.1.tar.gz
WORK=$(PWD)/work

GMPOPT=NTL_GMP_LIP=on GMP_PREFIX=${OpenXM_HOME} GMP_INCDIR=${OpenXM_HOME}/include GMP_LIBDIR=${OpenXM_HOME}/lib

.SUFFIXES: .cpp .o .c

SRCC=oxserv.c oxstack.c
SRCP=main.cpp ntl.cpp ntlconv.cpp cmon.cpp

OBJC=$(SRCC:.c=.o) $(SRCP:.cpp=.o)


all: $(PROG)


$(PROG): $(OBJC) $(LIBDIR)/libox.a $(NTL_LIBDIR)/libntl.a
	$(CPP) $(CPPFLAGS) -L$(LIBDIR) -L$(NTL_LIBDIR) $(OBJC) -o $@ -lntl -lgmp -lox -lgc ${LIBS}


oxserv.c: oxserv.h oxstack.h

oxstack.c: oxstack.h

main.cpp: oxserv.h ntl.h oxstack.h

cmon.cpp: ntl.h

ntl.cpp: ntl.h

ntlconv.cpp: ntl.h

.cpp.o:
	$(CPP) $(CPPFLAGS) $(DEFS) -c $< -o $@

.c.o:
	$(CC) $(CFLAGS) $(DEFS) -c $< -o $@

install: all
	$(CP) $(PROG) $(PREFIX)/bin

clean:
	-$(RM) -f $(OBJC) $(PROG) ox_ntl_debug oxserv_debug
	-$(RM) -fr ${WORK}

distclean:
	-$(RM) -f .*-ntl-done


fetch-ntl:
	@if [ ! -f .fetch-ntl-done ]; then \
		${OpenXM_HOME}/bin/oxfetch.sh ftp://${MASTER_CITES}/pub/OpenXM/misc/1.2.2/${DISTFILE} ${DISTDIR}; \
		touch .fetch-ntl-done; \
	fi

extract-ntl: fetch-ntl
	-mkdir ${WORK}
	@if [ ! -f .extract-ntl-done ]; then \
		(cd ${WORK}; tar xvzf ${DISTDIR}/${DISTFILE}); \
		touch .extract-ntl-done; \
	fi

patch-ntl: extract-ntl
	@if [ ! -f .patch-ntl-done ]; then \
		touch .patch-ntl-done; \
	fi


build-ntl: patch-ntl
	@if [ ! -f .build-ntl-done ]; then \
		(cd ${WORK}/${DISTFILEDIR}/src; \
		 LD_LIBRARY_PATH=${OpenXM_HOME}/lib; \
		 export LD_LIBRARY_PATH; \
		 ./configure LIBDIR=${NTL_LIBDIR} PREFIX=${PREFIX} ${GMPOPT};	\
		 make); \
		touch .build-ntl-done; \
	fi

install-ntl: build-ntl
	@if [ ! -f .install-ntl-done ]; then \
		(cd ${WORK}/${DISTFILEDIR}/src; make install); \
		touch .install-ntl-done; \
	fi

#########################################
# DEBUG
#########################################
ox_ntl_debug: ntl.cpp ntl.h ntlconv.cpp oxserv.h
	$(CPP) $(CPPFLAGS) -g -D__NTL_DEBUG=1 -L/usr/local/lib -L$(LIBDIR) -L$(NTL_LIBDIR) $(DEFS) ntl.cpp ntlconv.o cmon.o oxserv.o oxstack.o -o $@ -lntl -lgmp -lox -lgc

oxserv_debug: oxserv.h oxserv.c
	$(CC) $(CFLAGS) -g -D__OXSERV_DEBUG=1 -L$(LIBDIR) $(DEFS) oxserv.c oxstack.o -o $@ -lgmp -lox -lgc


