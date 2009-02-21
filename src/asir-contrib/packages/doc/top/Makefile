#$OpenXM: OpenXM/src/asir-contrib/packages/doc/top/Makefile,v 1.6 2007/09/29 07:29:31 takayama Exp $

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

include ../M/Makefile

clean_private:
### base

${MYNAME}.en ${MYNAME}.ja:

${MYNAME}-en.texi: ${ENAUTO} ${ENSRCS}
${MYNAME}-ja.texi: ${JAAUTO} ${JASRCS}

base-auto.en: ${CONTRIB}/names.rr
	cat ${CONTRIB}/names.rr | ${OXWEAVE} usage \
	| ${OXGENTEXI} --category base --en --upnode "Basic (Standard Functions)" ${OPTGENTEXI} > $@

base-auto.ja: ${CONTRIB}/names.rr
	cat ${CONTRIB}/names.rr | ${OXWEAVE} usage \
	| ${OXGENTEXI} --category base --ja --upnode "Basic (Standard Functions)" ${OPTGENTEXI} > $@

### number
number-auto.en: ${CONTRIB}/names.rr
	cat ${CONTRIB}/names.rr | ${OXWEAVE} usage \
	| ${OXGENTEXI}  --category number --en --upnode "Numbers (Standard Mathematical Functions)" ${OPTGENTEXI} > $@
number-auto.ja: ${CONTRIB}/names.rr
	cat ${CONTRIB}/names.rr | ${OXWEAVE} usage \
	| ${OXGENTEXI}   --category number  --ja --upnode "Numbers (Standard Mathematical Functions)" ${OPTGENTEXI} > $@

### matrix
matrix-auto.en: ${CONTRIB}/names.rr
	cat ${CONTRIB}/names.rr | ${OXWEAVE} usage \
	| ${OXGENTEXI} --category matrix --en --upnode "Matrix (Standard Mathematical Functions)" ${OPTGENTEXI} > $@

matrix-auto.ja: ${CONTRIB}/names.rr
	cat ${CONTRIB}/names.rr | ${OXWEAVE} usage \
	| ${OXGENTEXI} --category matrix --ja --upnode "Matrix (Standard Mathematical Functions)" ${OPTGENTEXI} > $@

### print
print-auto.en: ${CONTRIB}/names.rr
	cat ${CONTRIB}/names.rr | ${OXWEAVE} usage \
	| ${OXGENTEXI} --category print --en --upnode "Print (Standard Mathematical Functions)" ${OPTGENTEXI} > $@

print-auto.ja: ${CONTRIB}/names.rr
	cat ${CONTRIB}/names.rr | ${OXWEAVE} usage \
	| ${OXGENTEXI} --category print --ja --upnode "Print (Standard Mathematical Functions)" ${OPTGENTEXI} > $@

### poly
poly-auto.en: ${CONTRIB}/names.rr
	cat ${CONTRIB}/names.rr | ${OXWEAVE} usage \
	| ${OXGENTEXI} --category poly --en --upnode "Polynomials (Standard Mathematical Functions)" ${OPTGENTEXI} >poly-auto.en

poly-auto.ja: ${CONTRIB}/names.rr
	cat ${CONTRIB}/names.rr | ${OXWEAVE} usage \
	| ${OXGENTEXI} --category poly --ja --upnode "Polynomials (Standard Mathematical Functions)" ${OPTGENTEXI} > $@

### glib
glib-auto.en: ${ASIRLIB}/glib
	cat ${ASIRLIB}/glib | ${OXWEAVE} usage \
	| ${OXGENTEXI}  --category glib --en --upnode "Graphic Library (2 dimensional)" ${OPTGENTEXI} > $@

glib-auto.ja: ${ASIRLIB}/glib
	cat ${ASIRLIB}/glib | ${OXWEAVE} usage \
	| ${OXGENTEXI}   --category glib  --ja --upnode "Graphic Library (2 dimensional)" ${OPTGENTEXI} > $@

### util
util-auto.en: ${CONTRIB}/names.rr
	cat ${CONTRIB}/names.rr | ${OXWEAVE} usage \
	| ${OXGENTEXI}  --category util --en --upnode "Utility Functions" ${OPTGENTEXI} > $@

util-auto.ja: ${CONTRIB}/names.rr
	cat ${CONTRIB}/names.rr | ${OXWEAVE} usage \
	| ${OXGENTEXI}   --category util  --ja --upnode "Utility Functions" ${OPTGENTEXI} > $@
### oxshell

oxshell-auto.en: ${CONTRIB}/oxshell.rr
	cat ${CONTRIB}/oxshell.rr | ${OXWEAVE} usage \
	| ${OXGENTEXI}  --category oxshell --en --upnode "Oxshell Functions" ${OPTGENTEXI} > $@

oxshell-auto.ja: ${CONTRIB}/oxshell.rr
	cat ${CONTRIB}/oxshell.rr | ${OXWEAVE} usage \
	| ${OXGENTEXI}   --category oxshell  --ja --upnode "Oxshell Functions" ${OPTGENTEXI} > $@

