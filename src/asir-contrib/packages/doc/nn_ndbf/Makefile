#$OpenXM$

# Name of this document.
MYNAME=nn_ndbf

TITLE = "nn_ndbf Manual"

GENTEXI_OPT=--noSorting --author "OpenXM.org" --infoName asir-contrib-${MYNAME}
include ../M/Makefile

clean_private: 
	${RM} nn_ndbf-en.texi

${MYNAME}.en ${MYNAME}.ja:

nn_ndbf-en.texi: 
	@printf "begin: plain|\nEnglish manual has not yet been written.\nend:" \
    | ${OXGENTEXI} ${GENTEXI_OPT} --title ${TITLE} > $@

