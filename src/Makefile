## $OpenXM: OpenXM/src/Makefile,v 1.87 2000/03/08 05:31:33 ohara Exp $

OPENXM_CONTRIB=../../OpenXM_contrib
OPENXM_CONTRIB2=../../OpenXM_contrib2
LIBDIR=../lib
BINDIR=../bin
RCDIR=../rc
COPYRIGHTDIR=../Copyright

all: configure all-gc all-gmp all-asir all-gnuplot all-k097 all-kan96xx \
     all-kxx all-pari all-phc all-openmath all-oxmath all-tigers 

all-gc: 
	@if test ! -f gc/.gc_make_done ; \
	then \
		(cd gc ; make all) ; \
	fi 
	@touch gc/.gc_make_done

all-gmp: configure-gmp
	if test -f /usr/lib/libgmp.a ; \
	then \
	  echo "Using /usr/lib/libgmp.a" ; \
	  /bin/rm -rf gmp/libgmp.a ; \
	  ln -s /usr/lib/libgmp.a gmp/libgmp.a ; \
	else \
	 (if test ! -f .gmp_make_done ; then cd gmp ; make ; touch .gmp_make_done ; fi) ; \
	fi

all-oxtoolkit: configure-oxtoolkit install-gmp
	(cd ox_toolkit; make)

all-asir: configure-asir
	@if test -d $(OPENXM_CONTRIB2) ; then \
		(cd asir2000 ; make all) ; \
	else \
		echo -n "Warning: Asir source is not found. " ; \
		echo "asir will not be compiled." ; \
	fi

all-gnuplot: configure-gnuplot
	(cd gnuplot ; make all)

all-k097: configure-k097 all-kan96xx
	(cd k097 ; make all)

all-kan96xx: configure-kan96xx install-gc install-gmp
	(cd kan96xx ; make all)

all-kxx: configure-kxx all-kan96xx
	(cd kxx ; make all)

all-openmath:
	@if ../misc/which.pl -s javac ; then \
		(cd OpenMath ; make all) ; \
	else \
		echo -n "Warning: javac (java compiler) is not found " ; \
		echo "in your search path. OpenMath will not be compiled." ; \
	fi

all-oxmath: configure-oxmath install-oxtoolkit
	@if test -x /usr/local/bin/math ; then \
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

all-misc :
	(cd ../rc ; make repl)
	(cd ../doc ; make all-install)
	(cd ../doc ; make all-spec)

install: install-asir install-gnuplot install-k097 install-kan96xx \
	 install-kxx install-openmath install-oxmath install-pari \
	 install-phc install-tigers install-asir-doc install-asir-contrib \
	 install-misc


install-gc : all-gc
	-@if test ! -f gc/.gc_installed ; \
	then \
		mkdir ../lib ; \
		cp gc/gc.a ../lib/libgc.a ; \
	fi 
	@touch gc/.gc_installed


install-gmp : all-gmp
	-@if test ! -f gmp/.gmp_installed ; \
	then \
		mkdir ../lib ; \
		cp gmp/libgmp.a ../lib ; \
		mkdir ../include ; \
		cp gmp/gmp.h ../include ; \
	fi 
	@touch gmp/.gmp_installed

install-asir: all-asir
	@if test -d $(OPENXM_CONTRIB2) ; then \
		(cd asir2000 ; make install) ; \
	else \
		echo -n "Warning: Asir source is not found. " ; \
		echo "asir will not be installed." ; \
	fi

install-gnuplot: all-gnuplot
	(cd gnuplot ; make install)

install-k097: all-k097
	(cd k097 ; make install)

install-kan96xx: all-kan96xx
	(cd kan96xx ; make install)

install-kxx: all-kxx
	(cd kxx ; make install)

install-openmath: all-openmath
	@if ../misc/which.pl -s javac ; then \
		(cd OpenMath ; make install) ; \
	else \
		echo -n "Warning: javac (java compiler) is not found " ; \
		echo "in your search path. OpenMath will not be installed." ; \
	fi

install-oxtoolkit: all-oxtoolkit
	(cd ox_toolkit; make install)

install-oxmath: all-oxmath
	@if test -x /usr/local/bin/math ; then \
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

install-asir-contrib: install-asir install-kxx
	(cd asir-contrib ; make install)

install-asir-doc: install-asir
	(cd asir-doc ; make install)

install-misc:
	(cd ../rc ; make install.man)

