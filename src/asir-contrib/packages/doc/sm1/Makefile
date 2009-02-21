#$OpenXM: OpenXM/src/asir-contrib/packages/doc/sm1/Makefile,v 1.2 2007/09/29 07:29:30 takayama Exp $

# Name of this document.
MYNAME=sm1

SRC_RR = ${CONTRIB}/${MYNAME}.rr

GENTEXI_OPT=--noSorting --author "OpenXM.org" --infoName asir-contrib-${MYNAME}
include ../M/Makefile

clean_private: 
	${RM} ${MYNAME}-ja.texi ${MYNAME}-en.texi

${MYNAME}-ja.texi: ${MYNAME}.oxg ${MYNAME}-auto.ja
	@echo "Sm1 OX Server =?ISO-2022-JP?B?GyRCJV4lSyVlJSIlaxsoQg==?= " >title.tmp
	cat ${MYNAME}.oxg | ${OXWEAVE} usage-ja | ${OXGENTEXI} ${GENTEXI_OPT} --title "`${NKF} -e title.tmp`" > $@
	${RM} title.tmp

${MYNAME}-en.texi: ${MYNAME}.oxg ${MYNAME}-auto.en
	cat ${MYNAME}.oxg | ${OXWEAVE} usage-en | ${OXGENTEXI} ${GENTEXI_OPT} --title "Sm1 OX Server Manual" > $@

${MYNAME}-auto.en: ${SRC_RR}
	cat ${SRC_RR} | ${OXWEAVE} usage | ${OXGENTEXI} --category sm1 --en --upnode "Top" ${OPTGENTEXI} > $@

${MYNAME}-auto.ja: ${SRC_RR}
	cat ${SRC_RR} | ${OXWEAVE} usage | ${OXGENTEXI} --category sm1 --en --upnode "Top" ${OPTGENTEXI} > $@
