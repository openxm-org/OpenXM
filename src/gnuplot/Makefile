# $OpenXM: OpenXM/src/gnuplot/Makefile,v 1.6 2000/02/01 10:23:33 maekawa Exp $

GNUPLOT = ../../../OpenXM_contrib/gnuplot
CURDIR = ../../OpenXM/src/gnuplot
BINDIR = ../../bin

all: configure
	@if [ ! -f $(GNUPLOT)/.make_done ]; then \
		if [ `uname` = "SunOS" ]; then \
			(cd $(GNUPLOT) ; make all LDFLAGS="-lsocket -lnsl" ) ; \
		else \
			(cd $(GNUPLOT) ; make all) ; \
		fi ; \
	fi
	@touch $(GNUPLOT)/.make_done

install: all
	if [ `uname` = "OSF1" ]; then \
		installbsd -c -s $(GNUPLOT)/gnuplot $(BINDIR)/gnuplot4ox ; \
		installbsd -c -s $(GNUPLOT)/gnuplot_x11 $(BINDIR)/gnuplot_x11 ; \
	else \
		install -c -s $(GNUPLOT)/gnuplot $(BINDIR)/gnuplot4ox ; \
		install -c -s $(GNUPLOT)/gnuplot_x11 $(BINDIR)/gnuplot_x11; \
	fi

clean: patch-clean
	@if [ -f $(GNUPLOT)/.make_done ]; then \
		(cd $(GNUPLOT) ; make distclean) \
	fi
	@rm -f $(GNUPLOT)/.make_done $(GNUPLOT)/.configure_done

distclean:
	rm -f $(BINDIR)/gnuplot4ox $(BINDIR)/gnuplot_x11

configure: patch
	@if [ ! -f $(GNUPLOT)/.configure_done ]; then \
		(cd $(GNUPLOT) ; ./configure --with-x) \
	fi
	@touch $(GNUPLOT)/.configure_done

patch:
	-@if [ ! -f $(GNUPLOT)/.patch_done ]; then \
		(cd $(GNUPLOT) ; patch < $(CURDIR)/plot.c.diff) \
	fi
	@touch $(GNUPLOT)/.patch_done

patch-clean:
	@if [ -f $(GNUPLOT)/.patch_done ]; then \
		(cd $(GNUPLOT) ; patch -R < $(CURDIR)/plot.c.diff) \
	fi
	@rm -f $(GNUPLOT)/.patch_done
