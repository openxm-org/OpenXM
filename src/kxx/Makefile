######### configurations.    
##  $OpenXM: OpenXM/src/kxx/Makefile,v 1.5 1999/11/07 06:45:26 takayama Exp $
KANHOME = ../SSkan

LFLAG =
SOLARISLFLAG = -lnsl -lsocket /usr/ucblib/libucb.a

CFLAGS = -g

########## end of configurations
### Set the server name in oxmain.c if you do not use the environmental
### variable LOAD_SM1_PATH.  cf. $KANHOME/lib/ox.sm1

all : all-plugin kanlib.a ox ox_sm1  oxlog oxweave

all2 : all-plugin kanlib.a ox ox_sm1 testclient oxlog

dist : 
	./makeDist

## ox_null can no longer be compiled.

## This is the Makefile at kxx.
## 1998, 2/7  , Old version is at S/kan.backup/kxx.tar.gz.19980206
##
.c.o :
	gcc  $(CFLAGS) -c  $<

all-plugin :
	(cd $(KANHOME)/plugin ; make )
kanlib.a :
	(cd $(KANHOME)/Kan ; make kanlib.a)
ox.a : 
	/bin/rm -f ox.a
	ln -s $(KANHOME)/plugin/ox.a ox.a
ox_kan.h :
	/bin/rm -f ox_kan.h
	ln -s $(KANHOME)/plugin/ox_kan.h ox_kan.h

ox : oxmain.c $(KANHOME)/plugin/mytcpio.o $(KANHOME)/plugin/oxmisc.o file2.o ox_kan.h
	if test `uname` = "SunOS" ; \
	then \
	gcc $(CFLAGS) oxmain.c $(KANHOME)/plugin/mytcpio.o $(KANHOME)/plugin/oxmisc.o file2.o -o ox  $(SOLARISLFLAG) ; \
	else \
	gcc $(CFLAGS) oxmain.c $(KANHOME)/plugin/mytcpio.o $(KANHOME)/plugin/oxmisc.o file2.o -o ox  $(LFLAG) ; \
	fi

ox_null : oxserver00.c ox.a nullstackmachine.o file2.o ox_kan.h
	if test `uname` = "SunOS" ; \
	then \
	gcc $(CFLAGS) oxserver00.c ox.a nullstackmachine.o file2.o -o ox_null  $(SOLARISLFLAG) ; \
	else \
	gcc $(CFLAGS) oxserver00.c ox.a nullstackmachine.o file2.o -o ox_null  $(LFLAG) ; \
	fi

ox_sm1 : oxserver00.c sm1stackmachine.o ox_kan.h ox.a $(KANHOME)/Kan/kanlib.a
	(cd $(KANHOME)/Kan ; make kanlib.a)
	if test `uname` = "SunOS" ; \
	then \
	gcc $(CFLAGS) oxserver00.c sm1stackmachine.o -o ox_sm1  $(SOLARISLFLAG) $(KANLIB) ; \
	else \
	gcc $(CFLAGS) oxserver00.c sm1stackmachine.o -o ox_sm1  $(LFLAG) $(KANLIB) ; \
	fi

oxlog : oxlog.c
	gcc -o oxlog oxlog.c

oxweave : oxweave.c

## file2.c file2.h is in SSkan/plugin/
file2.o : $(KANHOME)/plugin/file2.c  $(KANHOME)/plugin/file2.h
	gcc $(CFLAGS) -DKXX=1 -I$(KANHOME)/plugin -c $(KANHOME)/plugin/file2.c 

nullstackmachine.o : nullstackmachine.c ox_kan.h
sm1stackmachine.o : sm1stackmachine.c  ox_kan.h

testclient : testclient.c ox.a file2.o ox_kan.h
	gcc $(CFLAGS) testclient.c -o testclient ox.a file2.o $(LFLAG)


