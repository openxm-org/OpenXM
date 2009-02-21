#$OpenXM: OpenXM/src/asir-contrib/packages/doc/noro_matrix/Makefile,v 1.1 2008/05/18 07:37:43 noro Exp $

# Name of this document.
MYNAME=noro_matrix

TITLE = "noro_matrix Manual"

GENTEXI_OPT=--noSorting --author "OpenXM.org" --infoName asir-contrib-${MYNAME}
include ../M/Makefile

clean_private: 
	${RM} noro_matrix-en.texi

${MYNAME}.en ${MYNAME}.ja:

noro_matrix-en.texi: 
	@printf "begin: plain|\nEnglish manual has not yet been written.\nend:" \
    | ${OXGENTEXI} ${GENTEXI_OPT} --title ${TITLE} > $@

