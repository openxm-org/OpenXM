# $OpenXM: OpenXM/src/asir2000/Makefile,v 1.6 2000/01/09 20:39:49 maekawa Exp $

ASIR = ../../../OpenXM_contrib2/asir2000
PARI = ../pari

all: configure
	@if [ ! -f ./.make_done ]; then \
		(cd $(ASIR) ; make all) \
	fi
	@touch ./.make_done

install: all
	(cd $(ASIR) ; make install ; make install-lib)

clean:
	(cd $(ASIR) ; make clean)
	@rm -f ./.make_done ./.configure_done ./.install-pari_done

distclean:
	@cat BINARIES | xargs -t rm -rf

configure: install-pari
	@if [ ! -f ./.configure_done ]; then \
		(cd $(ASIR) ; ./configure ; xmkmf -a ; make depend) \
	fi
	@touch ./.configure_done

install-pari:
	@if [ ! -f ./.install-pari_done ]; then \
		(cd $(PARI) ; make install) \
	fi
	@touch ./.install-pari_done
