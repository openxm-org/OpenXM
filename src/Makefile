## Type in make configure and make all to build all
## $OpenXM: OpenXM/src/Makefile,v 1.9 1999/11/07 06:45:24 takayama Exp $
all : all1 all2 all3

all1 :
	(cd kan96xx ; make all )
	(cd kan96xx ; make install2)
	(cd kxx ; make all)
	(cd kxx ; make install2)
all2 :
	(cd gnuplot-diff ; make all)
	(cd gnuplot-diff ; make install)
	(cd Ti ; make all )
	(cd phc ; make all )
all3 :
	(cd k097 ; make all ; cp k0 ../../bin)
all4 :
	(cd OpenMath ; make )
	   

clean :
	/bin/rm -rf ../*~  *~
	(cd kan96xx ; make cleana )
	(cd kxx ; make clean)
	(cd Ti ; make clean)
	/bin/rm -rf kan96.tgz kxx.tgz *~ ../*~
	/bin/rm -rf k097.tgz
#	(cd kan96xx ; make clean-binary )
#	(cd kxx ; make clean-binary )
	(cd gnuplot-diff ; make clean )
	/bin/rm -rf ../bin/tigers
	(cd phc ; make clean )
	(cd k097 ; make clean ; /bin/rm -rf ../../bin/k0)
	/bin/rm -rf bin/sm1 bin/ox bin/oxlog bin/ox_sm1 bin/phc6

configure :
	/bin/rm -f bin SSkan gmp gc ../lib/k097 ../lib/sm1
	ln -f -s ../bin bin
	ln -f -s ./kan96xx SSkan
	ln -f -s ./kan96xx/gmp gmp
	ln -f -s ./kan96xx/gc gc
	(cd ../lib ; ln -f -s ../src/k097 k097 ; ln -f -s ../src/kan96xx/Doc sm1)
	(cd ../bin ; ./configure2 )
	(cd kan96xx ; make configure )
	(cd kxx ; /bin/rm -f ox_kan.h )
	(cd kxx ; ln -f -s  ../SSkan/plugin/ox_kan.h ox_kan.h)
	(cd ../lib/asir ; ./configure2 )



