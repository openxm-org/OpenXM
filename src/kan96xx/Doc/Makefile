########## Documents
all : ttt.tex onlinehelp.tex
	echo "The document kan/example is in the file ttt.tex."
	echo "onlinehelp.tex is a collection of online help."
ttt.dvi : ttt.tex
	latex ttt ; latex ttt

ttt.tex : ex.tex ole cv0.sm1 cv2.sm1 gbelim.sm1 gblex.sm1 gbrev.sm1 gbrevsim.sm1
	/bin/rm -f ttt.tex
	./ole <ex.tex >ttt.tex

onlinehelp.tex :
	./printOnlineHelp
	latex onlinehelp ; latex onlinehelp

ole : ole.c

########### kan/library
CC = gcc
KAN_HOME = ../Kan
GMP_HOME = ../gmp
LIBGMP = $(GMP_HOME)/libgmp.a
## This line is for buggy linux-gmp.
#LIBGMP = $(GMP_HOME)/libgmp.a $(GMP_HOME)/mpn/libmpn.a
GC = ../gc/gc.a
#GC = /fuji/taka/noro/gc.org.a
kanlib1 : kanlib1.c 
	(cd $(KAN_HOME); make kanlib.a)
	$(CC) -I$(GMP_HOME) -I$(KAN_HOME) -o kanlib1 kanlib1.c $(KAN_HOME)/kanlib.a $(LIBGMP) $(GC)

########## cleaning
clean:
	/bin/rm ttt.* ole *.dvi *.aux *.log *~ kanlib1 onlinehelp.tex a.out bin/ox bin/oxlog bin/ox_sm1

clean2:
	/bin/rm ttt.* ole *.dvi *.aux *.log *~ kanlib1 onlinehelp.tex a.out 
