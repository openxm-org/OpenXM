#$OpenXM: OpenXM/src/asir-contrib/packages/doc/taji_alc/Makefile,v 1.1 2008/01/26 05:20:16 takayama Exp $

# Name of this document.
MYNAME=taji_alc

TITLE = "Algebraic Local Cohomology in One Variable Manual"

GENTEXI_OPT=--noSorting --author "OpenXM.org" --infoName asir-contrib-${MYNAME}
include ../M/Makefile

clean_private: 
	${RM} taji_alc-en.texi 

${MYNAME}.en ${MYNAME}.ja:

taji_alc-en.texi: 
	@printf "begin: plain|\nEnglish manual has not yet been written.\nend:" \
    | ${OXGENTEXI} ${GENTEXI_OPT} --title ${TITLE} > $@

