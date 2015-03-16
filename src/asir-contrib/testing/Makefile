#$OpenXM: OpenXM/src/asir-contrib/testing/Makefile,v 1.11 2011/01/16 08:52:44 noro Exp $
all: t.texi

t.texi: tr-ja.oxt
	nkf -e tr-ja.oxt | oxgentexi --noSorting --title 'Term rewriting functions for Risa/Asir' --author 'Nobuki Takayama' >t.texi
	@echo "--------------------------------------------------"
	@echo "Document for tr.rr is generated with the name t.texi"
	@echo "--------------------------------------------------"

link-to-src:
	-(cd ../packages/src ; rm -f noro_rewrite.rr; cp ../../testing/noro/new_rewrite.rr noro_rewrite.rr)
	-(cd ../packages/src ; rm -f noro_de.rr; cp ../../testing/noro/de.rr noro_de.rr)
	-(cd ../packages/src ; rm -f noro_gw.rr; cp ../../testing/noro/gw.rr noro_gw.rr)
	-(cd ../packages/src ; rm -f nn_ndbf.rr; cp ../../testing/noro/ndbf.rr nn_ndbf.rr)
	-(cd ../packages/src ; rm -f noro_module_syz.rr; cp ../../testing/noro/module_syz.rr noro_module_syz.rr)
	-(cd ../packages/src ; rm -f noro_mwl.rr; cp ../../testing/noro/mwl.rr noro_mwl.rr)
	-(cd ../packages/src ; rm -f noro_pd.rr; cp ../../testing/noro/new_pd.rr noro_pd.rr)
	-(cd ../packages/src ; rm -f noro_grcrt.rr; cp ../../testing/noro/grcrt.rr noro_grcrt.rr)
