## Type in make configure and make all to build all
## $OpenXM: OpenXM/src/Makefile,v 1.51 2000/01/09 18:20:14 maekawa Exp $

all: configure all-kan96xx all-kxx all-tigers all-phc all-k097 \
     all-pari all-asir all-gnuplot all-openmath all-oxmath

all-asir: configure-asir
	(cd asir2000 ; make all)

all-gnuplot: configure-gnuplot
	(cd gnuplot ; make all)

all-k097: configure-k097
	(cd k097 ; make all)

all-kan96xx: configure-kan96xx
	(cd kan96xx ; make all)

all-kxx: configure-kxx
	(cd kxx ; make all)

all-openmath:
	@if ../misc/which.pl -s javac ; then \
		(cd OpenMath ; make) ; \
	else \
		echo -n "Warning: javac (java compiler) is not found " ; \
		echo "in your search path. OpenMath will not be compiled." ; \
	fi

all-oxmath: configure-oxmath
	@if test -d /usr/local/mathematica ; then \
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

install: install-asir install-gnuplot install-k097 install-kan96xx \
	 install-kxx install-pari install-phc install-tigers

install-asir: all-asir
	(cd asir2000 ; make install)

install-gnuplot: all-gnuplot
	(cd gnuplot ; make install)

install-k097: all-k097
	(cd k097 ; make install)

install-kan96xx: all-kan96xx
	(cd kan96xx ; make install2)

install-kxx: all-kxx
	(cd kxx ; make install2)

install-pari: all-pari
	(cd pari ; make install)

install-phc: all-phc
	(cd phc ; make install)

install-tigers: all-tigers
	(cd tigers ; make install)

clean:
	(cd asir2000 ; make clean)
	(cd gnuplot ; make clean)
	(cd k097 ; make clean)
	(cd kan96xx ; make clean)
	(cd kxx ; make clean)
#	(cd ox_math ; make clean)
	(cd pari ; make clean)
	(cd phc ; make clean)
	(cd tigers ; make clean)
	/bin/rm -rf bin/sm1 bin/ox bin/oxlog bin/ox_sm1 bin/k0 bin/oxweave bin/fep.linux bin/lin_phcv2p

distclean: clean
	(cd asir2000 ; make distclean)
	(cd gnuplot ; make distclean)
	(cd kan96xx ; make distclean)
	(cd pari ; make distclean)
	(cd phc ; make distclean)
	(cd tigers ; make distclean)

configure: configure-links configure-asir configure-bin configure-gnuplot \
	   configure-k097 configure-kan96xx configure-kxx configure-oxmath \
	   configure-pari

configure-asir:
	(cd asir2000; make configure)

configure-bin:
	(cd ../bin ; ./configure2)

configure-gnuplot:
	(cd gnuplot ; make configure)

configure-k097: install-kan96xx

configure-kan96xx:
	(cd kan96xx ; make configure)

configure-kxx: install-kan96xx
	(cd kxx ; make configure)

configure-links:
	./make-links

configure-oxmath:
	(cd ox_math; ./configure)

configure-pari:
	(cd pari ; make configure)
