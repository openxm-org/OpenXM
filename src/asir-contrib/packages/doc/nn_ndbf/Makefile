#$OpenXM: OpenXM/src/asir-contrib/packages/doc/nn_ndbf/Makefile,v 1.1 2009/11/01 07:44:54 noro Exp $

# Name of this document.
MYNAME=nn_ndbf

TITLE = "nn_ndbf Manual"

GENTEXI_OPT=--noSorting --author "OpenXM.org" --infoName asir-contrib-${MYNAME}
include ../M/Makefile

clean_private: 
	${RM} nn_ndbf-ja.texi nn_ndbf-en.texi

${MYNAME}.en ${MYNAME}.ja:

nn_ndbf-ja.texi: 
	 ../../../../asir-doc/extract_man ja nn_ndbf.texi | nkf -e | sed -e "s/@fref/@ref/g" > nn_ndbf-ja.texi

nn_ndbf-en.texi: 
	 ../../../../asir-doc/extract_man en nn_ndbf.texi > nn_ndbf-en.texi

