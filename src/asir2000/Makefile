# $OpenXM: OpenXM/src/asir2000/Makefile,v 1.9 2000/01/19 05:17:02 noro Exp $

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

distclean: clean
	@cat BINARIES | xargs -t rm -rf

configure: install-pari
	@if [ ! -f ./.configure_done ]; then \
		(cd $(ASIR) ; ./configure ; xmkmf ; make Makefiles ; /bin/rm -f */.depend) \
	fi
	@touch ./.configure_done

install-pari:
	@if [ ! -f ./.install-pari_done ]; then \
		(cd $(PARI) ; make install) \
	fi
	@touch ./.install-pari_done
