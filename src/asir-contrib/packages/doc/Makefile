#$OpenXM: OpenXM/src/asir-contrib/packages/doc/Makefile,v 1.26 2002/02/10 08:24:01 takayama Exp $
# targets
# all :   
#        Extracts all tex-info files from ../src/*
# install : install-document install-help
# install-help : all
#        Installing online help to OpenXM/lib/asir/help-*
# install-document :  all
#        Copy the documents to OpenXM/doc/asir-contrib
# install-html-help
#        Copy html help to OpenXM/doc/asir-contrib
#
# NOTE: install-help should be executed after installing asir2000.
OpenXM_ROOT=../../../../../OpenXM
ASIRDOC=$(OpenXM_ROOT)/src/asir-doc
ASIRLIB=$(OpenXM_ROOT)/../OpenXM_contrib2/asir2000/lib
CONTRIB=$(OpenXM_ROOT)/src/asir-contrib/packages/src
OPTGENTEXI= --include 
#OPTGENTEXI= --include --example
## PS documents are installed under $(OpenXM_doc)/asir-contrib
OpenXM_doc=$(OpenXM_ROOT)/doc
DVIPS=dvips -f
OXWEAVE= ../../../kxx/oxweave
TEXI2HTML=texi2html -menu -split_node

JPSRCS= oxxm.texi oxmath.texi oxphc.texi oxgnuplot.texi oxsm1.texi oxom.texi dsolv-jp.texi longname-jp.texi oxtigers-jp.texi names-ja.texi hypergeometric-ja.texi number-ja.texi calculus-ja.texi series-ja.texi matrix-ja.texi  graphic-ja.texi print-ja.texi poly-ja.texi complex-ja.texi intro.texi Diff.texi Dmodule.texi Matrix.texi base-ja.texi misc-ja.texi
EGSRCS= oxxm-eg.texi oxmath-eg.texi oxphc-eg.texi oxgnuplot-eg.texi oxsm1-eg.texi oxom-eg.texi dsolv-eg.texi longname-eg.texi oxtigers-eg.texi names-en.texi hypergeometric-en.texi number-en.texi calculus-en.texi series-en.texi matrix-en.texi  graphic-en.texi print-en.texi poly-en.texi complex-en.texi intro-eg.texi cman-eg.texi base-en.texi misc-en.texi

JAAUTO=base-auto-ja.texi pfp-auto-ja.texi poly-auto-ja.texi \
       print-auto-ja.texi matrix-auto-ja.texi
ENAUTO=base-auto-en.texi pfp-auto-en.texi poly-auto-en.texi \
       print-auto-en.texi matrix-auto-en.texi

all : gentexi $(JPSRCS) $(EGSRCS) $(JAAUTO) $(ENAUTO)
install : install-document install-help install-html-help

gentexi : gentexi.c
	gcc -o gentexi gentexi.c

base-ja.texi : base.oxweave
	/bin/rm -f base-ja.texi
	$(OXWEAVE) C ja  <base.oxweave >base-ja.texi

base-en.texi : base.oxweave
	/bin/rm -f base-en.texi
	$(OXWEAVE) C en  <base.oxweave >base-en.texi

names-ja.texi : names.oxweave
	/bin/rm -f names-ja.texi
	$(OXWEAVE) C ja  <names.oxweave >names-ja.texi

names-en.texi : names.oxweave
	/bin/rm -f names-en.texi
	$(OXWEAVE) C en  <names.oxweave >names-en.texi

plucker-ja.texi : plucker.oxweave
	/bin/rm -f plucker-ja.texi
	$(OXWEAVE) C ja  <plucker.oxweave >plucker-ja.texi

plucker-en.texi : plucker.oxweave
	/bin/rm -f plucker-en.texi
	$(OXWEAVE) C en  <plucker.oxweave >plucker-en.texi

hypergeometric-ja.texi : hypergeometric.oxweave
	/bin/rm -f hypergeometric-ja.texi
	$(OXWEAVE) C ja  <hypergeometric.oxweave >hypergeometric-ja.texi

hypergeometric-en.texi : hypergeometric.oxweave
	/bin/rm -f hypergeometric-en.texi
	$(OXWEAVE) C en  <hypergeometric.oxweave >hypergeometric-en.texi

number-ja.texi : number.oxweave
	/bin/rm -f number-ja.texi
	$(OXWEAVE) C ja  <number.oxweave >number-ja.texi

number-en.texi : number.oxweave
	/bin/rm -f number-en.texi
	$(OXWEAVE) C en  <number.oxweave >number-en.texi

calculus-ja.texi : calculus.oxweave
	/bin/rm -f calculus-ja.texi
	$(OXWEAVE) C ja  <calculus.oxweave >calculus-ja.texi

