## $OpenXM: OpenXM/src/Makefile,v 1.188 2020/03/05 00:29:44 takayama Exp $

OpenXM_HOME=..
OpenXM_contrib2=../../OpenXM_contrib2
LIBDIR=../lib
BINDIR=../bin
DOCDIR=../doc
RCDIR=../rc
SHAREDIR=../share
COPYRIGHTDIR=../Copyright

all: all-util all-gc all-asirgc all-gmp all-mpfr all-mpc all-mpfi all-asir all-asir2000 all-gnuplot all-k097 all-kan96xx \
     all-kxx all-pari all-phc all-openmath all-oxmath all-tigers all-fep all-phcpack all-oxcdd \
	all-oxmgraph all-pcc all-oxpari

all-editline: install-util 
	(cd editline; ${MAKE} all)

all-gc: install-util 
	(cd gc; ${MAKE} all)

all-asirgc: install-util 
	(cd asir-gc; ${MAKE} all)

all-gmp: configure-gmp
	(cd gmp; ${MAKE} all)

all-mpfr: configure-mpfr
	(cd mpfr; ${MAKE} all)

all-mpc: configure-mpc
	(cd mpc; ${MAKE} all)

all-mpfi: configure-mpfi
	(cd mpfi; ${MAKE} all)

all-oxtoolkit: configure-oxtoolkit install-gmp install-mpfr install-mpc install-gc install-util install-oxtags
	(cd ox_toolkit; ${MAKE} all)

all-oxc: configure-oxc install-oxtoolkit 
	-(cd oxc; ${MAKE} all)

all-asir: configure-asir install-pari install-editline install-asirgc install-gmp install-mpfr install-mpc install-mpfi
	(cd asir2018; ${MAKE} all)

all-asir2000: configure-asir2000 install-pari install-gmp install-mpfr install-mpc install-mpfi
	(cd asir2000; ${MAKE} all)

all-gnuplot: configure-gnuplot
	-(cd gnuplot; ${MAKE} all)

all-k097: configure-k097 all-kan96xx all-kxx install-oxtags
	(cd k097; ${MAKE} all)

all-kan96xx: configure-kan96xx install-gc install-gmp install-util install-oxtags
	(cd kan96xx; ${MAKE} all)

all-kxx: configure-kxx all-kan96xx install-util install-oxtags
	(cd kxx; ${MAKE} all)

all-openmath:
	-@if ../misc/which.pl -s javac; then \
		(cd OpenMath; ${MAKE} all); \
	else \
		echo -n "Warning: javac (java compiler) is not found "; \
		echo "in your search path. OpenMath will not be compiled."; \
	fi

all-oxmath: configure-oxmath install-oxtoolkit install-gc
	-@if test -d /usr/local/mathematica; then \
		(cd ox_math; ${MAKE} all); \
	else \
		echo -n "Warning: Mathematica is not found. "; \
		echo "ox_math will not be compiled."; \
	fi

all-pari: configure-pari
	(cd pari; ${MAKE} PERL_USE_UNSAFE_INC=1 all)

all-phc:
	(cd phc; ${MAKE} all)

all-phcpack:
	-@if ../misc/which.pl -s gnatmake; then \
		(cd phcpack; ${MAKE} all) ; \
	fi

all-tigers:
	(cd tigers; ${MAKE} all)

all-fep:
	-(cd ${OpenXM_contrib2}/fep; ${MAKE} all)


all-misc: install-util
	(cd ${RCDIR}; ${MAKE} repl)
	(cd ../doc; ${MAKE} all-install all-spec)

all-rc:
	(cd ${RCDIR}; ${MAKE})

all-util: all-util-bin all-util-lib

all-util-bin: configure-util
	(cd util; ${MAKE} all-bin)

all-util-lib: configure-util
	(cd util; ${MAKE} all-lib)

all-oxcdd: install-oxtoolkit
	-(cd ox_cdd; ${MAKE} all)

all-oxpari: configure-oxpari
	-(cd ox_pari; ${MAKE} all)

all-oxmgraph: configure-oxmgraph
	-(cd oxmgraph; ${MAKE} all)
all-pcc: configure-pcc
	-(cd pcc; ${MAKE} all)

all-document-asir-contrib: configure-document-asir-contrib
	(cd asir-contrib/packages/doc; ${MAKE} all)

