##  $OpenXM: OpenXM/src/kan96xx/Makefile,v 1.10 2000/01/10 19:11:59 maekawa Exp $

## Type in make configure and make all to build all
OpenXM_contrib=../../../OpenXM_contrib
OPENXM_CONTRIB=../../../OpenXM_contrib
GC=$(OPENXM_CONTRIB)/gc
GMP=$(OPENXM_CONTRIB)/gmp

all :   all-gc all-gmp
	(cd plugin; make all)
	(cd Kan; make all)

all2 :  all-gc all-gmp
	(cd Kan; make all)

all-gc:
	(cd $(GC) ; make all)

all-gmp: configure-gmp
	if test -f /usr/lib/libgmp.a ; \
	then \
	  echo "Using /usr/lib/libgmp.a" ; \
	  /bin/rm -rf $(GMP)/libgmp.a ; \
	  ln -s /usr/lib/libgmp.a $(GMP)/libgmp.a ; \
	else \
	 (cd $(GMP) ; make) ; \
	fi



### When you make sm1 without the garbage collector, type in make sm1.nogc
sm1.nogc :
	(cd Kan; make sm1.nogc)

configure: configure_symbolic_links configure-gmp

configure-gmp:
	(cd $(GMP) ; ./configure)

configure_symbolic_links :
	/bin/rm -f gc gmp lib Doc/var.sm1
	ln -f -s $(OpenXM_contrib)/gc gc
	ln -f -s $(OpenXM_contrib)/gmp gmp
#	ln -f -s gmp-2.0.2-ssh-2 gmp
	ln -f -s ./Doc lib
	(cd Doc; ln -f -s ../Kan/var.sm1 var.sm1)

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
	/bin/rm -f ../bin/sm1
	cp Kan/sm1 ../bin
install2 :
	/bin/rm -f ../bin/sm1
	ln -f -s ../src/kan96xx/Kan/sm1 ../bin/sm1

##############
clean :
	if test -f $(GMP)/Makefile ; \
	then \
	  (cd $(GMP) ; make clean) ; \
	fi
	if test -f $(GC)/Makefile ; \
	then \
	  (cd $(GC); make clean) ; \
	fi
	(cd Kan; make clean)
	(cd plugin; make clean)
	/bin/rm -f sm1


### clean also *.a files.
cleana:  distclean

distclean :
	if test -f $(GMP)/Makefile ; \
	then \
	  (cd $(GMP) ; make distclean) ; \
	fi
	if test -f $(GC)/Makefile ; \
	then \
	  (cd $(GC); make clean; /bin/rm -f *.a) ; \
	fi
	(cd Kan; make cleana)
	(cd plugin; make clean; /bin/rm -f *.a)
