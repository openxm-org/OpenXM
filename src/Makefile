## $OpenXM: OpenXM/src/Makefile,v 1.125 2003/09/02 22:18:39 takayama Exp $

OpenXM_HOME=..
OpenXM_contrib=../../OpenXM_contrib
OpenXM_contrib2=../../OpenXM_contrib2
LIBDIR=../lib
BINDIR=../bin
RCDIR=../rc
COPYRIGHTDIR=../Copyright

all: all-util all-gc all-gmp all-asir all-gnuplot all-k097 all-kan96xx \
     all-kxx all-pari all-phc all-openmath all-oxmath all-tigers all-fep

all-gc: 
	(cd gc; make all)

all-gmp: configure-gmp
	(cd gmp; make all)

all-oxtoolkit: configure-oxtoolkit install-gmp install-gc install-util
	(cd ox_toolkit; make)

all-oxc: configure-oxc install-oxtoolkit
	(cd oxc; make)

all-asir: configure-asir install-pari
	(cd asir2000 ; make all)

all-gnuplot: configure-gnuplot
	-(cd gnuplot ; make all)

all-k097: configure-k097 all-kan96xx all-kxx install-oxtags
	(cd k097 ; make all)

all-kan96xx: configure-kan96xx install-gc install-gmp install-util install-oxtags
	(cd kan96xx ; make all)

all-kxx: configure-kxx all-kan96xx install-util install-oxtags
	(cd kxx ; make all)

all-openmath:
	-@if ../misc/which.pl -s javac ; then \
		(cd OpenMath ; make all) ; \
	else \
		echo -n "Warning: javac (java compiler) is not found " ; \
		echo "in your search path. OpenMath will not be compiled." ; \
	fi

all-oxmath: configure-oxmath install-oxtoolkit
	-@if test -d /usr/local/mathematica ; then \
		(cd ox_math ; make) ; \
	else \
		echo -n "Warning: Mathematica is not found. " ; \
		echo "ox_math will not be compiled." ; \
	fi

all-pari: configure-pari
	(cd pari ; make all)

all-phc:
	(cd phc ; make all)

all-tigers:
	(cd tigers ; make all)

all-fep:
	-(cd ${OpenXM_contrib2}/fep ; make all)


all-misc : install-util
	(cd ../rc ; make repl)
	(cd ../doc ; make all-install)
	(cd ../doc ; make all-spec)

all-rc:
	(cd ../rc ; make )
all-util: 
	(cd util ; make all)

install: install-oxtags install-util install-asir install-gnuplot \
	 install-k097 install-kan96xx \
	 install-kxx install-openmath install-oxmath install-pari \
	 install-phc install-tigers install-asir-doc install-asir-contrib \
	 install-misc install-fep install-uuencoded 

install-oxtags:
	(cd ../doc/OpenXM-web ; make install-exec)

install-util: all-util
	(cd util; make install)

install-gc : all-gc
	(cd gc; make install)


install-gmp : all-gmp
	(cd gmp; make install)

install-asir: all-asir
	(cd asir2000 ; make install) ; \

install-gnuplot: all-gnuplot
	-(cd gnuplot ; make install)

install-k097:  install-kxx all-k097
	(cd k097 ; make install)

install-kan96xx: all-kan96xx
	(cd kan96xx ; make install)

install-kxx: all-kxx
	(cd kxx ; make install)

install-openmath: all-openmath
	-@if ../misc/which.pl -s javac ; then \
		(cd OpenMath ; make install) ; \
	else \
		echo -n "Warning: javac (java compiler) is not found " ; \
		echo "in your search path. OpenMath will not be installed." ; \
	fi

install-oxtoolkit: all-oxtoolkit
	(cd ox_toolkit; make install)

install-oxc: all-oxc
	(cd oxc; make install)

install-oxmath: all-oxmath 
	-@if test -d /usr/local/mathematica ; then \
		(cd ox_math ; make install) ; \
	else \
		echo -n "Warning: Mathematica is not found. " ; \
		echo "ox_math will not be installed." ; \
	fi
	-(cd ox_math/documents ; make install)
	-(cd ox_math/lib ; make install)

install-pari: all-pari
	(cd pari ; make install)

install-phc: all-phc
	(cd phc ; make install)

install-tigers: all-tigers
	(cd tigers ; make install)

install-asir-contrib: install-asir install-kxx install-util
	(cd asir-contrib ; make install)

install-asir-doc: install-asir
	-(cd asir-doc ; make install)

install-misc:
	(cd ../rc ; make install.man)

install-fep: all-fep
	-(cd ${OpenXM_contrib2}/fep ; make PREFIX=../../OpenXM install)

install-rc: all-rc
	(cd ../rc ; make install)

install-uuencoded:
	(cd uuencoded ; make install)

