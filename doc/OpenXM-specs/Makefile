#$OpenXM: OpenXM/doc/OpenXM-specs/Makefile,v 1.8 2000/09/09 03:01:24 takayama Exp $
all : doc

OXWEAVE=../../src/kxx/oxweave
OXWEAVEFLAG=--recursive --plain
SRC= OpenXM.tex cmo-basic0.tex cmo-basic1.tex\
cmo-register.tex communication-model.tex control.tex\
error.tex formal-expression.tex howtowrite.tex\
implementation.tex introduction.tex library.tex\
ox-localdata.tex project.tex redefine.tex\
stackmachine.tex start-session.tex string-expression.tex

SRC101=OX-RFC-101.tex

doc: OpenXM-en.dvi OpenXM-ja.dvi OX-RFC-101-en.dvi OX-RFC-101-ja.dvi

OpenXM-en.dvi: $(OXWEAVE) $(SRC)
	-rm -rf tmpEN
	-mkdir tmpEN
	-for i in $(SRC); do $(OXWEAVE) $(OXWEAVEFLAG) C eg < $$i >tmpEN/$$i; done
	(cd tmpEN; ln -s ../reset.eps .; latex OpenXM; latex OpenXM)
	-mv tmpEN/OpenXM.dvi OpenXM-en.dvi
	touch OpenXM-en.dvi

OpenXM-ja.dvi: $(OXWEAVE) $(SRC)
	-rm -rf tmpJA
	-mkdir tmpJA
	-for i in $(SRC); do $(OXWEAVE) $(OXWEAVEFLAG) C jp < $$i >tmpJA/$$i; done
	-(cd tmpJA; ln -s ../reset.eps .; platex OpenXM; platex OpenXM)
	-mv tmpJA/OpenXM.dvi OpenXM-ja.dvi
	touch OpenXM-ja.dvi

OX-RFC-101-en.dvi: $(OXWEAVE) $(SRC101)
	-rm -rf tmp101EN
	-mkdir tmp101EN
	-for i in $(SRC101); do $(OXWEAVE) $(OXWEAVEFLAG) C eg < $$i >tmp101EN/$$i; done
	(cd tmp101EN; latex OX-RFC-101; latex OX-RFC-101)
	-mv tmp101EN/OX-RFC-101.dvi OX-RFC-101-en.dvi
	touch OX-RFC-101-en.dvi

OX-RFC-101-ja.dvi: $(OXWEAVE) $(SRC101)
	-rm -rf tmp101JA
	-mkdir tmp101JA
	-for i in $(SRC101); do $(OXWEAVE) $(OXWEAVEFLAG) C jp < $$i | nkf -e >tmp101JA/$$i; done
	-(cd tmp101JA;  platex OX-RFC-101; platex OX-RFC-101)
	-mv tmp101JA/OX-RFC-101.dvi OX-RFC-101-ja.dvi
	touch OX-RFC-101-ja.dvi

$(OXWEAVE) : $(OXWEAVE).c
	(cd ../../src/kxx ; make oxweave)

clean :
	/bin/rm -rf tmpEN tmpJA tmp101EN tmp101JA
	/bin/rm -f OpenXM-en.dvi OpenXM-ja.dvi OX-RFC-101-en.dvi OX-RFC-101-ja.dvi


