#$OpenXM: OpenXM/doc/OpenXM-specs/Makefile,v 1.4 2000/01/23 05:28:33 noro Exp $
all : doc

OXWEAVE=../../src/kxx/oxweave
OXWEAVEFLAG=--recursive --plain
doc: doc-jp doc-eg
doc-eg: $(OXWEAVE) OpenXM.tex
	-rm -rf tmpEG
	-mkdir tmpEG
	-for i in *.tex; do $(OXWEAVE) $(OXWEAVEFLAG) C eg < $$i >tmpEG/$$i; done
	(cd tmpEG; ln -s ../reset.eps .; latex OpenXM)
doc-jp: $(OXWEAVE) OpenXM.tex
	-rm -rf tmpJP
	-mkdir tmpJP
	-for i in *.tex; do $(OXWEAVE) $(OXWEAVEFLAG) C jp < $$i >tmpJP/$$i; done
	-(cd tmpJP; ln -s ../reset.eps .; platex OpenXM)

$(OXWEAVE) : $(OXWEAVE).c
	(cd ../../src/kxx ; make oxweave)

clean :
	/bin/rm -rf tmpEG tmpJP


