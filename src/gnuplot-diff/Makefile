# $OpenXM: OpenXM/src/gnuplot-diff/Makefile,v 1.6 1999/11/19 16:39:29 maekawa Exp $

GNUPLOT = ../gnuplot
CURDIR = ../gnuplot-diff
BINDIR = ../../bin

all : patch
	@if [ ! -f $(CURDIR)/.make_done ]; then \
		(cd $(GNUPLOT) ; ./configure --with-x ; make) \
	fi
	@touch $(CURDIR)/.make_done

install: all
	cp $(GNUPLOT)/gnuplot $(BINDIR)/gnuplot4ox
	cp $(GNUPLOT)/gnuplot_x11 $(BINDIR)/gnuplot_x11

clean : patch-clean
	@if [ -f $(CURDIR).make_done ]; then \
		(cd $(GNUPLOT) ; make distclean) \
	fi
	rm -f $(BINDIR)/gnuplot4ox $(BINDIR)/gnuplot_x11
	@rm -f $(CURDIR)/.make_done

patch:
	@if [ ! -f $(CURDIR)/.patch_done ]; then \
		(cd $(GNUPLOT) ; patch < $(CURDIR)/plot.c.diff) \
	fi
	@touch $(CURDIR)/.patch_done

patch-clean:
	@if [ -f $(CURDIR)/.patch_done ]; then \
		(cd $(GNUPLOT) ; patch -R < $(CURDIR)/plot.c.diff) \
	fi
	@rm -f $(CURDIR)/.patch_done
