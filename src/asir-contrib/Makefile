# $OpenXM: OpenXM/src/asir-contrib/Makefile,v 1.2 2000/01/16 22:16:41 takayama Exp $

# targets:
# all
# install
# clean

all :
	(cd packages/doc ; make all)

install :
	(cd packages/src ; make install)
	-(cd packages/doc ; make install)

clean :
	(cd packages/src ; make clean)
	(cd packages/doc ; make clean)
