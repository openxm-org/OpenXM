#$OpenXM: OpenXM/src/asir-contrib/packages/doc/om/Makefile,v 1.3 2009/02/21 22:09:12 ohara Exp $

# Name of this document.
MYNAME=om

GENTEXI_OPT=--noSorting --author "OpenXM.org" --infoName asir-contrib-${MYNAME}
include ../M/Makefile

clean_private: 
	${RM} ${MYNAME}-ja.texi ${MYNAME}-en.texi ${MYNAME}.oxw

${MYNAME}.oxw: ${CONTRIB}/${MYNAME}.rr
	@echo "--------------------------------------" > $@
	@echo "THIS IS AUTHO GENERATED!! DO NOT EDIT." >> $@
	@echo "--------------------------------------" >> $@
	@echo "/*&ja" >> $@
	${OXWEAVE} C ja <${CONTRIB}/${MYNAME}.rr >> $@
	@echo "*/" >> $@
	@echo "/*&en" >> $@
	${OXWEAVE} C en <${CONTRIB}/${MYNAME}.rr >> $@
	@echo "*/" >> $@

${MYNAME}-ja.texi: ${MYNAME}.oxg
	@echo "OpenMath XML Translation Server =?ISO-2022-JP?B?GyRCJV4lSyVlJSIlaxsoQg==?= " > title.tmp
	cat ${MYNAME}.oxg | ${OXWEAVE} usage-ja | ${OXGENTEXI} ${GENTEXI_OPT} --title "`${NKF} -w title.tmp`" > $@
	${RM} title.tmp

${MYNAME}-en.texi: ${MYNAME}.oxg
	cat ${MYNAME}.oxg | ${OXWEAVE} usage-en | ${OXGENTEXI} ${GENTEXI_OPT} --title "OpenMath XML Translation Server Manual" > $@
