# $OpenXM$

# Collect all Copyright notice here.
OpenXM_HOME=../../OpenXM
OpenXM_contrib=../../OpenXM_contrib

all : copy-copyright

copy-copyright :
	/bin/rm -rf Contrib
	mkdir Contrib
	cp -f $(OpenXM_contrib)/gnuplot/Copyright Contrib/Copyright.gnuplot
	cp -f $(OpenXM_contrib)/pari/COPYRIGHT Contrib/COPYRIGHT.pari
	cp -f $(OpenXM_contrib)/gmp/COPYING.LIB Contrib/COPYING.LIB.gmp
	cp -f $(OpenXM_contrib)/gc/README Contrib/README.gc
