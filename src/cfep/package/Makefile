# Moved from misc-2005/12/xcode/Makefile.
OpenXM_HOME=${HOME}/OX4/OpenXM
BUILD=${OpenXM_HOME}/src/cfep/build
usage:
	@echo ${BUILD}
	@echo "setup-cfep (developer), clean, "
	@echo "dist-cfep (build a distribution under work), install, gentar," 
	@echo "copy-intro-ja : Copy intro-ja.html and images to /home/web/HOME/taka/2005/cfep/index-ja.html" 
	@echo "gentar-html(orange2) --- tmporal"

setup-cfep:
	@echo "Generate symbolic links for cfep"
	rm -f ${BUILD}/Debug/cfep.app/OpenXM
	rm -rf ${BUILD}/Release/cfep.app/OpenXM
	mkdir -p ${BUILD}/Debug/cfep.app
	(cd ${BUILD}/Debug/cfep.app ; ln -s ~/OX4/OpenXM .)
	mkdir -p ${BUILD}/Release/cfep.app
	(cd ${BUILD}/Release/cfep.app ; ln -s ~/OX4/OpenXM .)
	rm -f ${BUILD}/Default/cfep.app/OpenXM
	mkdir -p ${BUILD}/Default/cfep.app
	(cd ${BUILD}/Default/cfep.app ; ln -s ~/OX4/OpenXM .)
	make setup-cfep-fix
# To debug html intro at ../doc
	(cd ../doc ; ln -s ${OpenXM_HOME}/doc/cfep/html-ja_JP.utf8 .)
	-cp `which nkf` ~/OX4/OpenXM/bin
setup-cfep-fix:
	(rm -f ~/OX4/OpenXM/lib/asir/cfep-init.rr ; cd ~/OX4/OpenXM/lib/asir ; ln -s ~/OX4/OpenXM/src/asir-contrib/packages/src/cfep-init.rr .)

