#$OpenXM: OpenXM/src/asir-contrib/packages/doc/top/Makefile,v 1.2 2005/04/09 05:49:24 takayama Exp $
# Targets
# gen-texi-en, gen-texi-ja : generating texi files.
# gen-html-en,  gen-html-ja :
#        Generate HTML documents under html-en and html-ja
# gen-help-en,  gen-help-ja :
#        Generate online help files under help-en, help-ja
# gen-dvi-en, gen-dvi-ja : generating dvi, ps, pdf files here.
# gen-ps-en,  gen-ps-ja  :
# gen-pdf-en, gen-pdf-ja :
#
# install-help-en, -ja 
#        Installing online help to OpenXM/lib/asir/help-eg/xm.help
#        Installing online help to OpenXM/lib/asir/help-jp/xm.help
# install-html-en, -ja
#        Copy html help to OpenXM/doc/asir-contrib/ja, en
# install-dvi-en, -ja; install-ps-en, -ja; install-pdf-en, -ja;
#        Copy the documents in dvi and ps to OpenXM/doc/asir-contrib/ja, en
#
# NOTE: install-help should be executed after installing asir2000.
########################################################################
#  How to add new documents in the category xxx
# "xxx*.texi <--- xxx.oxweave (manually written) + xxx*-auto.{en,ja} (automatic)"
#   (1) "xxx.oxweave" --> xxx.en and xxx.ja
#       xxx-auto.en and xxx-auto.ja should be included from
#       xxx.en and xxx.ja  (JASRCS, ENSRCS)
#     NOTE: xxx.oxweave might be embedded in xxx.rr or xxx,
#           but this style is obsolete.
#   (2) "xxx.rr" or "xxx" --> xxx-auto.en and xxx-auto.ja
#       (JAAUTO, ENAUTO)
#       xxx-auto*.texi's are extracted from xxx or xxx.rr
#   (3) xxx.en and xxx.ja should be included from cman.texi
#     and cman-en.texi
#  cf.  glib*  base*
########################################################################
# They are configured and set by the upper level Makefile
INSTALL_doc=${OpenXM_HOME}/doc
INSTALL_lib=${OpenXM_HOME}/lib
OXWEAVE=${OpenXM_HOME}/bin/oxweave
OXGENTEXI=${OpenXM_HOME}/bin/oxgentexi
#
OpenXM_ROOT=../../../../../../OpenXM
ASIRDOC=${OpenXM_ROOT}/src/asir-doc
ASIRLIB=${OpenXM_ROOT}/../OpenXM_contrib2/asir2000/lib
CONTRIB=${OpenXM_ROOT}/src/asir-contrib/packages/src
OPTGENTEXI= --include
#OPTGENTEXI= --include --example
## PS documents are installed under ${OpenXM_doc}/asir-contrib
DVIPS=dvips -f
TEXI2HTML=texi2html -menu -split_node
RM=rm
DVIPDF=dvipdfm

SRC = base.oxw calculus.oxw complex.oxw \
      glib.oxw graphic.oxw hypergeometric.oxw \
      intro.oxw matrix.oxw misc.oxw names.oxw \
      number.oxw oxrfc100.oxw \
      poly.oxw print.oxw series.oxw \
      util.oxw windows.oxw oxshell.oxw

ENSRCS = ${SRC:.oxw=.en} 
JASRCS = ${SRC:.oxw=.ja} 
ENAUTO=  base-auto.en glib-auto.en matrix-auto.en number-auto.en \
         poly-auto.en print-auto.en util-auto.en \
         oxshell-auto.en
JAAUTO=${ENAUTO:.en=.ja}

.SUFFIXES: .en .ja .oxw .dvi .pdf

.oxw.en:
	${RM} -f ${<:.oxw=.en}
	${OXWEAVE} C en < $< > ${<:.oxw=.en}

.oxw.ja:
	${RM} -f ${<:.oxw=.ja}
	${OXWEAVE} C ja < $< > ${<:.oxw=.ja}

.dvi.pdf:
	${DVIPDF} $<

gen-texi-ja: ${JASRCS} ${JAAUTO} cman-ja.texi
gen-texi-en: ${ENSRCS} ${ENAUTO} cman-en.texi

### base
base-auto.en : ${CONTRIB}/names.rr 
	${RM} -f base-auto.en
	${OXWEAVE} usage  <${CONTRIB}/names.rr | ${OXGENTEXI} --category base --en --upnode "Basic (Standard Functions)" ${OPTGENTEXI} >base-auto.en
base-auto.ja : ${CONTRIB}/names.rr 
	${RM} -f base-auto.ja
	${OXWEAVE} usage  <${CONTRIB}/names.rr | ${OXGENTEXI} --category base --ja --upnode "Basic (Standard Functions)" ${OPTGENTEXI} >base-auto.ja

### number
number-auto.en :  ${CONTRIB}/names.rr 
	${RM} -f number-auto.en
	cat ${CONTRIB}/names.rr | ${OXWEAVE} usage | ${OXGENTEXI}  --category number --en --upnode "Numbers (Standard Mathematical Functions)" ${OPTGENTEXI} >number-auto.en
