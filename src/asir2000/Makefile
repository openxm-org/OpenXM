# $OpenXM: OpenXM/src/asir2000/Makefile,v 1.1 2000/01/08 19:11:10 maekawa Exp $

ASIR = ../../../OpenXM_contrib2/asir2000
BINDIR = ../../bin
LIBDIR = ../../lib

all: configure
	@if [ ! -f ./.make_done ]; then \
		(cd $(ASIR) ; make all) \
	fi
	@touch ./.make_done

install: all
	(cd $(ASIR) ; make install ; make install2)

clean:
	(cd $(ASIR) ; make clean)
	@rm -f ./.make_done ./.configure_done

distclean:
	@cat BINARIES | xargs -t rm -rf

configure:
	@if [ ! -f ./.configure_done ]; then \
		(cd $(ASIR) ; ./configure ; xmkmf -a ; make depend) \
	fi
	@touch ./.configure_done
