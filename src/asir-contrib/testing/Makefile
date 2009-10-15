#$OpenXM: OpenXM/src/asir-contrib/testing/Makefile,v 1.7 2009/10/14 07:00:56 noro Exp $
all: t.texi

t.texi: tr-ja.oxt
	nkf -e tr-ja.oxt | oxgentexi --noSorting --title 'Term rewriting functions for Risa/Asir' --author 'Nobuki Takayama' >t.texi
	@echo "--------------------------------------------------"
	@echo "Document for tr.rr is generated with the name t.texi"
	@echo "--------------------------------------------------"

link-to-src:
	-(cd ../packages/src ; ln -s ../../testing/noro/new_rewrite.rr noro_rewrite.rr)
	-(cd ../packages/src ; ln -s ../../testing/noro/de.rr noro_de.rr)
	-(cd ../packages/src ; ln -s ../../testing/noro/gw.rr noro_gw.rr)
	-(cd ../packages/src ; ln -s ../../testing/noro/ndbf.rr nn_ndbf.rr)
	-(cd ../packages/src ; ln -s ../../testing/noro/module_syz.rr noro_module_syz.rr)
