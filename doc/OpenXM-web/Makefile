# $OpenXM: OpenXM/doc/OpenXM-web/Makefile,v 1.35 2001/10/27 11:22:35 takayama Exp $
#
WEBKOBE=/home/web/OpenXM
WEBKOBE1=/home/web/OpenXM/1.2.1
OpenXM_doc=../.
OpenXM_spec=../OpenXM-specs
OpenXM_calc=../calc2000p
OpenXM_ascm=../ascm2001
DVIPS=dvips -f
LATEX2HTML=latex2html -tmp /tmp
OXWEAVE=../../bin/oxweave
OW_FLAG=--recursive
RSYNC=rsync -avz --delete -e ssh
WHO=taka@orange
SSH=ssh -l taka orange

### For full copy :  make copy-html; make ALL
# Only for print target.
PS=a2ps-j
LPR=lpr -Pdousan

all : install-web-kobe

ALL : all all-spec copy-doc

all-spec : ${WEBKOBE1}/openxm-en.ps ${WEBKOBE1}/openxm-ja.ps


copy-doc :
	/bin/rm -rf ${WEBKOBE1}/doc
	mkdir ${WEBKOBE1}/doc
	-(cd ../../doc ; cp -r Papers asir-contrib asir2000 kan96xx ox_math pari ${WEBKOBE1}/doc)

copy-html: copy-html-intro copy-html-spec copy-html-poster copy-html-spec-101

copy-html-spec: copy-html-spec-ja copy-html-spec-en

copy-html-spec-ja: 
	$(SSH) /bin/rm -rf ${WEBKOBE1}/html/OpenXM-ja
	-$(SSH) mkdir ${WEBKOBE1}/html
	$(SSH) mkdir ${WEBKOBE1}/html/OpenXM-ja
	-(cd $(OpenXM_spec) ; cd tmpJA; platex OpenXM; platex OpenXM ; $(LATEX2HTML) OpenXM )
	-(cd $(OpenXM_spec) ; cd tmpJA ; $(RSYNC) OpenXM $(WHO):${WEBKOBE1}/html/OpenXM-ja)

copy-html-spec-en: 
	$(SSH) /bin/rm -rf ${WEBKOBE1}/html/OpenXM-en
	-$(SSH) mkdir ${WEBKOBE1}/html
	$(SSH) mkdir ${WEBKOBE1}/html/OpenXM-en
	-(cd $(OpenXM_spec) ; make ; cd tmpEN ; latex OpenXM; latex OpenXM ; $(LATEX2HTML) OpenXM )
	-(cd $(OpenXM_spec) ; cd tmpEN ; $(RSYNC) OpenXM $(WHO):${WEBKOBE1}/html/OpenXM-en)

copy-html-spec-101: copy-html-spec-101-ja copy-html-spec-101-en

copy-html-spec-101-ja:
	$(SSH) /bin/rm -rf ${WEBKOBE1}/html/OX-RFC-101-ja
	-$(SSH) mkdir ${WEBKOBE1}/html
	$(SSH) mkdir ${WEBKOBE1}/html/OX-RFC-101-ja
	-(cd $(OpenXM_spec) ; cd tmp101JA; platex OX-RFC-101; platex OX-RFC-101 ; $(LATEX2HTML) OX-RFC-101 )
	-(cd $(OpenXM_spec) ; cd tmp101JA ; $(RSYNC) OX-RFC-101 $(WHO):${WEBKOBE1}/html/OX-RFC-101-ja)

copy-html-spec-101-en:
	$(SSH) /bin/rm -rf ${WEBKOBE1}/html/OX-RFC-101-en
	-$(SSH) mkdir ${WEBKOBE1}/html
	$(SSH) mkdir ${WEBKOBE1}/html/OX-RFC-101-en
	-(cd $(OpenXM_spec) ; make ; cd tmp101EN ; latex OX-RFC-101; latex OX-RFC-101 ; $(LATEX2HTML) OX-RFC-101 )
	-(cd $(OpenXM_spec) ; cd tmp101EN ; $(RSYNC) OX-RFC-101 $(WHO):${WEBKOBE1}/html/OX-RFC-101-en)

