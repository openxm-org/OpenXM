#$OpenXM: OpenXM/src/asir-contrib/packages/doc/Makefile,v 1.52 2004/03/05 14:43:36 ohara Exp $
# targets
# all :   
#     Extracts all tex-info files from ../src/* and generate *.texi files.
# install : install-help
# install-help : all
#        Installing online help to OpenXM/lib/asir/help-*
# install-document :  all  gen-dvi-ps
#        Copy the documents in dvi and ps to OpenXM/doc/asir-contrib
# install-html-help
#        Copy html help to OpenXM/doc/asir-contrib
# gen-html-en  gen-html-ja
#        Generate HTML documents under html-en and html-ja
# gen-help-en  gen-help-ja
#        Generate help files.
# gen-dvi-ps
#        Generate dvi and ps files.
#
# NOTE: install-help should be executed after installing asir2000.
########################################################################
#  How to add new documents in the category xxx
# "xxx*.texi <--- xxx.oxweave (manually written) + xxx*auto*.texi (automatic)"
#   (1) "xxx.oxweave" --> xxx-en.texi and xxx-ja.texi
#       xxx-auto-en.texi and xxx-auto-ja.texi should be included from 
#       xxx-en.texi and xxx-ja.texi  (JPSRCS, EGSRCS)
#     NOTE: xxx.oxweave might be embedded in xxx.rr or xxx, 
#           but this style is obsolete.
#   (2) "xxx.rr" or "xxx" --> xxx-auto-en.texi and xxx-auto-ja.texi 
#       (JAAUTO, ENAUTO)
#       xxx-auto*.texi's are extracted from xxx or xxx.rr
#   (3) xxx-en.texi and xxx-ja.texi should be included from cman.texi 
#     and cman-en.texi
#  cf.  glib*  base*
########################################################################
OpenXM_ROOT=../../../../../OpenXM
ASIRDOC=$(OpenXM_ROOT)/src/asir-doc
ASIRLIB=$(OpenXM_ROOT)/../OpenXM_contrib2/asir2000/lib
CONTRIB=$(OpenXM_ROOT)/src/asir-contrib/packages/src
OPTGENTEXI= --include 
#OPTGENTEXI= --include --example
## PS documents are installed under $(OpenXM_doc)/asir-contrib
OpenXM_doc=$(OpenXM_ROOT)/doc
DVIPS=dvips -f
OXWEAVE=$(OpenXM_ROOT)/bin/oxweave
TEXI2HTML=texi2html -menu -split_node
RM=rm

JPSRCS= opening.texi oxrfc100-ja.texi oxmath-ja.texi oxphc-ja.texi oxgnuplot-ja.texi oxsm1-ja.texi oxom-ja.texi dsolv-ja.texi oxtigers-ja.texi names-ja.texi hypergeometric-ja.texi number-ja.texi calculus-ja.texi series-ja.texi matrix-ja.texi  graphic-ja.texi print-ja.texi poly-ja.texi complex-ja.texi intro-ja.texi Diff-ja.texi Dmodule-ja.texi  base-ja.texi misc-ja.texi glib-ja.texi pfp-ja.texi plucker-ja.texi windows-ja.texi util-ja.texi 
EGSRCS= opening.texi oxrfc100-en.texi oxmath-en.texi oxphc-en.texi oxgnuplot-en.texi oxsm1-en.texi oxom-en.texi dsolv-en.texi oxtigers-en.texi names-en.texi hypergeometric-en.texi number-en.texi calculus-en.texi series-en.texi matrix-en.texi  graphic-en.texi print-en.texi poly-en.texi complex-en.texi intro-en.texi cman-en.texi base-en.texi misc-en.texi glib-en.texi pfp-en.texi plucker-en.texi windows-en.texi util-en.texi 

