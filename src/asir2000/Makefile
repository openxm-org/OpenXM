# $OpenXM: OpenXM/src/asir2000/Makefile,v 1.38 2019/03/26 08:07:18 ohara Exp $

OpenXM_HOME=../..
PREFIX=${OpenXM_HOME}
DISTDIR=${OpenXM_HOME}/../OpenXM_dist
MASTER_SITE=https://www.math.kobe-u.ac.jp/pub/OpenXM/misc/
# Original. http://www.hboehm.info/gc/gc_source
DISTNAME=gc-7.4.18
DISTFILES=${DISTNAME}.tar.gz libatomic_ops-7.4.14.tar.gz

ASIR = ../../../OpenXM_contrib2/asir2000
PARI = ../pari

all: configure install-pari
	@if [ ! -f ./.make_done ]; then \
		(cd ${ASIR} ; ${MAKE} all) \
	fi
	@touch ./.make_done

all-nox: configure-nox install-pari
	@if [ ! -f ./.make_done ]; then \
		(cd ${ASIR} ; ${MAKE} all) \
	fi
	@touch ./.make_done

install: all
	(cd ${ASIR} ; ${MAKE} install-openxm)
#	cp -p asir2000 ${PREFIX}/bin/

clean:
	(cd ${ASIR} ; ${MAKE} clean)
	@rm -f ./.make_* ./.configure_* ./.install-* ./.fetch*

distclean: clean
	@cat BINARIES | xargs -t rm -rf

fetch:
	@if [ ! -f .fetch_done ]; then \
		for i in ${DISTFILES}; do \
			${OpenXM_HOME}/bin/oxfetch.sh ${MASTER_SITE}$$i ${DISTDIR}; \
		done \
	fi
	@touch .fetch_done

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