copy-html-poster:
	-mkdir ${WEBKOBE1}/html
	/bin/rm -rf ${WEBKOBE1}/html/OpenXM-poster
	mkdir ${WEBKOBE1}/html/OpenXM-poster
	/bin/rm -rf tmp-poster
	mkdir tmp-poster
	cp ../calc2000/katsura7.ps tmp-poster
	cp $(OpenXM_calc)/rgb.sty tmp-poster
	cp $(OpenXM_calc)/3k.ps tmp-poster
	cp $(OpenXM_calc)/func1.tex tmp-poster
	cp $(OpenXM_calc)/func2.tex tmp-poster
	cp $(OpenXM_calc)/efficient.tex tmp-poster
	-(cd tmp-poster ; latex func1 ; $(LATEX2HTML) func1 )
	-(cd tmp-poster ; latex func2 ; $(LATEX2HTML) func2 )
	-(cd tmp-poster ; latex efficient ; $(LATEX2HTML) efficient )
	-(cd tmp-poster ; cp -rf func1 func2 efficient ${WEBKOBE1}/html/OpenXM-poster)

copy-html-intro: 
	$(SSH) /bin/rm -rf ${WEBKOBE}/ascm2001
	-$(SSH) mkdir ${WEBKOBE}/ascm2001
	$(SSH) mkdir ${WEBKOBE1}/ascm2001
	-(cd $(OpenXM_ascm) ; latex ascm2001; latex ascm2001 ; $(LATEX2HTML) ascm2001 )
	-(cd $(OpenXM_ascm) ; $(RSYNC) ascm2001 $(WHO):${WEBKOBE}/ascm2001)

## Under construction.
namagize-html: copy-html
	-(cd ${WEBKOBE1}/html/OpenXM-ja ; rm -rf OpenXM-ja.index ; mkdir OpenXM-ja.index )
	-(cd ${WEBKOBE1}/html/OpenXM-ja/OpenXM-ja.index ; mknmz ../OpenXM-ja/OpenXM)

usage:
	@echo "Targets are "
	@echo "    install-web-kobe all-spec copy-doc copy-html"
	@echo "   "
	@echo " install-web-kobe-main :  main html files."
	@echo "Several examples"
	@echo " 1. Generate OpenXM-FAQ-en.txt for a test."
	@echo "   make WEBKOBE1=. ./OpenXM-FAQ-en.txt"
	@echo " 2. Generate OpenXM-FAQ-ja.txt for a test."
	@echo "   make WEBKOBE1=. ./OpenXM-FAQ-ja.txt"

checkDir :
	if test ! -d ${WEBKOBE1} ; \
	then \
	  mkdir ${WEBKOBE1} ;  \
	fi 
	if test ! -d ${WEBKOBE1}/doc ; \
	then \
	  mkdir ${WEBKOBE1}/doc ;  \
	fi \


install-web-kobe : install-web-kobe-main install-web-kobe-suppl
install-web-kobe-main : checkDir ${WEBKOBE}/index.html ${WEBKOBE}/OpenXM-RFC.html  ${WEBKOBE1}/cmotag.h ${WEBKOBE1}/oxMessageTag.h ${WEBKOBE1}/smCommand.h  ${WEBKOBE1}/install-en.txt ${WEBKOBE1}/install-ja.txt  ${WEBKOBE1}/ERRATA.html ${WEBKOBE1}/index-binary.html ${WEBKOBE1}/index-packages.html ${WEBKOBE1}/install-faq.txt ${WEBKOBE1}/OpenXM-FAQ-en.txt ${WEBKOBE1}/OpenXM-FAQ-ja.txt ${WEBKOBE1}/doc/index.html ${WEBKOBE}/people.html ${WEBKOBE}/anonymous.html

install-web-kobe-suppl : ${WEBKOBE}/issac2000.ps

${WEBKOBE}/index.html : index.html
	cp -f index.html ${WEBKOBE}
	cp -f index.html ${WEBKOBE1}

${WEBKOBE}/people.html : people.html
	cp -f people.html ${WEBKOBE}

${WEBKOBE}/anonymous.html : anonymous.html
	cp -f anonymous.html ${WEBKOBE}

${WEBKOBE1}/index-binary.html : index-binary.html
	cp -f index-binary.html ${WEBKOBE1}

${WEBKOBE}/OpenXM-RFC.html : OpenXM-RFC.html
	cp -f OpenXM-RFC.html ${WEBKOBE}

${WEBKOBE1}/index-packages.html : index-packages.html
	cp -f index-packages.html ${WEBKOBE1}

${WEBKOBE}/issac2000.ps : $(OpenXM_doc)/issac2000/issac2000.ps
	cp -f $(OpenXM_doc)/issac2000/issac2000.ps ${WEBKOBE}