JAAUTO=base-auto-ja.texi pfp-auto-ja.texi poly-auto-ja.texi \
       print-auto-ja.texi matrix-auto-ja.texi glib-auto-ja.texi \
	   sm1-auto-ja.texi plucker-auto-ja.texi util-auto-ja.texi \
       number-auto-ja.texi
ENAUTO=base-auto-en.texi pfp-auto-en.texi poly-auto-en.texi \
       print-auto-en.texi matrix-auto-en.texi glib-auto-en.texi \
	   sm1-auto-en.texi plucker-auto-en.texi util-auto-en.texi \
       number-auto-en.texi

all : gentexi $(JPSRCS) $(EGSRCS) $(JAAUTO) $(ENAUTO)
	@echo "Type in  tex cman-en.texi to generate the manual(en) in dvi format."
	@echo "Type in  ptex cman-ja.texi   to generate the manual(ja) in dvi format."
install :
	@if [ ! -f ./.done_install ] ; \
	then \
		make install2 ; \
		touch ./.done_install ; \
	else \
		echo "NOTE: Installation is already done. Remove .done_install for a new installation." ; \
	fi

install-document : install-html-help
	@if [ ! -f ./.done_install_document ] ; \
	then \
		make install-document2 ; \
		touch ./.done_install_document ; \
	else \
		echo "NOTE: Installation of document is already done. Remove .done_install_document for a new installation." ; \
	fi


install2 : install-help 

gentexi : gentexi.c
	gcc -o gentexi gentexi.c

###base
base-ja.texi : base.oxweave
	$(RM) -f base-ja.texi
	$(OXWEAVE) C ja  <base.oxweave >base-ja.texi
base-en.texi : base.oxweave
	$(RM) -f base-en.texi
	$(OXWEAVE) C en  <base.oxweave >base-en.texi
base-auto-en.texi : $(CONTRIB)/names.rr gentexi
	$(RM) -f base-auto-en.texi
	$(OXWEAVE) usage  <$(CONTRIB)/names.rr | ./gentexi --category base --en --upnode "Basic (Standard Functions)" ${OPTGENTEXI} >base-auto-en.texi
base-auto-ja.texi : $(CONTRIB)/names.rr gentexi
	$(RM) -f base-auto-ja.texi
	$(OXWEAVE) usage  <$(CONTRIB)/names.rr | ./gentexi --category base --ja --upnode "Basic (Standard Functions)" ${OPTGENTEXI} >base-auto-ja.texi


names-ja.texi : names.oxweave
	$(RM) -f names-ja.texi
	$(OXWEAVE) C ja  <names.oxweave >names-ja.texi

names-en.texi : names.oxweave
	$(RM) -f names-en.texi
	$(OXWEAVE) C en  <names.oxweave >names-en.texi

###plucker
plucker-ja.texi : plucker.oxweave
	$(RM) -f plucker-ja.texi
	$(OXWEAVE) C ja  <plucker.oxweave >plucker-ja.texi
plucker-en.texi : plucker.oxweave
	$(RM) -f plucker-en.texi
	$(OXWEAVE) C en  <plucker.oxweave >plucker-en.texi
plucker-auto-en.texi : $(CONTRIB)/plucker.rr gentexi
	$(RM) -f plucker-auto-en.texi
	$(OXWEAVE) usage  <$(CONTRIB)/plucker.rr | ./gentexi --category plucker --en --upnode "Plucker relations" ${OPTGENTEXI} >plucker-auto-en.texi
plucker-auto-ja.texi : $(CONTRIB)/plucker.rr gentexi
	$(RM) -f plucker-auto-ja.texi
	$(OXWEAVE) usage  <$(CONTRIB)/plucker.rr | ./gentexi --category plucker --ja --upnode "Plucker relations" ${OPTGENTEXI} >plucker-auto-ja.texi

hypergeometric-ja.texi : hypergeometric.oxweave
	$(RM) -f hypergeometric-ja.texi
	$(OXWEAVE) C ja  <hypergeometric.oxweave >hypergeometric-ja.texi

