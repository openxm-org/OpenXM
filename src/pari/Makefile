# $OpenXM: OpenXM/src/pari/Makefile,v 1.5 2000/01/08 18:41:19 maekawa Exp $

PARI = ../../../OpenXM_contrib/pari
CURDIR = ../../OpenXM/src/pari
BINDIR = ../../bin

all: configure
	@if [ ! -f ./.make_done ]; then \
		(cd $(PARI) ; make all) \
	fi
	@touch ./.make_done

install: all
	(cd $(PARI) ; make install install-lib-sta)

clean:
	@if [ -f ./.make_done ]; then \
		(cd $(PARI) ; make clean) \
	fi
	@rm -f ./.make_done ./.configure_done

distclean:
	rm -rf ../../lib/libpari.* ../../lib/pari
	rm -rf ../../include/pari
	rm -rf $(BINDIR)/gp $(BINDIR)/gp-2.0

configure:
	@if [ ! -f ./.configure_done ]; then \
		(cd $(PARI) ; ./Configure --prefix=../../../OpenXM) \
	fi
	@touch ./.configure_done
