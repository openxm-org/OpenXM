# $OpenXM: OpenXM/src/k097/Makefile,v 1.7 2000/01/17 21:12:22 takayama Exp $
# targets
# all 
#     compile
# intall : all install-lib
#     
# todo : install-lib-for-debug
#
BDATE =19970416
CC = gcc
GC = $(KHOME)/gc/gc.a
OpenXM_lib=../../lib

## Kan source.  kan sources must be built up by hand.
KHOME = ../kan96xx


## 
LOAD_K_PATH ='"/usr/local/lib/k0/"'
LOAD_SM1_PATH2 ='"/home/nobuki/SSkan/Kan/"'
##
GMP_HOME = $(KHOME)/gmp
LIBGMP=$(GMP_HOME)/libgmp.a
## This line is for buggy linux-gmp.
## LIBGMP = $(GMP_HOME)/libgmp.a $(GMP_HOME)/mpn/libmpn.a
EXTRALIB = 
########### end of configuration.

### These are to switch k2 linux and k2 solaris version.
SOLARIS_SLIB =   -lsocket -lnsl -lc /usr/ucblib/libucb.a  
### if you use K2LFLAG below, you don't need ALIB.
#K2LFLAG = -lm $(SOLARIS_ALIB)  
K2LFLAG = -lfunc -lparse -lengine -lm -Wl,-rpath,/usr/X11R6/lib
#CALL_ASIR=callasir.solaris.c
CALL_ASIR=callasir.c
PARI_LIB = -lpari
###########################################################
SOLARIS_ALIB =  -L$(AHOME)/usrlib $(AHOME)/io/tcpf.o $(AHOME)/plot/plotf.o $(AHOME)/builtin/libfunc.a $(AHOME)/parse/libparse.a $(AHOME)/io/libio.a $(AHOME)/engine/libca.a $(AHOME)/asm/libasm.a  $(PARI_LIB) $(SOLARIS_SLIB)
SOLARIS_EXTRA_INCLUDE = -I$(AHOME)/parse -I$(AHOME)/io
###########################################################

CC = gcc
MYYACC = bison
YACCPREFIX = -p KC
FLAG = -g  -DLOAD_K_PATH=$(LOAD_K_PATH) -DLOAD_SM1_PATH2=$(LOAD_SM1_PATH2)
KAN_HOME = $(KHOME)/Kan

all : d0 k0 

## Making a new Makefile
new : 
	touch Makefile.new
	/bin/cp Makefile.new Trash
	/bin/rm -f Makefile.new
	sed -f new.sed <Makefile >Makefile.new

d0 : simple.tab.o d.o dic.o dm.c 
	$(CC) $(FLAG) -o d0 dm.c d.o dic.o simple.tab.o

d.o : d.c simple.tab.h Stable/sm1symbol.h
	$(CC) $(FLAG) -o d.o -c d.c

simple.tab.o : simple.tab.c 
	$(CC) $(FLAG) -o simple.tab.o -c simple.tab.c

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

backup :
	cp simple.y d.c dm.c k2.c ki.c msg*.c d.h Makefile  *.k incmac.sm1 memo.txt Backup/backup$(BDATE)

test:   d0
	touch t.sm1
	/bin/rm t.sm1
	./d0 <test.ccc >t.sm1

slib:   d0
	touch slib.sm1
	/bin/rm slib.sm1
	./d0 -DebugMode <slib.k >slib.sm1

####################################################################
#############    kxx main parts.
###################################################################
KXX = simple.tab.o d.o ki.o dic.o
KXXASIR = simple.tab.o d.o ki-asir.o dic.o

k0 :  k0.o $(KXX) $(KAN_HOME)/kanlib.a $(GC) $(GMP) $(KAN_HOME)/lookup.h
	if test `uname` = "SunOS" ; \
	then \
	$(CC) $(FLAG) -I$(GMP_HOME) -I$(KAN_HOME) -o k0 k0.o $(KXX)  $(KAN_HOME)/kanlib.a $(LIBGMP) $(GC) $(EXTRALIB) $(SOLARIS_SLIB) ; \
	else \
	$(CC) $(FLAG) -I$(GMP_HOME) -I$(KAN_HOME) -o k0 k0.o $(KXX)  $(KAN_HOME)/kanlib.a $(LIBGMP) $(GC) $(EXTRALIB) ; \
	fi