hypergeometric-en.texi : hypergeometric.oxweave
	$(RM) -f hypergeometric-en.texi
	$(OXWEAVE) C en  <hypergeometric.oxweave >hypergeometric-en.texi

### number
number-ja.texi : number.oxweave
	$(RM) -f number-ja.texi
	$(OXWEAVE) C ja  <number.oxweave >number-ja.texi

number-en.texi : number.oxweave
	$(RM) -f number-en.texi
	$(OXWEAVE) C en  <number.oxweave >number-en.texi
number-auto-en.texi :  $(CONTRIB)/names.rr gentexi
	$(RM) -f number-auto-en.texi
	cat ${CONTRIB}/names.rr | $(OXWEAVE) usage | ./gentexi  --category number --en --upnode "Numbers (Standard Mathematical Functions)" ${OPTGENTEXI} >number-auto-en.texi
number-auto-ja.texi : $(CONTRIB)/names.rr gentexi
	$(RM) -f number-auto-ja.texi
	cat ${CONTRIB}/names.rr | $(OXWEAVE) usage  | ./gentexi   --category number  --ja --upnode "Numbers (Standard Mathematical Functions)" ${OPTGENTEXI} >number-auto-ja.texi

calculus-ja.texi : calculus.oxweave
	$(RM) -f calculus-ja.texi
	$(OXWEAVE) C ja  <calculus.oxweave >calculus-ja.texi

calculus-en.texi : calculus.oxweave
	$(RM) -f calculus-en.texi
	$(OXWEAVE) C en  <calculus.oxweave >calculus-en.texi

series-ja.texi : series.oxweave
	$(RM) -f series-ja.texi
	$(OXWEAVE) C ja  <series.oxweave >series-ja.texi

series-en.texi : series.oxweave
	$(RM) -f series-en.texi
	$(OXWEAVE) C en  <series.oxweave >series-en.texi

### matrix
matrix-ja.texi : matrix.oxweave
	$(RM) -f matrix-ja.texi
	$(OXWEAVE) C ja  <matrix.oxweave >matrix-ja.texi
matrix-en.texi : matrix.oxweave
	$(RM) -f matrix-en.texi
	$(OXWEAVE) C en  <matrix.oxweave >matrix-en.texi
matrix-auto-en.texi : $(CONTRIB)/names.rr gentexi
	$(RM) -f matrix-auto-en.texi
	$(OXWEAVE) usage  <$(CONTRIB)/names.rr | ./gentexi --category matrix --en --upnode "Matrix (Standard Mathematical Functions)" ${OPTGENTEXI} >matrix-auto-en.texi
matrix-auto-ja.texi : $(CONTRIB)/names.rr gentexi
	$(RM) -f matrix-auto-ja.texi
	$(OXWEAVE) usage  <$(CONTRIB)/names.rr | ./gentexi --category matrix --ja --upnode "Matrix (Standard Mathematical Functions)" ${OPTGENTEXI} >matrix-auto-ja.texi


graphic-ja.texi : graphic.oxweave
	$(RM) -f graphic-ja.texi
	$(OXWEAVE) C ja  <graphic.oxweave >graphic-ja.texi

graphic-en.texi : graphic.oxweave
	$(RM) -f graphic-en.texi
	$(OXWEAVE) C en  <graphic.oxweave >graphic-en.texi

### print
print-ja.texi : print.oxweave
	$(RM) -f print-ja.texi
	$(OXWEAVE) C ja  <print.oxweave >print-ja.texi
print-en.texi : print.oxweave
	$(RM) -f print-en.texi
	$(OXWEAVE) C en  <print.oxweave >print-en.texi
print-auto-en.texi : $(CONTRIB)/names.rr gentexi
	$(RM) -f print-auto-en.texi
	$(OXWEAVE) usage  <$(CONTRIB)/names.rr | ./gentexi --category print --en --upnode "Print (Standard Mathematical Functions)" ${OPTGENTEXI} >print-auto-en.texi
