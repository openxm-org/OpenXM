## $OpenXM: OpenXM/src/Makefile,v 1.55 2000/01/09 20:13:24 maekawa Exp $

OPENXM_CONTRIB=../../OpenXM_contrib
LIBDIR=../lib
BINDIR=../bin

all: configure all-asir all-gnuplot all-k097 all-kan96xx all-kxx \
     all-pari all-phc all-openmath all-oxmath all-tigers

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
	 install-kxx install-openmath install-oxmath install-pari \
	 install-phc install-tigers

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

install-openmath: all-openmath

install-oxmath: all-oxmath

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

distclean: clean
	(cd $(BINDIR) ; make distclean)
	(cd asir2000 ; make distclean)
	(cd gnuplot ; make distclean)
	(cd kan96xx ; make distclean)
	(cd pari ; make distclean)
	(cd phc ; make distclean)
	(cd tigers ; make distclean)
	rm -rf $(BINDIR)/d0 $(BINDIR)/k0 $(BINDIR)/ox $(BINDIR)/ox_sm1 \
	       $(BINDIR)/oxlog $(BINDIR)/oxweave $(BINDIR)/sm1 \
	       $(LIBDIR)/k097 $(LIBDIR)/sm1 SSkan bin gc gmp

configure: configure-links configure-bin configure-asir configure-gnuplot \
	   configure-k097 configure-kan96xx configure-kxx configure-oxmath \
	   configure-pari

configure-asir:
	(cd asir2000; make configure)

configure-bin:
	(cd $(BINDIR) ; make configure)

configure-gnuplot:
	(cd gnuplot ; make configure)

configure-k097: install-kan96xx

configure-kan96xx:
	(cd kan96xx ; make configure)

configure-kxx: install-kan96xx
	(cd kxx ; make configure)

configure-links:
	rm -f $(LIBDIR)/k097 $(LIBDIR)/sm1 SSkan bin gc gmp
	(cd $(LIBDIR) ; ln -f -s ../src/k097 k097)
	(cd $(LIBDIR) ; ln -f -s ../src/kan96xx/Doc sm1)
	ln -f -s kan96xx SSkan
	ln -f -s $(BINDIR) bin
	ln -f -s $(OPENXM_CONTRIB)/gc gc
	ln -f -s $(OPENXM_CONTRIB)/gmp gmp

configure-oxmath:
	(cd ox_math; ./configure)

configure-pari:
	(cd pari ; make configure)
