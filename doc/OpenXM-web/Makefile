# $OpenXM: OpenXM/doc/OpenXM-web/Makefile,v 1.1 2000/01/18 01:33:48 takayama Exp $
#
WEBKOBE=/home/web/OpenXM
OpenXM_doc=../.

all : install-web-kobe

install-web-kobe :  $(WEBKOBE)/index.html $(WEBKOBE)/issac2000.ps

$(WEBKOBE)/index.html : index.html
	cp -f index.html $(WEBKOBE)

$(WEBKOBE)/issac2000.ps : $(OpenXM_doc)/issac2000/issac2000.ps
	cp -f $(OpenXM_doc)/issac2000/issac2000.ps $(WEBKOBE)
