# $OpenXM: OpenXM/src/asir2018/Makefile,v 1.5 2022/09/07 03:20:07 ohara Exp $

## Variables
## TARGET: a target type for cross compiling
## INTERVAL: turn on the interval arithmetric
## STATICGMP: statically link the gmp library.
## NOX: not support X11.
## NOFEP: not support readline.

## Example:
## make install TARGET=amd64-unknown-freebsd14.1 NOX=yes

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
		if [ ! -z "${NOX}" ]; then args="--disable-plot"; else args="--enable-plot"; fi; \
		if [ ! -z "${NOFEP}" ];  then args="$${args} --enable-fep"; fi; \
		if [ ! -z "${TARGET}" ]; then args="$${args} --host=${TARGET}"; fi; \
		if [ ! -z "${INTERVAL}" ]; then args="$${args} --enable-interval"; fi; \
		if [ ! -z "${STATICGMP}" ]; then args="$${args} --enable-static-gmp"; fi; \
	    (cd ${ASIR} ; echo ./configure $${args} --prefix="`cd ../../OpenXM; pwd`") > .configure_done ; \
	    (cd ${ASIR} ; ./configure $${args} --prefix="`cd ../../OpenXM; pwd`") \
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
