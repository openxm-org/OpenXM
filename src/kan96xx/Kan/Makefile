###  $OpenXM: OpenXM/src/kan96xx/Kan/Makefile,v 1.10 2000/09/17 03:12:11 takayama Exp $
all : sm1
# .SUFFIXES:
########################## configuration ##############################
TOPDIR=../../../../OpenXM
OpenXM_HOME=${TOPDIR}
OpenXM_include=$(OpenXM_HOME)/include
####  What C compiler do you use?
CC = gcc
#### Gargabe collector? If GC = nogc.c, then you need to define -DNOGC.
GC = $(OpenXM_HOME)/lib/libgc.a
#GC = nogc.c
#### -D options
DDD = -D_BSD_SOURCE
#DDD = -DNOGC -D_BSD_SOURCE
#STATIC_LINK_FLAG = -static
STATIC_LINK_FLAG = 

#### Optimization level etc.
#CFLAGS = 
#### option  for dbx
CFLAGS = -O
#### optimizer which seems not to be  reliable at level 2,3 and 4.
#CFLAGS = -O
#### profiler to output mon.out
#CFLAGS = -O

GMP_HOME2 = ../../gmp
LIBGMP = $(OpenXM_HOME)/lib/libgmp.so -Wl,--rpath -Wl,$(OpenXM_HOME)/lib
## This line is for buggy linux-gmp.
##LIBGMP = $(GMP_HOME)/libgmp.a $(GMP_HOME)/mpn/libmpn.a

## Library you need on Solaris 2.*
SOLARISLIB = -lnsl -lsocket /usr/ucblib/libucb.a

###################  Plugin modules
## Note that you need to make PLUGIN_LIB by your hand.
## Example:
##  (1)
##PLUGIN = ../plugin
##PLUGIN_PROTOTYPE  = $(PLUGIN)/sample.h  $(PLUGIN)/graphic0.h
##PLUGIN_LIB  = $(PLUGIN)/sample.a $(PLUGIN)/graphic0.a
##PLUGIN_EXT = $(PLUGIN)/sample.hh $(PLUGIN)/graphic0.hh
##PLUGIN_SM1 =$(PLUGIN)/sample.sm1 $(PLUGIN)/graphic0.sm1
##PLUGIN_LINKFLAG = -lx11
###### end of example.   If you do not use plugins, remove the following lines
######                   which starting with PLUGIN*
####  (2) cmo, file2, socket
#### Note : if you change below, make the same changes at AAA.
PLUGIN = ../plugin
PLUGIN_PROTOTYPE  = $(PLUGIN)/mathcap.h $(PLUGIN)/file2.h $(PLUGIN)/sm1Socket.h  $(PLUGIN)/cmo.h $(PLUGIN)/ox.h
PLUGIN_LIB  = $(PLUGIN)/sm1Socket.a  $(PLUGIN)/cmo.a $(PLUGIN)/file2.a $(PLUGIN)/ox.a
PLUGIN_EXT = $(PLUGIN)/file2.hh $(PLUGIN)/sm1Socket.hh $(PLUGIN)/cmo.hh  $(PLUGIN)/ox.hh
PLUGIN_SM1 = $(PLUGIN)/sm1Socket.sm1 $(PLUGIN)/file2.sm1 $(PLUGIN)/cmo.sm1 $(PLUGIN)/oxx.sm1
PLUGIN_LINKFLAG = 
####  AAA (2) cmo, file2, socket
#PLUGIN = ../plugin
#PLUGIN_PROTOTYPE  = $(PLUGIN)/mathcap.h $(PLUGIN)/file2.h $(PLUGIN)/sm1Socket.h  $(PLUGIN)/cmo.h $(PLUGIN)/ox.h
#PLUGIN_LIB  = $(PLUGIN)/sm1Socket.a  $(PLUGIN)/cmo.a $(PLUGIN)/file2.a $(PLUGIN)/ox.a
#PLUGIN_EXT = $(PLUGIN)/file2.hh $(PLUGIN)/sm1Socket.hh $(PLUGIN)/cmo.hh  $(PLUGIN)/ox.hh
#PLUGIN_SM1 = $(PLUGIN)/sm1Socket.sm1 $(PLUGIN)/file2.sm1 $(PLUGIN)/cmo.sm1 $(PLUGIN)/oxx.sm1
#PLUGIN_LINKFLAG = 
####  (3)  cmo only
#PLUGIN = ../plugin
#PLUGIN_PROTOTYPE  = $(PLUGIN)/mathcap.h $(PLUGIN)/cmo.h
#PLUGIN_LIB  = $(PLUGIN)/cmo.a
#PLUGIN_EXT = $(PLUGIN)/cmo.hh 
#PLUGIN_SM1 = 
#PLUGIN_LINKFLAG = 
###  (4) sm1 with asir, file2, socket cmo. 
###      You also need the special GC for asir.
#PLUGIN = ../plugin
#PLUGIN_PROTOTYPE  = $(PLUGIN)/mathcap.h $(PLUGIN)/file2.h $(PLUGIN)/sample.h $(PLUGIN)/sm1Socket.h $(PLUGIN)/asir.h $(PLUGIN)/cmo.h
#PLUGIN_LIB  = $(PLUGIN)/file2.a $(PLUGIN)/sample.a $(PLUGIN)/sm1Socket.a $(PLUGIN)/libasir.a $(PLUGIN)/asirconv.a $(PLUGIN)/ak.a $(PLUGIN)/cmo.a
#PLUGIN_EXT = $(PLUGIN)/file2.hh $(PLUGIN)/sample.hh $(PLUGIN)/sm1Socket.hh $(PLUGIN)/asir.hh $(PLUGIN)/cmo.hh 
#PLUGIN_SM1 = $(PLUGIN)/file2.sm1 $(PLUGIN)/sample.sm1 $(PLUGIN)/sm1Socket.sm1
#PLUGIN_LINKFLAG = -lpari  -lXaw -lXmu -L/usr/X11R6/lib -lXt -lX11 -lXt -lSM -lICE -lXext -lX11  -lm
############################ end of configuration ########################

