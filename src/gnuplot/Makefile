# $OpenXM: OpenXM/src/gnuplot/Makefile,v 1.4 2000/01/19 07:07:51 takayama Exp $

GNUPLOT = ../../../OpenXM_contrib/gnuplot
CURDIR = ../../OpenXM/src/gnuplot
BINDIR = ../../bin

all: configure
	@if [ ! -f ./.make_done ]; then \
	   if test `uname` = "SunOS" ; \
	   then \
	    (cd $(GNUPLOT) ; make all LDFLAGS=-lsocket -lnsl ) ; \
	   else \
		(cd $(GNUPLOT) ; make all) ; \
	   fi ; \
	fi
	@touch ./.make_done

install: all
	if test `uname` = "OSF1" ; then \
		installbsd -c -s $(GNUPLOT)/gnuplot $(BINDIR)/gnuplot4ox ; \
		installbsd -c -s $(GNUPLOT)/gnuplot_x11 $(BINDIR)/gnuplot_x11 ; \
	else \
		install -c -s $(GNUPLOT)/gnuplot $(BINDIR)/gnuplot4ox ; \
		install -c -s $(GNUPLOT)/gnuplot_x11 $(BINDIR)/gnuplot_x11; \
	fi

clean: patch-clean
	@if [ -f ./.make_done ]; then \
		(cd $(GNUPLOT) ; make distclean) \
	fi
	@rm -f ./.make_done ./.configure_done

distclean:
	rm -f $(BINDIR)/gnuplot4ox $(BINDIR)/gnuplot_x11

configure: patch
	@if [ ! -f ./.configure_done ]; then \
		(cd $(GNUPLOT) ; ./configure --with-x) \
	fi
	@touch ./.configure_done

patch:
	-@if [ ! -f ./.patch_done ]; then \
		(cd $(GNUPLOT) ; patch < $(CURDIR)/plot.c.diff) \
	fi
	@touch ./.patch_done

patch-clean:
	@if [ -f ./.patch_done ]; then \
		(cd $(GNUPLOT) ; patch -R < $(CURDIR)/plot.c.diff) \
	fi
	@rm -f ./.patch_done
