## Type in make configure and make all to build all
all : all1 all2 all3

all1 :
	(cd kan96xx ; make all )
	(cd kan96xx ; make install-openXM-bin)
	(cd kxx ; make all)
	(cd kxx ; make install)
all2 :
	(cd gnuplot-diff ; make all)
	(cd Ti ; make all )
	(cd phc ; make all )
all3 :
	(cd k097 ; make all ; cp k0 ../../bin)

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
	ln -fs ../bin bin
	ln -fs ./kan96xx SSkan
	ln -fs ./kan96xx/gmp gmp
	(cd ../lib ; ln -fs ../src/k097 k097 ; ln -fs ../src/kan96xx/Doc sm1)
	(cd ../bin ; ./configure2 )
	(cd kan96xx ; make configure )
	(cd kxx ; ln -fs  ../SSkan/plugin/ox_kan.h ox_kan.h)
	(cd ../lib/asir ; ./configure2 )

install-usr-local : 
	ln -sf `pwd`/SSkan/Doc    /usr/local/lib/sm1
	ln -sf `pwd`/../lib/asir  /usr/local/lib/asir
	ln -sf `pwd`/../bin/sm1  /usr/local/bin/sm1
	ln -sf `pwd`/../bin/fep  /usr/local/bin/fep