calculus-en.texi : calculus.oxweave
	/bin/rm -f calculus-en.texi
	$(OXWEAVE) C en  <calculus.oxweave >calculus-en.texi

series-ja.texi : series.oxweave
	/bin/rm -f series-ja.texi
	$(OXWEAVE) C ja  <series.oxweave >series-ja.texi

series-en.texi : series.oxweave
	/bin/rm -f series-en.texi
	$(OXWEAVE) C en  <series.oxweave >series-en.texi

matrix-ja.texi : matrix.oxweave
	/bin/rm -f matrix-ja.texi
	$(OXWEAVE) C ja  <matrix.oxweave >matrix-ja.texi

matrix-en.texi : matrix.oxweave
	/bin/rm -f matrix-en.texi
	$(OXWEAVE) C en  <matrix.oxweave >matrix-en.texi

graphic-ja.texi : graphic.oxweave
	/bin/rm -f graphic-ja.texi
	$(OXWEAVE) C ja  <graphic.oxweave >graphic-ja.texi

graphic-en.texi : graphic.oxweave
	/bin/rm -f graphic-en.texi
	$(OXWEAVE) C en  <graphic.oxweave >graphic-en.texi

print-ja.texi : print.oxweave
	/bin/rm -f print-ja.texi
	$(OXWEAVE) C ja  <print.oxweave >print-ja.texi

print-en.texi : print.oxweave
	/bin/rm -f print-en.texi
	$(OXWEAVE) C en  <print.oxweave >print-en.texi

poly-ja.texi : poly.oxweave
	/bin/rm -f poly-ja.texi
	$(OXWEAVE) C ja  <poly.oxweave >poly-ja.texi

poly-en.texi : poly.oxweave
	/bin/rm -f poly-en.texi
	$(OXWEAVE) C en  <poly.oxweave >poly-en.texi

complex-ja.texi : complex.oxweave
	/bin/rm -f complex-ja.texi
	$(OXWEAVE) C ja  <complex.oxweave >complex-ja.texi

complex-en.texi : complex.oxweave
	/bin/rm -f complex-en.texi
	$(OXWEAVE) C en  <complex.oxweave >complex-en.texi


oxxm.texi : $(ASIRLIB)/xm Makefile
	/bin/rm -f oxxm.texi
	$(OXWEAVE) C-texi jp-texi  <$(ASIRLIB)/xm >oxxm.texi

oxxm-eg.texi : $(ASIRLIB)/xm Makefile
	/bin/rm -f oxxm-eg.texi
	$(OXWEAVE) C-texi eg-texi  <$(ASIRLIB)/xm >oxxm-eg.texi

oxmath.texi : $(CONTRIB)/m Makefile
	/bin/rm -f oxmath.texi
	$(OXWEAVE) C-texi jp-texi  <$(CONTRIB)/m >oxmath.texi

oxmath-eg.texi : $(CONTRIB)/m Makefile
	/bin/rm -f oxmath-eg.texi
	$(OXWEAVE) C-texi eg-texi  <$(CONTRIB)/m >oxmath-eg.texi

oxphc.texi : $(CONTRIB)/phc Makefile
	/bin/rm -f oxphc.texi
	$(OXWEAVE) C-texi jp-texi  <$(CONTRIB)/phc >oxphc.texi

oxphc-eg.texi : $(CONTRIB)/phc Makefile
	/bin/rm -f oxphc-eg.texi
	$(OXWEAVE) C-texi eg-texi  <$(CONTRIB)/phc >oxphc-eg.texi

oxgnuplot.texi : $(CONTRIB)/gnuplot Makefile
	/bin/rm -f oxgnuplot.texi
	$(OXWEAVE) C-texi jp-texi  <$(CONTRIB)/gnuplot >oxgnuplot.texi

oxgnuplot-eg.texi : $(CONTRIB)/gnuplot Makefile
	/bin/rm -f oxgnuplot-eg.texi
	$(OXWEAVE) C-texi eg-texi  <$(CONTRIB)/gnuplot >oxgnuplot-eg.texi

oxsm1.texi : sm1.oxweave Makefile
	/bin/rm -f oxsm1.texi
	$(OXWEAVE) C-texi jp-texi  <sm1.oxweave >oxsm1.texi

oxsm1-eg.texi : sm1.oxweave Makefile
	/bin/rm -f oxsm1-eg.texi
	$(OXWEAVE) C-texi eg-texi  <sm1.oxweave >oxsm1-eg.texi

oxom.texi : $(CONTRIB)/om Makefile
	/bin/rm -f oxom.texi
	$(OXWEAVE) C-texi jp-texi  <$(CONTRIB)/om >oxom.texi

oxom-eg.texi : $(CONTRIB)/om Makefile
	/bin/rm -f oxom-eg.texi
	$(OXWEAVE) C-texi eg-texi  <$(CONTRIB)/om >oxom-eg.texi

