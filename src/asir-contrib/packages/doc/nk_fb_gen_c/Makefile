#$OpenXM: OpenXM/src/asir-contrib/packages/doc/nk_fb_gen_c/Makefile,v 1.3 2017/03/30 06:51:38 takayama Exp $

# Name of this document.
MYNAME=nk_fb_gen_c

#alltexi: ${MYNAME}-ja.texi  ${MYNAME}-en.texi

GENTEXI_OPT=--noSorting --author "OpenXM.org" --infoName asir-contrib-${MYNAME}

include ../M/Makefile
${MYNAME}.en ${MYNAME}.ja:

clean_private: 
	${RM} ${MYNAME}-ja.texi ${MYNAME}-en.texi a.out

${MYNAME}-ja.texi: ${MYNAME}.oxg 
	@echo "Fisher-Bingham MLE =?ISO-2022-JP?B?GyRCJV4lSyVlJSIlaxsoQg==?= " >title.tmp
	nkf -w ${MYNAME}.oxg | ${OXWEAVE} usage-ja | ${OXGENTEXI} ${GENTEXI_OPT} --title "`${NKF} -w title.tmp`" > $@
	${RM} title.tmp

${MYNAME}-en.texi: ${MYNAME}.oxg 
	cat ${MYNAME}.oxg | ${OXWEAVE} usage-en | ${OXGENTEXI} ${GENTEXI_OPT} --title "Fisher-Bingham MLE Manual" > $@

