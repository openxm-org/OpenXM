# $Id$


all:
	@echo "'make compile': compile"
	@echo "'make clean'  : delete object file"

xxx:
	(cd radix; make $(OPT))
	(cd sha1; make $(OPT))
	(cd rsa; make $(OPT))


compile:
	make xxx OPT= OpenXM_HOME=${PWD}/../../..

clean:
	make xxx OPT=clean
	
