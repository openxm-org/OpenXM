# $OpenXM: OpenXM/doc/OpenXM-web/Makefile,v 1.4 2000/01/20 07:25:25 takayama Exp $
#
WEBKOBE=/home/web/OpenXM
WEBKOBE1=/home/web/OpenXM/20000124
OpenXM_doc=../.
OpenXM_spec=../OpenXM-specs
DVIPS=dvips -f

all : install-web-kobe

ALL : all all-spec copy-doc

all-spec : $(WEBKOBE1)/openxm-eg.ps $(WEBKOBE1)/openxm-jp.ps

copy-doc :
	/bin/rm -rf $(WEBKOBE1)/doc
	mkdir $(WEBKOBE1)/doc
	-(cd ../../doc ; cp -r Papers asir-contrib asir2000 kan96xx $(WEBKOBE1)/doc)



install-web-kobe :  $(WEBKOBE)/index.html $(WEBKOBE)/issac2000.ps $(WEBKOBE1)/cmotag.h $(WEBKOBE1)/oxMessageTag.h $(WEBKOBE1)/smCommand.h  $(WEBKOBE1)/install-eg.txt $(WEBKOBE1)/install-jp.txt 

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

$(WEBKOBE1)/openxm-eg.ps : $(OpenXM_spec)/tmpEG/OpenXM.tex
	-rm -f $(WEBKOBE1)/openxm-eg.ps
	-(cd $(OpenXM_spec) ; make doc ; cd tmpEG ; latex OpenXM ; latex OpenXM)
	-$(DVIPS) $(OpenXM_spec)/tmpEG/OpenXM >$(WEBKOBE1)/openxm-eg.ps

$(WEBKOBE1)/openxm-jp.ps : $(OpenXM_spec)/tmpJP/OpenXM.tex
	-rm -f $(WEBKOBE1)/openxm-jp.ps
	-(cd $(OpenXM_spec) ; make doc ; cd tmpJP ; platex OpenXM ; platex OpenXM)
	-$(DVIPS) $(OpenXM_spec)/tmpJP/OpenXM >$(WEBKOBE1)/openxm-jp.ps

$(WEBKOBE1)/install-eg.txt : $(OpenXM_doc)/install-eg.txt
	-cp $(OpenXM_doc)/install-eg.txt $(WEBKOBE1)

$(OpenXM_spec)/tmpEG/OpenXM.tex :
	-(cd $(OpenXM_spec) ; make )
$(OpenXM_spec)/tmpJP/OpenXM.tex :
	-(cd $(OpenXM_spec) ; make )

$(WEBKOBE1)/install-jp.txt : $(OpenXM_doc)/install-jp.txt
	-cp $(OpenXM_doc)/install-jp.txt $(WEBKOBE1)
$(OpenXM_doc)/install-eg.txt : $(OpenXM_doc)/install.oxweave
	( cd $(OpenXM_doc) ; make )
$(OpenXM_doc)/install-jp.txt : $(OpenXM_doc)/install.oxweave
	( cd $(OpenXM_doc) ; make )

clean-web-kobe :
	-rm $(WEBKOBE)/index.html $(WEBKOBE)/issac2000.ps $(WEBKOBE1)/cmotag.h $(WEBKOBE1)/oxMessageTag.h $(WEBKOBE1)/smCommand.h $(WEBKOBE1)/openxm-eg.ps $(WEBKOBE1)/openxm-jp.ps
