#$OpenXM: OpenXM/src/asir-contrib/packages/doc/f_res/Makefile,v 1.1 2007/03/17 10:18:54 noro Exp $

# Name of this document.
MYNAME=f_res

SRC = 

ENSRCS = ${SRC:.oxw=.en} 
JASRCS = ${SRC:.oxw=.ja} 
ENAUTO= 
JAAUTO=${ENAUTO:.en=.ja}
GENTEXI_OPT=--noSorting --author "OpenXM.org" --infoName asir-contrib-${MYNAME}
include ../M/Makefile

clean_private: 
	${RM} -f f_res-en.texi 

f_res-en.texi : 
	@echo "begin: plain|" >auto.en
	@echo "English manual has not yet been written." >>auto.en
	@echo "end:" >>auto.en
	${OXGENTEXI} ${GENTEXI_OPT} --title "Resultant Manual" <auto.en >${MYNAME}-en.texi