${WEBKOBE1}/cmotag.h : cmotag.h
	cp -f cmotag.h ${WEBKOBE1}

${WEBKOBE1}/oxMessageTag.h : oxMessageTag.h
	cp -f oxMessageTag.h ${WEBKOBE1}

${WEBKOBE1}/smCommand.h : smCommand.h
	cp -f smCommand.h ${WEBKOBE1}

${WEBKOBE1}/openxm-en.ps : $(OpenXM_spec)/tmpEN/OpenXM.tex
	-rm -f ${WEBKOBE1}/openxm-en.ps
	-(cd $(OpenXM_spec) ; make doc ; cd tmpEN ; latex OpenXM ; latex OpenXM)
	-( cd $(OpenXM_spec)/tmpEN ; $(DVIPS) OpenXM >${WEBKOBE1}/openxm-en.ps)

${WEBKOBE1}/openxm-ja.ps : $(OpenXM_spec)/tmpJA/OpenXM.tex
	-rm -f ${WEBKOBE1}/openxm-ja.ps
	-(cd $(OpenXM_spec) ; make doc ; cd tmpJA ; platex OpenXM ; platex OpenXM)
	-(cd $(OpenXM_spec)/tmpJA ; $(DVIPS) OpenXM >${WEBKOBE1}/openxm-ja.ps)

${WEBKOBE1}/install-en.txt : $(OpenXM_doc)/install-en.txt
	-cp $(OpenXM_doc)/install-en.txt ${WEBKOBE1}

$(OpenXM_spec)/tmpEN/OpenXM.tex :
	-(cd $(OpenXM_spec) ; make )
$(OpenXM_spec)/tmpJA/OpenXM.tex :
	-(cd $(OpenXM_spec) ; make )

${WEBKOBE1}/install-ja.txt : $(OpenXM_doc)/install-ja.txt
	-cp $(OpenXM_doc)/install-ja.txt ${WEBKOBE1}
$(OpenXM_doc)/install-en.txt : $(OpenXM_doc)/install.oxweave
	( cd $(OpenXM_doc) ; make )
$(OpenXM_doc)/install-ja.txt : $(OpenXM_doc)/install.oxweave
	( cd $(OpenXM_doc) ; make )
${WEBKOBE1}/ERRATA.html : ERRATA.html
	cp -f ERRATA.html ${WEBKOBE1}/ERRATA.html
${WEBKOBE1}/install-faq.txt : install-faq.txt
	cp -f install-faq.txt ${WEBKOBE1}/install-faq.txt
${WEBKOBE1}/OpenXM-FAQ-en.txt : ../OpenXM-FAQ.oxweave
	-rm ${WEBKOBE1}/OpenXM-FAQ-en.txt
	echo "DO NOT EDIT THIS FILE" >${WEBKOBE1}/OpenXM-FAQ-en.txt
	${OXWEAVE} ${OW_FLAG} en C <../OpenXM-FAQ.oxweave >>${WEBKOBE1}/OpenXM-FAQ-en.txt
${WEBKOBE1}/OpenXM-FAQ-ja.txt : ../OpenXM-FAQ.oxweave
	-rm ${WEBKOBE1}/OpenXM-FAQ-ja.txt
	echo "DO NOT EDIT THIS FILE" >${WEBKOBE1}/OpenXM-FAQ-ja.txt
	${OXWEAVE} ${OW_FLAG} ja C <../OpenXM-FAQ.oxweave >>${WEBKOBE1}/OpenXM-FAQ-ja.txt
${WEBKOBE1}/doc/index.html : index-doc.html
	cp -f index-doc.html ${WEBKOBE1}/doc/index.html

copy-errata : 
	cp -f ERRATA.html ${WEBKOBE1}/ERRATA.html
copy-index :
	cp -f index.html ${WEBKOBE1}/index.html

copy-errata-top : 
	cp -f ERRATA.html ${WEBKOBE}/ERRATA.html
copy-index-top :
	cp -f index.html ${WEBKOBE}/index.html

clean-web-kobe :
	-rm ${WEBKOBE}/index.html ${WEBKOBE}/issac2000.ps ${WEBKOBE1}/cmotag.h ${WEBKOBE1}/oxMessageTag.h ${WEBKOBE1}/smCommand.h ${WEBKOBE1}/openxm-en.ps ${WEBKOBE1}/openxm-ja.ps

clean:
	/bin/rm -rf tmp-poster calc2000

print:
	$(PS) Makefile | $(LPR)
	$(PS) index.html | $(LPR)
