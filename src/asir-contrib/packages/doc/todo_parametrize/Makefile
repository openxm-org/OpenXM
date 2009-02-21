#$OpenXM: OpenXM/src/asir-contrib/packages/doc/todo_parametrize/Makefile,v 1.3 2007/09/29 07:29:31 takayama Exp $

# Name of this document.
MYNAME=todo_parametrize

TITLE = "Parametrize Manual"

GENTEXI_OPT=--noSorting --author "OpenXM.org" --infoName asir-contrib-${MYNAME}
include ../M/Makefile

clean_private:
	${RM} todo_parametrize-en.texi

${MYNAME}.en ${MYNAME}.ja:

todo_parametrize-en.texi:
	@printf "begin: plain|\nEnglish manual has not yet been written.\nend:" \
    | ${OXGENTEXI} ${GENTEXI_OPT} --title ${TITLE} > $@



