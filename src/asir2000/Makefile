# $OpenXM: OpenXM/src/asir2000/Makefile,v 1.23 2003/09/02 14:26:07 takayama Exp $

OpenXM_HOME=../..
PREFIX=${OpenXM_HOME}
DISTDIR=${OpenXM_HOME}/../OpenXM_dist
MASTER_SITE=ftp://ftp.math.kobe-u.ac.jp/pub/OpenXM/misc/
# Original. http://www.hpl.hp.com/personal/Hans_Boehm/gc/gc_source/
DISTNAME=gc6.5
DISTFILES=${DISTNAME}.tar.gz

ASIR = ../../../OpenXM_contrib2/asir2000
ASIR_PARI = ../../../OpenXM_contrib2/asir2000-pari
ASIR_SMALL = ../../../OpenXM_contrib2/asir2000-small
PARI = ../pari

all: configure install-pari
	@if [ ! -f ./.make_done ]; then \
		(cd $(ASIR) ; make all) \
	fi
	@touch ./.make_done

with-pari: configure-with-pari
	@if [ ! -f ./.make_with_pari_done ]; then \
		(cd $(ASIR_PARI) ; make all) \
	fi
	@touch ./.make_with_pari_done

small: configure-small
	@if [ ! -f ./.make_small_done ]; then \
		(cd $(ASIR_SMALL) ; make all) \
	fi
	@touch ./.make_small_done

install-xmkmf: all
	(cd $(ASIR) ; make install-bin-lib ; make install-libasir ; make install-libgc ; make install-include )

install: all
	(cd $(ASIR) ; make install-openxm)

install-lib-with-pari: with-pari
	(cd $(ASIR_PARI) ; make install-libasir)

install-lib-small: small
	(cd $(ASIR_SMALL) ; make install-libasir)

install-libasir-gc: configure
	(cd $(ASIR) ; make install-libasir-gc)

clean:
	(cd $(ASIR) ; make clean)
	-@if [ -d $(ASIR_PARI) ]; then (cd $(ASIR_PARI) ; make clean)  fi
	-@if [ -d $(ASIR_SMALL) ]; then (cd $(ASIR_SMALL) ; make clean)  fi
	@rm -f ./.make_* ./.configure_* ./.install-* ./.fetch*

distclean: clean
	@rm -rf $(ASIR_PARI)
	@rm -rf $(ASIR_SMALL)
	@cat BINARIES | xargs -t rm -rf

fetch:
	@if [ ! -f .fetch_done ]; then \
		${OpenXM_HOME}/bin/oxfetch.sh ${MASTER_SITE}${DISTFILES} ${DISTDIR}; \
	fi
	@touch .fetch_done

configure: fetch
	@if [ ! -f ./.configure_done ]; then \
		(cd $(ASIR) ; \
            ./configure --enable-plot --with-pari --prefix="`cd ../../OpenXM; pwd`") \
	fi
	@touch ./.configure_done

configure-with-pari: configure
	-@if [ ! -f ./.configure_with_pari_done ]; then \
		mkdir $(ASIR_PARI) ; \
		(cd $(ASIR_PARI) ; ../../OpenXM/bin/lndir.sh ../asir2000 ; \
			rm -f config.status config.log ; \
            ./configure --with-pari --prefix="`cd ../../OpenXM; pwd`") \
	fi
	@touch ./.configure_with_pari_done

configure-small: configure
	-@if [ ! -f ./.configure_small_done ]; then \
		mkdir $(ASIR_SMALL) ; \
		(cd $(ASIR_SMALL) ; ../../OpenXM/bin/lndir.sh ../asir2000 ; \
			rm -f config.status config.log ; \
            ./configure --prefix="`cd ../../OpenXM; pwd`") \
	fi
	@touch ./.configure_small_done

configure-xmkmf: 
	@if [ ! -f ./.configure_done ]; then \
		(cd $(ASIR) ; ./configure-xmkmf -plot -pari ) \
	fi
	@touch ./.configure_done

configure-xmkmf-with-pari: configure-xmkmf
	-@if [ ! -f ./.configure_with_pari_done ]; then \
		mkdir $(ASIR_PARI) ; \
		(cd $(ASIR_PARI) ; ../../OpenXM/bin/lndir.sh ../asir2000 ; ./configure-xmkmf -pari ) \
	fi
	@touch ./.configure_with_pari_done

configure-xmkmf-small: configure-xmkmf
	-@if [ ! -f ./.configure_small_done ]; then \
		mkdir $(ASIR_SMALL) ; \
		(cd $(ASIR_SMALL) ; ../../OpenXM/bin/lndir.sh ../asir2000 ; ./configure-xmkmf ) \
	fi
	@touch ./.configure_small_done

install-pari:
	@if [ ! -f ./.install-pari_done ]; then \
		(cd $(PARI) ; make install) \
	fi
	@touch ./.install-pari_done
