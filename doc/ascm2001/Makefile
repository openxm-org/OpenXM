# $OpenXM$
LATEX2HTML=latex2html -tmp /tmp
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
		touch .all-html.done ; \
	fi

.all-html.done: ${FILES}

clean:
	rm -rf .[a-z]*done ascm