##  $OpenXM: OpenXM/src/kxx/Makefile,v 1.15 2000/01/16 01:35:50 takayama Exp $
## Targets :  
##   all  : generates ox oxlog ox_sm1 oxweave
##   install : copy the binaries to $(OpenXM_bin),
##             remove symbolic link from $(OpenXM_lib)/sm1 to $(KANHOME)/Doc/*
##             copy $(KANHOME)/Doc/* to $(OpenXM_lib)/sm1
##             Create symbolic links in $(OpenXM_lib)/sm1/bin
##   install-for-debug :
##             Create symbolic links instead of copying.
##   install-document :
##             Nothing to do for now.
######### configurations.    
OpenXM_contrib=../../../OpenXM_contrib
OpenXM_ROOT=../../../OpenXM
OpenXM_bin=$(OpenXM_ROOT)/bin
OpenXM_lib=$(OpenXM_ROOT)/lib
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

oxweave : oxweave.c oxweaveUsage.h
	gcc -o oxweave -g oxweave.c

## file2.c file2.h is in SSkan/plugin/
file2.o : $(KANHOME)/plugin/file2.c  $(KANHOME)/plugin/file2.h
	gcc $(CFLAGS) -DKXX=1 -I$(KANHOME)/plugin -c $(KANHOME)/plugin/file2.c 

nullstackmachine.o : nullstackmachine.c ox_kan.h
sm1stackmachine.o : sm1stackmachine.c  ox_kan.h

testclient : testclient.c ox.a file2.o ox_kan.h
	gcc $(CFLAGS) testclient.c -o testclient ox.a file2.o $(LFLAG)


#KANLIB = $(KANHOME)/Kan/kanlib.a $(KANHOME)/gmp/libgmp.a $(KANHOME)/gmp/mpn/libmpn.a $(KANHOME)/gc/gc.a
KANLIB = $(KANHOME)/Kan/kanlib.a $(KANHOME)/gmp/libgmp.a $(KANHOME)/gc/gc.a

## file2 should be linked from kanlib.a

testclient2a : testclient2.c ox.a  ox_kan.h $(KANHOME)/Kan/kanlib.a
	gcc $(CFLAGS) -I$(KANHOME)/Kan testclient2.c ox.a  -o testclient2a  $(LFLAG) $(KANLIB)

testclient2 : testclient2.c  ox_kan.h $(KANHOME)/Kan/kanlib.a ox.a
	gcc $(CFLAGS) -I$(KANHOME)/Kan testclient2.c -o testclient2  $(LFLAG) $(KANLIB)


clean : 
	-/bin/rm -f ox testclient ox_null ox_sm1 testclient2 core *.o a.out *.dvi *.aux openxxx.log  ox_sm1 oxlog *~ oxweave openxm-eg.tex openxm-jp.tex

distclean : clean clean-for-install

## install targets.
install-for-debug : clean-for-install install_lib_bin-for-debug
	/bin/rm -f ../bin/ox ../bin/ox_sm1 ../bin/oxlog ../bin/oxweave
	ln -f -s  ../src/kxx/ox ../bin/ox
	ln -f -s  ../src/kxx/ox_sm1 ../bin/ox_sm1
	ln -f -s  ../src/kxx/oxlog ../bin/oxlog
	ln -f -s  ../src/kxx/oxweave ../bin/oxweave

install_lib_bin-for-debug :
	ln -f -s ../src/kan96xx/Doc $(OpenXM_lib)/sm1
	/bin/rm -f $(OpenXM_lib)/sm1/bin/ox*
	ln -f -s ../../../bin/ox $(OpenXM_lib)/sm1/bin/ox
	ln -f -s ../../../bin/oxlog $(OpenXM_lib)/sm1/bin/oxlog
	ln -f -s ../../../bin/ox_sm1 $(OpenXM_lib)/sm1/bin/ox_sm1
	ln -f -s ./ox_sm1 $(OpenXM_lib)/sm1/bin/ox_sm1_forAsir
	ln -f -s ./ox_sm1 $(OpenXM_lib)/sm1/bin/ox_sm1_gnuplot
	ln -f -s ./ox_sm1 $(OpenXM_lib)/sm1/bin/ox_sm1_phc
	ln -f -s ./ox_sm1 $(OpenXM_lib)/sm1/bin/ox_sm1_tigers
	ln -f -s ./ox_sm1 $(OpenXM_lib)/sm1/bin/ox_sm1_basicCD

install : copy-kan-lib install_lib_bin
	cp -f ox ox_sm1 oxlog oxweave $(OpenXM_bin)

install_lib_bin : 
	/bin/rm -f $(OpenXM_lib)/sm1/bin/ox*
	ln -f -s ../../../bin/ox $(OpenXM_lib)/sm1/bin/ox
	ln -f -s ../../../bin/oxlog $(OpenXM_lib)/sm1/bin/oxlog
	ln -f -s ../../../bin/ox_sm1 $(OpenXM_lib)/sm1/bin/ox_sm1
	ln -f -s ./ox_sm1 $(OpenXM_lib)/sm1/bin/ox_sm1_forAsir
	ln -f -s ./ox_sm1 $(OpenXM_lib)/sm1/bin/ox_sm1_gnuplot
	ln -f -s ./ox_sm1 $(OpenXM_lib)/sm1/bin/ox_sm1_phc
	ln -f -s ./ox_sm1 $(OpenXM_lib)/sm1/bin/ox_sm1_tigers
	ln -f -s ./ox_sm1 $(OpenXM_lib)/sm1/bin/ox_sm1_basicCD

# This target overrides the symbolic link 
# from $(OpenXM_ROOT)/src/kan96xx/lib to $(OpenXM_lib)/sm1
# generated in the top level Makefile
copy-kan-lib : clean-for-install
	mkdir $(OpenXM_lib)/sm1
	touch $(OpenXM_lib)/sm1/DO_NOT_EDIT_THIS_DIRECTORY._ALL_FILES_ARE_COPIED_FROM_kan96xx_Doc
	cp -r $(KANHOME)/Doc/* $(OpenXM_lib)/sm1
	-/bin/rm -rf $(OpenXM_lib)/sm1/CVS
	cp $(KANHOME)/Kan/var.sm1 $(OpenXM_lib)/sm1
clean-for-install :
	-/bin/rm -rf $(OpenXM_lib)/sm1
	-/bin/rm -rf $(OpenXM_bin)/oxlog $(OpenXM_bin)/oxweave $(OpenXM_bin)/ox_sm1 $(OpenXM_bin)/ox   

configure:
	/bin/rm -f ox_kan.h 
	ln -f -s  ../SSkan/plugin/ox_kan.h ox_kan.h

## openxm document generation.
install-document :
	echo "Do nothing for now"

OXWEAVEFLAG=--recursive
doc:  openxxx.tex
	-/bin/rm openxm-eg.tex openxm-jp.tex
	echo "% DO NOT EDIT THIS FILE. This is automatically generated from openxxx.tex" >openxm-eg.tex
	echo "% DO NOT EDIT THIS FILE. This is automatically generated from openxxx.tex" >openxm-jp.tex
	./oxweave $(OXWEAVEFLAG) C eg <openxxx.tex >>openxm-eg.tex
	./oxweave $(OXWEAVEFLAG) C jp <openxxx.tex >>openxm-jp.tex

clean-binary :
	echo " ../open-sm1-binary*  will be removed"
	echo "after 10 seconds. To cancel type in ctrl-C."
	( sleep 10 ; /bin/rm -rf ../open-sm1-binary* )