install: install-oxtags install-util install-asir install-asir2000 install-gnuplot \
	 install-k097 install-kan96xx \
	 install-kxx install-openmath install-oxmath install-pari \
	 install-phc install-tigers install-asir-doc install-asir-contrib \
	 install-misc install-fep install-phcpack install-oxcdd \
     install-oxmgraph install-pcc install-oxpari \
	install-asir-mode

install-oxtags:
	(cd ../doc/OpenXM-web; ${MAKE} install-exec)

install-util-bin: all-util-bin
	(cd util; ${MAKE} install-bin)

install-util-lib: all-util-lib
	(cd util; ${MAKE} install-lib)

install-util: install-util-bin install-util-lib

install-editline: all-editline
	(cd editline; ${MAKE} install)

install-gc: all-gc
	(cd gc; ${MAKE} install)

install-asirgc: all-asirgc
	(cd asir-gc; ${MAKE} install)

install-gmp: all-gmp
	(cd gmp; ${MAKE} install)

install-mpfr: all-mpfr
	(cd mpfr; ${MAKE} install)

install-mpc: all-mpc
	(cd mpc; ${MAKE} install)

install-mpfi: all-mpfi
	(cd mpfi; ${MAKE} install)

install-asir: all-asir
	-(cd asir2018; ${MAKE} install)

install-asir2000: all-asir2000
	-(cd asir2000; ${MAKE} install)

install-gnuplot: all-gnuplot
	-(cd gnuplot; ${MAKE} install)

install-k097:  install-kxx all-k097 install-oxc
	(cd k097; ${MAKE} install)

install-kan96xx: all-kan96xx
	(cd kan96xx; ${MAKE} install)

install-kxx: all-kxx
	(cd kxx; ${MAKE} install)

install-openmath: all-openmath
	-@if ../misc/which.pl -s javac; then \
		(cd OpenMath; ${MAKE} install); \
	else \
		echo -n "Warning: javac (java compiler) is not found "; \
		echo "in your search path. OpenMath will not be installed."; \
	fi

install-oxtoolkit: all-oxtoolkit
	(cd ox_toolkit; ${MAKE} install)

install-oxc: all-oxc
	-(cd oxc; ${MAKE} install)

install-oxmath: all-oxmath 
	-@if test -d /usr/local/mathematica; then \
		(cd ox_math; ${MAKE} install); \
	else \
		echo -n "Warning: Mathematica is not found. "; \
		echo "ox_math will not be installed."; \
	fi
	-(cd ox_math/documents; ${MAKE} install)
	-(cd ox_math/lib; ${MAKE} install)

install-pari: all-pari
	(cd pari; ${MAKE} install)

install-phc: all-phc
	(cd phc; ${MAKE} install)

install-phcpack: all-phcpack
	-@if ../misc/which.pl gnatmake; then \
		(cd phcpack; ${MAKE} install) ; \
	fi

install-tigers: all-tigers
	(cd tigers; ${MAKE} install)

install-asir-contrib: configure-asir-contrib install-asir install-util \
                      install-asir-contrib-oxservers
	(cd asir-contrib; ${MAKE} install)

install-asir-contrib-oxservers: install-asir install-util \
                install-k097 install-kxx install-openmath install-oxmath \
                install-phc install-tigers install-gnuplot

install-asir-doc: configure-asir-doc
	-(cd asir-doc; ${MAKE} install)

install-misc:
	(cd ${RCDIR}; ${MAKE} install-man)

install-fep: all-fep
	-(cd ${OpenXM_contrib2}/fep; ${MAKE} PREFIX=../../OpenXM install)

install-rc: all-rc
	(cd ${RCDIR}; ${MAKE} install)

install-asir-port:
	(cd asir-port; ${MAKE} install)

install-oxcdd: all-oxcdd
	-(cd ox_cdd; ${MAKE} install)

install-oxpari: all-oxpari
	-(cd ox_pari; ${MAKE} install)

install-oxmgraph: all-oxmgraph
	-(cd oxmgraph; ${MAKE} install)

install-pcc: all-pcc
	-(cd pcc; ${MAKE} install)

install-asir-mode:
	-(cd asir-mode; ${MAKE} install)

clean:
	-rm ./.configure-links-done
	-@for i in OpenMath asir-contrib asir-doc asir2000 asir2018 asir-gc gc gmp gnuplot \
	           k097 kan96xx kxx ox_math ox_toolkit oxc pari phc phcpack tigers \
               oxmgraph pcc mpfr mpfi ox_cdd ox_pari \
	           util ${OpenXM_contrib2}/fep ; do \
		(cd $$i; ${MAKE} clean); \
	done