print-auto-ja.texi : $(CONTRIB)/names.rr gentexi
	$(RM) -f print-auto-ja.texi
	$(OXWEAVE) usage  <$(CONTRIB)/names.rr | ./gentexi --category print --ja --upnode "Print (Standard Mathematical Functions)" ${OPTGENTEXI} >print-auto-ja.texi

###poly
poly-ja.texi : poly.oxweave
	$(RM) -f poly-ja.texi
	$(OXWEAVE) C ja  <poly.oxweave >poly-ja.texi
poly-en.texi : poly.oxweave
	$(RM) -f poly-en.texi
	$(OXWEAVE) C en  <poly.oxweave >poly-en.texi
poly-auto-en.texi : $(CONTRIB)/names.rr gentexi
	$(RM) -f poly-auto-en.texi
	$(OXWEAVE) usage  <$(CONTRIB)/names.rr | ./gentexi --category poly --en --upnode "Polynomials (Standard Mathematical Functions)" ${OPTGENTEXI} >poly-auto-en.texi
poly-auto-ja.texi : $(CONTRIB)/names.rr gentexi
	$(RM) -f poly-auto-ja.texi
	$(OXWEAVE) usage  <$(CONTRIB)/names.rr | ./gentexi --category poly --ja --upnode "Polynomials (Standard Mathematical Functions)" ${OPTGENTEXI} >poly-auto-ja.texi

complex-ja.texi : complex.oxweave
	$(RM) -f complex-ja.texi
	$(OXWEAVE) C ja  <complex.oxweave >complex-ja.texi

complex-en.texi : complex.oxweave
	$(RM) -f complex-en.texi
	$(OXWEAVE) C en  <complex.oxweave >complex-en.texi


oxrfc100-ja.texi : oxrfc100.oxweave Makefile
	$(RM) -f oxrfc100-ja.texi
	$(OXWEAVE) C-texi ja  <oxrfc100.oxweave >oxrfc100-ja.texi

oxrfc100-en.texi : oxrfc100.oxweave Makefile
	$(RM) -f oxrfc100-en.texi
	$(OXWEAVE) C-texi en  <oxrfc100.oxweave >oxrfc100-en.texi

oxmath-ja.texi : m.oxweave Makefile
	$(RM) -f oxmath-ja.texi
	$(OXWEAVE) C-texi ja  <m.oxweave >oxmath-ja.texi

oxmath-en.texi : m.oxweave Makefile
	$(RM) -f oxmath-en.texi
	$(OXWEAVE) C-texi en  <m.oxweave >oxmath-en.texi

oxphc-ja.texi : phc.oxweave Makefile
	$(RM) -f oxphc-ja.texi
	$(OXWEAVE) C-texi ja  <phc.oxweave >oxphc-ja.texi

oxphc-en.texi : phc.oxweave Makefile
	$(RM) -f oxphc-en.texi
	$(OXWEAVE) C-texi en  <phc.oxweave >oxphc-en.texi

oxgnuplot-ja.texi : gnuplot.oxweave Makefile
	$(RM) -f oxgnuplot-ja.texi
	$(OXWEAVE) C-texi ja  <gnuplot.oxweave >oxgnuplot-ja.texi

oxgnuplot-en.texi : gnuplot.oxweave Makefile
	$(RM) -f oxgnuplot-en.texi
	$(OXWEAVE) C-texi en  <gnuplot.oxweave >oxgnuplot-en.texi

###sm1
oxsm1-ja.texi : sm1.oxweave Makefile sm1-auto-ja.texi
	$(RM) -f oxsm1-ja.texi
	$(OXWEAVE) C-texi ja  <sm1.oxweave >oxsm1-ja.texi
oxsm1-en.texi : sm1.oxweave Makefile sm1-auto-en.texi
	$(RM) -f oxsm1-en.texi
	$(OXWEAVE) C-texi en  <sm1.oxweave >oxsm1-en.texi
