# $OpenXM: OpenXM/src/asir2018/Makefile,v 1.2 2018/12/22 01:54:58 kondoh Exp $

OpenXM_HOME=../..
PREFIX=${OpenXM_HOME}
DISTDIR=${OpenXM_HOME}/../OpenXM_dist
ASIR = ../../../OpenXM_contrib2/asir2018
PARI = ../pari
ASIR_GC = ../asir-gc

all: configure
	@if [ ! -f ./.make_done ]; then \
		(cd ${ASIR} ; ${MAKE} all) \
	fi
	@touch ./.make_done

all-nox: configure-nox
	@if [ ! -f ./.make_done ]; then \
		(cd ${ASIR} ; ${MAKE} all) \
	fi
	@touch ./.make_done

install: all
	(cd ${ASIR} ; ${MAKE} install-openxm)

clean:
	(cd ${ASIR} ; ${MAKE} clean)
	@rm -f ./.make_* ./.configure_* ./.install-* ./.fetch*

distclean: clean
	@cat BINARIES | xargs -t rm -rf

fetch: install-pari install-asirgc

configure: fetch
	@if [ ! -f ./.configure_done ]; then \
		(cd $(ASIR) ; \
            ./configure --enable-plot --prefix="`cd ../../OpenXM; pwd`") \
	fi
	@touch ./.configure_done

configure-nox: fetch
	@if [ ! -f ./.configure_done ]; then \
		(cd $(ASIR) ; \
            ./configure --disable-plot --prefix="`cd ../../OpenXM; pwd`") \
	fi
	@touch ./.configure_done

configure-static: fetch
	@if [ ! -f ./.configure_done ]; then \
		(cd $(ASIR) ; \
            ./configure --enable-static-gmp --enable-plot --prefix="`cd ../../OpenXM; pwd`") \
	fi
	@touch ./.configure_done

configure-interval: fetch
	@if [ ! -f ./.configure_done ]; then \
		(cd $(ASIR) ; \
            ./configure --enable-plot --enable-interval --prefix="`cd ../../OpenXM; pwd`") \
	fi
	@touch ./.configure_done

install-pari:
	@if [ ! -f ./.install-pari_done ]; then \
		(cd ${PARI} ; ${MAKE} install) \
	fi
	@touch ./.install-pari_done

install-asirgc:
	@if [ ! -f ./.install-asirgc_done ]; then \
		(cd ${ASIR_GC} ; ${MAKE} install) \
	fi
	@touch ./.install-asirgc_done
