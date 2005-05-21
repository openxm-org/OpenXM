#$OpenXM$
all: t.texi

t.texi: tr-ja.oxt
	nkf -e tr-ja.oxt | oxgentexi --noSorting --title 'Term rewriting functions for Risa/Asir' --author 'Nobuki Takayama' >t.texi
	@echo "--------------------------------------------------"
	@echo "Document for tr.rr is generated with the name t.texi"
	@echo "--------------------------------------------------"