dsolv-jp.texi : dsolv.oxweave Makefile
	/bin/rm -f dsolv-jp.texi
	$(OXWEAVE) C-texi jp-texi  <dsolv.oxweave >dsolv-jp.texi

dsolv-eg.texi : dsolv.oxweave Makefile
	/bin/rm -f dsolv-eg.texi
	$(OXWEAVE) C-texi eg-texi  <dsolv.oxweave >dsolv-eg.texi

longname-jp.texi : $(CONTRIB)/longname Makefile
	/bin/rm -f longname-jp.texi
	$(OXWEAVE) C-texi jp-texi  <$(CONTRIB)/longname >longname-jp.texi

longname-eg.texi : $(CONTRIB)/longname Makefile
	/bin/rm -f longname-eg.texi
	$(OXWEAVE) C-texi eg-texi  <$(CONTRIB)/longname >longname-eg.texi

oxtigers-jp.texi : $(CONTRIB)/tigers Makefile
	/bin/rm -f oxtigers-jp.texi
	$(OXWEAVE) C-texi jp-texi  <$(CONTRIB)/tigers >oxtigers-jp.texi

oxtigers-eg.texi : $(CONTRIB)/tigers Makefile
	/bin/rm -f oxtigers-eg.texi
	$(OXWEAVE) C-texi eg-texi  <$(CONTRIB)/tigers >oxtigers-eg.texi

### Generating *auto*.texi   cf. JAAUTO,  ENAUTO
## base
base-auto-en.texi : $(CONTRIB)/names.rr gentexi
	rm -f base-auto-en.texi
	$(OXWEAVE) usage  <$(CONTRIB)/names.rr | ./gentexi --category base --en --upnode Top ${OPTGENTEXI} >base-auto-en.texi

base-auto-ja.texi : $(CONTRIB)/names.rr gentexi
	rm -f base-auto-ja.texi
	$(OXWEAVE) usage  <$(CONTRIB)/names.rr | ./gentexi --category base --ja --upnode Top ${OPTGENTEXI} >base-auto-ja.texi

## poly
poly-auto-en.texi : $(CONTRIB)/names.rr gentexi
	rm -f poly-auto-en.texi
	$(OXWEAVE) usage  <$(CONTRIB)/names.rr | ./gentexi --category poly --en --upnode Top ${OPTGENTEXI} >poly-auto-en.texi

poly-auto-ja.texi : $(CONTRIB)/names.rr gentexi
	rm -f poly-auto-ja.texi
	$(OXWEAVE) usage  <$(CONTRIB)/names.rr | ./gentexi --category poly --ja --upnode Top ${OPTGENTEXI} >poly-auto-ja.texi

## print
print-auto-en.texi : $(CONTRIB)/names.rr gentexi
	rm -f print-auto-en.texi
	$(OXWEAVE) usage  <$(CONTRIB)/names.rr | ./gentexi --category print --en --upnode Top ${OPTGENTEXI} >print-auto-en.texi

print-auto-ja.texi : $(CONTRIB)/names.rr gentexi
	rm -f print-auto-ja.texi
	$(OXWEAVE) usage  <$(CONTRIB)/names.rr | ./gentexi --category print --ja --upnode Top ${OPTGENTEXI} >print-auto-ja.texi

## matrix
matrix-auto-en.texi : $(CONTRIB)/names.rr gentexi
	rm -f matrix-auto-en.texi
	$(OXWEAVE) usage  <$(CONTRIB)/names.rr | ./gentexi --category matrix --en --upnode Top ${OPTGENTEXI} >matrix-auto-en.texi

matrix-auto-ja.texi : $(CONTRIB)/names.rr gentexi
	rm -f matrix-auto-ja.texi
	$(OXWEAVE) usage  <$(CONTRIB)/names.rr | ./gentexi --category matrix --ja --upnode Top ${OPTGENTEXI} >matrix-auto-ja.texi

# pfp
pfp-auto-en.texi : $(CONTRIB)/pfpcoh.rr $(CONTRIB)/pfphom.rr gentexi
	rm -f pfp-auto-en.texi
	cat $(CONTRIB)/pfpcoh.rr $(CONTRIB)/pfphom.rr | $(OXWEAVE) usage | ./gentexi  --en --upnode Top ${OPTGENTEXI} >pfp-auto-en.texi

pfp-auto-ja.texi : $(CONTRIB)/pfpcoh.rr $(CONTRIB)/pfphom.rr gentexi
	rm -f pfp-auto-ja.texi
	cat $(CONTRIB)/pfpcoh.rr $(CONTRIB)/pfphom.rr | $(OXWEAVE) usage  | ./gentexi  --ja --upnode Top ${OPTGENTEXI} >pfp-auto-ja.texi

