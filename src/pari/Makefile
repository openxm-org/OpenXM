# $OpenXM: OpenXM/src/pari/Makefile,v 1.6 2000/01/08 19:10:25 maekawa Exp $

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
	@cat BINARIES | xargs -t rm -rf

configure:
	@if [ ! -f ./.configure_done ]; then \
		(cd $(PARI) ; ./Configure --prefix=../../../OpenXM) \
	fi
	@touch ./.configure_done
