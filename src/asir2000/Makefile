# $OpenXM: OpenXM/src/asir2000/Makefile,v 1.11 2000/01/20 02:33:46 noro Exp $

ASIR = ../../../OpenXM_contrib2/asir2000
PARI = ../pari

all: configure
	@if [ ! -d $(ASIR) ]; then \
	  echo "You do not have the source of asir2000. Skip asir2000." ; \
	  exit 0 ; \
    fi
	@if [ ! -f ./.make_done ]; then \
		(cd $(ASIR) ; make all) \
	fi
	@touch ./.make_done

install: all
	@if [ ! -d $(ASIR) ]; then \
	  echo "You do not have the source of asir2000. Skip asir2000." ; \
	  exit 0 ; \
    fi
	(cd $(ASIR) ; make install ; make install-lib)

clean:
	@if [ ! -d $(ASIR) ]; then \
	  echo "You do not have the source of asir2000. Skip asir2000." ; \
	  exit 0 ; \
    fi
	(cd $(ASIR) ; make clean)
	@rm -f ./.make_done ./.configure_done ./.install-pari_done

distclean: clean
	@cat BINARIES | xargs -t rm -rf

configure: install-pari
	@if [ ! -d $(ASIR) ]; then \
	  echo "You do not have the source of asir2000. Skip asir2000." ; \
	  exit 0 ; \
    fi
	@if [ ! -f ./.configure_done ]; then \
		(cd $(ASIR) ; ./configure ; xmkmf ; make Makefiles ; make depend) \
	fi
	@touch ./.configure_done

install-pari:
	@if [ ! -f ./.install-pari_done ]; then \
		(cd $(PARI) ; make install) \
	fi
	@touch ./.install-pari_done