distclean: clean
	-@for i in OpenMath asir2000 asir2018 asir-gc gc gmp gnuplot kan96xx kxx ox_math \
	           ox_toolkit oxc pari phc phcpack tigers oxmgraph pcc \
                   asir-doc k097 ox_cdd util mpc mpfr mpfi ox_pari ; do \
		(cd $$i; ${MAKE} distclean); \
	done
	-(cd ${BINDIR} && rm -f  d0 k0 ox ox_sm1 oxlog oxweave sm1)
	-(cd ${LIBDIR} && rm -rf k097 sm1)

configure: configure-util configure-asir \
	   configure-gnuplot \
	   configure-k097 configure-kan96xx configure-kxx configure-oxmath \
	   configure-pari configure-gmp configure-asir-doc configure-asir-contrib \
       configure-oxmgraph configure-pcc
configure-static: configure
	make configure-asir-static
	make configure-oxpari-static
	make configure-oxc-static

configure-util:
	(cd util; ./make-configure)

configure-gmp: install-util
	(cd gmp; ${MAKE} configure)

configure-mpfr: install-util install-gmp
	(cd mpfr; ${MAKE} configure)

configure-mpc: install-util install-gmp install-mpfr
	(cd mpc; ${MAKE} configure)

configure-mpfi: install-util install-gmp install-mpfr
	(cd mpfi; ${MAKE} configure)

configure-asir: install-util 
	(cd asir2018; ${MAKE} configure) 

configure-asir2000: install-util 
	(cd asir2000; ${MAKE} configure) 

configure-asir-static: install-util 
	(cd asir2018; rm -f .configure_done; ${MAKE} configure-static) 

configure-asir-contrib: 
	-(cd asir-contrib; ./make-configure) 

configure-asir-doc: 
	-(cd asir-doc; ./make-configure) 

configure-gnuplot:
	-(cd gnuplot; ${MAKE} configure)

configure-k097:
	(cd k097; ./make-configure)	

configure-kan96xx:
	(cd kan96xx; ./make-configure)

configure-kxx: 
	(cd kxx; ./make-configure) 

configure-oxtoolkit:
	(cd ox_toolkit; ./make-configure)

configure-oxpari: install-oxtoolkit install-pari
	(cd ox_pari; ./make-configure)
configure-oxpari-static: install-oxtoolkit install-pari
	(cd ox_pari; rm -f .configure_done ; ./make-configure --disable-shared)

configure-oxc: 
	(cd oxc; ./make-configure)
configure-oxc-static: 
	(cd oxc; rm -f .configure_done ; ./make-configure --disable-shared)

configure-oxmath:
	-@if test -d /usr/local/mathematica; then \
		(cd ox_math; ./make-configure); \
	else \
		echo -n "Warning: Mathematica is not found. "; \
		echo "ox_math will not be compiled."; \
	fi

configure-pari: install-util
	(cd pari; ${MAKE} configure)

configure-oxmgraph: 
	-(cd oxmgraph; ./make-configure)

configure-pcc:
	-(cd pcc; ${MAKE} configure)

configure-document-asir-contrib: install-util-bin install-asir-doc
	(cd asir-contrib/packages/doc; ./make-configure)

clean-rc:
	(cd $(RCDIR); ${MAKE} clean)

all-Copyright:
	(cd $(COPYRIGHTDIR); ${MAKE} all)

install-document: install-document-kan96xx install-document-asir-contrib

install-document-kan96xx: install-kan96xx
	(cd kan96xx; ${MAKE} install-document)

install-document-asir-contrib: all-document-asir-contrib
	(cd asir-contrib/packages/doc; ${MAKE} install ; ${MAKE} install-document)
## Targets for debugging
install-for-debug:
	(cd kan96xx; ${MAKE} install-for-debug)
	(cd kxx; ${MAKE} install-for-debug)
	(cd k097; ${MAKE} install-for-debug)
	(cd ${BINDIR}; rm -f asir; ln -s ../../OpenXM_contrib2/asir2018/.libs/asir .)
	(cd ${BINDIR}; rm -f ox_pari; ln -s ../../OpenXM/src/ox_pari/ox_pari .)

fetch:
	-@for i in gc gmp gnuplot pari tigers; do \
		(cd $$i && ${MAKE} fetch); \
	done