install:  cfep-grep
	mkdir -p ${OpenXM_HOME}/doc/cfep
	mkdir -p ${OpenXM_HOME}/lib/cfep
	cp ../Doc/intro-ja.html ${OpenXM_HOME}/doc/cfep
	cp ../Doc/intro-en.html ${OpenXM_HOME}/doc/cfep
	cp ../Doc/*.png ${OpenXM_HOME}/doc/cfep
	cp cfep.init ${OpenXM_HOME}/lib/cfep
	cp cfep-grep ${OpenXM_HOME}/bin
	cp ../tests/cfep-opengl.rr ${OpenXM_HOME}/lib/asir-contrib
	-cp `which nkf` ~/OX4/OpenXM/bin

clean:
	rm -f tmp-* gendoc cfep-grep ../doc/html-ja_JP.utf8

dist-cfep: gentar install
	rm -rf work
	mkdir work
	(cd work ; (cd ${OpenXM_HOME}/.. ; \
		  tar cf - OpenXM/lib/libgc* OpenXM/lib/libgmp* OpenXM/rc \
		    OpenXM/bin/ox OpenXM/bin/ox_asir OpenXM/bin/asir \
			OpenXM/lib/cfep OpenXM/lib/sm1/bin \
			OpenXM/doc/cfep )  | tar xvf -)
	(cd work ; (cd ${OpenXM_HOME}/.. ; \
		  tar cf - --exclude help --exclude help-jp --exclude help-eg  OpenXM/lib/asir  )  | tar xvf -)
	(cd work/OpenXM/rc ; make clean)
	rm -rf work/OpenXM/lib/asir/help* work/OpenXM/lib/asir/cfep-init.rr
	mkdir work/OpenXM/lib/asir-contrib
	(cd work/OpenXM/lib/asir-contrib ; (cd ${OpenXM_HOME}/src/asir-contrib/packages/src ; tar cf - * )  | tar xvf -)
	rm -f work/OpenXM/lib/asir-contrib/noro_rewrite.rr
	(cp ${OpenXM_HOME}/src/asir-contrib/testing/noro/new_rewrite.rr work/OpenXM/lib/asir-contrib/noro_rewrite.rr)
	(cp ../tests/cfep-opengl.rr work/OpenXM/lib/asir-contrib)
	rm -f work/OpenXM/bin/ox work/OpenXM/bin/asir
	(cp ${OpenXM_HOME}/bin/ox work/OpenXM/bin)
	(cp ${OpenXM_HOME}/bin/asir work/OpenXM/bin)
	(cp ${OpenXM_HOME}/bin/ox_sm1 work/OpenXM/bin)
	(cp ${OpenXM_HOME}/bin/ox_texmacs work/OpenXM/bin)
	(cp ${OpenXM_HOME}/bin/ox100start work/OpenXM/bin)
	(cp ${OpenXM_HOME}/bin/ox work/OpenXM/bin)
	(cp ${OpenXM_HOME}/bin/oxlog work/OpenXM/bin)
#	(cp ${OpenXM_HOME}/bin/oxgreph work/OpenXM/bin)
	(cp ${OpenXM_HOME}/bin/nkf work/OpenXM/bin)
	(cp ${OpenXM_HOME}/bin/sm1 work/OpenXM/bin)
	(cp ${OpenXM_HOME}/bin/ox_k0 work/OpenXM/bin)
	(mkdir -p work/OpenXM/lib/sm1 ; cp -r ${OpenXM_HOME}/lib/sm1/*.sm1 work/OpenXM/lib/sm1)
	(cp ${OpenXM_HOME}/lib/sm1/*.asir work/OpenXM/lib/sm1)
	(cp -r ${OpenXM_HOME}/lib/sm1/Resource work/OpenXM/lib/sm1)
# It is not a static linked program. So, you will need a copy of libraries, too.
#	(cd work/OpenXM ; tar xzf ../../tmp-OpenXM-html.tar.gz) 
	(cd ${BUILD}/Release ; tar czvf - cfep.app) | (cd work; tar xzf -)
	rm -f work/cfep.app/OpenXM
	(cd work; mv OpenXM cfep.app)
#
	-(cd work; ln -s cfep.app/OpenXM .)
	-mkdir -p work/Samples
#	Copy files from Samples ; cp ../Sample/?? work/Samples
	cp ../Samples/*.rr work/Samples
#	(cd work; tar czvf cfep.tar.gz cfep.app)
#	@echo "work/cfep.tar.gz is for the distribution."
	@echo "Generate diskimage by hand. Name should be cfep-200xmmdd.dmg.gz"
	@echo "Put it under /home/web/HOME/taka/2005/cfep"
	@echo "Doc/intro-ja.html is used cfep intro on web, too"

gentar:
#	@if [ ! -f tmp-OpenXM-html.tar.gz ]; then \
#		scp taka@lemon.math.kobe-u.ac.jp:this05/misc-2005/A1/cfep/package/tmp-OpenXM-html.tar.gz . ; \
#	fi 


# Do it at orange2
gentar-html:
	@if [ ! -f tmp-OpenXM-html.tar.gz ]; then \
		(cd /home/web/OpenXM/Current ; tar czvf - doc/asir2000/html-jp) >tmp-OpenXM-html.tar.gz ; \
	fi 

copy-intro-ja:
	scp ../Doc/intro-ja.html taka@lemon.math.kobe-u.ac.jp:/home/web/HOME/taka/2005/cfep/index-ja.html
# Copy screenshots, too.
	scp ../Doc/*.png taka@lemon.math.kobe-u.ac.jp:/home/web/HOME/taka/2005/cfep


gendoc: gendoc.c
	gcc -o gendoc gendoc.c

cfep-grep: cfep-grep.c
	gcc -o cfep-grep -I${OpenXM_HOME}/include cfep-grep.c -L${OpenXM_HOME}/lib -lox_pathfinder