### Generating online manual
gen-help-eg: $(EGSRCS) $(ASIRDOC)/extract_func $(ENAUTO)
	-rm -rf tmp help-eg
	mkdir help-eg tmp
	cp cman-eg.texi $(EGSRCS) $(ENAUTO) tmp
	-(cd tmp; makeinfo --force cman-eg.texi)
	(cd help-eg; for i in ../tmp/asir-contrib*; do ../$(ASIRDOC)/extract_func $$i; done)
## duplicate some help files
	-(cd help-eg; cp sm1_hilbert hilbert_polynomial)

gen-help-jp: $(JPSRCS) $(ASIRDOC)/extract_func $(JAAUTO)
	-rm -rf tmp-ja help-jp
	mkdir help-jp tmp-ja
	for i in cman.texi $(JPSRCS) $(JAAUTO); do nkf -e $$i > tmp-ja/$$i; done
	-(cd tmp-ja; makeinfo --force cman.texi)
	(cd help-jp; for i in ../tmp-ja/asir-contrib*; do ../$(ASIRDOC)/extract_func -j $$i; done)
## duplicate some help files
	-(cd help-jp; cp sm1_hilbert hilbert_polynomial)

gen-html-en: gen-help-eg
	-rm -rf html-en
	-mkdir html-en
	-cp tmp/*.texi html-en
	-(cd html-en; ${TEXI2HTML} cman-eg.texi)

gen-html-ja: gen-help-jp
	-mkdir html-ja
	-cp tmp-ja/*.texi html-ja
	-cp tmp-ja/cman.texi html-ja/cman-ja.texi
	-(cd html-ja; ${TEXI2HTML} cman-ja.texi)

help: gen-help-eg gen-help-jp

install-help: help
	/bin/rm -rf $(OpenXM_ROOT)/lib/asir/help-jp/xm.help
	mkdir $(OpenXM_ROOT)/lib/asir/help-jp/xm.help
	-cp -f help-jp/* $(OpenXM_ROOT)/lib/asir/help-jp/xm.help
	/bin/rm -rf $(OpenXM_ROOT)/lib/asir/help-eg/xm.help
	mkdir $(OpenXM_ROOT)/lib/asir/help-eg/xm.help
	-cp -f help-eg/* $(OpenXM_ROOT)/lib/asir/help-eg/xm.help

install-html-help: gen-html-en gen-html-ja
	-( tar cf - html-en | (cd $(OpenXM_doc)/asir-contrib ; tar xf -))
	-( tar cf - html-ja | (cd $(OpenXM_doc)/asir-contrib ; tar xf -))

install-document : all clean-document
	-ptex cman.texi
	-ptex cman.texi
	-cp cman.dvi $(OpenXM_doc)/asir-contrib/cman-jp.dvi
	-$(DVIPS) cman >$(OpenXM_doc)/asir-contrib/cman-jp.ps
	-tex cman-eg.texi
	-tex cman-eg.texi
	-cp cman-eg.dvi $(OpenXM_doc)/asir-contrib/cman-eg.dvi
	-$(DVIPS) cman-eg >$(OpenXM_doc)/asir-contrib/cman-eg.ps


clean-document :
	/bin/rm -rf $(OpenXM_doc)/asir-contrib
	mkdir $(OpenXM_doc)/asir-contrib
	touch $(OpenXM_doc)/asir-contrib/DO_NOT_EDIT_THIS_DIRECTORY._ALL_FILES_ARE_AUTOMATICDALLY_GENERATED.


clean :
	rm -f gentexi $(JAAUTO) $(ENAUTO) gentexi-in.tmp gentexi-out.tmp
	rm -f oxxm.texi oxxm-eg.texi oxmath.texi *~ *.aux *.cp *.dvi *.fn *.ky *.log *.pg *.toc *.tp *.vr oxmath-eg.texi oxphc.texi oxphc-eg.texi oxgnuplot.texi oxgnuplot-eg.texi oxsm1.texi oxsm1-eg.texi oxom-eg.texi oxom.texi dsolv-jp.texi dsolv-eg.texi longname-jp.texi longname-eg.texi oxtigers-jp.texi oxtigers-eg.texi names-ja.texi names-en.texi hypergeometric-ja.texi hypergeometric-en.texi number-en.texi number-ja.texi calculus-en.texi calculus-ja.texi series-en.texi series-ja.texi matrix-en.texi matrix-ja.texi graphic-en.texi graphic-ja.texi print-en.texi print-ja.texi poly-en.texi poly-ja.texi complex-en.texi complex-ja.texi plucker-ja.texi plucker-en.texi base-ja.texi base-en.texi
	/bin/rm -rf help-jp help-eg tmp tmp-ja html-en html-ja
