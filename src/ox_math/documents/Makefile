# $OpenXM$

OpenXM_HOME = ../../..
LATEX = latex
DOCUMENTS = math2ox.tex math2ox.dvi ox_math.tex readme.jp samplelog-sm1.txt

.SUFFIXES: .dvi .tex

.tex.dvi:
	${LATEX} $<
	${LATEX} $<

all: math2ox.dvi

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