#$OpenXM: OpenXM/src/asir-contrib/packages/doc/pfpcoh/Makefile,v 1.4 2009/02/21 22:09:12 ohara Exp $

# Name of this document.
MYNAME=pfpcoh

SRC_RR = ${CONTRIB}/pfpcoh.rr ${CONTRIB}/pfphom.rr

GENTEXI_OPT=--noSorting --author "OpenXM.org" --infoName asir-contrib-${MYNAME}

include ../M/Makefile

clean_private: 
	${RM} ${MYNAME}-ja.texi ${MYNAME}-en.texi

${MYNAME}-ja.texi: ${MYNAME}.oxg ${MYNAME}-auto.ja
	@echo "pfpcoh (cohomology/homology groups for p F q) =?ISO-2022-JP?B?GyRCJV4lSyVlJSIlaxsoQg==?= " > title.tmp
	cat ${MYNAME}.oxg | ${OXWEAVE} usage-ja | ${OXGENTEXI} ${GENTEXI_OPT} --title "`${NKF} -w title.tmp`" > $@
	${RM} title.tmp

${MYNAME}-en.texi: ${MYNAME}.oxg ${MYNAME}-auto.en
	cat ${MYNAME}.oxg | ${OXWEAVE} usage-en | ${OXGENTEXI} ${GENTEXI_OPT} --title "Pfpcoh (cohomology/homology groups for p F q)  Manual" > $@

${MYNAME}-auto.en: ${SRC_RR}
	cat ${SRC_RR} | ${OXWEAVE} usage  | ${OXGENTEXI} --en --upnode "Pfpcohom" ${OPTGENTEXI} > $@

${MYNAME}-auto.ja: ${SRC_RR}
	cat ${SRC_RR} | ${OXWEAVE} usage  | ${OXGENTEXI} --ja --upnode "Pfpcohom" ${OPTGENTEXI} > $@

${MYNAME}-ja.dvi: ${MYNAME}-auto.ja
${MYNAME}-en.dvi: ${MYNAME}-auto.en

