# $OpenXM: OpenXM/src/k097/Makefile,v 1.21 2003/01/15 00:11:08 takayama Exp $
# targets
# all 
#     compile
# intall : all install-lib
#     
# install-for-debug
#
TOPDIR=../../../OpenXM
OpenXM_HOME=${TOPDIR}
OpenXM_include=$(OpenXM_HOME)/include
OpenXM_lib=../../lib
CC = gcc
BDATE =19970416
GC = $(OpenXM_HOME)/lib/libgc.a
DDD=-D_BSD_SOURCE
#STATIC_LINK_FLAG=-static
STATIC_LINK_FLAG=

## Kan source.  kan sources must be built up by hand.
KHOME = $(OpenXM_HOME)/src/kan96xx

## 
LOAD_K_PATH ='"/usr/local/lib/k0/"'
LOAD_SM1_PATH2 ='"/this_directory_does_not_exists/"'
##
LIBGMP=-L$(OpenXM_HOME)/lib -lgmp 
## This line is for buggy linux-gmp.
## LIBGMP = $(GMP_HOME)/libgmp.a $(GMP_HOME)/mpn/libmpn.a
EXTRALIB = 
########### end of configuration.

SOLARIS_SLIB =   -lsocket -lnsl -lc /usr/ucblib/libucb.a  
MYYACC = bison
YACCPREFIX = -p KC
FLAG = -g  -DLOAD_K_PATH=$(LOAD_K_PATH) -DLOAD_SM1_PATH2=$(LOAD_SM1_PATH2)
KAN_HOME = $(KHOME)/Kan

all : d0 k0 


d0 : simple.tab.o d.o dic.o dm.c 
	$(CC) ${STATIC_LINK_FLAG} $(FLAG) $(DDD) -o d0 dm.c d.o dic.o simple.tab.o

d.o : d.c simple.tab.h Stable/sm1symbol.h
	$(CC) $(FLAG) $(DDD) -o d.o -c d.c

simple.tab.o : simple.tab.c 
	$(CC) $(FLAG) $(DDD) -o simple.tab.o -c simple.tab.c

simple.tab.c : simple.y d.h
	$(MYYACC) $(YACCPREFIX) simple.y

simple.tab.h : simple.y d.h
	$(MYYACC) $(YACCPREFIX) -d simple.y

Stable/sm1symbo.h : Stable/reserved.txt
	(cd Stable; make)

clean :
	rm -f simple d0 k0 test0 *.o *~  simple.output asir-tmp.t asir-tmp.tt asir-tmp-out.t asir-tmp-out.tt k2
	(cd Stable; make clean)
	(cd try; make clean)
##	rm -f simple.tab.c simple.tab.h

test:   d0
	touch t.sm1
	/bin/rm t.sm1
	./d0 <test.ccc >t.sm1

slib:   d0
	touch slib.sm1
	/bin/rm slib.sm1
	./d0 -DebugMode <slib.k >slib.sm1

slib.sm1 : slib.k
	make slib

####################################################################
#############    kxx main parts.
###################################################################
KXX = simple.tab.o d.o ki.o dic.o

k0 :  k0.o $(KXX) $(KAN_HOME)/kanlib.a $(KAN_HOME)/lookup.h
	if test `uname` = "SunOS" ; \
	then \
	$(CC) $(FLAG) $(DDD) -I$(OpenXM_include) -I$(KAN_HOME) -o k0 k0.o $(KXX)  $(KAN_HOME)/kanlib.a $(LIBGMP) $(GC) $(EXTRALIB) $(SOLARIS_SLIB) ; \
	else \
	$(CC) ${STATIC_LINK_FLAG} $(FLAG) $(DDD) -I$(OpenXM_include) -I$(KAN_HOME) -o k0 k0.o $(KXX)  $(KAN_HOME)/kanlib.a $(LIBGMP) $(GC) $(EXTRALIB) ; \
	fi

k0.o : k2.c
	$(CC) -c $(FLAG) $(DDD) -I$(OpenXM_include) -I$(KAN_HOME) -o k0.o k2.c

ki.o : ki.c
	$(CC) -c $(FLAG) $(DDD) -I$(OpenXM_include) -I$(KAN_HOME) -o ki.o ki.c

dic.o : dic.c d.h
	$(CC) -c $(FLAG) $(DDD)  -o dic.o dic.c

$(KAN_HOME)/kanlib.a : $(KAN_HOME)/sm1
	(cd $(KAN_HOME); make kanlib.a)

kanlib.a : 
	(cd $(KAN_HOME); make kanlib.a)

$(KAN_HOME)/lookup.h :
	(cd $(KAN_HOME); make lookup.h)


ox_k0 : ox_k0.c k0 ox_k0.c sm1sm.c
	$(CC) ${STATIC_LINK_FLAG} $(FLAG) $(DDD) -I$(OpenXM_include) -I$(KAN_HOME) -o ox_k0 ox_k0.c sm1sm.c $(KXX)  $(KAN_HOME)/kanlib.a $(LIBGMP) $(GC) $(EXTRALIB)

##########  
## A sample to compilation:  source is test0.c, output is test0
test0 :  test0.c  $(KXX) $(KAN_HOME)/kanlib.a  $(GC)  $(KAN_HOME)/lookup.h
	$(CC) $(FLAG) $(DDD) -I$(OpenXM_include) -I$(KAN_HOME) -o test0 test0.c $(KXX) $(KAN_HOME)/kanlib.a $(LIBGMP) $(GC)



install0 : all
	cp -f k0 d0 $(HOME)/.Hbin
	cp lib/mklib $(HOME)/.Hbin
	echo "Set the environment variable in $(HOME)/bin/mklib"

install : all install-lib
	/bin/rm -f ../../bin/k0 ../../bin/d0
	cp -f k0 d0 ../../bin

install-for-debug : all install-lib-for-debug
	rm -f ../../bin/k0 ../../bin/d0
	(cd ../../bin ; ln -s ../src/k097/k0 k0)
	(cd ../../bin ; ln -s ../src/k097/d0 d0)

install-lib :
	/bin/rm -rf $(OpenXM_lib)/k097
	mkdir $(OpenXM_lib)/k097
	cp -f $(KHOME)/Doc/var.sm1 $(OpenXM_lib)/k097
	cp -rf *.sm1 *.k *.kk debug lib $(OpenXM_lib)/k097

install-lib-for-debug :
	/bin/rm -rf $(OpenXM_lib)/k097
	ln -s ../src/k097 $(OpenXM_lib)/k097

dist :
	./makeDist