#HEADERS = datatype.h  stackm.h lookup.h extern.h  kclass.h Makefile
HEADERS = datatype.h  stackm.h lookup.h extern.h  kclass.h 
.c.o :
	$(CC) -I$(OpenXM_include) $(CFLAGS) $(DDD) -c  $<

kanlibP = kanExport0.o kanExport1.o
kanlibS1 = stackmachine.o 
kanlibS2 = scanner.o scanner2.o list.o global.o kclass.o
kanlibS3 = usage.o option.o
kanlibS4 = primitive.o
kanlibK = matrix.o poly4.o replace.o poly3.o poly.o output.o coeff.o order.o switch.o poly2.o parser.o parserpass0.o rat.o ext.o
kanlibK2 = gradedset.o red.o gb.o gbGM.o syz0.o redm.o sugar.o resol.o hilbert.o
kanlibKclass = Kclass/sample.o Kclass/rawcmo.o Kclass/indeterminate.o

##### Add also to Kclass/Makefile
Kclass/sample.o : $(HEADERS) Kclass/sample.c
	(cd Kclass ; make CC=$(CC) GMP_HOME2=$(GMP_HOME2) CFLAGS=$(CFLAGS) DDD=$(DDD) sample.o )
Kclass/rawcmo.o : $(HEADERS) Kclass/rawcmo.c
	(cd Kclass ; make CC=$(CC) GMP_HOME2=$(GMP_HOME2) CFLAGS=$(CFLAGS) DDD=$(DDD) rawcmo.o )
Kclass/indeterminate.o : $(HEADERS) Kclass/indeterminate.c
	(cd Kclass ; make CC=$(CC) GMP_HOME2=$(GMP_HOME2) CFLAGS=$(CFLAGS) DDD=$(DDD) indeterminate.o )


######
redm.o : extern2.h gradedset.h $(HEADERS)
sugar.o : extern2.h gradedset.h $(HEADERS)
syz0.o : extern2.h gradedset.h $(HEADERS)
resol.o : extern2.h gradedset.h $(HEADERS)
hilbert.o : extern2.h gradedset.h $(HEADERS)
gbGM.o : extern2.h gradedset.h $(HEADERS)
gb.o : extern2.h gradedset.h $(HEADERS)
red.o : extern2.h gradedset.h $(HEADERS)
gradedset.o : extern2.h gradedset.h $(HEADERS)

########
matrix.o : extern2.h matrix.h $(HEADERS)
poly4.o : extern2.h matrix.h $(HEADERS)
replace3.o : extern2.h $(HEADERS)
poly3.o : extern2.h $(HEADERS)
parserpass0.o : extern2.h $(HEADERS)
parser.o : extern2.h $(HEADERS)
poly2.o : extern2.h $(HEADERS)
switch.o : extern2.h $(HEADERS) gradedset.h
order.o : extern2.h $(HEADERS)
coeff.o : extern2.h $(HEADERS)
output.o : extern2.h $(HEADERS)
poly.o : extern2.h $(HEADERS)
rat.o : extern2.h $(HEADERS)
ext.o : extern2.h $(HEADERS) plugin.h plugin.hh

MSG0 = msg0.o

