# $OpenXM: OpenXM/src/phc/Makefile,v 1.7 2000/10/31 06:09:19 maekawa Exp $

OPENXM_CONTRIB=../../../OpenXM_contrib
PROG=phc6

SRCS=phc6.c

#CC=gcc
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
