# $OpenXM: OpenXM/src/asir-contrib/Makefile,v 1.3 2000/03/18 09:02:16 takayama Exp $

# targets:
# all
# install
# install-document
# clean

all :
	(cd packages/doc ; make all)

install :
	(cd packages/src ; make install)
	-(cd packages/doc ; make install)

install-document:
	-(cd packages/doc ; make install-document)

clean :
	(cd packages/src ; make clean)
	(cd packages/doc ; make clean)
