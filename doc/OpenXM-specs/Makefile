#$OpenXM: OpenXM/doc/OpenXM-specs/Makefile,v 1.16 2003/08/22 23:56:27 takayama Exp $
all : doc

LATEX2HTML=latex2html -tmp /tmp
JFIX=${OpenXM_HOME}/src/util/sample1.sh
DVIPS=dvips -f
RSYNC=rsync -Cavz
CURRENT=/home/web/OpenXM/Current
NKF=nkf -e

OXWEAVE=../../bin/oxweave
OXWEAVE_C=../../src/util/oxweave.c
OXWEAVEFLAG=--recursive --plain
OXREPLACE=../../src/util/oxreplace
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
	-for i in $(SRC); do $(OXWEAVE) $(OXWEAVEFLAG) C jp < $$i | ${NKF} >tmpJA/$$i; done
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
	-for i in $(SRC101); do $(OXWEAVE) $(OXWEAVEFLAG) C jp < $$i | ${NKF} >tmp101JA/$$i; done
	-(cd tmp101JA;  platex OX-RFC-101; platex OX-RFC-101)
	-mv tmp101JA/OX-RFC-101.dvi OX-RFC-101-ja.dvi
	touch OX-RFC-101-ja.dvi

$(OXWEAVE) : $(OXWEAVE_C)
	(cd ../../src/util ; make oxweave)

all-html: all-html-ja all-html-en
all-html-en: doc
	if test ! -f .all-html-100-en.done ; \
	then \
		(cd tmpEN ; latex OpenXM; latex OpenXM ; $(LATEX2HTML) OpenXM ) ; \
	    (cd tmpEN/OpenXM ; $(JFIX) *.html ; rm -f *-tmp.old ) ; \
		touch .all-html-100-en.done ; \
	fi
	if test ! -f .all-html-101-en.done ; \
	then \
		(cd tmp101EN ; latex OX-RFC-101; latex OX-RFC-101 ; $(LATEX2HTML) OX-RFC-101 ) ; \
	    (cd tmp101EN/OX-RFC-101 ; $(JFIX) *.html ; rm -f *-tmp.old ) ; \
		touch .all-html-101-en.done ; \
	fi

all-html-ja: doc 
	if test ! -f .all-html-100-ja.done ; \
	then \
		(cd tmpJA; platex OpenXM; platex OpenXM ; $(LATEX2HTML) OpenXM) ; \
		(cd tmpJA/OpenXM; ../../${OXREPLACE} --old iso-8859-1 --new euc-jp *.html) ; \
		touch .all-html-100-ja.done ; \
	fi
	if test ! -f .all-html-101-ja.done ; \
	then \
		(cd tmp101JA; platex OX-RFC-101; platex OX-RFC-101 ; $(LATEX2HTML) OX-RFC-101 ) ; \
		(cd tmp101JA/OX-RFC-101; ../../${OXREPLACE} --old iso-8859-1 --new euc-jp *.html) ; \
		touch .all-html-101-ja.done ; \
	fi


install-web-all: all-html
	if test ! -d ${CURRENT}/doc ; then mkdir ${CURRENT}/doc ; fi  
	if test ! -d ${CURRENT}/doc/OX-RFC-100-en ; then mkdir ${CURRENT}/doc/OX-RFC-100-en ; fi  
	if test ! -d ${CURRENT}/doc/OX-RFC-100-ja ; then mkdir ${CURRENT}/doc/OX-RFC-100-ja ; fi 
	if test ! -d ${CURRENT}/doc/OX-RFC-101-en ; then mkdir ${CURRENT}/doc/OX-RFC-101-en ; fi 
	if test ! -d ${CURRENT}/doc/OX-RFC-101-ja ; then mkdir ${CURRENT}/doc/OX-RFC-101-ja ; fi 
	(cd tmpEN; ${RSYNC} OpenXM ${CURRENT}/doc/OX-RFC-100-en)
	(cd tmpJA; ${RSYNC} OpenXM ${CURRENT}/doc/OX-RFC-100-ja)
	(cd tmp101JA; ${RSYNC} OX-RFC-101 ${CURRENT}/doc/OX-RFC-101-ja)
	(cd tmp101EN; ${RSYNC} OX-RFC-101 ${CURRENT}/doc/OX-RFC-101-en)
	if test ! -f .install-ps-done ; then \
	(cd tmpEN ; $(DVIPS) OpenXM >${CURRENT}/openxm-en.ps) ; \
	(cd tmpJA ; $(DVIPS) OpenXM >${CURRENT}/openxm-ja.ps) ; \
	touch .install-ps-done ; \
	fi


clean : clean-done
	/bin/rm -rf tmpEN tmpJA tmp101EN tmp101JA
	/bin/rm -f OpenXM-en.dvi OpenXM-ja.dvi OX-RFC-101-en.dvi OX-RFC-101-ja.dvi
clean-done:
	/bin/rm -f .[a-z]*done
clean-en-done:
	/bin/rm -f .[a-z]*-en.done