sm1-auto-en.texi : $(CONTRIB)/sm1 gentexi
	$(RM) -f sm1-auto-en.texi
	$(OXWEAVE) usage  <$(CONTRIB)/sm1 | ./gentexi --category sm1 --en --upnode "SM1 Functions" ${OPTGENTEXI} >sm1-auto-en.texi
sm1-auto-ja.texi : $(CONTRIB)/sm1 gentexi
	$(RM) -f sm1-auto-ja.texi
	$(OXWEAVE) usage  <$(CONTRIB)/sm1 | ./gentexi --category sm1 --ja --upnode "SM1 Functions" ${OPTGENTEXI} >sm1-auto-ja.texi

oxom-ja.texi : $(CONTRIB)/om Makefile
	$(RM) -f oxom-ja.texi
	$(OXWEAVE) C-texi ja  <$(CONTRIB)/om >oxom-ja.texi

oxom-en.texi : $(CONTRIB)/om Makefile
	$(RM) -f oxom-en.texi
	$(OXWEAVE) C-texi en  <$(CONTRIB)/om >oxom-en.texi

dsolv-ja.texi : dsolv.oxweave Makefile
	$(RM) -f dsolv-ja.texi
	$(OXWEAVE) C-texi ja  <dsolv.oxweave >dsolv-ja.texi

dsolv-en.texi : dsolv.oxweave Makefile
	$(RM) -f dsolv-en.texi
	$(OXWEAVE) C-texi en  <dsolv.oxweave >dsolv-en.texi


oxtigers-ja.texi : tigers.oxweave Makefile
	$(RM) -f oxtigers-ja.texi
	$(OXWEAVE) C-texi ja  <tigers.oxweave >oxtigers-ja.texi

oxtigers-en.texi : tigers.oxweave Makefile
	$(RM) -f oxtigers-en.texi
	$(OXWEAVE) C-texi en  <tigers.oxweave >oxtigers-en.texi

### windows
windows-ja.texi : windows.oxweave Makefile
	$(RM) -f windows-ja.texi
	$(OXWEAVE) C-texi ja  <windows.oxweave >windows-ja.texi

windows-en.texi : windows.oxweave Makefile
	$(RM) -f windows-en.texi
	$(OXWEAVE) C-texi en  <windows.oxweave >windows-en.texi

### intro
intro-ja.texi : intro.oxweave Makefile
	$(RM) -f intro-ja.texi
	$(OXWEAVE) C-texi ja  <intro.oxweave >intro-ja.texi

intro-en.texi : intro.oxweave Makefile
	$(RM) -f intro-en.texi
	$(OXWEAVE) C-texi en  <intro.oxweave >intro-en.texi

###misc
misc-ja.texi : misc.oxweave
	$(RM) -f misc-ja.texi
	$(OXWEAVE) C ja  <misc.oxweave >misc-ja.texi
misc-en.texi : misc.oxweave
	$(RM) -f misc-en.texi
	$(OXWEAVE) C en  <misc.oxweave >misc-en.texi

###glib
glib-ja.texi : glib.oxweave
	$(RM) -f glib-ja.texi
	$(OXWEAVE) C ja  <glib.oxweave >glib-ja.texi
glib-en.texi : glib.oxweave
	$(RM) -f glib-en.texi
	$(OXWEAVE) C en  <glib.oxweave >glib-en.texi
glib-auto-en.texi : ${ASIRLIB}/glib gentexi
	$(RM) -f glib-auto-en.texi
	cat ${ASIRLIB}/glib | $(OXWEAVE) usage | ./gentexi  --category glib --en --upnode "Graphic Library (2 dimensional)" ${OPTGENTEXI} >glib-auto-en.texi
