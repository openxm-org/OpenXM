#$OpenXM$

# Name of this document.
MYNAME=noro_matrix

SRC = 

ENSRCS = ${SRC:.oxw=.en} 
JASRCS = ${SRC:.oxw=.ja} 
ENAUTO= 
JAAUTO=${ENAUTO:.en=.ja}
GENTEXI_OPT=--noSorting --author "OpenXM.org" --infoName asir-contrib-${MYNAME}
include ../M/Makefile

clean_private: 
	${RM} -f f_res-en.texi 

noro_matrix-en.texi : 
	@echo "begin: plain|" >auto.en
	@echo "English manual has not yet been written." >>auto.en
	@echo "end:" >>auto.en
	${OXGENTEXI} ${GENTEXI_OPT} --title "noro_matrix Manual" <auto.en >${MYNAME}-en.texi

