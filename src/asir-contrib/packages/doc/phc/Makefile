#$OpenXM: OpenXM/src/asir-contrib/packages/doc/phc/Makefile,v 1.2 2007/09/29 07:29:30 takayama Exp $

# Name of this document.
MYNAME=phc

GENTEXI_OPT=--noSorting --author "OpenXM.org" --infoName asir-contrib-${MYNAME}
include ../M/Makefile

clean_private: 
	${RM} ${MYNAME}-ja.texi ${MYNAME}-en.texi

${MYNAME}-ja.texi: ${MYNAME}.oxg
	@echo "PHC OX server =?ISO-2022-JP?B?GyRCJV4lSyVlJSIlaxsoQg==?= " > title.tmp
	cat ${MYNAME}.oxg | ${OXWEAVE} usage-ja | ${OXGENTEXI} ${GENTEXI_OPT} --title "`${NKF} -e title.tmp`" > $@
	${RM} title.tmp

${MYNAME}-en.texi: ${MYNAME}.oxg
	cat ${MYNAME}.oxg | ${OXWEAVE} usage-en | ${OXGENTEXI} ${GENTEXI_OPT} --title "PHC OX server Manual" > $@

