# $OpenXM: OpenXM/src/asir/doc/Makefile,v 1.6 19/11/19 08:17:40 noro Exp $
all: texindex manual help
texindex:
	(cd jtexindex/C; ./configure; make)

manual: man.dvi

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

man.dvi: $(SRCS)
	ptex man.texi
	jtexindex/C/texindex man.??
	ptex man.texi

help: $(SRCS) extract_func
	-rm -rf euc help-jp
	mkdir help-jp euc euc/parts euc/parts/builtin
	for i in ${SRCS}; do nkf -e $$i > euc/$$i; done
	(cd euc; makeinfo man.texi)
	(cd help-jp; for i in ../euc/risaman*; do ../extract_func $$i; done)

html: $(SRCS)
	-rm -rf euc html
	mkdir html euc euc/parts euc/parts/builtin
	for i in ${SRCS}; do nkf -e $$i | sed -e "s/@fref/@ref/g" > euc/$$i; done
	(cd html; texi2html -menu -split_node ../euc/man.texi)

extract_func: extract_func.c
	gcc -o extract_func extract_func.c

clean:
	(cd jtexindex/C; make clean)
	rm -f man.?? man.???
	rm -rf euc html help-jp extract_func
