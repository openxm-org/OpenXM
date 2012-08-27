# $OpenXM: OpenXM/src/asir2000/Makefile,v 1.29 2012/05/30 19:24:33 ohara Exp $

OpenXM_HOME=../..
PREFIX=${OpenXM_HOME}
DISTDIR=${OpenXM_HOME}/../OpenXM_dist
MASTER_SITE=ftp://ftp.math.kobe-u.ac.jp/pub/OpenXM/misc/
# Original. http://www.hpl.hp.com/personal/Hans_Boehm/gc/gc_source/
#DISTNAME=gc-7.2b
DISTNAME=gc6.8
DISTFILES=${DISTNAME}.tar.gz

ASIR = ../../../OpenXM_contrib2/asir2000
PARI = ../pari

all: configure install-pari
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

fetch:
	@if [ ! -f .fetch_done ]; then \
		for i in ${DISTFILES}; do \
			${OpenXM_HOME}/bin/oxfetch.sh ${MASTER_SITE}${DISTFILES} ${DISTDIR}; \
		done \
	fi
	@touch .fetch_done

configure: fetch
	@if [ ! -f ./.configure_done ]; then \
		(cd $(ASIR) ; \
            ./configure --enable-plot --with-pari --prefix="`cd ../../OpenXM; pwd`") \
	fi
	@touch ./.configure_done

install-pari:
	@if [ ! -f ./.install-pari_done ]; then \
		(cd ${PARI} ; ${MAKE} install) \
	fi
	@touch ./.install-pari_done
