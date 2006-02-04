# $OpenXM$
# Do not add OpenXM tags to translation resources.
usage:
	@echo "make utf8 to translate eucJP files into utf8"
	@echo "After the translation, commit them."

utf8: asir-ja_JP.UTF8.sm1 sm1-ja_JP.UTF8.sm1

asir-ja_JP.UTF8.sm1 : asir-ja_JP.eucJP.sm1
	nkf -w asir-ja_JP.eucJP.sm1 >asir-ja_JP.UTF8.sm1

sm1-ja_JP.UTF8.sm1 : sm1-ja_JP.eucJP.sm1
	nkf -w sm1-ja_JP.eucJP.sm1 >sm1-ja_JP.UTF8.sm1
