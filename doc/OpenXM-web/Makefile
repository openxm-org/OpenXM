# $OpenXM: OpenXM/doc/OpenXM-web/Makefile,v 1.21 2000/04/13 00:43:27 takayama Exp $
#
WEBKOBE=/home/web/OpenXM
WEBKOBE1=/home/web/OpenXM/1.1.3
OpenXM_doc=../.
OpenXM_spec=../OpenXM-specs
DVIPS=dvips -f
LATEX2HTML=jlatex2html -tmp /tmp

### For full copy :  make copy-html; make ALL
# Only for print target.
PS=a2ps-j
LPR=lpr -Pdousan

all : install-web-kobe

ALL : all all-spec copy-doc

all-spec : $(WEBKOBE1)/openxm-en.ps $(WEBKOBE1)/openxm-ja.ps


checkDir :
	if test ! -d ${WEBKOBE1} ; \
	then \
	  mkdir ${WEBKOBE1} ;  \
	fi

copy-doc :
	/bin/rm -rf $(WEBKOBE1)/doc
	mkdir $(WEBKOBE1)/doc
	-(cd ../../doc ; cp -r Papers asir-contrib asir2000 kan96xx ox_math pari $(WEBKOBE1)/doc)

copy-html :
	/bin/rm -rf $(WEBKOBE1)/html
	mkdir $(WEBKOBE1)/html
	mkdir $(WEBKOBE1)/html/OpenXM-en
	mkdir $(WEBKOBE1)/html/OpenXM-ja
	-(cd $(OpenXM_spec) ; make ; cd tmpEG ; latex OpenXM; latex OpenXM ; $(LATEX2HTML) OpenXM )
	-(cd $(OpenXM_spec) ; cd tmpJP ; platex OpenXM; platex OpenXM ; $(LATEX2HTML) OpenXM )
	-(cd $(OpenXM_spec) ; cd tmpEG ; cp -rf OpenXM $(WEBKOBE1)/html/OpenXM-en)
	-(cd $(OpenXM_spec) ; cd tmpJP ; cp -rf OpenXM $(WEBKOBE1)/html/OpenXM-ja)

## Under construction.
namagize-html: copy-html
	-(cd $(WEBKOBE1)/html/OpenXM-ja ; rm -rf OpenXM-ja.index ; mkdir OpenXM-ja.index )
	-(cd $(WEBKOBE1)/html/OpenXM-ja/OpenXM-ja.index ; mknmz ../OpenXM-ja/OpenXM)



install-web-kobe :  checkDir $(WEBKOBE)/index.html $(WEBKOBE)/issac2000.ps $(WEBKOBE1)/cmotag.h $(WEBKOBE1)/oxMessageTag.h $(WEBKOBE1)/smCommand.h  $(WEBKOBE1)/install-en.txt $(WEBKOBE1)/install-ja.txt  $(WEBKOBE1)/ERRATA.html $(WEBKOBE1)/index-binary.html $(WEBKOBE1)/index-packages.html $(WEBKOBE1)/install-faq.txt $(WEBKOBE1)/OpenXM-FAQ.txt $(WEBKOBE1)/doc/index.html

$(WEBKOBE)/index.html : index.html
	cp -f index.html $(WEBKOBE)
	cp -f index.html $(WEBKOBE1)

$(WEBKOBE1)/index-binary.html : index-binary.html
	cp -f index-binary.html $(WEBKOBE1)

$(WEBKOBE1)/index-packages.html : index-packages.html
	cp -f index-packages.html $(WEBKOBE1)

$(WEBKOBE)/issac2000.ps : $(OpenXM_doc)/issac2000/issac2000.ps
	cp -f $(OpenXM_doc)/issac2000/issac2000.ps $(WEBKOBE)

$(WEBKOBE1)/cmotag.h : cmotag.h
	cp -f cmotag.h $(WEBKOBE1)

$(WEBKOBE1)/oxMessageTag.h : oxMessageTag.h
	cp -f oxMessageTag.h $(WEBKOBE1)

$(WEBKOBE1)/smCommand.h : smCommand.h
	cp -f smCommand.h $(WEBKOBE1)

$(WEBKOBE1)/openxm-en.ps : $(OpenXM_spec)/tmpEG/OpenXM.tex
	-rm -f $(WEBKOBE1)/openxm-en.ps
	-(cd $(OpenXM_spec) ; make doc ; cd tmpEG ; latex OpenXM ; latex OpenXM)
	-( cd $(OpenXM_spec)/tmpEG ; $(DVIPS) OpenXM >$(WEBKOBE1)/openxm-en.ps)

$(WEBKOBE1)/openxm-ja.ps : $(OpenXM_spec)/tmpJP/OpenXM.tex
	-rm -f $(WEBKOBE1)/openxm-ja.ps
	-(cd $(OpenXM_spec) ; make doc ; cd tmpJP ; platex OpenXM ; platex OpenXM)
	-(cd $(OpenXM_spec)/tmpJP ; $(DVIPS) OpenXM >$(WEBKOBE1)/openxm-ja.ps)

$(WEBKOBE1)/install-en.txt : $(OpenXM_doc)/install-en.txt
	-cp $(OpenXM_doc)/install-en.txt $(WEBKOBE1)

$(OpenXM_spec)/tmpEG/OpenXM.tex :
	-(cd $(OpenXM_spec) ; make )
$(OpenXM_spec)/tmpJP/OpenXM.tex :
	-(cd $(OpenXM_spec) ; make )

$(WEBKOBE1)/install-ja.txt : $(OpenXM_doc)/install-ja.txt
	-cp $(OpenXM_doc)/install-ja.txt $(WEBKOBE1)
$(OpenXM_doc)/install-en.txt : $(OpenXM_doc)/install.oxweave
	( cd $(OpenXM_doc) ; make )
$(OpenXM_doc)/install-ja.txt : $(OpenXM_doc)/install.oxweave
	( cd $(OpenXM_doc) ; make )
$(WEBKOBE1)/ERRATA.html : ERRATA.html
	cp -f ERRATA.html $(WEBKOBE1)/ERRATA.html
$(WEBKOBE1)/install-faq.txt : install-faq.txt
	cp -f install-faq.txt $(WEBKOBE1)/install-faq.txt
$(WEBKOBE1)/OpenXM-FAQ.txt : ../OpenXM-FAQ.oxweave
	cp -f ../OpenXM-FAQ.oxweave $(WEBKOBE1)/OpenXM-FAQ.txt
$(WEBKOBE1)/doc/index.html : index-doc.html
	cp -f index-doc.html $(WEBKOBE1)/doc/index.html

copy-errata : 
	cp -f ERRATA.html $(WEBKOBE1)/ERRATA.html
copy-index :
	cp -f index.html $(WEBKOBE1)/index.html

copy-errata-top : 
	cp -f ERRATA.html $(WEBKOBE)/ERRATA.html
copy-index-top :
	cp -f index.html $(WEBKOBE)/index.html

clean-web-kobe :
	-rm $(WEBKOBE)/index.html $(WEBKOBE)/issac2000.ps $(WEBKOBE1)/cmotag.h $(WEBKOBE1)/oxMessageTag.h $(WEBKOBE1)/smCommand.h $(WEBKOBE1)/openxm-en.ps $(WEBKOBE1)/openxm-ja.ps

print:
	$(PS) Makefile | $(LPR)
	$(PS) index.html | $(LPR)
