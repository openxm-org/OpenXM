#$OpenXM: OpenXM/doc/OpenXM-specs/Makefile,v 1.1.1.1 2000/01/20 08:52:46 noro Exp $
all : doc

OXWEAVE=../../src/kxx/oxweave
OXWEAVEFLAG=--recursive --plain
doc:  $(OXWEAVE) OpenXM.tex
	-rm -rf tmpEG
	-mkdir tmpEG
	-for i in *.tex; do $(OXWEAVE) $(OXWEAVEFLAG) C eg < $$i >tmpEG/$$i; done
	-rm -rf tmpJP
	-mkdir tmpJP
	-for i in *.tex; do $(OXWEAVE) $(OXWEAVEFLAG) C jp < $$i >tmpJP/$$i; done

$(OXWEAVE) : $(OXWEAVE).c
	(cd ../../src/kxx ; make oxweave)

clean :
	/bin/rm -rf tmpEG tmpJP


