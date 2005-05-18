#$OpenXM: OpenXM/src/asir-contrib/packages/doc/top/Makefile,v 1.4 2005/04/11 11:13:32 takayama Exp $

# Name of this document.
MYNAME=cman

SRC = base.oxw calculus.oxw complex.oxw \
      glib.oxw graphic.oxw hypergeometric.oxw \
      intro.oxw matrix.oxw misc.oxw names.oxw \
      number.oxw oxrfc100.oxw \
      poly.oxw print.oxw series.oxw \
      util.oxw windows.oxw oxshell.oxw load.oxw

ENSRCS = ${SRC:.oxw=.en} 
JASRCS = ${SRC:.oxw=.ja} 
ENAUTO=  base-auto.en glib-auto.en matrix-auto.en number-auto.en \
         poly-auto.en print-auto.en util-auto.en \
         oxshell-auto.en
JAAUTO=${ENAUTO:.en=.ja}

include ../Makefile.common
clean_private:
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

