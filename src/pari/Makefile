# $OpenXM: OpenXM/src/pari/Makefile,v 1.9 2000/01/08 21:47:04 maekawa Exp $

PARI = ../../../OpenXM_contrib/pari
OpenXM_ROOT = ../../../OpenXM

all: configure
	@if [ ! -f ./.make_done ]; then \
		(cd $(PARI) ; make all) \
	fi
	@touch ./.make_done

install: all install-lib
	(cd $(PARI) ; make install)
	@if [ ! -d $(OpenXM_ROOT)/doc/pari ]; then \
	   mkdir $(OpenXM_ROOT)/doc/pari ;\
	fi
	-cp $(OpenXM_ROOT)/lib/pari/users.dvi  $(OpenXM_ROOT)/doc/pari/users.dvi
	-cp $(OpenXM_ROOT)/lib/pari/tutorial.dvi  $(OpenXM_ROOT)/doc/pari/tutorial.dvi

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
	(cd $(PARI) ; make install-lib-sta)
	ranlib ../../lib/libpari.a
