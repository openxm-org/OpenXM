# $OpenXM: OpenXM/src/ox_ntl/Makefile,v 1.2 2003/11/03 22:06:09 ohara Exp $
#
#
#
#
# wget should be installed for install `NTL'
#

PROG=ox_ntl

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

SRCC=oxserv.c
SRCP=main.cpp ntl.cpp

OBJC=$(SRCC:.c=.o) $(SRCP:.cpp=.o)


all: $(PROG)


$(PROG): $(OBJC) $(LIBDIR)/libox.a $(NTL_LIBDIR)/libntl.a
	$(CPP) $(CPPFLAGS) -L$(LIBDIR) -L$(NTL_LIBDIR) $(OBJC) -o $@ -lntl -lgmp -lox -lgc

oxserv.c: oxserv.h

main.cpp: oxserv.h ntl.h

ntl.cpp: ntl.h

.cpp.o:
	$(CPP) $(CPPFLAGS) -c $< -o $@

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

install: all
	$(CP) $(PROG) $(PREFIX)/bin

clean:
	-$(RM) -f $(OBJC) $(PROG)
	-$(RM) -fr ${WORK}
	-$(RM) -f .*-ntl-done


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
		(cd ${WORK}/${DISTFILEDIR}/src; ./configure LIBDIR=${NTL_LIBDIR} PREFIX=${PREFIX} ${GMPOPT}; make); \
		touch .build-ntl-done; \
	fi

install-ntl: build-ntl
	@if [ ! -f .install-ntl-done ]; then \
		(cd ${WORK}/${DISTFILEDIR}/src; make install); \
		touch .install-ntl-done; \
	fi

