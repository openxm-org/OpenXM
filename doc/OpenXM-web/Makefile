# $OpenXM: OpenXM/doc/OpenXM-web/Makefile,v 1.2 2000/01/18 01:36:46 takayama Exp $
#
WEBKOBE=/home/web/OpenXM
WEBKOBE1=/home/web/OpenXM/20000124
OpenXM_doc=../.
OpenXM_spec=../../src/kxx
DVIPS=dvips -f

all : install-web-kobe

install-web-kobe :  $(WEBKOBE)/index.html $(WEBKOBE)/issac2000.ps $(WEBKOBE1)/cmotag.h $(WEBKOBE1)/oxMessageTag.h $(WEBKOBE1)/smCommand.h $(WEBKOBE1)/openxm-eg.ps $(WEBKOBE1)/openxm-jp.ps

$(WEBKOBE)/index.html : index.html
	cp -f index.html $(WEBKOBE)

$(WEBKOBE)/issac2000.ps : $(OpenXM_doc)/issac2000/issac2000.ps
	cp -f $(OpenXM_doc)/issac2000/issac2000.ps $(WEBKOBE)

$(WEBKOBE1)/cmotag.h : cmotag.h
	cp -f cmotag.h $(WEBKOBE1)

$(WEBKOBE1)/oxMessageTag.h : oxMessageTag.h
	cp -f oxMessageTag.h $(WEBKOBE1)

$(WEBKOBE1)/smCommand.h : smCommand.h
	cp -f smCommand.h $(WEBKOBE1)

$(WEBKOBE1)/openxm-eg.ps : $(OpenXM_spec)/openxxx.tex
	-rm -f $(WEBKOBE1)/openxm-eg.ps
	-(cd $(OpenXM_spec) ; make doc ; latex openxm-eg ; latex openxm-eg)
	-$(DVIPS) $(OpenXM_spec)/openxm-eg >$(WEBKOBE1)/openxm-eg.ps

$(WEBKOBE1)/openxm-jp.ps : $(OpenXM_spec)/openxxx.tex
	-rm -f $(WEBKOBE1)/openxm-jp.ps
	-(cd $(OpenXM_spec) ; make doc ; platex openxm-jp ; platex openxm-jp)
	-$(DVIPS) $(OpenXM_spec)/openxm-jp >$(WEBKOBE1)/openxm-jp.ps

clean-web-kobe :
	-rm $(WEBKOBE)/index.html $(WEBKOBE)/issac2000.ps $(WEBKOBE1)/cmotag.h $(WEBKOBE1)/oxMessageTag.h $(WEBKOBE1)/smCommand.h $(WEBKOBE1)/openxm-eg.ps $(WEBKOBE1)/openxm-jp.ps
