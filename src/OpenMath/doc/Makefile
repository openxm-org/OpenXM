# $OpenXM: OpenXM/src/OpenMath/doc/Makefile,v 1.2 2001/11/04 09:58:14 takayama Exp $
SRC = man.texi omproxy.texi

all : OpenXM_javadoc  OpenXM_javadoc_ja
OpenXM_javadoc:
	-rm -rf tmp-doc-openxm-tam
	-(mkdir tmp-doc-openxm-tam ) 
	-(cd ../ORG/openxm/tam ; javadoc -d ../../../doc/tmp-doc-openxm-tam *.java)


OpenXM_javadoc_ja:
	-rm -rf tmp-doc-openxm-tam-ja
	-(mkdir tmp-doc-openxm-tam-ja)
	-(cd tmp-doc-openxm-tam-ja; cp -r ../../ORG . ; rm -rf ORG/CVS)
	-(cd tmp-doc-openxm-tam-ja/ORG/openxm/tam ; ln ../../../../to-ja . ; ls *.java | xargs ./to-ja)
	-(cd tmp-doc-openxm-tam-ja/ORG/openxm/tam ; javadoc -d ../../.. OpenXM.java)

JP:
	-rm -rf tmp
	mkdir tmp
	cp jtexinfo.tex texinfo-js.tex tmp/
	for i in ${SRC}; do \
		echo $$i ;\
		awk '/^\\JP/	{sub(/^\\JP[ \t]*/,"");print;next;}	\
		    /^\\BJP/	{getline;while($$0 !~ /^\\E/){	\
					print;getline;	\
					};next;}	\
		    /^\\B../	{getline;while($$0 !~ /^\\E/){	\
					getline;	\
					};next;}	\
		    /^\\../	{next;}	\
		    {print;}' $$i > tmp/$$i;\
	done
	(cd tmp; ptex man.texi; cp man.dvi ../man-ja.dvi)

EG:
	-rm -rf tmp
	mkdir tmp
	cp texinfo.tex tmp/
	for i in ${SRC}; do \
		echo $$i ;\
		awk '/^\\EG/	{sub(/^\\EG[ \t]*/,"");print;next;}	\
		    /^\\BEG/	{getline;while($$0 !~ /^\\E/){	\
					print;getline;	\
					};next;}	\
		    /^\\B../	{getline;while($$0 !~ /^\\E/){	\
					getline;	\
					};next;}	\
		    /^\\../	{next;}	\
		    {print;}' $$i > tmp/$$i;\
	done
	(cd tmp; tex man.texi; cp man.dvi ../man-en.dvi)


clean:
	-(rm -rf tmp-doc-openxm-tam tmp-doc-openxm-tam-ja)