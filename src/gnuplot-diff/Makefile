# $OpenXM: OpenXM/src/gnuplot-diff/Makefile,v 1.10 1999/11/27 20:37:07 maekawa Exp $

GNUPLOT = ../../../OpenXM_contrib/gnuplot
CURDIR = ../../OpenXM/src/gnuplot-diff
BINDIR = ../../bin

all: patch
	@if [ ! -f ./.make_done ]; then \
		(cd $(GNUPLOT) ; ./configure --with-x ; make) \
	fi
	@touch ./.make_done

install: all
	install -c -s $(GNUPLOT)/gnuplot $(BINDIR)/gnuplot4ox
	install -c -s $(GNUPLOT)/gnuplot_x11 $(BINDIR)/gnuplot_x11

clean: patch-clean
	@if [ -f ./.make_done ]; then \
		(cd $(GNUPLOT) ; make distclean) \
	fi
	rm -f $(BINDIR)/gnuplot4ox $(BINDIR)/gnuplot_x11
	@rm -f ./.make_done

patch:
	@if [ ! -f ./.patch_done ]; then \
		(cd $(GNUPLOT) ; patch < $(CURDIR)/plot.c.diff) \
	fi
	@touch ./.patch_done

patch-clean:
	@if [ -f ./.patch_done ]; then \
		(cd $(GNUPLOT) ; patch -R < $(CURDIR)/plot.c.diff) \
	fi
	@rm -f ./.patch_done
