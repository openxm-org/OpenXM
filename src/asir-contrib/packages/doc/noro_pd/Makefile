#$OpenXM$

# Name of this document.
MYNAME=noro_pd

TITLE = "noro_pd Manual"

GENTEXI_OPT=--noSorting --author "OpenXM.org" --infoName asir-contrib-${MYNAME}
include ../M/Makefile

clean_private: 
	${RM} noro_pd-en.texi

${MYNAME}.en ${MYNAME}.ja:

noro_pd-en.texi: 
	@printf "begin: plain|\nEnglish manual has not yet been written.\nend:" \
    | ${OXGENTEXI} ${GENTEXI_OPT} --title ${TITLE} > $@

