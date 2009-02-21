#$OpenXM: OpenXM/src/asir-contrib/packages/doc/f_res/Makefile,v 1.2 2007/09/29 07:29:29 takayama Exp $

# Name of this document.
MYNAME=f_res

TITLE = "Resultant Manual"

GENTEXI_OPT=--noSorting --author "OpenXM.org" --infoName asir-contrib-${MYNAME}
include ../M/Makefile

clean_private: 
	${RM} f_res-en.texi 

${MYNAME}.en ${MYNAME}.ja:

f_res-en.texi : 
	@printf "begin: plain|\nEnglish manual has not yet been written.\nend:" \
    | ${OXGENTEXI} ${GENTEXI_OPT} --title ${TITLE} > $@

