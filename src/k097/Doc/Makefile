#$OpenXM$
# targets
# all :   
#      
# Configure here.
OpenXM_ROOT=$(HOME)/OpenXM
OpenXM_HOME=${OpenXM_ROOT}
#

ASIRDOC=${OpenXM_ROOT}/src/asir-doc
ASIRLIB=${OpenXM_ROOT}/../OpenXM_contrib2/asir2000/lib
CONTRIB=${OpenXM_ROOT}/src/asir-contrib/packages/src
HELP=${OpenXM_HOME}/lib/k097/help
HELP_JA=$(HELP)/help-ja
HELP_EN=$(HELP)/help-en
## PS documents are installed under $(OpenXM_doc)/asir-contrib
OpenXM_doc=$(OpenXM_ROOT)/doc
DVIPS=dvips -f
OXWEAVE=$(OpenXM_ROOT)/src/kxx/oxweave

INFO=complex standard
JPSRCS= complex-ja.texi standard-ja.texi
ENSRCS= complex-en.texi standard-en.texi
# INFO=complex 
# JPSRCS= complex-ja.texi 
# ENSRCS= complex-en.texi 

all : make_help_dir all-ja all-en
make_help_dir:
	-mkdir $(HELP)
all-ja: $(JPSRCS) $(ASIRDOC)/extract_func
	-rm -rf $(HELP_JA)
	mkdir $(HELP_JA)
	for i in $(JPSRCS); do nkf -e $$i > $(HELP_JA)/$$i; done
	-for i in $(JPSRCS); do (cd $(HELP_JA); makeinfo --force $$i); done
	(cd $(HELP_JA); for i in $(INFO); do $(ASIRDOC)/extract_func -j $$i; done)
all-en: $(ENSRCS) $(ASIRDOC)/extract_func
	-rm -rf $(HELP_EN)
	mkdir $(HELP_EN)
	for i in $(ENSRCS); do nkf -e $$i > $(HELP_EN)/$$i; done
	-for i in $(ENSRCS); do (cd $(HELP_EN); makeinfo --force $$i); done
	(cd $(HELP_EN); for i in $(INFO); do $(ASIRDOC)/extract_func -j $$i; done)

standard.texi:
	-rm -rf standard.texi
	k0 <genhelp.k >standard.texi

complex-ja.texi : complex.texi
	/bin/rm -f complex-ja.texi
	$(OXWEAVE) C ja <complex.texi >complex-ja.texi
complex-en.texi : complex.texi
	/bin/rm -f complex-en.texi
	$(OXWEAVE) C en <complex.texi >complex-en.texi
standard-ja.texi : standard.texi
	/bin/rm -f standard-ja.texi
	$(OXWEAVE) C en <standard.texi >standard-ja.texi
standard-en.texi : standard.texi
	/bin/rm -f standard-en.texi
	$(OXWEAVE) C en <standard.texi >standard-en.texi

clean :
	-/bin/rm -rf $(HELP)
	-/bin/rm -f  *~ *.aux *.cp *.dvi *.fn *.ky *.log *.pg *.toc *.tp *.vr 
	-/bin/rm -f complex-en.texi complex-ja.texi standard-en.texi standard-ja.texi standard.texi

