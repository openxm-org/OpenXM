# $OpenXM: OpenXM/src/gnuplot-diff/Makefile,v 1.4 1999/11/16 17:45:30 maekawa Exp $

GNUPLOT = ../gnuplot
BINDIR = ../../bin

all : patch
	(cd $(GNUPLOT) ; ./configure --with-x ; make )
	/bin/rm -f $(BINDIR)/gnuplot4ox
	mv $(GNUPLOT)/gnuplot $(BINDIR)/gnuplot4ox
	mv $(GNUPLOT)/gnuplot_x11 $(BINDIR)/gnuplot_x11

clean : patch-clean
	(cd $(GNUPLOT) ; make distclean )
	/bin/rm -f $(BINDIR)/gnuplot4ox $(BINDIR)/gnuplot_x11

patch:
	(cd $(GNUPLOT) ; patch < ../gnuplot-diff/plot.c.diff)

patch-clean:
	(cd $(GNUPLOT) ; mv ./plot.c.orig plot.c ; rm -f plot.c.rej)
