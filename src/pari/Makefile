# $OpenXM: OpenXM/src/pari/Makefile,v 1.10 2000/02/27 07:26:21 takayama Exp $

PARI = ../../../OpenXM_contrib/pari
OpenXM_ROOT = ../../../OpenXM

all: configure
	@if [ ! -f ./.make_done ]; then \
		(cd $(PARI) ; make all) \
	fi
	@touch ./.make_done

install: install-lib-bin

install-lib-bin: all install-lib
	(cd $(PARI)/O*-* ; make install-lib-dyn)
	(cd $(PARI)/O*-* ; make install-include)
	(cd $(PARI)/O*-* ; make install-bin)
	(cd $(PARI)/O*-* ; make install-misc)
	(cd $(PARI)/O*-* ; make install-man)
	(cd $(PARI)/O*-* ; make install-data)

install-doc: all 
	(cd $(PARI) ; make doc)
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