k0.o : k2.c
	$(CC) -c $(FLAG) -I$(GMP_HOME) -I$(KAN_HOME) -o k0.o k2.c

ki.o : ki.c
	$(CC) -c $(FLAG) -I$(GMP_HOME) -I$(KAN_HOME) -o ki.o ki.c

ki-asir.o : ki.c
	$(CC) -c $(FLAG) -DCALLASIR -I$(GMP_HOME) -I$(KAN_HOME) -o ki-asir.o ki.c

dic.o : dic.c d.h
	$(CC) -c $(FLAG)  -o dic.o dic.c

$(KAN_HOME)/kanlib.a : $(KAN_HOME)/sm1
	(cd $(KAN_HOME); make kanlib.a)

kanlib.a : 
	(cd $(KAN_HOME); make kanlib.a)

$(KAN_HOME)/lookup.h :
	(cd $(KAN_HOME); make lookup.h)

######## How to make k2.  k2 is k0&asir&sm1
## asir GC.
ASIRGC = $(AHOME)/gc/libgc.a
## asir include
ASIR_INCLUDE = $(AHOME)/include
ASIR_INCLUDE2 = $(ALIB)/include
LFLAG = $(K2LFLAG)
GMP = $(LIBGMP)
ASIRKAN = $(ASIRKAN_HOME)/ak.a
K2INCLUDE = -I$(ASIR_INCLUDE)  -I$(ASIR_INCLUDE2) -I$(KHOME)/Kan -I$(ASIRKAN_HOME) $(SOLARIS_EXTRA_INCLUDE)

k2 : k2.c $(KAN_HOME)/kanlib.a  $(KXXASIR) msg1.o $(ASIRKAN) $(ASIRKAN_HOME)/$(CALL_ASIR) $(ASIRGC) $(GMP)
	$(CC) $(FLAG) -DCALLASIR $(K2INCLUDE) -o k2 k2.c $(KXXASIR) msg1.o  $(ASIRKAN_HOME)/$(CALL_ASIR) -L/usr/X11R6/lib -L$(ALIB) $(LFLAG) $(KAN_HOME)/kanlib.a $(ASIRKAN) $(GMP) $(ASIRGC)

msg1.o : msg1.c
	$(CC) $(FLAG) -DCALLASIR $(K2INCLUDE) -c msg1.c


$(ASIRKAN) :
	(cd $(ASIRKAN_HOME); make ak.a)


$(GMP) :
	(cd $(KHOME)/gmp ; ./configure; make )
$(GC) :
	(cd $(KHOME)/gc ; make )


##########  
## A sample to compilation:  source is test0.c, output is test0
test0 :  test0.c  $(KXX) $(KAN_HOME)/kanlib.a  $(GC) $(GMP) $(KAN_HOME)/lookup.h
	$(CC) $(FLAG) -I$(GMP_HOME) -I$(KAN_HOME) -o test0 test0.c $(KXX) $(KAN_HOME)/kanlib.a $(LIBGMP) $(GC)



install0 : all
	cp -f k0 d0 $(HOME)/.Hbin
	cp lib/mklib $(HOME)/.Hbin
	echo "Set the environment variable in $(HOME)/bin/mklib"

install : all install-lib
	cp -f k0 d0 ../../bin

install-lib :
	/bin/rm -rf $(OpenXM_lib)/k097
	mkdir $(OpenXM_lib)/k097
	cp -f $(KHOME)/Doc/var.sm1 $(OpenXM_lib)/k097
	cp -rf *.sm1 *.k *.kk debug $(OpenXM_lib)/k097

install-for-debug :
	/bin/rm -rf $(OpenXM_lib)/k097
	ln -s ../src/k097 $(OpenXM_lib)/k097

dist :
	./makeDist
