# $OpenXM: OpenXM/src/phc/Makefile,v 1.8 2014/03/29 19:50:31 ohara Exp $

OPENXM_CONTRIB=../../../OpenXM_contrib
OpenXM_HOME=../../../OpenXM
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
install-for-debug:
	rm -f ${OpenXM_HOME}/bin/phc6
	(cd ${OpenXM_HOME}/bin ; ln -s ../src/phc/phc6 .)
clean:
	rm -f $(PROG)

distclean:
	rm -f $(BINDIR)/$(PROG)

# -g for lldb
$(PROG): $(SRCS)
	$(CC) -o $(PROG) -g -I${OpenXM_HOME}/include $(SRCS) -L${OpenXM_HOME}/lib -lgc

phcpack:
	(cd ${OPENXM_CONTRIB}/PHC/Objects/ ; make phc)
	gzip -c -9 /tmp/phc | uuencode phc.gz > ${BINDIR}/fbsd-phcv2p.gz.uuencoded
