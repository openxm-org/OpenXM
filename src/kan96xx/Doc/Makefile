## $OpenXM: OpenXM/src/kan96xx/Doc/Makefile,v 1.2 2000/01/15 22:45:32 takayama Exp $
########## Documents
OpenXM_ROOT=../../../../OpenXM
OpenXM_doc=$(OpenXM_ROOT)/doc
DVIPS=dvips -f

all : ttt.tex onlinehelp.tex
	echo "The document kan/example is in the file ttt.tex."
	echo "onlinehelp.tex is a collection of online help."
ttt.dvi : ttt.tex
	-latex ttt 
	-latex ttt

ttt.tex : ex.tex ole cv0.sm1 cv2.sm1 gbelim.sm1 gblex.sm1 gbrev.sm1 gbrevsim.sm1
	/bin/rm -f ttt.tex
	./ole <ex.tex >ttt.tex

onlinehelp.tex :
	-./printOnlineHelp

onlinehelp.dvi : onlinehelp.tex
	-latex onlinehelp 
	-latex onlinehelp

ole : ole.c

###
install-document : onlinehelp.dvi ttt.dvi clean-OpenXM-doc
	mkdir $(OpenXM_doc)/kan96xx
	touch $(OpenXM_doc)/kan96xx/DO_NOT_EDIT_THIS_DIRECTORY._ALL_FILES_ARE_AUTOMATICDALLY_GENERATED.
	-cp ttt.tex $(OpenXM_doc)/kan96xx/intro.tex
	-cp onlinehelp.tex $(OpenXM_doc)/kan96xx/onlinehelp.tex
	-$(DVIPS)  ttt >$(OpenXM_doc)/kan96xx/intro.ps
	-$(DVIPS) onlinehelp >$(OpenXM_doc)/kan96xx/onlinehelp.ps

clean-OpenXM-doc : 
	-/bin/rm -rf $(OpenXM_doc)/kan96xx

########## cleaning
clean:
	-/bin/rm ttt.* ole *.dvi *.aux *.log *~ kanlib1 onlinehelp.tex a.out bin/ox bin/oxlog bin/ox_sm1

clean-document:
	-/bin/rm ttt.* ole *.dvi *.aux *.log *~ kanlib1 onlinehelp.tex a.out 

########### kan/library
CC=gcc
KAN_HOME=../Kan
GMP_HOME=../gmp
LIBGMP=$(GMP_HOME)/libgmp.a
## This line is for buggy linux-gmp.
#LIBGMP = $(GMP_HOME)/libgmp.a $(GMP_HOME)/mpn/libmpn.a
GC = ../gc/gc.a
#GC = /fuji/taka/noro/gc.org.a
kanlib1 : kanlib1.c 
	(cd $(KAN_HOME); make kanlib.a)
	$(CC) -I$(GMP_HOME) -I$(KAN_HOME) -o kanlib1 kanlib1.c $(KAN_HOME)/kanlib.a $(LIBGMP) $(GC)
