# $OpenXM: OpenXM/src/asir-doc/Makefile,v 1.15 2003/09/03 02:17:39 takayama Exp $

WHICH=../../misc/which.pl

all:
	if [ ! -f .make_done ]; then make all2 ; touch .make_done ; fi
install:
	if [ ! -f .install_done ] ; then make install2 ; touch .install_done ; fi

all2: all-eg all-jp
install2: install-eg install-jp 

all-eg: texindex extract_man extract_func man-eg.dvi help-eg html-eg
all-jp: texindex extract_man extract_func man-jp.dvi help-jp html-jp html-internal-ja

install-eg: all-eg
	-rm -r -f ../../lib/asir/help-eg
	-tar cf - help-eg | (cd ../../lib/asir; tar xf - )
	-mkdir ../../doc/asir2000
	-rm -r -f ../../doc/asir2000/html-eg
	-tar cf - html-eg | ( cd ../../doc/asir2000; tar xf - )
	-cp man-eg.dvi ../../doc/asir2000

install-jp: all-jp
	-rm -r -f ../../lib/asir/help-jp
	-tar cf - help-jp | (cd ../../lib/asir; tar xf - )
	-mkdir ../../doc/asir2000
	-rm -r -f ../../doc/asir2000/html-jp
	-tar cf - html-jp | ( cd ../../doc/asir2000; tar xf - )
	-cp man-jp.dvi ../../doc/asir2000
	-tar cf - html-internal-ja | ( cd ../../doc/asir2000; tar xf - )

install-man-pages:
	-cp asir.1 ../../man/man1

configure:
	(cd jtexindex/C; ./configure; make clean)

texindex:
	(cd jtexindex/C; make)

nkf:
	(cd nkf-1.7; make)

BUILTINS=\
	parts/builtin/array.texi parts/builtin/bit.texi\
	parts/builtin/function.texi parts/builtin/io.texi\
	parts/builtin/list.texi parts/builtin/misc.texi\
	parts/builtin/num.texi parts/builtin/poly.texi\
	parts/builtin/string.texi\
	parts/builtin/type.texi parts/builtin/upoly.texi\
	parts/builtin/structure.texi parts/builtin/module.texi 

PARTS=\
	parts/algnum.texi parts/appendix.texi parts/asir.texi\
	parts/builtin.texi parts/debug.texi\
	parts/process.texi\
	parts/ff.texi parts/groebner.texi\
	parts/intro.texi parts/risa.texi parts/type.texi

SRCS=man.texi $(PARTS) $(BUILTINS)

INTERNAL_SRCS=internal.texi int-parts/datatype.texi int-parts/gc.texi\
    int-parts/inter.texi int-parts/intro.texi int-parts/macro.texi\
    int-parts/operation.texi int-parts/parser.texi int-parts/port.texi\
    int-parts/datatype/algnum.texi\
    int-parts/datatype/char2ff.texi\
    int-parts/datatype/dpoly.texi\
    int-parts/datatype/float.texi\
    int-parts/datatype/largeff.texi\
    int-parts/datatype/list.texi\
    int-parts/datatype/mat.texi\
    int-parts/datatype/number.texi\
    int-parts/datatype/poly.texi\
    int-parts/datatype/ratexp.texi\
    int-parts/datatype/ratnum.texi\
    int-parts/datatype/risaobj.texi\
    int-parts/datatype/smallff.texi\
    int-parts/datatype/string.texi\
    int-parts/datatype/vect.texi

tmpJP: $(SRCS)
	-rm -rf tmpJP
	-mkdir tmpJP tmpJP/parts tmpJP/parts/builtin
	-cp texinfo.* txi*tex tmpJP
	-for i in ${SRCS}; do ./extract_man JP $$i > tmpJP/$$i; done

tmpJPeuc: $(SRCS)
	-rm -rf tmpJPeuc
	-mkdir tmpJPeuc tmpJPeuc/parts tmpJPeuc/parts/builtin
	-cp texinfo.* txi*tex tmpJPeuc
	-for i in ${SRCS}; do \
	  ./extract_man JP $$i | nkf -e > tmpJPeuc/$$i; \
	done

tmpJPhtml: $(SRCS)
	-rm -rf tmpJPhtml
	-mkdir tmpJPhtml tmpJPhtml/parts tmpJPhtml/parts/builtin
	-cp texinfo.* txi*tex tmpJPhtml
	-for i in ${SRCS}; do \
	  ./extract_man JP $$i | nkf -e | sed -e "s/@fref/@ref/g" > tmpJPhtml/$$i; \
	done 

