# $OpenXM: OpenXM/src/gnuplot-diff/Makefile,v 1.5 1999/11/17 07:46:07 maekawa Exp $

GNUPLOT = ../gnuplot
BINDIR = ../../bin

all : patch
	(cd $(GNUPLOT) ; ./configure --with-x ; make )
	touch ./.make_done

install:
	cp $(GNUPLOT)/gnuplot $(BINDIR)/gnuplot4ox
	cp $(GNUPLOT)/gnuplot_x11 $(BINDIR)/gnuplot_x11

clean : patch-clean
	@if [ -f ./.make_done ]; then \
		(cd $(GNUPLOT) ; make distclean ) \
	fi
	rm -f $(BINDIR)/gnuplot4ox $(BINDIR)/gnuplot_x11 ./.make_done

patch:
	@if [ ! -f ./.patch_done ]; then \
		(cd $(GNUPLOT) ; patch < ../gnuplot-diff/plot.c.diff) \
	fi
	touch ./.patch_done

patch-clean:
	@if [ -f ./.patch_done ]; then \
		(cd $(GNUPLOT) ; patch -R < ../gnuplot-diff/plot.c.diff) \
	fi
	rm -f ./.patch_done