clean:
	-rm ./.configure-links-done
	-rm ${OpenXM_contrib}/gmp/.gmp_*
	-rm ${OpenXM_contrib}/gc/.gc_*
	-(cd ${OpenXM_contrib}/gmp ; make clean)
	-(cd ${OpenXM_contrib}/gc ; make clean ; /bin/rm -f *.a)
	-(cd asir2000 ; make clean)
	-(cd gnuplot ; make clean)
	-(cd k097 ; make clean)
	-(cd kan96xx ; make clean)
	-(cd kxx ; make clean)
	-(cd OpenMath ; make clean)
	-(cd ox_math ; make clean)
	-(cd pari ; make clean)
	-(cd phc ; make clean)
	-(cd tigers ; make clean)
	-(cd asir-doc ; make clean)
	-(cd asir-contrib ; make clean)
	-(cd ${OpenXM_contrib2}/fep ; make clean)
	-(cd util ; make clean)

distclean: clean
	(cd uuencoded ; make distclean)
	-(cd ${OpenXM_contrib}/gmp ; make distclean)
	(cd ${OpenXM_contrib}/gc ; make clean ; /bin/rm -f *.a)
	(cd asir2000 ; make distclean)
	(cd gnuplot ; make distclean)
	(cd kan96xx ; make distclean)
	-(cd kxx ; make distclean)
	(cd OpenMath ; make distclean)
	(cd pari ; make distclean)
	(cd phc ; make distclean)
	(cd tigers ; make distclean)
	-(cd ox_toolkit ; make distclean)
	-(cd ox_math ; make distclean)
	rm -rf $(BINDIR)/d0 $(BINDIR)/k0 $(BINDIR)/ox $(BINDIR)/ox_sm1 \
	       $(BINDIR)/oxlog $(BINDIR)/oxweave $(BINDIR)/sm1 \
	       $(LIBDIR)/k097 $(LIBDIR)/sm1 SSkan bin gc gmp

configure: configure-asir \
	   configure-gnuplot \
	   configure-k097 configure-kan96xx configure-kxx configure-oxmath \
	   configure-pari configure-gmp

configure-gmp: 
	(cd gmp; make configure)

configure-asir: 
	(cd asir2000 ; make configure) 

configure-gnuplot:
	-(cd gnuplot ; make configure)

configure-k097:
	(cd k097 ; ./make-configure)	

configure-kan96xx:
	(cd kan96xx ; make configure)

configure-kxx: 
	( cd kxx ; ./make-configure) 

configure-oxtoolkit:
	(cd ox_toolkit; ./make-configure)

configure-oxc: 
	(cd oxc; ./make-configure)

configure-oxmath:
	(cd ox_math; ./make-configure)

configure-pari:
	(cd pari ; make configure)

generate-source-distribution : generate-source-distribution-kxx \
                               generate-source-distribution-ox_toolkit

generate-source-distribution-kxx :
	(cd kxx ; rm -f .configure_done Makefile )

generate-source-distribution-ox_toolkit :
	(cd ox_toolkit ; rm -f Makefile ; autoconf)

clean-rc :
	(cd $(RCDIR) ; make clean )
all-Copyright :
	(cd $(COPYRIGHTDIR) ; make copy-copyright )

post-install-correction: install
# I think that we no longer need post-install-correction-for-asir
# post-install-corrections-other
	(cd ../rc ; make install.man)

post-install-correction-for-asir:
	(cd ../bin ; rm -f asir ox_asir ) 
	cp ../lib/asir/asir ../bin/asir
	(cd ../bin ; ln -s ./asir ox_asir)
	(cd ../lib/asir ; rm -f asir ; ln -s ../../bin/asir asir)
	(cd ../lib/asir ; rm -f ox_asir ox_launch ox_plot help ; ln -s ./asir ox_asir ; ln -s ./asir ox_launch ; ln -s ./asir ox_plot ; ln -s ./help-eg help)
	-(cd asir-doc ; make install-man-pages)

install-all-asir-lib:
	(cd asir2000 ; make install-lib-small)

binary-dist : all-Copyright clean-rc all all-misc post-install-correction install-all-asir-lib

## Targets for debugging
install-for-debug:
	(cd kan96xx; make install-for-debug)
	(cd kxx; make install-for-debug)
	(cd k097 ; make install-for-debug)
	(cd ../bin ; rm -f asir ; ln -s ../../OpenXM_contrib2/asir2000/asir .)


##  Sub binary packages
##  1: asir: asir only.   target:  all-asir install-asir install-asir-doc

##  2: kan-ox: k0 and sm1 with ox_asir.  target: all-kan-ox install-kan-ox
configure-all-kan-ox :  configure-links configure-include  configure-asir \
                       configure-kan96xx configure-kxx configure-k097 \
                       configure-oxc
all-kan-ox:  configure-all-kan-ox all-asir all-kan96xx all-kxx all-k097 all-oxc
install-kan-ox: all-kan-ox  install-asir install-kan96xx install-kxx \
               install-k097 install-oxc install-asir-doc \
               post-install-kan-ox
post-install-kan-ox:
	(cd ../bin ; /bin/rm -f ox_asir; ln -s ./asir ox_asir)

