#$OpenXM$

# install-help : all
#        Installing online help to OpenXM/lib/asir/help-*
# install-html-help
#        Copy html help to OpenXM/doc/asir-contrib
# gen-html-en  gen-html-ja
#        Generate HTML documents under html-en and html-ja
# gen-help-en  gen-help-ja
#        Generate help files.
OpenXM_ROOT=../../../../../../OpenXM
ASIRDOC=../../../../asir-doc
ASIRLIB=${OpenXM_ROOT}/../OpenXM_contrib2/asir2000/lib
CONTRIB=../../src
#The belows are set by the upper Makefile
INSTALL_doc=${HOME}/OX4/OpenXM/doc
INSTALL_lib=${HOME}/OX4/OpenXM/lib
OXWEAVE=oxweave
DVIPS=dvips -f
TEXI2HTML=texi2html -menu -split_node
RM=rm

##
DVIPDF=dvipdfm

INSTALL_doc_name=todo_parametrize
INSTALL_doc_contrib_ja=${INSTALL_doc}/asir-contrib/html-ja/${INSTALL_doc_name}
INSTALL_doc_contrib_en=${INSTALL_doc}/asir-contrib/html-en/${INSTALL_doc_name}

SRC_JA=todo_parametrize_ja.texi
SRC_EN=
TOP_JA=todo_parametrize_ja.texi
TOP_EN=todo_parametrize_en.texi

TOP_JA_DVI=${TOP_JA:.texi=.dvi}
TOP_EN_DVI=${TOP_EN:.texi=.dvi}
TOP_JA_PDF=${TOP_JA:.texi=.pdf}
TOP_EN_PDF=${TOP_EN:.texi=.pdf}

all : 
	@echo "Type in  ptex todo_parametrize_ja.texi to generate the manual in dvi format"


### Generating online manual
gen-help-en: ${SRC_EN} 
	-${RM} -rf tmp-en help-en
	-mkdir help-en tmp-en
	-cp ${SRC_EN} tmp-en
	-(cd tmp-en; makeinfo --force ${TOP_EN})
	-(cd help-en; for i in ../tmp-en/todo_parametrize_en; do ../${ASIRDOC}/extract_func $$i; done)

gen-help-ja: ${SRC_JA}
	-${RM} -rf tmp-ja help-ja
	-mkdir help-ja tmp-ja
	-for i in ${SRC_JA} ; do nkf -e $$i > tmp-ja/$$i; done
	-(cd tmp-ja; makeinfo --force ${TOP_JA})
	-(cd help-ja; for i in ../tmp-ja/todo_parametrize_ja; do ../${ASIRDOC}/extract_func -j $$i; done)

### Generate html manual
gen-html-en: gen-help-en
	-${RM} -rf html-en
	-mkdir html-en
	-cp tmp-en/*.texi tmp-en/*.en html-en
	-(cd html-en; ${TEXI2HTML} ${TOP_EN})

gen-html-ja: gen-help-ja
	-${RM} -rf html-ja
	-mkdir html-ja
	-cp tmp-ja/*.texi tmp-ja/*.ja html-ja
	-(cd html-ja; ${TEXI2HTML} ${TOP_JA})

### Generate dvi
gen-dvi: ${SRC_EN} ${SRC_JA}
	-@if [ -f ${TOP_JA} ] ; then \
	ptex ${TOP_JA} ; \
	ptex ${TOP_JA} ; \
	fi
	-@if [ -f ${TOP_EN} ] ; then \
	tex ${TOP_EN} ; \
	tex ${TOP_EN} ; \
	fi

install-help: 
	-make install-help-en
	-make install-help-ja

install-help-ja: gen-help-ja
	-cp -r -f help-ja/* ${INSTALL_lib}/asir/help-jp/xm.help

install-help-en: gen-help-en
	-cp -r -f help-en/* ${INSTALL_lib}/asir/help-eg/xm.help

install-html-help: gen-html-en gen-html-ja
	-mkdir ${INSTALL_doc_contrib_en}
	-((cd html-en; tar cf - .) | (cd ${INSTALL_doc_contrib_en}; tar xf -))
	-mkdir ${INSTALL_doc_contrib_ja}
	-((cd html-ja; tar cf - .) | (cd ${INSTALL_doc_contrib_ja}; tar xf -))


install-document2 :  gen-dvi
	-cp ${TOP_JA_DVI}  ${INSTALL_doc}/asir-contrib/
	-$(DVIPDF) ${TOP_JA_DVI}
	-cp ${TOP_JA_PDF} ${INSTALL_doc}/asir-contrib/
#
	-cp ${TOP_EN_DVI}  ${INSTALL_doc}/asir-contrib/
	-$(DVIPDF) ${TOP_EN_DVI}
	-cp ${TOP_EN_PDF} ${INSTALL_doc}/asir-contrib/

clean:
	-${RM} -rf ./.done_install ./.done_install_document
	-${RM} -f *.aux *.cp *.dvi *.fn *.ky *.log *.pg *.toc *.tp *.vr *.pdf *~
	-${RM} -f  *.en *.ja
	-${RM} -rf help-ja help-en tmp-en tmp-ja html-en html-ja
