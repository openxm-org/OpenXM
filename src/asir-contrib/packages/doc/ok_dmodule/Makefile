#$OpenXM: OpenXM/src/asir-contrib/packages/doc/ok_dmodule/Makefile,v 1.3 2009/02/21 22:09:12 ohara Exp $

# Name of this document.
MYNAME=ok_dmodule

GENTEXI_OPT=--noSorting --author "OpenXM.org" --infoName asir-contrib-${MYNAME}
include ../M/Makefile

clean_private: 
	${RM} ${MYNAME}-ja.texi ${MYNAME}-en.texi

${MYNAME}-ja.texi: ${MYNAME}.oxg
	@echo "Ok_dmodule (Okutani =?ISO-2022-JP?B?GyRCSEcbKEI=?=  D-module =?ISO-2022-JP?B?GyRCNFgbKEI=?= =?ISO-2022-JP?B?GyRCP3QbKEI=?= ) =?ISO-2022-JP?B?GyRCJV4lSyVlGyhC?= =?ISO-2022-JP?B?GyRCJSIlaxsoQg==?=" > title.tmp
	cat ${MYNAME}.oxg | ${OXWEAVE} usage-ja | ${OXGENTEXI} ${GENTEXI_OPT} --title "`${NKF} -w title.tmp`" > $@
	${RM} title.tmp

${MYNAME}-en.texi: ${MYNAME}.oxg
	cat ${MYNAME}.oxg | ${OXWEAVE} usage-en | ${OXGENTEXI} ${GENTEXI_OPT} --title "Ok_dmodule (a library for D-modules by Okutani) Manual" > $@
