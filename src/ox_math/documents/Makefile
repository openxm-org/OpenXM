# $OpenXM: OpenXM/src/ox_math/documents/Makefile,v 1.1 2000/01/20 07:47:30 ohara Exp $

## platex is a localized tex compiler for Japanese language.
## ox_math.tex is written in Japanese by the EUC-JP coded character set.
## Therefore, we need platex, which supports the EUC-JP, to compile it.

OpenXM_HOME = ../../..
LATEX = latex
PLATEX = platex
DOCUMENTS = math2ox.tex math2ox.dvi ox_math.tex ox_math.dvi readme.jp samplelog-sm1.txt

all: ${DOCUMENTS}

math2ox.dvi: math2ox.tex
	${LATEX} math2ox.tex
	${LATEX} math2ox.tex

ox_math.dvi: ox_math.tex
	${PLATEX} ox_math.tex
	${PLATEX} ox_math.tex

install: install-document install-binary

install-binary:
	@echo "No bainaries are installed."

install-for-debug:
	@echo "No bainaries are installed."

install-document: ${DOCUMENTS}
	-mkdir ${OpenXM_HOME}/doc/ox_math
	cp -f ${DOCUMENTS} ${OpenXM_HOME}/doc/ox_math/

clean:
	-rm -f *~ *.log *.aux *.dvi