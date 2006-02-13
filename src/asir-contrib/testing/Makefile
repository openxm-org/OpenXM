#$OpenXM: OpenXM/src/asir-contrib/testing/Makefile,v 1.1 2005/05/21 10:21:26 takayama Exp $
all: t.texi

t.texi: tr-ja.oxt
	nkf -e tr-ja.oxt | oxgentexi --noSorting --title 'Term rewriting functions for Risa/Asir' --author 'Nobuki Takayama' >t.texi
	@echo "--------------------------------------------------"
	@echo "Document for tr.rr is generated with the name t.texi"
	@echo "--------------------------------------------------"

link-to-src:
	-(cd ../packages/src ; ln -s ../../testing/noro/new_rewrite.rr noro_rewrite.rr)
