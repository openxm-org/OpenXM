# $OpenXM: OpenXM/src/ox_ntl/Makefile,v 1.5 2003/11/12 12:56:05 iwane Exp $
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

CFLAGS=-I$(PREFIX)/include
CPPFLAGS=$(CFLAGS)

PREFIX=$(OpenXM_HOME)

MASTER_CITES=ftp.math.sci.kobe-u.ac.jp

NTL_LIBDIR=$(PREFIX)/lib/NTL
LIBDIR=$(PREFIX)/lib 

DISTDIR=$(PWD)/distfile
DISTFILEDIR=ntl-5.3.1
DISTFILE=ntl-5.3.1.tar.gz
WORK=$(PWD)/work

GMPOPT=NTL_GMP_LIP=on GMP_PREFIX=${OpenXM_HOME} GMP_INCDIR=${OpenXM_HOME}/include GMP_LIBDIR=${OpenXM_HOME}/lib

.SUFFIXES: .cpp .o .c

SRCC=oxserv.c oxstack.c
SRCP=main.cpp ntl.cpp ntlconv.cpp

OBJC=$(SRCC:.c=.o) $(SRCP:.cpp=.o)


all: $(PROG)


$(PROG): $(OBJC) $(LIBDIR)/libox.a $(NTL_LIBDIR)/libntl.a
	$(CPP) $(CPPFLAGS) -L$(LIBDIR) -L$(NTL_LIBDIR) $(OBJC) -o $@ -lntl -lgmp -lox -lgc


ox_ntl_debug: ntl.cpp ntl.h oxstack.o
	$(CPP) $(CPPFLAGS) -g -D__NTL_DEBUG=1 -L/usr/local/lib -L$(LIBDIR) -L$(NTL_LIBDIR) ntl.cpp ntlconv.o oxstack.o -o $@ -lntl -lgmp -lox -lgc

oxserv_debug: oxserv.h oxserv.c
	$(CC) $(CFLAGS) -g -D__OXSERV_DEBUG=1 -L$(LIBDIR) oxserv.c oxstack.o -o $@ -lox -lgc

cpp.exe: cpp.cpp
	$(CPP) $(CPPFLAGS) -g -L/usr/local/lib -L$(LIBDIR) -L$(NTL_LIBDIR) cpp.cpp -o $@ -lntl -lgmp -lox -lgc

oxserv.c: oxserv.h oxstack.h

oxstack.c: oxstack.h

main.cpp: oxserv.h ntl.h oxstack.h

ntl.cpp: ntl.h

.cpp.o:
	$(CPP) $(CPPFLAGS) -c $< -o $@

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

install: all
	$(CP) $(PROG) $(PREFIX)/bin

clean:
	-$(RM) -f $(OBJC) $(PROG) ox_ntl_debug oxserv_debug
	-$(RM) -fr ${WORK}
	@-$(RM) -f .*-ntl-done


pre-fetch-ntl:
	@if [ ! -f .pre-fetch-ntl-done ]; then\
		mkdir -p ${DISTDIR} ${WORK}; \
		touch .pre-fetch-ntl-done; \
	fi

fetch-ntl: pre-fetch-ntl
	@if [ ! -f .fetch-ntl-done ]; then \
		if [ ! -f ${DISTDIR}/${DISTFILE} ]; then \
			(cd ${DISTDIR}; wget --no-directories --passive-ftp ftp://$(MASTER_CITES)/pub/OpenXM/misc/1.2.2/$(DISTFILE)) ; \
		fi ; \
		(cd ${WORK}; tar xvzf ${DISTDIR}/${DISTFILE}); \
		touch .fetch-ntl-done; \
	fi

patch-ntl: fetch-ntl
	@if [ ! -f .patch-ntl-done ]; then \
		touch .patch-ntl-done; \
	fi


build-ntl: patch-ntl
	@if [ ! -f .build-ntl-done ]; then \
		(cd ${WORK}/${DISTFILEDIR}/src; \
		 LD_LIBRARY_PATH=${OpenXM_HOME}/lib; \
		 export LD_LIBRARY_PATH; \
		 ./configure LIBDIR=${NTL_LIBDIR} PREFIX=${PREFIX} ${GMPOPT}; make); \
		touch .build-ntl-done; \
	fi

install-ntl: build-ntl
	@if [ ! -f .install-ntl-done ]; then \
		(cd ${WORK}/${DISTFILEDIR}/src; make install); \
		touch .install-ntl-done; \
	fi

