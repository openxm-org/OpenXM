# $OpenXM: OpenXM/src/asir-doc/Makefile,v 1.1.1.1 1999/12/08 05:47:43 noro Exp $

all-eg: texindex man-eg.dvi help-eg html-eg
all-jp: texindex man-jp.dvi help-jp html-jp

texindex:
	(cd jtexindex/C; ./configure; make)

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

tmpJP: $(SRCS) extract_man
	-rm -rf tmpJP
	mkdir tmpJP tmpJP/parts tmpJP/parts/builtin
	cp jtexinfo.tex texinfo-js.tex tmpJP
	for i in ${SRCS}; do extract_man JP $$i > tmpJP/$$i; done

tmpJPeuc: $(SRCS) extract_man
	-rm -rf tmpJPeuc
	mkdir tmpJPeuc tmpJPeuc/parts tmpJPeuc/parts/builtin
	cp jtexinfo.tex texinfo-js.tex tmpJPeuc
	for i in ${SRCS}; do extract_man JP $$i | nkf -e > tmpJPeuc/$$i; done

tmpJPhtml: $(SRCS) extract_man
	-rm -rf tmpJPhtml
	mkdir tmpJPhtml tmpJPhtml/parts tmpJPhtml/parts/builtin
	cp jtexinfo.tex texinfo-js.tex tmpJPhtml
	for i in ${SRCS}; do extract_man JP $$i | nkf -e | sed -e "s/@fref/@ref/g" > tmpJPhtml/$$i; done

tmpEG: $(SRCS) extract_man
	-rm -rf tmpEG
	mkdir tmpEG tmpEG/parts tmpEG/parts/builtin
	cp texinfo.tex tmpEG
	for i in ${SRCS}; do extract_man EG $$i > tmpEG/$$i; done

tmpEGhtml: $(SRCS) extract_man
	-rm -rf tmpEGhtml
	mkdir tmpEGhtml tmpEGhtml/parts tmpEGhtml/parts/builtin
	cp texinfo.tex tmpEG
	for i in ${SRCS}; do extract_man EG $$i | sed -e "s/@fref/@ref/g" > tmpEGhtml/$$i; done
	
man-jp.dvi: tmpJP texindex
	-rm man-jp.dvi
	(cd tmpJP; ptex man.texi; ../jtexindex/C/texindex man.??; ptex man.texi)
	mv tmpJP/man.dvi man-jp.dvi

man-eg.dvi: tmpEG texindex
	-rm man-eg.dvi
	(cd tmpEG; ptex man.texi; ../jtexindex/C/texindex man.??; tex man.texi)
	mv tmpEG/man.dvi man-eg.dvi

help-jp: tmpJPeuc extract_func
	-rm -rf help-jp
	mkdir help-jp
	(cd tmpJPeuc; makeinfo man.texi)
	(cd help-jp; for i in ../tmpJPeuc/risaman*; do ../extract_func $$i; done)

help-eg: tmpEG extract_func
	-rm -rf help-eg
	mkdir help-eg
	(cd tmpEG; makeinfo man.texi)
	(cd help-eg; for i in ../tmpEG/risaman*; do ../extract_func $$i; done)

html-jp: tmpJPhtml
	-rm -rf html-jp
	mkdir html-jp
	(cd html-jp; texi2html -menu -split_node ../tmpJPhtml/man.texi)

html-eg: tmpEGhtml
	-rm -rf html-eg
	mkdir html-eg
	(cd html-eg; texi2html -menu -split_node ../tmpEGhtml/man.texi)

extract_func: extract_func.c
	gcc -o extract_func extract_func.c

extract_man: extract_man.c
	gcc -o extract_man extract_man.c

clean:
	(cd jtexindex/C; make clean)
	rm -f man*.dvi man-jp.dvi extract_man extract_func
	rm -rf tmp*
	rm -rf help-*
	rm -rf html-*
