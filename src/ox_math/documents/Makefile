# $OpenXM: OpenXM/src/ox_math/documents/Makefile,v 1.3 2000/01/21 10:56:56 ohara Exp $

## platex is a localized tex compiler for Japanese language.
## ox_math.tex is written in Japanese by the EUC-JP coded character set.
## Therefore, we need platex, which supports the EUC-JP, to compile it.

OpenXM_HOME = ../../..
OXWEAVE = ${OpenXM_HOME}/bin/oxweave
LATEX = latex
PLATEX = platex
DOCUMENTS = math2ox.tex math2ox.dvi ox_math.tex ox_math.dvi readme-jp readme-eg samplelog-sm1.txt

all: ${DOCUMENTS}

math2ox.dvi: math2ox.tex
	${LATEX} math2ox.tex
	${LATEX} math2ox.tex

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

install-document: ${DOCUMENTS}
	-mkdir ${OpenXM_HOME}/doc/ox_math
	cp -f ${DOCUMENTS} ${OpenXM_HOME}/doc/ox_math/

clean:
	-rm -f *~ *.log *.aux *.dvi readme-jp readme-eg math2ox.?? *.fns *.toc
