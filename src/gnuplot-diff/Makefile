# $OpenXM$

GNUPLOT = ../gnuplot
BINDIR = ../../bin

all :
	(cd $(GNUPLOT) ; patch < ../gnuplot-diff/plot.c.diff)
	(cd $(GNUPLOT) ; ./configure --with-x ; make )
	/bin/rm -f $(BINDIR)/gnuplot4ox
	mv $(GNUPLOT)/gnuplot $(BINDIR)/gnuplot4ox
	mv $(GNUPLOT)/gnuplot_x11 $(BINDIR)/gnuplot_x11

clean :
	(cd $(GNUPLOT) ; mv ./plot.c.orig plot.c)
	(cd $(GNUPLOT) ; make distclean )
	/bin/rm -f $(BINDIR)/gnuplot4ox $(BINDIR)/gnuplot_x11