glib-auto-ja.texi : ${ASIRLIB}/glib gentexi
	$(RM) -f glib-auto-ja.texi
	cat ${ASIRLIB}/glib | $(OXWEAVE) usage  | ./gentexi   --category glib  --ja --upnode "Graphic Library (2 dimensional)" ${OPTGENTEXI} >glib-auto-ja.texi

###pfp
pfp-ja.texi : pfp.oxweave
	$(RM) -f pfp-ja.texi
	$(OXWEAVE) C ja  <pfp.oxweave >pfp-ja.texi
pfp-en.texi : pfp.oxweave
	$(RM) -f pfp-en.texi
	$(OXWEAVE) C en  <pfp.oxweave >pfp-en.texi
pfp-auto-en.texi : $(CONTRIB)/pfpcoh.rr $(CONTRIB)/pfphom.rr gentexi
	$(RM) -f pfp-auto-en.texi
	cat $(CONTRIB)/pfpcoh.rr $(CONTRIB)/pfphom.rr | $(OXWEAVE) usage | ./gentexi  --en --upnode  Pfpcohom ${OPTGENTEXI} >pfp-auto-en.texi
pfp-auto-ja.texi : $(CONTRIB)/pfpcoh.rr $(CONTRIB)/pfphom.rr gentexi
	$(RM) -f pfp-auto-ja.texi
	cat $(CONTRIB)/pfpcoh.rr $(CONTRIB)/pfphom.rr | $(OXWEAVE) usage  | ./gentexi  --ja --upnode Pfpcohom ${OPTGENTEXI} >pfp-auto-ja.texi

###util
util-ja.texi : util.oxweave
	$(RM) -f util-ja.texi
	$(OXWEAVE) C ja <util.oxweave >util-ja.texi
util-en.texi : util.oxweave
	$(RM) -f util-en.texi
	$(OXWEAVE) C en  <util.oxweave >util-en.texi
util-auto-en.texi :  $(CONTRIB)/names.rr gentexi
	$(RM) -f util-auto-en.texi
	cat ${CONTRIB}/names.rr | $(OXWEAVE) usage | ./gentexi  --category util --en --upnode "Utility Functions" ${OPTGENTEXI} >util-auto-en.texi
util-auto-ja.texi : $(CONTRIB)/names.rr gentexi
	$(RM) -f util-auto-ja.texi
	cat ${CONTRIB}/names.rr | $(OXWEAVE) usage  | ./gentexi   --category util  --ja --upnode "Utility Functions" ${OPTGENTEXI} >util-auto-ja.texi

###### Generating online manual
gen-help-en: $(EGSRCS) $(ASIRDOC)/extract_func $(ENAUTO)
	-$(RM) -rf tmp help-en
	mkdir help-en tmp
	cp cman-en.texi $(EGSRCS) $(ENAUTO) tmp
	-(cd tmp; makeinfo --force cman-en.texi)
	(cd help-en; for i in ../tmp/asir-contrib*; do ../$(ASIRDOC)/extract_func $$i; done)

gen-help-ja: $(JPSRCS) $(ASIRDOC)/extract_func $(JAAUTO)
	-$(RM) -rf tmp-ja help-ja
	mkdir help-ja tmp-ja
	for i in cman-ja.texi $(JPSRCS) $(JAAUTO); do nkf -e $$i > tmp-ja/$$i; done
	-(cd tmp-ja; makeinfo --force cman-ja.texi)
	(cd help-ja; for i in ../tmp-ja/asir-contrib*; do ../$(ASIRDOC)/extract_func -j $$i; done)

