# $OpenXM: OpenXM/src/pari/Makefile,v 1.13 2001/12/27 07:10:38 noro Exp $

PARI = ../../../OpenXM_contrib/pari-2.2
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

clean: patch-clean
	@if [ -f ./.make_done ]; then \
		(cd $(PARI) ; make clean) \
	fi
	@rm -f ./.make_done ./.configure_done

distclean:
	@cat BINARIES | xargs -t rm -rf

configure: patch
	@if [ ! -f ./.configure_done ]; then \
		(cd $(PARI) ; ./Configure --prefix=../../../OpenXM) \
	fi
	@touch ./.configure_done

patch: 
	@if [ ! -f ./.patch_done ]; then \
		(cd $(PARI)/config ; patch < ../../../OpenXM/src/pari/Makefile.SH.diff) \
	fi
	@touch ./.patch_done

patch-clean: 
	@if [ -f ./.patch_done ]; then \
		(cd $(PARI)/config ; patch -R < ../../../OpenXM/src/pari/Makefile.SH.diff) \
	fi
	@rm -f ./.patch_done

install-lib: all
	(cd $(PARI) ; make install-lib-sta)
	ranlib ../../lib/libpari-2.2.a
