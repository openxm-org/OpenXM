# $OpenXM: OpenXM/doc/ascm2001/Makefile,v 1.2 2003/03/28 04:42:29 takayama Exp $
LATEX2HTML=latex2html -tmp /tmp
JFIX=${OpenXM_HOME}/src/util/sample1.sh
FILES=ascm2001.tex bib.tex data-format.tex design-outline.tex \
      heterotic-network.tex homogeneous-network.tex \
      openxm-clients.tex openxm-development-environment.tex \
	 openxm-servers.tex openxm-stackmachines.tex ox-messages.tex \
	session-management.tex katsura7.ps speedup.ps

all: all-html

all-html: .all-html.done
	if test ! -f .all-html.done ; \
	then  \
		 (latex ascm2001; latex ascm2001 ; ${LATEX2HTML} ascm2001) ; \
	     (cd ascm2001 ; $(JFIX) *.html ; rm -f *-tmp.old ) ; \
		touch .all-html.done ; \
	fi

.all-html.done: ${FILES}

clean:
	rm -rf .[a-z]*done ascm
	rm -rf ascm2001
