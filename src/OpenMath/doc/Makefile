
SRC = man.texi omproxy.texi

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
	(cd tmp; ptex man.texi; cp man.dvi ../man-jp.dvi)

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
	(cd tmp; tex man.texi; cp man.dvi ../man-eg.dvi)
