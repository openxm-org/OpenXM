# $OpenXM: OpenXM/src/ox_math/documents/Makefile,v 1.8 2000/03/17 15:14:20 takayama Exp $

## platex is a localized tex compiler for Japanese language.
## ox_math.tex is written in Japanese by the EUC-JP coded character set.
## Therefore, we need platex, which supports the EUC-JP, to compile it.

OpenXM_HOME = ../../..
OXWEAVE     = ${OpenXM_HOME}/bin/oxweave
TEXI2DVI    = texi2dvi
#TEXI2DVI_OX    = ${OpenXM_HOME}/misc/texi2dvi
TEXI2DVI_OX    = ptex
LATEX       = latex
PLATEX      = platex
DOCUMENTS   = math2ox.texi math2ox.dvi readme-eg
JDOCUMENTS  = ox_math.tex ox_math.dvi readme-jp samplelog-sm1.txt
WHICH       = ${OpenXM_HOME}/misc/which.pl

all: ${DOCUMENTS} ${JDOCUMENTS}

math2ox.dvi: math2ox.texi
	@if ${WHICH} -s texi2dvi ; then \
	   ${TEXI2DVI} math2ox.texi ; \
	else \
	   ${TEXI2DVI_OX} math2ox.texi ; \
	fi

ox_math.dvi: ox_math.tex
	${PLATEX} ox_math.tex
	${PLATEX} ox_math.tex

readme-jp: readme.oxweave
	${OXWEAVE} ja < readme.oxweave > readme-jp

readme-eg: readme.oxweave
	${OXWEAVE} en < readme.oxweave > readme-eg


install: install-document install-binary

install-binary:
	@echo "No bainaries are installed."

install-for-debug:
	@echo "No bainaries are installed."

install-document: ${DOCUMENTS} ${JDOCUMENTS} 
	-mkdir ${OpenXM_HOME}/doc/ox_math
	cp -f ${DOCUMENTS} ${JDOCUMENTS} ${OpenXM_HOME}/doc/ox_math/

clean:
	-rm -f *~ *.log *.aux *.dvi readme-jp readme-eg 
	-rm -f *.cp *.fn *.ky *.pg *.tp *.vr *.fns *.toc
