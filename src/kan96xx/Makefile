##  $OpenXM: OpenXM/src/kan96xx/Makefile,v 1.25 2000/12/25 06:19:10 takayama Exp $

## Type in make configure and make all to build all
TOPDIR=../../../OpenXM
OpenXM_contrib=../../../OpenXM_contrib
OpenXM_ROOT=${TOPDIR}
OpenXM_bin=$(OpenXM_ROOT)/bin

GC=${OpenXM_contrib}/gc

all :   all-plugin all-sm1 all-kanlib.a
## .SUFFIXES:

all2 :  all-gc 
	(cd Kan; make all)

all-gc: configure_symbolic_links
	(cd $(GC) ; make all)


all-sm1 : all-plugin
	(cd Kan; make all)

all-plugin : configure_symbolic_links
	 (cd plugin ; make all)

all-kanlib.a : all-plugin
	(cd Kan ; make kanlib.a)



### When you make sm1 without the garbage collector, type in make sm1.nogc
sm1.nogc :
	(cd Kan; make sm1.nogc)

configure: configure_symbolic_links 
	(cd Kan; sh configure)

configure_symbolic_links :
	@if test ! -f ./.done_links  ; \
	then \
	 /bin/rm -f gc gmp lib Doc/var.sm1 ; \
	 ln -f -s ${OpenXM_contrib}/gc gc ; \
	 ln -f -s ${OpenXM_contrib}/gmp gmp ; \
	 ln -f -s ./Doc lib ; \
	 (cd Doc; ln -f -s ../Kan/var.sm1 var.sm1) ; \
	fi
	@touch ./.done_links

### generate a distribution.
dist :
	./makeDist
###############
install : install-bin-OpenXM install-doc install-man

install.man: install-man  #aliases

install-man :  
	@if [ ! -d $(OpenXM_ROOT)/man ]; then  \
	   mkdir $(OpenXM_ROOT)/man ; \
	fi
	@if [ ! -d $(OpenXM_ROOT)/man/man1 ]; then  \
	   mkdir $(OpenXM_ROOT)/man/man1 ; \
	fi
	-cp Doc/sm1.1 $(OpenXM_ROOT)/man/man1

install-usr-local: install-bin-usr-local  #alias

install-bin-usr-local : 
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

install-openXM-bin: install-bin-OpenXM  #alias

install-bin-OpenXM: clean-for-debug
	if [ "`uname`" = "CYGWIN_NT-5.0" ]; then \
	  cp Kan/sm1.exe $(OpenXM_bin) ; \
	else \
	  cp Kan/sm1 $(OpenXM_bin) ; \
	fi

install-for-debug :
	if [ "`uname`" = "CYGWIN_NT-5.0" ]; then \
	  ln -f -s ../src/kan96xx/Kan/sm1.exe $(OpenXM_bin)/sm1.exe ; \
	else \
	  ln -f -s ../src/kan96xx/Kan/sm1 $(OpenXM_bin)/sm1 ; \
	fi

install-doc:  install-document

install-document :   #alias
	-(cd Doc ; make install-document)

##############
clean-for-debug :
	/bin/rm -f $(OpenXM_bin)/sm1

clean :
	/bin/rm -f ./.done_links
	(cd Kan; make clean)
	(cd plugin; make clean)
	/bin/rm -f sm1


### clean also *.a files.
cleana:  distclean

distclean : clean
	(cd Kan; make cleana)
	(cd plugin; make clean; /bin/rm -f *.a)
	/bin/rm -rf gc gmp lib Doc/var.sm1