gen-html-en: gen-help-en
	-$(RM) -rf html-en
	-mkdir html-en
	-cp tmp/*.texi html-en
	-(cd html-en; ${TEXI2HTML} cman-en.texi)

gen-html-ja: gen-help-ja
	-mkdir html-ja
	-cp tmp-ja/*.texi html-ja
	-cp tmp-ja/cman-ja.texi html-ja/cman-ja.texi
	-(cd html-ja; ${TEXI2HTML} cman-ja.texi)

help: gen-help-en gen-help-ja

install-help: help
	$(RM) -rf $(OpenXM_ROOT)/lib/asir/help-jp/xm.help
	mkdir $(OpenXM_ROOT)/lib/asir/help-jp/xm.help
	-cp -f help-ja/* $(OpenXM_ROOT)/lib/asir/help-jp/xm.help
	$(RM) -rf $(OpenXM_ROOT)/lib/asir/help-eg/xm.help
	mkdir $(OpenXM_ROOT)/lib/asir/help-eg/xm.help
	-cp -f help-en/* $(OpenXM_ROOT)/lib/asir/help-eg/xm.help

install-html-help: gen-html-en gen-html-ja
	-mkdir $(OpenXM_doc)/asir-contrib
	-( tar cf - html-en | (cd $(OpenXM_doc)/asir-contrib ; tar xf -))
	-( tar cf - html-ja | (cd $(OpenXM_doc)/asir-contrib ; tar xf -))

install-document2 :  gen-dvi-ps
	-mkdir $(OpenXM_doc)/asir-contrib
	-cp cman-ja.dvi $(OpenXM_doc)/asir-contrib/cman-ja.dvi
	-$(DVIPS) cman-ja >$(OpenXM_doc)/asir-contrib/cman-ja.ps
	-cp cman-en.dvi $(OpenXM_doc)/asir-contrib/cman-en.dvi
	-$(DVIPS) cman-en >$(OpenXM_doc)/asir-contrib/cman-en.ps
gen-dvi-ps: all clean-mk-openxm-asir-contrib-document-dir cman-ja.dvi cman-en.dvi
cman-ja.dvi: $(JPSRCS)
	-ptex cman-ja.texi
	-ptex cman-ja.texi
cman-en.dvi: $(ENSRCS)
	-tex cman-en.texi
	-tex cman-en.texi


clean-mk-openxm-asir-contrib-document-dir :
	$(RM) -rf $(OpenXM_doc)/asir-contrib
	$(RM) -f ./.done_install ./.done_install_document
	mkdir $(OpenXM_doc)/asir-contrib
	touch $(OpenXM_doc)/asir-contrib/DO_NOT_EDIT_THIS_DIRECTORY._ALL_FILES_ARE_AUTOMATICDALLY_GENERATED.


clean :
	$(RM) -f ./.done_install ./.done_install_document
	$(RM) -f gentexi $(JAAUTO) $(ENAUTO) gentexi-in.tmp gentexi-out.tmp
	$(RM) -f oxrfc100-ja.texi oxrfc100-en.texi oxmath-ja.texi *~ *.aux *.cp *.dvi *.fn *.ky *.log *.pg *.toc *.tp *.vr oxmath-en.texi oxphc-ja.texi oxphc-en.texi oxgnuplot-ja.texi oxgnuplot-en.texi oxsm1-ja.texi oxsm1-en.texi oxom-en.texi oxom-ja.texi dsolv-ja.texi dsolv-en.texi longname-ja.texi longname-en.texi oxtigers-ja.texi oxtigers-en.texi names-ja.texi names-en.texi hypergeometric-ja.texi hypergeometric-en.texi number-en.texi number-ja.texi calculus-en.texi calculus-ja.texi series-en.texi series-ja.texi matrix-en.texi matrix-ja.texi graphic-en.texi graphic-ja.texi print-en.texi print-ja.texi poly-en.texi poly-ja.texi complex-en.texi complex-ja.texi plucker-ja.texi plucker-en.texi base-ja.texi base-en.texi glib-ja.texi glib-en.texi misc-ja.texi misc-en.texi pfp-ja.texi pfp-en.texi windows-ja.texi windows-en.texi intro-ja.texi intro-en.texi util-en.texi util-ja.texi
	$(RM) -rf help-ja help-en tmp tmp-ja html-en html-ja
