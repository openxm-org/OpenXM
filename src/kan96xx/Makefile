all :
	(cd gc; make)
	(cd gmp; ./configure; make)
	(cd plugin; make all)
	(cd Kan; make all)

all2 :
	(cd gc; make)
	(cd gmp; ./configure; make)
	(cd Kan; make all)


### When you make sm1 without the garbage collector, type in make sm1.nogc
sm1.nogc :
	(cd Kan; make sm1.nogc)

### generate a distribution.
dist :
	./makeDist
###############
install : 
	mkdir /usr/local/lib/sm1
	cp lib/* /usr/local/lib/sm1
	cp Kan/sm1 /usr/local/bin

###############
install-binary :
	/bin/rm -f sm1
	echo ":" > sm1
	echo "LOAD_SM1_PATH=`pwd`/lib/" >>sm1
	echo "export LOAD_SM1_PATH" >>sm1
	echo -n "`pwd`/`ls Kan/sm1.*` " >>sm1
	echo  -n '$$' >>sm1
	echo '*' >>sm1
	chmod +x sm1
	ln -s sm1 Kan/sm1
install-binary-solaris :
	/bin/rm -f sm1
	/usr/ucb/echo ":" > sm1
	/usr/ucb/echo "LOAD_SM1_PATH=`pwd`/lib/" >>sm1
	/usr/ucb/echo "export LOAD_SM1_PATH" >>sm1
	/usr/ucb/echo -n "`pwd`/`ls Kan/sm1.*` " >>sm1
	/usr/ucb/echo  -n '$$' >>sm1
	/usr/ucb/echo '*' >>sm1
	chmod +x sm1
	ln -s sm1 Kan/sm1
clean-binary :
	echo " ../kanbinary*  will be removed"
	echo "after 10 seconds. To cancel type in ctrl-C."
	( sleep 10 ; /bin/rm -rf ../kanbinary* )
install-openXM-bin :
	cp Kan/sm1 ../bin

##############
clean :
	(cd gmp; make distclean)
	(cd gc; make clean)
	(cd Kan; make clean)
	(cd plugin; make clean)
	/bin/rm -f sm1


### clean also *.a files.
cleana :
	(cd gmp; make distclean; /bin/rm -f *.a)
	(cd gc; make clean; /bin/rm -f *.a)
	(cd Kan; make cleana)
	(cd plugin; make clean; /bin/rm -f *.a)
