## $OpenXM: OpenXM/src/kan96xx/Doc/Makefile,v 1.8 2003/09/07 08:58:03 takayama Exp $
########## Documents
OpenXM_HOME=../../..
OpenXM_doc=$(OpenXM_HOME)/doc
OpenXM_lib=${OpenXM_HOME}/lib
DVIPS=dvips -f
CC=gcc
LATEX2HTML=latex2html

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
	-./printOnlineHelp ${OpenXM_lib}

onlinehelp.dvi : onlinehelp.tex
	-latex onlinehelp 
	-latex onlinehelp

ole : ole.c
	${CC} -o ole ole.c

make-html-document : onlinehelp.tex ttt.tex
	-${LATEX2HTML} onlinehelp
	-${LATEX2HTML} ttt

###
install-document :
	@if [ ! -f ./.done_install-document ] ; \
	then \
		make install-document2 ; \
		touch ./.done_install-document ; \
	else \
		echo "NOTE: Install-document is already done. Remove .done_install-document for a new install-document." ; \
	fi

install-document2 : onlinehelp.dvi ttt.dvi clean-OpenXM-doc make-html-document
	mkdir $(OpenXM_doc)/kan96xx
	touch $(OpenXM_doc)/kan96xx/DO_NOT_EDIT_THIS_DIRECTORY._ALL_FILES_ARE_AUTOMATICDALLY_GENERATED.
	-cp ttt.tex $(OpenXM_doc)/kan96xx/intro.tex
	-cp onlinehelp.tex $(OpenXM_doc)/kan96xx/onlinehelp.tex
	-$(DVIPS)  ttt >$(OpenXM_doc)/kan96xx/intro.ps
	-$(DVIPS) onlinehelp >$(OpenXM_doc)/kan96xx/onlinehelp.ps
	-cp -r onlinehelp $(OpenXM_doc)/kan96xx
	-cp -r ttt $(OpenXM_doc)/kan96xx

clean-OpenXM-doc : 
	-/bin/rm -rf $(OpenXM_doc)/kan96xx

########## cleaning
clean:
	-/bin/rm -rf ttt.* ole *.dvi *.aux *.log *~ kanlib1 onlinehelp.tex a.out bin/ox bin/oxlog bin/ox_sm1 onlinehelp ttt ./.done_install-document

clean-document:
	-/bin/rm -rf ttt.* ole *.dvi *.aux *.log *~ kanlib1 onlinehelp.tex a.out onlinehelp ttt ./.done_install-document

distclean: clean clean-document
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

