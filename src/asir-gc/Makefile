# $OpenXM$

OpenXM_HOME=../..
PREFIX=${OpenXM_HOME}
DISTDIR=${OpenXM_HOME}/../OpenXM_dist
MASTER_SITE=http://www.math.kobe-u.ac.jp/pub/OpenXM/misc/
# Original. http://www.hboehm.info/gc/gc_source
DISTNAME=gc-7.4.2
DISTFILES=${DISTNAME}.tar.gz libatomic_ops-7.4.0.tar.gz

ASIR_GC = ../../../OpenXM_contrib2/asir-gc

all: configure
	@if [ ! -f .make_done ]; then \
		(cd ${ASIR_GC} ; ${MAKE} all) \
	fi
	@touch .make_done

install: all
	(cd ${ASIR_GC} ; ${MAKE} install)

clean:
	-(cd ${ASIR_GC} ; ${MAKE} clean)
	@rm -f .make_done .configure_done .fetch_done

distclean: clean

fetch:
	@if [ ! -f .fetch_done ]; then \
		for i in ${DISTFILES}; do \
			${OpenXM_HOME}/bin/oxfetch.sh ${MASTER_SITE}$$i ${DISTDIR}; \
		done \
	fi
	@touch .fetch_done

configure: fetch
	@if [ ! -f .configure_done ]; then \
		(cd $(ASIR_GC) ; \
            ./configure --prefix="`cd ../../OpenXM; pwd`") \
	fi
	@touch .configure_done