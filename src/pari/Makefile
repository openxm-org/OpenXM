# $OpenXM: OpenXM/src/pari/Makefile,v 1.18 2003/11/10 09:12:24 noro Exp $

#PARI = ../../../OpenXM_contrib/pari-2.2
PARI = ../../../OpenXM_contrib/pari
OpenXM_ROOT = ../../../OpenXM

all: configure
	@if [ ! -f ./.make_done ]; then \
		(cd $(PARI) ; make lib-sta);  \
	fi
	@touch ./.make_done

install: install-lib-bin

#install-lib-bin: all install-lib
#	if [ ! -f ./.install-lib-bin_done ]; then \
#	(cd $(PARI)/O*-* ; make install-include) ; \
#	if [ "`uname | sed -e 's/_.*$$//'`" != "CYGWIN" ]; then \
#		(cd $(PARI)/O*-* ; make install-lib-dyn);  \
#		(cd $(PARI)/O*-* ; make install-bin);  \
#		(cd $(PARI)/O*-* ; make install-misc);  \
#		(cd $(PARI)/O*-* ; make install-man);  \
#		(cd $(PARI)/O*-* ; make install-data);  \
#	fi ; \
#	touch ./.install-lib-bin_done ; \
#	fi

install-lib-bin: all install-lib
	if [ ! -f ./.install-lib-bin_done ]; then \
	(cd $(PARI)/O*-* ; make install-include) ; \
	touch ./.install-lib-bin_done ; \
	fi


install-doc: all 
	(cd $(PARI) ; make doc)
	@if [ ! -d $(OpenXM_ROOT)/doc/pari ]; then \
	   mkdir $(OpenXM_ROOT)/doc/pari ;\
	fi
	-cp $(PARI)/doc/users.dvi  $(OpenXM_ROOT)/doc/pari/users.dvi
	-cp $(PARI)/doc/tutorial.dvi  $(OpenXM_ROOT)/doc/pari/tutorial.dvi

clean: patch-clean
	@if [ -f ./.make_done ]; then \
		(cd $(PARI) ; make clean) \
	fi
	@rm -f ./.*_done

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
	if [ ! -f ./.install-lib_done ]; then \
	(cd $(PARI) ; make install-lib-sta) ; \
	ranlib ../../lib/libpari.a ; \
	touch ./.install-lib_done ; \
	fi
