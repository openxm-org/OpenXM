## Type in make configure and make all to build all
## $OpenXM: OpenXM/src/Makefile,v 1.34 2000/01/07 22:53:41 maekawa Exp $

all: all-kan96xx all-kxx all-tigers all-phc all-k097 \
     all-pari all-asir all-gnuplot all-openmath all-oxmath

all-asir:
	(cd asir2000 ; xmkmf -a ; make ; make install ; make install2)

all-gnuplot:
	(cd gnuplot ; make all ; make install)

all-kan96xx:
	(cd kan96xx ; make all ; make install2)

all-kxx:
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

all-oxmath:
	@if test -d /usr/local/mathematica ; then \
		(cd ox_math ; make) ; \
	else \
		echo -n "Warning: Mathematica is not found. " ; \
		echo "ox_math will not be compiled." ; \
	fi

all-pari:
	(cd pari ; make install ; make install-lib-sta)
	(cd pari ; ranlib ../../OpenXM/lib/libpari.a)

all-phc:
	(cd phc ; make all ; make install)

all-tigers:
	(cd tigers ; make all ; make install)

clean:
	/bin/rm -rf ../*~  *~
	(cd kan96xx ; make clean )
	(cd kxx ; make clean)
	(cd tigers ; make clean)
	(cd gnuplot ; make clean)
	(cd phc ; make clean )
	(cd k097 ; make clean )
	(cd asir2000 ; make clean )
#	(cd ox_math ; make clean )
	/bin/rm -rf bin/sm1 bin/ox bin/oxlog bin/ox_sm1 bin/phc6 bin/tigers bin/k0 bin/oxweave bin/gp-2.0 bin/gphelp bin/fep.linux bin/lin_phcv2p

distclean:
	(cd kan96xx ; make distclean )
	(cd kxx ; make clean)
	(cd tigers ; make clean)
	(cd gnuplot ; make distclean)
	(cd phc ; make clean )
	(cd k097 ; make clean )
	(cd asir2000 ; make clean )
#	(cd ox_math ; make clean )
	/bin/rm -rf bin/sm1 bin/ox bin/oxlog bin/ox_sm1 bin/phc6 bin/tigers bin/k0 bin/oxweave bin/gp-2.0 bin/gphelp bin/fep.linux bin/lin_phcv2p

configure:
	./make-links
	(cd ../bin ; ./configure2 )
	(cd kxx ; make configure )
	(cd kan96xx ; make configure )
	(cd gnuplot ; make configure)
	(cd pari ; ./Configure --prefix=../../../OpenXM)
	(cd asir2000; ./configure)
	(cd ox_math; ./configure)
