#$OpenXM: OpenXM/src/k097/Doc/Makefile,v 1.1 2001/01/04 12:29:32 takayama Exp $
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

INFO=complex standard standard-suppl
JPSRCS= complex-ja.texi standard-ja.texi standard-suppl-ja.texi
ENSRCS= complex-en.texi standard-en.texi standard-suppl-en.texi
# INFO=complex 
# JPSRCS= complex-ja.texi 
# ENSRCS= complex-en.texi 

all : make_help_dir build post-install
build: all-ja all-en
post-install:
	-rm -rf $(HELP_JA)/*.texi $(HELP_JA)/complex $(HELP_JA)/standard $(HELP_JA)/standard-suppl
	-rm -rf $(HELP_EN)/*.texi $(HELP_EN)/complex $(HELP_EN)/standard $(HELP_EN)/standard-suppl
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
standard-suppl-ja.texi : standard-suppl.texi
	/bin/rm -f standard-suppl-ja.texi
	$(OXWEAVE) C ja <standard-suppl.texi >standard-suppl-ja.texi
standard-suppl-en.texi : standard-suppl.texi
	/bin/rm -f standard-suppl-en.texi
	$(OXWEAVE) C en <standard-suppl.texi >standard-suppl-en.texi

clean :
	-/bin/rm -rf $(HELP)
	-/bin/rm -f  *~ *.aux *.cp *.dvi *.fn *.ky *.log *.pg *.toc *.tp *.vr 
	-/bin/rm -f complex-en.texi complex-ja.texi standard-en.texi standard-ja.texi standard.texi standard-suppl-en.texi standard-suppl-ja.texi


mybackup :
	(cd ../.. ; rsync  -avz  -e "ssh -l taka" --exclude '*~' --exclude 'k0' --exclude 'd0' --exclude '*.o' --exclude 'help' k097 lemon.math.sci.kobe-u.ac.jp:tmp/tmp-backup)
