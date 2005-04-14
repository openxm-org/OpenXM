#$OpenXM: OpenXM/src/asir-contrib/packages/doc/todo_parametrize/Makefile,v 1.1 2005/03/24 08:50:02 takayama Exp $

# Name of this document.
MYNAME=todo_parametrize

SRC = 

ENSRCS = ${SRC:.oxw=.en} 
JASRCS = ${SRC:.oxw=.ja} 
ENAUTO= 
JAAUTO=${ENAUTO:.en=.ja}
GENTEXI_OPT=--noSorting --author "OpenXM.org" --infoName asir-contrib-${MYNAME}
include ../Makefile.common

clean_private: 
	${RM} -f todo_parametrize-en.texi 

todo_parametrize-en.texi : 
	@echo "begin: plain|" >auto.en
	@echo "English manual has not yet been written." >>auto.en
	@echo "end:" >>auto.en
	${OXGENTEXI} ${GENTEXI_OPT} --title "Parametrize Manual" <auto.en >${MYNAME}-en.texi

