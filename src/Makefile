## Type in make configure and make all to build all
## $OpenXM: OpenXM/src/Makefile,v 1.13 1999/11/23 01:38:32 takayama Exp $
all : all1 all3  all4 pari asir all2 all5 all6

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
	if test -d /usr/local/jdk1.1.7 ; \
	then \
	(cd OpenMath ; make ) ; \
	fi 

all6 :
	if test -d /usr/local/mathematica ; \
	then \
	(cd ox_math ; make ) ; \
	fi 
	   
pari :
	(cd pari-2.0.17.beta ; make install ; make install-lib-sta ; ranlib ../../lib/libpari.a)
asir :
	(cd asir99; xmkmf -a; make; make install ; make install2)


clean :
	/bin/rm -rf ../*~  *~
	(cd kan96xx ; make clean )
	(cd kxx ; make clean)
	(cd Ti ; make clean)
	(cd gnuplot-diff ; make clean )
	(cd gnuplot ; make clean)
	(cd phc ; make clean )
	(cd k097 ; make clean )
	(cd asir99 ; make clean )
#	(cd ox_math ; make clean )
	/bin/rm -rf bin/sm1 bin/ox bin/oxlog bin/ox_sm1 bin/phc6 bin/tigers bin/k0 bin/oxweave bin/gp-2.0 bin/gphelp bin/fep.linux bin/lin_phcv2p


distclean :
	(cd kan96xx ; make distclean )
	(cd kxx ; make clean)
	(cd Ti ; make clean)
	(cd gnuplot-diff ; make clean )
	(cd gnuplot ; make distclean )
	(cd phc ; make clean )
	(cd k097 ; make clean )
	(cd asir99 ; make clean )
#	(cd ox_math ; make clean )
	/bin/rm -rf bin/sm1 bin/ox bin/oxlog bin/ox_sm1 bin/phc6 bin/tigers bin/k0 bin/oxweave bin/gp-2.0 bin/gphelp bin/fep.linux bin/lin_phcv2p


configure :
	./make-links
	(cd ../bin ; ./configure2 )
	(cd kxx ; make configure )
	(cd kan96xx ; make configure )
	(cd pari-2.0.17.beta; ./Configure --prefix=../../..)
	(cd asir99; ./configure)
	(cd ox_math; ./configure)



