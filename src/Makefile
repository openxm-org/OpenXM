## Type in make configure and make all to build all
## $OpenXM: OpenXM/src/Makefile,v 1.45 2000/01/08 20:54:57 maekawa Exp $

all: configure all-kan96xx all-kxx all-tigers all-phc all-k097 \
     all-pari all-asir all-gnuplot all-openmath all-oxmath

all-asir: configure-asir
	(cd asir2000 ; make all ; make install)

all-gnuplot: configure-gnuplot
	(cd gnuplot ; make all ; make install)

all-kan96xx: configure-kan96xx
	(cd kan96xx ; make all ; make install2)

all-kxx: configure-kxx
	(cd kxx ; make all ; make install2)

all-k097:
	(cd k097 ; make all ; make install)

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
	(cd phc ; make all ; make install)

all-tigers:
	(cd tigers ; make all ; make install)

install: install-pari

install-pari: all-pari
	(cd pari ; make install)

clean:
	(cd kan96xx ; make clean)
	(cd kxx ; make clean)
	(cd tigers ; make clean)
	(cd gnuplot ; make clean)
	(cd pari ; make clean)
	(cd phc ; make clean)
	(cd k097 ; make clean)
	(cd asir2000 ; make clean)
#	(cd ox_math ; make clean)
	/bin/rm -rf bin/sm1 bin/ox bin/oxlog bin/ox_sm1 bin/k0 bin/oxweave bin/fep.linux bin/lin_phcv2p

distclean: clean
	(cd asir2000 ; make distclean)
	(cd gnuplot ; make distclean)
	(cd kan96xx ; make distclean)
	(cd pari ; make distclean)
	(cd phc ; make distclean)
	(cd tigers ; make distclean)

configure: configure-links configure-asir configure-bin configure-gnuplot \
	   configure-kan96xx configure-kxx configure-oxmath configure-pari

configure-asir: install-pari
	(cd asir2000; make configure)

configure-bin:
	(cd ../bin ; ./configure2)

configure-gnuplot:
	(cd gnuplot ; make configure)

configure-kan96xx:
	(cd kan96xx ; make configure)

configure-kxx:
	(cd kxx ; make configure)

configure-links:
	./make-links

configure-oxmath:
	(cd ox_math; ./configure)

configure-pari:
	(cd pari ; make configure)
