## Type in make configure and make all to build all
## $OpenXM: OpenXM/src/Makefile,v 1.17 1999/12/11 12:57:03 takayama Exp $
all : all1 all3  all4 all-pari all-asir all2 all5 all6

all1 :
	(cd kan96xx ; make all )
	(cd kan96xx ; make install2)
	(cd kxx ; make all)
	(cd kxx ; make install2)
all2 :
	(cd gnuplot-diff ; make all)
	(cd gnuplot-diff ; make install)
all3:
	(cd Ti ; make all ; make install)
	(cd phc ; make all ; make install )
all4 :
	(cd k097 ; make all ; make install)
all5 :
	if ../misc/which.pl -s javac ; \
	then \
	(cd OpenMath ; make ) ; \
	else \
      echo "Warning: javac (java compiler) is not found in your search path. OpenMath will not be compiled." ; \
    fi
 

all6 :
	if test -d /usr/local/mathematica ; \
	then \
	(cd ox_math ; make ) ; \
	fi 
	   
all-pari :
	(cd pari ; make install ; make install-lib-sta ; ranlib ../../OpenXM/lib/libpari.a)
all-asir : 
	(cd asir2000; xmkmf -a; make; make install ; make install2)


clean :
	/bin/rm -rf ../*~  *~
	(cd kan96xx ; make clean )
	(cd kxx ; make clean)
	(cd Ti ; make clean)
	(cd gnuplot-diff ; make clean )
	(cd phc ; make clean )
	(cd k097 ; make clean )
	(cd asir2000 ; make clean )
#	(cd ox_math ; make clean )
	/bin/rm -rf bin/sm1 bin/ox bin/oxlog bin/ox_sm1 bin/phc6 bin/tigers bin/k0 bin/oxweave bin/gp-2.0 bin/gphelp bin/fep.linux bin/lin_phcv2p


distclean :
	(cd kan96xx ; make distclean )
	(cd kxx ; make clean)
	(cd Ti ; make clean)
	(cd gnuplot-diff ; make clean )
	(cd phc ; make clean )
	(cd k097 ; make clean )
	(cd asir2000 ; make clean )
#	(cd ox_math ; make clean )
	/bin/rm -rf bin/sm1 bin/ox bin/oxlog bin/ox_sm1 bin/phc6 bin/tigers bin/k0 bin/oxweave bin/gp-2.0 bin/gphelp bin/fep.linux bin/lin_phcv2p


configure :
	./make-links
	(cd ../bin ; ./configure2 )
	(cd kxx ; make configure )
	(cd kan96xx ; make configure )
	(cd pari ; ./Configure --prefix=../../../OpenXM)
	(cd asir2000; ./configure)
	(cd ox_math; ./configure)