tmpEG: $(SRCS)
	-rm -rf tmpEG
	-mkdir tmpEG tmpEG/parts tmpEG/parts/builtin
	-cp texinfo.* txi*tex tmpEG
	-for i in ${SRCS}; do ./extract_man EG $$i > tmpEG/$$i; done

tmpEGhtml: $(SRCS)
	-rm -rf tmpEGhtml
	-mkdir tmpEGhtml tmpEGhtml/parts tmpEGhtml/parts/builtin
	-cp texinfo.* txi*tex tmpEG
	-for i in ${SRCS}; do \
		./extract_man EG $$i | sed -e "s/@fref/@ref/g" > tmpEGhtml/$$i;\
	done

tmp-internal-JAhtml: $(INTERNAL_SRCS)
	-rm -rf tmp-internal-JAhtml
	-mkdir tmp-internal-JAhtml tmp-internal-JAhtml/int-parts tmp-internal-JAhtml/int-parts/datatype
	-cp texinfo.* txi*tex tmp-internal-JAhtml
	-for i in ${INTERNAL_SRCS}; do \
	  ./extract_man JP $$i | nkf -e | sed -e "s/@fref/@ref/g" > tmp-internal-JAhtml/$$i; \
	done 

man-jp.dvi: tmpJP
	-rm man-jp.dvi
	-(cd tmpJP; ptex man.texi; ../jtexindex/C/texindex man.??; ptex man.texi)
	-mv tmpJP/man.dvi man-jp.dvi
	-touch man-jp.dvi

man-eg.dvi: tmpEG
	-rm man-eg.dvi
	-(cd tmpEG; tex man.texi; ../jtexindex/C/texindex man.??; tex man.texi)
	-(cd tmpEG; makeinfo man.texi)
	-mv tmpEG/man.dvi man-eg.dvi
	-touch man-eg.dvi

help-jp: tmpJPeuc man-jp.dvi
	-rm -rf help-jp
	-mkdir help-jp
	-(cd tmpJPeuc; makeinfo man.texi)
	-(cd help-jp; for i in ../tmpJPeuc/risaman*; do ../extract_func -j $$i; done)

help-eg: tmpEG man-eg.dvi
	-rm -rf help-eg
	-mkdir help-eg
	-(cd help-eg; for i in ../tmpEG/risaman*; do ../extract_func $$i; done)

html-jp: tmpJPhtml
	-rm -rf html-jp
	-mkdir html-jp
	-(cd html-jp; texi2html -menu -split_node ../tmpJPhtml/man.texi)

html-eg: tmpEGhtml
	-rm -rf html-eg
	-mkdir html-eg
	-(cd html-eg; texi2html -menu -split_node ../tmpEGhtml/man.texi)

html-win: html-jp gen_hh
	-rm -rf html-win
	-mkdir html-win
	(cd html-jp; for i in *; do nkf -s $$i > ../html-win/$$i; done)
	gen_hh html-win html-win

html-win-eg: html-eg gen_hh
	-rm -rf html-win-eg
	-mkdir html-win-eg
	(cd html-eg; for i in *; do nkf -s $$i > ../html-win-eg/$$i; done)
	gen_hh html-win-eg html-win-eg

html-internal-ja: tmp-internal-JAhtml
	-rm -rf html-internal-ja
	-mkdir html-internal-ja
	-(cd html-internal-ja; texi2html ../tmp-internal-JAhtml/internal.texi)

internal-jp.dvi: tmp-internal-JAhtml
	-rm internal-jp.dvi
	-(cd tmp-internal-JAhtml; ptex internal.texi; ../jtexindex/C/texindex internal.??; ptex internal.texi)
	-mv tmp-internal-JAhtml/internal.dvi internal-jp.dvi
	-touch internal-jp.dvi
extract_func: extract_func.c
	gcc -o extract_func extract_func.c

extract_man: extract_man.c
	gcc -o extract_man extract_man.c

gen_hh: html_tools/gen_hh.c
	gcc -o gen_hh html_tools/gen_hh.c

targzuu: help-eg help-jp html-eg html-jp
	-tar cf - help-eg | gzip -9 | uuencode help-eg.tgz > help-eg.uu
	-tar cf - help-jp | gzip -9 | uuencode help-jp.tgz > help-jp.uu
	-tar cf - html-eg | gzip -9 | uuencode html-eg.tgz > html-eg.uu
	-tar cf - html-jp | gzip -9 | uuencode html-jp.tgz > html-jp.uu

clean:
	(cd jtexindex/C; make clean)
	-rm -f man*.dvi man-jp.dvi extract_man extract_func gen_hh .*done
	-rm -f internal-jp.dvi
	-rm -rf tmp*
	-rm -rf help-*
	-rm -rf html-*
