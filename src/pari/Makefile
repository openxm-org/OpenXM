# $OpenXM: OpenXM/src/pari/Makefile,v 1.7 2000/01/08 20:31:47 maekawa Exp $

PARI = ../../../OpenXM_contrib/pari

all: configure
	@if [ ! -f ./.make_done ]; then \
		(cd $(PARI) ; make all) \
	fi
	@touch ./.make_done

install: all install-lib
	(cd $(PARI) ; make install)

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

install-lib: all
	(cd $(PARI) ; make install-lib-sta ; ranlib ../../lib/libpari.a)
