#$OpenXM: OpenXM/doc/OpenXM-specs/Makefile,v 1.5 2000/01/24 07:36:35 noro Exp $
all : doc

OXWEAVE=../../src/kxx/oxweave
OXWEAVEFLAG=--recursive --plain
SRC= OpenXM.tex cmo-basic0.tex cmo-basic1.tex\
cmo-register.tex communication-model.tex control.tex\
error.tex formal-expression.tex howtowrite.tex\
implementation.tex introduction.tex library.tex\
ox-localdata.tex project.tex redefine.tex\
stackmachine.tex start-session.tex string-expression.tex

doc: OpenXM-eg.dvi OpenXM-jp.dvi

OpenXM-eg.dvi: $(OXWEAVE) $(SRC)
	-rm -rf tmpEG
	-mkdir tmpEG
	-for i in *.tex; do $(OXWEAVE) $(OXWEAVEFLAG) C eg < $$i >tmpEG/$$i; done
	(cd tmpEG; ln -s ../reset.eps .; latex OpenXM; latex OpenXM)
	-mv tmpEG/OpenXM.dvi OpenXM-eg.dvi
	touch OpenXM-eg.dvi

OpenXM-jp.dvi: $(OXWEAVE) $(SRC)
	-rm -rf tmpJP
	-mkdir tmpJP
	-for i in *.tex; do $(OXWEAVE) $(OXWEAVEFLAG) C jp < $$i >tmpJP/$$i; done
	-(cd tmpJP; ln -s ../reset.eps .; platex OpenXM; platex OpenXM)
	-mv tmpJP/OpenXM.dvi OpenXM-jp.dvi
	touch OpenXM-jp.dvi

$(OXWEAVE) : $(OXWEAVE).c
	(cd ../../src/kxx ; make oxweave)

clean :
	/bin/rm -rf tmpEG tmpJP


