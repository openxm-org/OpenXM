GNUPLOT = ../gnuplot-3.5beta6.347.orig
BINDIR = ../../bin
all :
	/bin/rm -rf $(GNUPLOT)/plot.c
	(cd $(GNUPLOT) ; ln -s ../gnuplot-diff/plot-forox.c plot.c)
	(cd $(GNUPLOT) ; ./configure ;	make )
	/bin/rm -rf $(BINDIR)/gnuplot_forox
	/bin/rm -rf $(BINDIR)/gnuplot_x11
	mv $(GNUPLOT)/gnuplot $(BINDIR)/gnuplot_forox
	mv $(GNUPLOT)/gnuplot_x11 $(BINDIR)/gnuplot_x11

clean :
	/bin/rm -rf *~
	/bin/rm -rf $(GNUPLOT)/plot.c
	(cd $(GNUPLOT) ; ln -s ./plot.c.original plot.c)
	(cd $(GNUPLOT) ; make distclean )
	/bin/rm -rf $(BINDIR)/gnuplot_forox $(BINDIR)/gnuplot_x11

