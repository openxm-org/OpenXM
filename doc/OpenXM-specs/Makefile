#$OpenXM: OpenXM/doc/OpenXM-specs/Makefile,v 1.7 2000/01/24 12:32:49 noro Exp $
all : doc

OXWEAVE=../../src/kxx/oxweave
OXWEAVEFLAG=--recursive --plain
SRC= OpenXM.tex cmo-basic0.tex cmo-basic1.tex\
cmo-register.tex communication-model.tex control.tex\
error.tex formal-expression.tex howtowrite.tex\
implementation.tex introduction.tex library.tex\
ox-localdata.tex project.tex redefine.tex\
stackmachine.tex start-session.tex string-expression.tex

doc: OpenXM-en.dvi OpenXM-ja.dvi

OpenXM-en.dvi: $(OXWEAVE) $(SRC)
	-rm -rf tmpEN
	-mkdir tmpEN
	-for i in *.tex; do $(OXWEAVE) $(OXWEAVEFLAG) C eg < $$i >tmpEN/$$i; done
	(cd tmpEN; ln -s ../reset.eps .; latex OpenXM; latex OpenXM)
	-mv tmpEN/OpenXM.dvi OpenXM-en.dvi
	touch OpenXM-en.dvi

OpenXM-ja.dvi: $(OXWEAVE) $(SRC)
	-rm -rf tmpJA
	-mkdir tmpJA
	-for i in *.tex; do $(OXWEAVE) $(OXWEAVEFLAG) C jp < $$i >tmpJA/$$i; done
	-(cd tmpJA; ln -s ../reset.eps .; platex OpenXM; platex OpenXM)
	-mv tmpJA/OpenXM.dvi OpenXM-ja.dvi
	touch OpenXM-ja.dvi

$(OXWEAVE) : $(OXWEAVE).c
	(cd ../../src/kxx ; make oxweave)

clean :
	/bin/rm -rf tmpEN tmpJA
	/bin/rm -f OpenXM-en.dvi OpenXM-ja.dvi