KANLIB = $(KANHOME)/Kan/kanlib.a $(KANHOME)/gmp/libgmp.a $(KANHOME)/gmp/mpn/libmpn.a $(KANHOME)/gc/gc.a

## file2 should be linked from kanlib.a

testclient2a : testclient2.c ox.a  ox_kan.h $(KANHOME)/Kan/kanlib.a
	gcc $(CFLAGS) -I$(KANHOME)/Kan testclient2.c ox.a  -o testclient2a  $(LFLAG) $(KANLIB)

testclient2 : testclient2.c  ox_kan.h $(KANHOME)/Kan/kanlib.a ox.a
	gcc $(CFLAGS) -I$(KANHOME)/Kan testclient2.c -o testclient2  $(LFLAG) $(KANLIB)


clean :
	/bin/rm -f ox testclient ox_null ox_sm1 testclient2 core *.o a.out *.dvi *.aux openxxx.log  ox_sm1 oxlog *~ 

BACKUPDIR = Old/19980112
backup :
	/bin/cp Makefile *.c *.h *.sm1 $(BACKUPDIR)

PR = lpt1
print :
	a2ps ox.sm1 | lpr -P$(PR)
	a2ps oxasir.sm1 | lpr -P$(PR)
	a2ps ox-1.sm1 | lpr -P$(PR)
	a2ps ox_kan.h | lpr -P$(PR)
	a2ps testclient.c | lpr -P$(PR)
	a2ps oxmain.c | lpr -P$(PR)
	a2ps oxserver00.c | lpr -P$(PR)
	a2ps nullstackmachine.c | lpr -P$(PR)
	a2ps $(KANHOME)/plugin/oxmisc.c | lpr -P$(PR)
	a2ps $(KANHOME)/plugin/oxmisc2.c | lpr -P$(PR)
	a2ps $(KANHOME)/plugin/mytcpio.c | lpr -P$(PR)
	a2ps $(KANHOME)/plugin/oxmisc2.h | lpr -P$(PR)
	a2ps $(KANHOME)/plugin/oxx.h | lpr -P$(PR)
	a2ps $(KANHOME)/plugin/file2.c | lpr -P$(PR)
	a2ps $(KANHOME)/plugin/file2.h | lpr -P$(PR)

install_lib_bin :
	/bin/rm -f $(KANHOME)/lib/bin/ox*
	ln -f -s ../../../bin/ox $(KANHOME)/lib/bin/ox
	ln -f -s ../../../bin/oxlog $(KANHOME)/lib/bin/oxlog
	ln -f -s ../../../bin/ox_sm1 $(KANHOME)/lib/bin/ox_sm1
	ln -f -s ./ox_sm1 $(KANHOME)/lib/bin/ox_sm1_forAsir
	ln -f -s ./ox_sm1 $(KANHOME)/lib/bin/ox_sm1_gnuplot
	ln -f -s ./ox_sm1 $(KANHOME)/lib/bin/ox_sm1_phc
	ln -f -s ./ox_sm1 $(KANHOME)/lib/bin/ox_sm1_tigers
	ln -f -s ./ox_sm1 $(KANHOME)/lib/bin/ox_sm1_basicCD

install : install_lib_bin
	/bin/rm -f ../bin/ox ../bin/ox_sm1 ../bin/oxlog
	cp  ox ox_sm1 oxlog ../bin

install2 : install_lib_bin
	/bin/rm -f ../bin/ox ../bin/ox_sm1 ../bin/oxlog
	ln -f -s  ../src/kxx/ox ../bin/ox
	ln -f -s  ../src/kxx/ox_sm1 ../bin/ox_sm1
	ln -f -s  ../src/kxx/oxlog ../bin/oxlog


clean-binary :
	echo " ../open-sm1-binary*  will be removed"
	echo "after 10 seconds. To cancel type in ctrl-C."
	( sleep 10 ; /bin/rm -rf ../open-sm1-binary* )
