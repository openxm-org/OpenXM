# $OpenXM: OpenXM/src/phc/Makefile,v 1.6 2000/01/31 05:52:06 noro Exp $

OPENXM_CONTRIB=../../../OpenXM_contrib
PROG=phc6

SRCS=phc6.c

CC=gcc
BINDIR=../../bin

all: $(PROG)

install: all
	if [ "`uname`" = "OSF1" ]; then \
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

phcpack:
	(cd ${OPENXM_CONTRIB}/PHC/Objects/ ; make phc)
	gzip -c -9 /tmp/phc | uuencode phc.gz > ${BINDIR}/fbsd-phcv2p.gz.uuencoded
