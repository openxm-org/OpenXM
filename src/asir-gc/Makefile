# $OpenXM: OpenXM/src/asir-gc/Makefile,v 1.4 2021/10/02 05:59:37 ohara Exp $

OpenXM_HOME=../..
PREFIX=${OpenXM_HOME}
DISTDIR=${OpenXM_HOME}/../OpenXM_dist
MASTER_SITE=https://www.math.kobe-u.ac.jp/pub/OpenXM/misc/
# Original. http://www.hboehm.info/gc/gc_source
#           https://github.com/ivmai/bdwgc/wiki/Download
DISTNAME=gc-8.0.6
DISTFILES=${DISTNAME}.tar.gz libatomic_ops-7.6.12.tar.gz

ASIR_GC = ../../../OpenXM_contrib2/asir-gc

all: configure
	@if [ ! -f .make_done ]; then \
		(cd ${ASIR_GC} ; ${MAKE} all) \
	fi
	@touch .make_done

install: all
	@if [ ! -f .install_done ]; then \
		(cd ${ASIR_GC} ; ${MAKE} install); \
	fi
	@touch .install_done

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
		if [ -z "${TARGET_TYPE}" ]; then \
		  (cd $(ASIR_GC) ; \
		  ./configure --prefix="`cd ../../OpenXM; pwd`") \
		else \
		  (cd $(ASIR_GC) ; \
		  ./configure --host=${TARGET_TYPE} --prefix="`cd ../../OpenXM; pwd`") \
		fi ; \
	fi
	@touch .configure_done
