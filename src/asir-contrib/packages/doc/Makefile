#$OpenXM: OpenXM/src/asir-contrib/packages/doc/Makefile,v 1.3 1999/12/10 05:05:39 noro Exp $
JPSRCS= oxxm.texi oxmath.texi oxphc.texi oxgnuplot.texi oxsm1.texi oxom.texi
EGSRCS= oxxm-eg.texi oxmath-eg.texi oxphc-eg.texi oxgnuplot-eg.texi oxsm1-eg.texi oxom-eg.texi
all : $(JPSRCS) $(EGSRCS)

ASIRDOC=$(OpenXM_HOME)/src/asir-doc
ASIRLIB=$(OpenXM_HOME)/src/asir2000/lib
CONTRIB=$(OpenXM_HOME)/src/asir-contrib/packages/src

oxxm.texi : $(ASIRLIB)/xm Makefile
	/bin/rm -f oxxm.texi
	oxweave C-texi jp-texi  <$(ASIRLIB)/xm >oxxm.texi

oxxm-eg.texi : $(ASIRLIB)/xm Makefile
	/bin/rm -f oxxm-eg.texi
	oxweave C-texi eg-texi  <$(ASIRLIB)/xm >oxxm-eg.texi

oxmath.texi : $(CONTRIB)/m Makefile
	/bin/rm -f oxmath.texi
	oxweave C-texi jp-texi  <$(CONTRIB)/m >oxmath.texi

oxmath-eg.texi : $(CONTRIB)/m Makefile
	/bin/rm -f oxmath-eg.texi
	oxweave C-texi eg-texi  <$(CONTRIB)/m >oxmath-eg.texi

oxphc.texi : $(CONTRIB)/phc Makefile
	/bin/rm -f oxphc.texi
	oxweave C-texi jp-texi  <$(CONTRIB)/phc >oxphc.texi

oxphc-eg.texi : $(CONTRIB)/phc Makefile
	/bin/rm -f oxphc-eg.texi
	oxweave C-texi eg-texi  <$(CONTRIB)/phc >oxphc-eg.texi

oxgnuplot.texi : $(CONTRIB)/gnuplot Makefile
	/bin/rm -f oxgnuplot.texi
	oxweave C-texi jp-texi  <$(CONTRIB)/gnuplot >oxgnuplot.texi

oxgnuplot-eg.texi : $(CONTRIB)/gnuplot Makefile
	/bin/rm -f oxgnuplot-eg.texi
	oxweave C-texi eg-texi  <$(CONTRIB)/gnuplot >oxgnuplot-eg.texi

oxsm1.texi : $(CONTRIB)/sm1 Makefile
	/bin/rm -f oxsm1.texi
	oxweave C-texi jp-texi  <$(CONTRIB)/sm1 >oxsm1.texi

oxsm1-eg.texi : $(CONTRIB)/sm1 Makefile
	/bin/rm -f oxsm1-eg.texi
	oxweave C-texi eg-texi  <$(CONTRIB)/sm1 >oxsm1-eg.texi

oxom.texi : $(CONTRIB)/om Makefile
	/bin/rm -f oxom.texi
	oxweave C-texi jp-texi  <$(CONTRIB)/om >oxom.texi

oxom-eg.texi : $(CONTRIB)/om Makefile
	/bin/rm -f oxom-eg.texi
	oxweave C-texi eg-texi  <$(CONTRIB)/om >oxom-eg.texi

help-eg: $(EGSRCS) $(ASIRDOC)/extract_func
	-rm -rf tmp help-eg
	mkdir help-eg tmp
	cp cman-eg.texi $(EGSRCS) tmp
	-(cd tmp; makeinfo --force cman-eg.texi)
	(cd help-eg; for i in ../tmp/asir-contrib*; do $(ASIRDOC)/extract_func $$i; done)

help-jp: $(JPSRCS) $(ASIRDOC)/extract_func
	-rm -rf tmp help-jp
	mkdir help-jp tmp
	for i in cman.texi $(JPSRCS); do nkf -e $$i > tmp/$$i; done
	-(cd tmp; makeinfo --force cman.texi)
	(cd help-jp; for i in ../tmp/asir-contrib*; do $(ASIRDOC)/extract_func $$i; done)

help: help-eg help-jp

install-help: help
	/bin/rm -f $(OpenXM_HOME)/lib/asir/help-jp/xm.help
	mkdir $(OpenXM_HOME)/lib/asir/help-jp/xm.help
	cp -f help-jp/* $(OpenXM_HOME)/lib/asir/help-jp/xm.help

clean :
	/bin/rm -f oxxm.texi oxxm-eg.texi oxmath.texi *~ *.aux *.cp *.dvi *.fn *.ky *.log *.pg *.toc *.tp *.vr oxmath-eg.texi oxphc.texi oxphc-eg.texi oxgnuplot.texi oxgnuplot-eg.texi oxsm1.texi oxsm1-eg.texi oxom-eg.texi oxom.texi
	/bin/rm -rf help-jp help-eg tmp