clean:
	-rm $(OPENXM_CONTRIB)/gmp/.gmp_*
	-rm $(OPENXM_CONTRIB)/gc/.gc_*
	-(cd $(OPENXM_CONTRIB)/gmp ; make clean)
	(cd $(OPENXM_CONTRIB)/gc ; make clean ; /bin/rm -f *.a)
	(cd asir2000 ; make clean)
	(cd gnuplot ; make clean)
	(cd k097 ; make clean)
	(cd kan96xx ; make clean)
	-(cd kxx ; make clean)
	(cd OpenMath ; make clean)
	-(cd ox_math ; make clean)
	(cd pari ; make clean)
	(cd phc ; make clean)
	(cd tigers ; make clean)
	(cd asir-doc ; make clean)
	(cd asir-contrib ; make clean)

distclean: clean
	(cd $(BINDIR) ; make distclean)
	-(cd $(OPENXM_CONTRIB)/gmp ; make distclean)
	(cd $(OPENXM_CONTRIB)/gc ; make clean ; /bin/rm -f *.a)
	(cd asir2000 ; make distclean)
	(cd gnuplot ; make distclean)
	(cd kan96xx ; make distclean)
	-(cd kxx ; make distclean)
	(cd OpenMath ; make distclean)
	(cd pari ; make distclean)
	(cd phc ; make distclean)
	(cd tigers ; make distclean)
	rm -rf $(BINDIR)/d0 $(BINDIR)/k0 $(BINDIR)/ox $(BINDIR)/ox_sm1 \
	       $(BINDIR)/oxlog $(BINDIR)/oxweave $(BINDIR)/sm1 \
	       $(LIBDIR)/k097 $(LIBDIR)/sm1 SSkan bin gc gmp

configure: configure-links configure-bin configure-asir \
	   configure-gnuplot \
	   configure-k097 configure-kan96xx configure-kxx configure-oxmath \
	   configure-pari configure-gmp

configure-gmp: configure-links
	@if test -f gmp/.gmp_configured ; \
	then \
	  echo "MESSAGE(kan96xx/Makefile): gmp is already configured. (.gmp_configured) " ;  \
	else \
	  (cd gmp ; ./configure ; touch .gmp_configured) ; \
	fi 

configure-asir:
	@if test -d $(OPENXM_CONTRIB2) ; then \
		(cd asir2000 ; make configure) ; \
	else \
		echo -n "Warning: Asir source is not found. " ; \
		echo "asir will not be compiled." ; \
	fi

configure-bin:
	(cd $(BINDIR) ; make configure)

configure-gnuplot:
	(cd gnuplot ; make configure)

configure-k097: 

configure-kan96xx:
	(cd kan96xx ; make configure)

configure-kxx: 
	( cd kxx ; ./make-configure) 

configure-links:
	rm -f -r $(LIBDIR)/k097 $(LIBDIR)/sm1 SSkan bin gc gmp
	(cd $(LIBDIR) ; ln -f -s ../src/k097 k097)
	(cd $(LIBDIR) ; ln -f -s ../src/kan96xx/Doc sm1)
	ln -f -s kan96xx SSkan
	ln -f -s $(BINDIR) bin
	ln -f -s $(OPENXM_CONTRIB)/gc gc
	ln -f -s $(OPENXM_CONTRIB)/gmp gmp

configure-oxtoolkit:
	(cd ox_toolkit; ./make-configure)

configure-oxmath:
	(cd ox_math; ./make-configure)

configure-pari:
	(cd pari ; make configure)

generate-source-distribution : generate-source-distribution-kxx \
                               generate-source-distribution-ox_toolkit

generate-source-distribution-kxx :
	(cd kxx ; rm -f .configure_done Makefile ; autoconf)
generate-source-distribution-ox_toolkit :
	(cd ox_toolkit ; rm -f Makefile ; autoconf)

clean-rc :
	(cd $(RCDIR) ; make clean )
all-Copyright :
	(cd $(COPYRIGHTDIR) ; make copy-copyright )

post-install-correction: install
	(cd ../bin ; rm -f asir ; ln -s ./ox_asir asir)
	(cd ../lib/asir ; rm -f ox_asir ox_launch ox_plot help ; ln -s ./asir ox_asir ; ln -s ./asir ox_launch ; ln -s ./asir ox_plot ; ln -s ./help-eg help)
	(cd ../rc ; make install.man)

binary-dist : all-Copyright clean-rc all all-misc post-install-correction


