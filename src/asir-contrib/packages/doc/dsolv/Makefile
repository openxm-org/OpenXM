#$OpenXM: OpenXM/src/asir-contrib/packages/doc/dsolv/Makefile,v 1.3 2009/02/21 22:09:12 ohara Exp $

# Name of this document.
MYNAME=dsolv

GENTEXI_OPT=--noSorting --author "OpenXM.org" --infoName asir-contrib-${MYNAME}
include ../M/Makefile

clean_private: 
	@for j in ${LANGUAGES} ; do ${RM} ${MYNAME}-$$j.texi ; done

${MYNAME}-ja.texi: ${MYNAME}.oxg
	@echo "Dsolv =?ISO-2022-JP?B?GyRCJV4lSyVlJSIlaxsoQg==?=" > title.tmp
	cat ${MYNAME}.oxg | ${OXWEAVE} usage-ja | ${OXGENTEXI} ${GENTEXI_OPT} --title "`${NKF} -w title.tmp`" > $@
	${RM} title.tmp

${MYNAME}-en.texi: ${MYNAME}.oxg
	cat ${MYNAME}.oxg | ${OXWEAVE} usage-en | ${OXGENTEXI} ${GENTEXI_OPT} --title "Dsolv Manual" > $@
