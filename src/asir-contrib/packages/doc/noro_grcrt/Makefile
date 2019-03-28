#$OpenXM$

# Name of this document.
MYNAME=noro_grcrt

TITLE = "noro_grcrt Manual"

GENTEXI_OPT=--noSorting --author "OpenXM.org" --infoName asir-contrib-${MYNAME}
include ../M/Makefile

clean_private: 
	${RM} noro_grcrt-en.texi

${MYNAME}.en ${MYNAME}.ja:

noro_grcrt-en.texi: 
	@printf "begin: plain|\nEnglish manual has not yet been written.\nend:" \
    | ${OXGENTEXI} ${GENTEXI_OPT} --title ${TITLE} > $@

