#$OpenXM: OpenXM/doc/OpenXM-specs/Makefile,v 1.2 2000/01/21 06:24:49 noro Exp $
all : doc

OXWEAVE=../../src/kxx/oxweave
OXWEAVEFLAG=--recursive --plain
doc:  $(OXWEAVE) OpenXM.tex
	-rm -rf tmpEG
	-mkdir tmpEG
	-for i in *.tex; do $(OXWEAVE) $(OXWEAVEFLAG) C eg < $$i >tmpEG/$$i; done
	(cd tmpEG; ln -s ../reset.eps .; platex OpenXM)
	-rm -rf tmpJP
	-mkdir tmpJP
	-for i in *.tex; do $(OXWEAVE) $(OXWEAVEFLAG) C jp < $$i >tmpJP/$$i; done
	-(cd tmpJP; ln -s ../reset.eps .; platex OpenXM)

$(OXWEAVE) : $(OXWEAVE).c
	(cd ../../src/kxx ; make oxweave)

clean :
	/bin/rm -rf tmpEG tmpJP