number-auto.ja : ${CONTRIB}/names.rr
	${RM} -f number-auto.ja
	cat ${CONTRIB}/names.rr | ${OXWEAVE} usage  | ${OXGENTEXI}   --category number  --ja --upnode "Numbers (Standard Mathematical Functions)" ${OPTGENTEXI} >number-auto.ja

### matrix
matrix-auto.en : ${CONTRIB}/names.rr
	${RM} -f matrix-auto.en
	${OXWEAVE} usage  <${CONTRIB}/names.rr | ${OXGENTEXI} --category matrix --en --upnode "Matrix (Standard Mathematical Functions)" ${OPTGENTEXI} >matrix-auto.en
matrix-auto.ja : ${CONTRIB}/names.rr
	${RM} -f matrix-auto.ja
	${OXWEAVE} usage  <${CONTRIB}/names.rr | ${OXGENTEXI} --category matrix --ja --upnode "Matrix (Standard Mathematical Functions)" ${OPTGENTEXI} >matrix-auto.ja

### print
print-auto.en : ${CONTRIB}/names.rr
	${RM} -f print-auto.en
	${OXWEAVE} usage  <${CONTRIB}/names.rr | ${OXGENTEXI} --category print --en --upnode "Print (Standard Mathematical Functions)" ${OPTGENTEXI} >print-auto.en
print-auto.ja : ${CONTRIB}/names.rr
	${RM} -f print-auto.ja
	${OXWEAVE} usage  <${CONTRIB}/names.rr | ${OXGENTEXI} --category print --ja --upnode "Print (Standard Mathematical Functions)" ${OPTGENTEXI} >print-auto.ja

### poly
poly-auto.en : ${CONTRIB}/names.rr
	${RM} -f poly-auto.en
	${OXWEAVE} usage  <${CONTRIB}/names.rr | ${OXGENTEXI} --category poly --en --upnode "Polynomials (Standard Mathematical Functions)" ${OPTGENTEXI} >poly-auto.en
poly-auto.ja : ${CONTRIB}/names.rr
	${RM} -f poly-auto.ja
	${OXWEAVE} usage  <${CONTRIB}/names.rr | ${OXGENTEXI} --category poly --ja --upnode "Polynomials (Standard Mathematical Functions)" ${OPTGENTEXI} >poly-auto.ja

### glib
glib-auto.en : ${ASIRLIB}/glib
	${RM} -f glib-auto.en
	cat ${ASIRLIB}/glib | ${OXWEAVE} usage | ${OXGENTEXI}  --category glib --en --upnode "Graphic Library (2 dimensional)" ${OPTGENTEXI} >glib-auto.en
glib-auto.ja : ${ASIRLIB}/glib
	${RM} -f glib-auto.ja
	cat ${ASIRLIB}/glib | ${OXWEAVE} usage  | ${OXGENTEXI}   --category glib  --ja --upnode "Graphic Library (2 dimensional)" ${OPTGENTEXI} >glib-auto.ja

### util
util-auto.en :  ${CONTRIB}/names.rr
	${RM} -f util-auto.en
	cat ${CONTRIB}/names.rr | ${OXWEAVE} usage | ${OXGENTEXI}  --category util --en --upnode "Utility Functions" ${OPTGENTEXI} >util-auto.en
util-auto.ja : ${CONTRIB}/names.rr
	${RM} -f util-auto.ja
	cat ${CONTRIB}/names.rr | ${OXWEAVE} usage  | ${OXGENTEXI}   --category util  --ja --upnode "Utility Functions" ${OPTGENTEXI} >util-auto.ja
### oxshell
oxshell-auto.en :  ${CONTRIB}/oxshell.rr
	${RM} -f oxshell-auto.en
	cat ${CONTRIB}/oxshell.rr | ${OXWEAVE} usage | ${OXGENTEXI}  --category oxshell --en --upnode "Oxshell Functions" ${OPTGENTEXI} >oxshell-auto.en
oxshell-auto.ja : ${CONTRIB}/oxshell.rr
	${RM} -f oxshell-auto.ja
	cat ${CONTRIB}/oxshell.rr | ${OXWEAVE} usage  | ${OXGENTEXI}   --category oxshell  --ja --upnode "Oxshell Functions" ${OPTGENTEXI} >oxshell-auto.ja

### Generating online manual.  gen-help
gen-help-en: ${ENSRCS} ${ASIRDOC}/extract_func ${ENAUTO}
	-${RM} -rf tmp-en help-en
	mkdir help-en tmp-en
	cp cman-en.texi ${ENSRCS} ${ENAUTO} tmp-en
	-(cd tmp-en; makeinfo --force cman-en.texi)
	(cd help-en; for i in ../tmp-en/asir-contrib*; do ../${ASIRDOC}/extract_func $$i; done)
	-for i in ${SUBDIRS}; do (cd $$i; make gen-help-en); done

