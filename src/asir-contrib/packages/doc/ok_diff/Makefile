#$OpenXM: OpenXM/src/asir-contrib/packages/doc/ok_diff/Makefile,v 1.3 2009/02/21 22:09:12 ohara Exp $

# Name of this document.
MYNAME=ok_diff

GENTEXI_OPT=--noSorting --author "OpenXM.org" --infoName asir-contrib-${MYNAME}
include ../M/Makefile

clean_private: 
	${RM} ${MYNAME}-ja.texi ${MYNAME}-en.texi

${MYNAME}-ja.texi: ${MYNAME}.oxg
	@echo "Ok_diff (Okutani =?ISO-2022-JP?B?GyRCSEcbKEI=?=  =?ISO-2022-JP?B?GyRCSHkbKEI=?= =?ISO-2022-JP?B?GyRCSiw6bk1RQUc0WD90GyhC?= ) =?ISO-2022-JP?B?GyRCJV4bKEI=?= =?ISO-2022-JP?B?GyRCJUslZSUiJWsbKEI=?= " > title.tmp
	cat ${MYNAME}.oxg | ${OXWEAVE} usage-ja | ${OXGENTEXI} ${GENTEXI_OPT} --title "`${NKF} -w title.tmp`" > $@
	${RM} title.tmp

${MYNAME}-en.texi: ${MYNAME}.oxg
	cat ${MYNAME}.oxg | ${OXWEAVE} usage-en | ${OXGENTEXI} ${GENTEXI_OPT} --title "Ok_diff (a library for differential operators by Okutani) Manual" > $@
