# $OpenXM: OpenXM/doc/OpenXM-web/Makefile,v 1.57 2020/03/14 02:25:07 takayama Exp $
#
OpenXM_HOME=../..
OpenXM_include=${OpenXM_HOME}/include
OXHOME=/home/web/OpenXM
#OXHOME=/home/taka/afo/OpenXM    ## for test.
CURRENT=${OXHOME}/Current
CURRENT_DOC=${CURRENT}/doc
STABLE=${OXHOME}/Stable
ASCM2001=${OXHOME}/ascm2001
OpenXM_doc=../.
OpenXM_spec=../OpenXM-specs
OpenXM_calc=../calc2000p
OpenXM_ascm=../ascm2001
DVIPS=dvips -f
LATEX2HTML=latex2html -tmp /tmp
OXWEAVE=${OpenXM_HOME}/bin/oxweave
OW_FLAG=--recursive
RSYNC=rsync -Cavz 
RM=rm

CURRENT_FILES=index.html index-ja.html index-spec.html index-spec-ja.html \
              OpenXM-RFC.html \
              cmotag.h oxMessageTag.h smCommand.h \
              anonymous.html ERRATA.html

OXWEAVE_FILES_in_doc=install-en.txt install-ja.txt OpenXM-FAQ-en.txt OpenXM-FAQ-ja.txt OpenXM-FAQ-ja.txt.html

LOGOS=logo-openxm2.jpg logo-openxm5.jpg logo-openxm_simple0.jpg

MANUALS=asir2000 asir-contrib kan96xx ox_math

usage:
	@echo "targets are all"
	@echo "all-top (copy only top html files to web)"
	@echo "all-current (generates html files form texi etc and copy to web)"
	@echo "all-current-index (no new generation of html files from texi etc, <all-current)"
	@echo "all-current-spec"
	@echo "all-FAQ (after generating files by oxweave in .. by hand)"
all: all-stable all-ascm2001 all-current all-top all-current-spec
	@echo "************************************************************"
	@echo "* cp -r  /home/web/OpenXM/Current /home/web/OpenXM/x.y.z   *"
	@echo "* to generate a page of the specific version x.y.z         *"
	@echo "* Edit /home/web/OpenXM/x.y.z/index.html by hand.          *"
	@echo "************************************************************"

configure-dir:
	if test ! -f .configure-dir.done ; \
	then \
		if test ! -d ${OXHOME} ; then mkdir ${OXHOME} ; fi ; \
		if test ! -d ${CURRENT} ; then mkdir ${CURRENT} ; fi ; \
		if test ! -d ${CURRENT}/doc ; then mkdir ${CURRENT}/doc ; fi ; \
		if test ! -d ${STABLE} ; then mkdir ${STABLE} ; fi ; \
		if test ! -d ${STABLE}/html ; then mkdir ${STABLE}/html ; fi ; \
		if test ! -d ${STABLE}/html/OpenXM-poster ; then mkdir ${STABLE}/html/OpenXM-poster ; fi ; \
		if test ! -d ${ASCM2001} ; then mkdir ${ASCM2001} ; fi ; \
		touch .configure-dir.done ; \
	fi

all-stable:  configure-dir
	make -f Makefile-intro all-html-poster
	(cd tmp-poster ; ${RSYNC} func1 func2 efficient ${STABLE}/html/OpenXM-poster)

all-ascm2001: configure-dir
	make -f Makefile-intro all-html-ascm
	-(cd ${OpenXM_ascm} ; ${RSYNC} ascm2001 ${OXHOME}/ascm2001)

all-current-index: configure-dir
	cp index-doc.html ${CURRENT_DOC}/index-doc.html
	cp index-doc-ja.html ${CURRENT_DOC}/index-doc-ja.html
	cp google-ja.html ${CURRENT_DOC}/google-ja.html
	cp index-doc-asir-contrib2-ja.html ${CURRENT_DOC}/asir-contrib
	cp index-doc-asir-contrib2-en.html ${CURRENT_DOC}/asir-contrib
	cp index-license.html ${CURRENT_DOC}/index-license.html
	cp index-doc-asir-ja.html ${CURRENT_DOC}/index-doc-asir-ja.html
	cp index-doc-asir-en.html ${CURRENT_DOC}/index-doc-asir-en.html
	cp index-doc-asir-en.html ${CURRENT_DOC}/index-doc-asir.html
	cp video.html ${CURRENT}/video.html
	cp video-ja.html ${CURRENT}/video-ja.html
	${RSYNC} ${CURRENT_FILES} ${CURRENT}
all-current: all-current-index configure-dir
	(cd ${OpenXM_doc} ; make all)
	(cd ${OpenXM_doc} ; ${RSYNC} ${OXWEAVE_FILES_in_doc} ${CURRENT})
	${RSYNC} ${CURRENT_FILES} ${CURRENT}
	@echo "***************************************************"
	@echo "*  Have you done (cd OpenXM/src ; make install)   *"
	@echo "*                (cd OpenXM/src ; make install-document) "
	@echo "*  to generate the documents?                     *"
	@echo "***************************************************"
	(cd ${OpenXM_doc} ; ${RSYNC} ${MANUALS} ${CURRENT_DOC})

all-current-spec: configure-dir
	(cd ${OpenXM_spec} ; make CURRENT=${CURRENT} install-web-all)

all-top: configure-dir
	cp top.html ${OXHOME}/index.html
	cp top-ja.html ${OXHOME}/index-ja.html
	cp inobuta-ja.html ${OXHOME}/inobuta-ja.html
	cp news-o.html ${OXHOME}/news-o.html
	cp news-o-ja.html ${OXHOME}/news-o-ja.html
	cp ../../Copyright/contrib-and-hist-ja.txt ${OXHOME}
	${RSYNC} people.html ${OXHOME}
	(cd Screenshot ; ${RSYNC} *.html ${OXHOME})
	${RSYNC} ${LOGOS} ${STABLE}
all-FAQ: configure-dir
	(cd ${OpenXM_doc} ; ${RSYNC} ${OXWEAVE_FILES_in_doc} ${CURRENT})
clean:
	-${RM} .[a-z]*done
	-make -f Makefile-intro clean
	-(cd ${OpenXM_spec} ; make clean)

clean-top:
	-${RM} ${OXHOME}/index.html 
clean-everything: clean clean-top
	-${RM} -rf ${CURRENT} ${STABLE} ${ASCM2001} ${CURRENT_DOC}
	-${RM} -rf tmp-poster


###
### target for buggy latex2html
###  (jlatex2html machine)% make
###  (latex2html machine)%  make clean-en-html
###  (latex2html machine)%  make 
clean-en-html:
	-(cd ../OpenXM-specs ; make clean-en-done)
	-(make -f Makefile-intro clean)

install-exec: ${OpenXM_include}/ox/cmotag.h ${OpenXM_include}/ox/oxMessageTag.h ${OpenXM_include}/ox/smCommand.h
${OpenXM_include}/ox/cmotag.h: cmotag.h 
	cp cmotag.h ${OpenXM_include}/ox
${OpenXM_include}/ox/oxMessageTag.h: oxMessageTag.h
	cp oxMessageTag.h ${OpenXM_include}/ox
${OpenXM_include}/ox/smCommand.h: smCommand.h
	cp smCommand.h ${OpenXM_include}/ox
