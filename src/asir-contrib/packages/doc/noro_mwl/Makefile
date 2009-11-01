#$OpenXM$

# Name of this document.
MYNAME=noro_mwl

TITLE = "noro_mwl Manual"

GENTEXI_OPT=--noSorting --author "OpenXM.org" --infoName asir-contrib-${MYNAME}
include ../M/Makefile

clean_private: 
	${RM} noro_mwl-en.texi

${MYNAME}.en ${MYNAME}.ja:

noro_mwl-en.texi: 
	@printf "begin: plain|\nEnglish manual has not yet been written.\nend:" \
    | ${OXGENTEXI} ${GENTEXI_OPT} --title ${TITLE} > $@

