# $OpenXM: OpenXM/src/asir-doc/Makefile,v 1.4 2000/01/13 08:29:55 noro Exp $

WHICH=../../misc/which.pl

all: all-eg all-jp
install: install-eg install-jp

all-eg: texindex extract_man extract_func man-eg.dvi help-eg html-eg
all-jp: texindex extract_man extract_func man-jp.dvi help-jp html-jp

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
	parts/builtin/type.texi parts/builtin/upoly.texi

PARTS=\
	parts/algnum.texi parts/appendix.texi parts/asir.texi\
	parts/builtin.texi parts/debug.texi\
	parts/process.texi\
	parts/ff.texi parts/groebner.texi\
	parts/intro.texi parts/risa.texi parts/type.texi

SRCS=man.texi $(PARTS) $(BUILTINS)

tmpJP: $(SRCS)
	-rm -rf tmpJP
	-mkdir tmpJP tmpJP/parts tmpJP/parts/builtin
	-cp jtexinfo.tex texinfo-js.tex tmpJP
	-for i in ${SRCS}; do ./extract_man JP $$i > tmpJP/$$i; done

tmpJPeuc: $(SRCS)
	-rm -rf tmpJPeuc
	-mkdir tmpJPeuc tmpJPeuc/parts tmpJPeuc/parts/builtin
	-cp jtexinfo.tex texinfo-js.tex tmpJPeuc
	-for i in ${SRCS}; do \
	  ./extract_man JP $$i | nkf -e > tmpJPeuc/$$i; \
	done

tmpJPhtml: $(SRCS)
	-rm -rf tmpJPhtml
	-mkdir tmpJPhtml tmpJPhtml/parts tmpJPhtml/parts/builtin
	-cp jtexinfo.tex texinfo-js.tex tmpJPhtml
	-for i in ${SRCS}; do \
	  ./extract_man JP $$i | nkf -e | sed -e "s/@fref/@ref/g" > tmpJPhtml/$$i; \
	done 

tmpEG: $(SRCS)
	-rm -rf tmpEG
	-mkdir tmpEG tmpEG/parts tmpEG/parts/builtin
	-cp texinfo.tex tmpEG
	-for i in ${SRCS}; do ./extract_man EG $$i > tmpEG/$$i; done

tmpEGhtml: $(SRCS)
	-rm -rf tmpEGhtml
	-mkdir tmpEGhtml tmpEGhtml/parts tmpEGhtml/parts/builtin
	-cp texinfo.tex tmpEG
	-for i in ${SRCS}; do \
		./extract_man EG $$i | sed -e "s/@fref/@ref/g" > tmpEGhtml/$$i;\
	done
	
man-jp.dvi: tmpJP
	-rm man-jp.dvi
	-(cd tmpJP; ptex man.texi; ../jtexindex/C/texindex man.??; ptex man.texi)
	-mv tmpJP/man.dvi man-jp.dvi
	-touch man-jp.dvi

man-eg.dvi: tmpEG
	-rm man-eg.dvi
	-(cd tmpEG; tex man.texi; ../jtexindex/C/texindex man.??; tex man.texi)
	-mv tmpEG/man.dvi man-eg.dvi
	-touch man-eg.dvi

help-jp: tmpJPeuc
	-rm -rf help-jp
	-mkdir help-jp
	-(cd tmpJPeuc; makeinfo man.texi)
	-(cd help-jp; for i in ../tmpJPeuc/risaman*; do ../extract_func $$i; done)

help-eg: tmpEG
	-rm -rf help-eg
	-mkdir help-eg
	-(cd tmpEG; makeinfo man.texi)
	-(cd help-eg; for i in ../tmpEG/risaman*; do ../extract_func $$i; done)

html-jp: tmpJPhtml
	-rm -rf html-jp
	-mkdir html-jp
	-(cd html-jp; texi2html -menu -split_node ../tmpJPhtml/man.texi)

html-eg: tmpEGhtml
	-rm -rf html-eg
	-mkdir html-eg
	-(cd html-eg; texi2html -menu -split_node ../tmpEGhtml/man.texi)

extract_func: extract_func.c
	gcc -o extract_func extract_func.c

extract_man: extract_man.c
	gcc -o extract_man extract_man.c

targzuu: help-eg help-jp html-eg html-jp
	-tar cf - help-eg | gzip -9 | uuencode help-eg.tgz > help-eg.uu
	-tar cf - help-jp | gzip -9 | uuencode help-jp.tgz > help-jp.uu
	-tar cf - html-eg | gzip -9 | uuencode html-eg.tgz > html-eg.uu
	-tar cf - html-jp | gzip -9 | uuencode html-jp.tgz > html-jp.uu
	
clean:
	(cd jtexindex/C; make clean)
	-rm -f man*.dvi man-jp.dvi extract_man extract_func
	-rm -rf tmp*
	-rm -rf help-*
	-rm -rf html-*
