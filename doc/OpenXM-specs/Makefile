#$OpenXM$
all : doc

OXWEAVE=../../src/kxx/oxweave
OXWEAVEFLAG=--recursive
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


