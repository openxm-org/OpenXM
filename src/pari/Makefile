# $OpenXM: OpenXM/src/pari/Makefile,v 1.3 2000/01/08 18:25:36 maekawa Exp $

PARI = ../../../OpenXM_contrib/pari
CURDIR = ../../OpenXM/src/pari
BINDIR = ../../bin

all: configure
	@if [ ! -f ./.make_done ]; then \
		(cd $(PARI) ; make all) \
	fi
	@touch ./.make_done

install: all
	@if [ ! -f ./.install_done ]; then \
		(cd $(PARI) ; make install install-lib-sta) \
	fi
	@touch ./.install_done

clean:
	@if [ -f ./.make_done ]; then \
		(cd $(PARI) ; make clean) \
	fi
	@rm -f ./.make_done ./.configure_done

distclean:
	rm -rf ../../lib/libpari.* ../../lib/pari
	rm -rf ../../include/pari
	rm -rf $(BINDIR)/gp $(BINDIR)/gp-2.0
	@rm -f ./.install_done

configure:
	@if [ ! -f ./.configure_done ]; then \
		(cd $(PARI) ; ./Configure --prefix=../../../OpenXM) \
	fi
	@touch ./.configure_done