sm1 : sm.c  $(kanlibS1) $(kanlibS2) $(kanlibS3) $(kanlibS4) $(kanlibP) $(kanlibK) $(kanlibK2) $(kanlibKclass) $(PLUGIN_LIB) ${GC} 
	if test `uname` = "SunOS" ; \
	then \
	$(CC) -I$(OpenXM_include) $(CFLAGS) $(DDD) -o sm1 sm.c $(kanlibS1) $(kanlibS2) $(kanlibS3) $(kanlibS4) $(kanlibP) $(kanlibK) $(kanlibK2)  $(kanlibKclass) $(PLUGIN_LIB) $(LIBGMP) ${GC} $(PLUGIN_LINKFLAG) $(SOLARISLIB) ; \
	else \
	$(CC) ${STATIC_LINK_FLAG} -I$(OpenXM_include) $(CFLAGS) $(DDD) -o sm1 sm.c $(kanlibS1) $(kanlibS2) $(kanlibS3) $(kanlibS4) $(kanlibP) $(kanlibK) $(kanlibK2)  $(kanlibKclass) $(PLUGIN_LIB) $(LIBGMP) ${GC} $(PLUGIN_LINKFLAG)  ; \
	fi


$(kanlibP) : $(HEADERS)  gradedset.h extern2.h
$(kanlibS1) : $(HEADERS) smacro.h  gradedset.h
$(kanlibS2) : $(HEADERS) gradedset.h extern2.h
$(kanlibS3) : $(HEADERS) gradedset.h extern2.h
$(kanlibS4) : $(HEADERS) gradedset.h


kanlib.a : $(kanlibS1) $(kanlibS2) $(kanlibS3) $(kanlibS4) $(kanlibP) $(kanlibK) $(kanlibK2) $(kanlibKclass) $(PLUGIN_LIB) kanlib.o
	(cd plugin-tmp; /bin/rm -f *.o; ../ar-decomp $(PLUGIN_LIB) ; cp ../kanlib.o .)
	ar ru kanlib.a $(kanlibS1) $(kanlibS2) $(kanlibS3) $(kanlibS4) $(kanlibP) $(kanlibK) $(kanlibK2) $(kanlibKclass) plugin-tmp/*.o
	../gc/if_not_there ../gc/on_sparc_sunos5 ranlib kanlib.a

##sm1 : sm.c kanlib.a  $(GC)
##	$(CC) -I$(OpenXM_include) $(CFLAGS) $(DDD) -o sm1 sm.c kanlib.a $(LIBGMP) $(GC) $(SOLARISLIB) $(PLUGIN_LINKFLAG)


############ smacro.h  and lookup.h
smacro.h : smacro.sm1 dr.sm1 module1.sm1 sugar.sm1 usage.sm1 mm $(PLUGIN_SM1)
	touch smacro.h
	/bin/rm smacro.h
	cat smacro.sm1 dr.sm1 module1.sm1 sugar.sm1 usage.sm1 $(PLUGIN_SM1) | ./mm >smacro.h

lookup.h : stackm.h mklookup
	touch lookup.h
	/bin/rm lookup.h
	./mklookup >lookup.h

mm : mm.c
	$(CC) -o mm mm.c
mklookup : mklookup.c
	$(CC) -I$(OpenXM_include) -o mklookup mklookup.c

############# sm1 stack machine without algebraic part.
sm00 : sm.c  $(kanlibS1) $(kanlibS2)  $(kanlibP) $(PLUGIN_LIB) ${GC} 
	$(CC) -I$(OpenXM_include) $(CFLAGS) $(DDD) -o sm00 sm.c $(kanlibS1) $(kanlibS2) $(PLUGIN_LIB) $(LIBGMP) ${GC} $(PLUGIN_LINKFLAG) $(SOLARISLIB)

############### plugins
plugin.hh  : Makefile $(PLUGIN_EXT)
	rm -f plugin.hh
	echo '/* Do not edit this file. */' > plugin.hh
	./cat0 $(PLUGIN_EXT) >>plugin.hh

plugin.h : Makefile $(PLUGIN_PROTOTYPE)
	rm -f plugin.h
	echo '/* Do not edit this file. */' > plugin.h
	./cat0 $(PLUGIN_PROTOTYPE) >>plugin.h

plugin.sm1 : Makefile $(PLUGIN_SM1)
	rm -f plugin.sm1
	echo '%% Do not edit this file.' > plugin.sm1

install-lib: kanlib.a
	cp ../plugin/kancmo.h /usr/src/local/yama
	cp kanlib.a   /usr/src/local/yama/kanlib.a


clean :
	rm -f mklookup mm  core sm1 lookup.h smacro.h *.o *~ *.a mon.out sm1.nogc kanlib.a plugin.h plugin.hh plugin-tmp/*.o Kclass/*.o Kclass/*~

cleana :
	rm -f mklookup mm  core sm1 lookup.h smacro.h *.o *~ *.a mon.out sm1.nogc kanlib.a plugin.h plugin.hh  plugin-tmp/*.o  Kclass/*.o Kclass/*~
