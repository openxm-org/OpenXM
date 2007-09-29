#$OpenXM: OpenXM/src/asir-contrib/packages/doc/todo_parametrize/Makefile,v 1.2 2005/04/14 02:21:49 takayama Exp $

# Name of this document.
MYNAME=todo_parametrize

SRC = 

ENSRCS = ${SRC:.oxw=.en} 
JASRCS = ${SRC:.oxw=.ja} 
ENAUTO= 
JAAUTO=${ENAUTO:.en=.ja}
GENTEXI_OPT=--noSorting --author "OpenXM.org" --infoName asir-contrib-${MYNAME}
include ../M/Makefile

clean_private: 
	${RM} -f todo_parametrize-en.texi 

todo_parametrize-en.texi : 
	@echo "begin: plain|" >auto.en
	@echo "English manual has not yet been written." >>auto.en
	@echo "end:" >>auto.en
	${OXGENTEXI} ${GENTEXI_OPT} --title "Parametrize Manual" <auto.en >${MYNAME}-en.texi

