#$OpenXM: OpenXM/src/asir-contrib/packages/doc/Makefile,v 1.12 2000/02/11 04:31:34 takayama Exp $
# targets
# all :   
#        Extracts all tex-info files from ../src/*
# install : install-document install-help
# install-help : all
#        Installing online help to OpenXM/lib/asir/help-*
# install-document :  all
#        Copy the documents to OpenXM/doc/asir-contrib
# NOTE: install-help should be executed after installing asir2000.
OpenXM_ROOT=../../../../../OpenXM
ASIRDOC=$(OpenXM_ROOT)/src/asir-doc
ASIRLIB=$(OpenXM_ROOT)/../OpenXM_contrib2/asir2000/lib
CONTRIB=$(OpenXM_ROOT)/src/asir-contrib/packages/src
## PS documents are installed under $(OpenXM_doc)/asir-contrib
OpenXM_doc=$(OpenXM_ROOT)/doc
DVIPS=dvips -f
OXWEAVE= ../../../kxx/oxweave

JPSRCS= oxxm.texi oxmath.texi oxphc.texi oxgnuplot.texi oxsm1.texi oxom.texi dsolv-jp.texi longname-jp.texi oxtigers-jp.texi
EGSRCS= oxxm-eg.texi oxmath-eg.texi oxphc-eg.texi oxgnuplot-eg.texi oxsm1-eg.texi oxom-eg.texi dsolv-eg.texi longname-eg.texi oxtigers-eg.texi

all : $(JPSRCS) $(EGSRCS)
install : install-document install-help

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

oxsm1.texi : $(CONTRIB)/sm1 Makefile
	/bin/rm -f oxsm1.texi
	$(OXWEAVE) C-texi jp-texi  <$(CONTRIB)/sm1 >oxsm1.texi

oxsm1-eg.texi : $(CONTRIB)/sm1 Makefile
	/bin/rm -f oxsm1-eg.texi
	$(OXWEAVE) C-texi eg-texi  <$(CONTRIB)/sm1 >oxsm1-eg.texi

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

help-eg: $(EGSRCS) $(ASIRDOC)/extract_func
	-rm -rf tmp help-eg
	mkdir help-eg tmp
	cp cman-eg.texi $(EGSRCS) tmp
	-(cd tmp; makeinfo --force cman-eg.texi)
	(cd help-eg; for i in ../tmp/asir-contrib*; do ../$(ASIRDOC)/extract_func $$i; done)

help-jp: $(JPSRCS) $(ASIRDOC)/extract_func
	-rm -rf tmp help-jp
	mkdir help-jp tmp
	for i in cman.texi $(JPSRCS); do nkf -e $$i > tmp/$$i; done
	-(cd tmp; makeinfo --force cman.texi)
	(cd help-jp; for i in ../tmp/asir-contrib*; do ../$(ASIRDOC)/extract_func -j $$i; done)

help: help-eg help-jp

install-help: help
	/bin/rm -rf $(OpenXM_ROOT)/lib/asir/help-jp/xm.help
	mkdir $(OpenXM_ROOT)/lib/asir/help-jp/xm.help
	-cp -f help-jp/* $(OpenXM_ROOT)/lib/asir/help-jp/xm.help
	/bin/rm -rf $(OpenXM_ROOT)/lib/asir/help-eg/xm.help
	mkdir $(OpenXM_ROOT)/lib/asir/help-eg/xm.help
	-cp -f help-eg/* $(OpenXM_ROOT)/lib/asir/help-eg/xm.help

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
	/bin/rm -f oxxm.texi oxxm-eg.texi oxmath.texi *~ *.aux *.cp *.dvi *.fn *.ky *.log *.pg *.toc *.tp *.vr oxmath-eg.texi oxphc.texi oxphc-eg.texi oxgnuplot.texi oxgnuplot-eg.texi oxsm1.texi oxsm1-eg.texi oxom-eg.texi oxom.texi dsolv-jp.texi dsolv-eg.texi longname-jp.texi longname-eg.texi oxtigers-jp.texi oxtigers-eg.texi
	/bin/rm -rf help-jp help-eg tmp