gen-help-ja: ${JASRCS} ${ASIRDOC}/extract_func ${JAAUTO}
	-${RM} -rf tmp-ja help-ja
	mkdir help-ja tmp-ja
	for i in cman-ja.texi ${JASRCS} ${JAAUTO}; do nkf -e $$i > tmp-ja/$$i; done
	-(cd tmp-ja; makeinfo --force cman-ja.texi)
	(cd help-ja; for i in ../tmp-ja/asir-contrib*; do ../${ASIRDOC}/extract_func -j $$i; done)
	-for i in ${SUBDIRS}; do (cd $$i; make gen-help-ja); done

gen-html-en: gen-help-en
	-${RM} -rf html-en
	-mkdir html-en
	-cp tmp-en/*.texi tmp-en/*.en html-en
	-(cd html-en; ${TEXI2HTML} cman-en.texi)
	-(cd html-en; rm -f *.en cman-en.texi)

gen-html-ja: gen-help-ja
	-mkdir html-ja
	-cp tmp-ja/*.texi tmp-ja/*.ja html-ja
	-cp tmp-ja/cman-ja.texi html-ja/cman-ja.texi
	-(cd html-ja; ${TEXI2HTML} cman-ja.texi)
	-(cd html-ja; rm -f *.ja cman-ja.texi)

gen-dvi-ja:  cman-ja.dvi
gen-dvi-en:  cman-en.dvi
gen-pdf-ja:  cman-ja.pdf
gen-pdf-en:  cman-en.pdf
gen-ps-ja:  cman-ja.ps
gen-ps-en:  cman-en.ps
cman-ja.dvi: ${JASRCS} ${JAAUTO}  cman-ja.texi
	-ptex cman-ja.texi
	-ptex cman-ja.texi
cman-en.dvi: ${ENSRCS} ${ENAUTO} cman-en.texi
	-tex cman-en.texi
	-tex cman-en.texi
cman-ja.pdf: cman-ja.dvi
	-${DVIPDF} cman-ja
cman-en.pdf: cman-en.dvi
	-${DVIPDF} cman-en
cman-ja.ps: cman-ja.dvi
	-${DVIPS} cman-ja >cman-ja.ps
cman-en.ps: cman-en.ps 
	-${DVIPS} cman-en >cman-en.ps

install-help-ja: gen-help-ja
	${RM} -rf ${INSTALL_lib}/asir/help-jp/xm.help
	mkdir ${INSTALL_lib}/asir/help-jp/xm.help
	-cp -r -f help-ja/* ${INSTALL_lib}/asir/help-jp/xm.help
install-help-en: gen-help-en
	${RM} -rf ${INSTALL_lib}/asir/help-eg/xm.help
	mkdir ${INSTALL_lib}/asir/help-eg/xm.help
	-cp -r -f help-en/* ${INSTALL_lib}/asir/help-eg/xm.help
install-html-ja: gen-html-ja
	-mkdir -p ${INSTALL_doc}/asir-contrib/ja
	-( tar cf - html-ja | (cd ${INSTALL_doc}/asir-contrib/ja ; tar xf - ; mv help-ja cman-html))
install-html-en: gen-html-en
	-mkdir -p ${INSTALL_doc}/asir-contrib/en
	-( tar cf - html-en | (cd ${INSTALL_doc}/asir-contrib/en ; tar xf - ; mv help-en cman-html))
install-dvi-ja: gen-dvi-ja
	-mkdir -p ${INSTALL_lib}/asir-contrib/ja
	-cp cman-ja.dvi ${INSTALL_doc}/asir-contrib/ja/cman-ja.dvi
install-dvi-en: gen-dvi-en
	-mkdir -p ${INSTALL_lib}/asir-contrib/en
	-cp cman-en.dvi ${INSTALL_doc}/asir-contrib/en/cman-en.dvi
install-ps-ja: gen-ps-ja
	-mkdir -p ${INSTALL_lib}/asir-contrib/ja
	-cp cman-ja.ps ${INSTALL_doc}/asir-contrib/ja/cman-ja.ps
install-ps-en: gen-ps-en
	-mkdir -p ${INSTALL_lib}/asir-contrib/en
	-cp cman-en.ps ${INSTALL_doc}/asir-contrib/en/cman-en.ps
install-pdf-ja: gen-pdf-ja
	-mkdir -p ${INSTALL_lib}/asir-contrib/ja
	-cp cman-ja.pdf ${INSTALL_doc}/asir-contrib/ja/cman-ja.pdf
install-pdf-en: gen-pdf-en
	-mkdir -p ${INSTALL_lib}/asir-contrib/en
	-cp cman-en.pdf ${INSTALL_doc}/asir-contrib/en/cman-en.pdf

clean:
	${RM} -f ${JAAUTO} ${ENAUTO} gentexi-in.tmp gentexi-out.tmp
	${RM} -f *.aux *.cp *.dvi *.fn *.ky *.log *.pg *.toc *.tp *.vr *~
	${RM} -f  *.en *.ja
	${RM} -f cman-en.ps cman-ja.ps cman-en.pdf cman-ja.pdf
	${RM} -rf help-ja help-en tmp-en tmp-ja html-en html-ja
