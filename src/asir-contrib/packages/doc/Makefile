#$OpenXM: OpenXM/src/asir-contrib/packages/doc/Makefile,v 1.1.1.1 1999/11/27 11:25:18 takayama Exp $
all : oxxm.texi oxxm-eg.texi oxmath.texi oxmath-eg.texi oxphc.texi oxphc-eg.texi oxgnuplot.texi oxgnuplot-eg.texi oxsm1.texi oxsm1-eg.texi oxom.texi oxom-eg.texi

ASIRDOC=$(OpenXM_HOME)/src/asir-doc/doc
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

clean :
	/bin/rm -f oxxm.texi oxxm-eg.texi oxmath.texi *~ *.aux *.cp *.dvi *.fn *.ky *.log *.pg *.toc *.tp *.vr oxmath-eg.texi oxphc.texi oxphc-eg.texi oxgnuplot.texi oxgnuplot-eg.texi oxsm1.texi oxsm1-eg.texi oxom-eg.texi oxom.texi
