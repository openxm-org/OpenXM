# $OpenXM: OpenXM/misc/packages/Windows/Makefile,v 1.45 2016/08/27 02:20:55 takayama Exp $
OpenXM_TOP=../../../../OpenXM
ASIR_LANG=ja
#ASIR_LANG=en
VERSION=1.2.3
WGET_ARG=http://www.math.kobe-u.ac.jp/pub/asir/asirwin-${ASIR_LANG}.tgz
WGET_PHC=http://www.math.kobe-u.ac.jp/pub/OpenXM/misc/1.2.2/phc.exe
WGET_HEAD=http://www.math.kobe-u.ac.jp/pub/OpenXM/Head/openxm-head.tar.gz

WINASIR=c:/home/asir

#obtained by ldd ox_pari.exe
SYS_DLL_FILES=/bin/cygwin1.dll /usr/bin/cyggcc_s-seh-1.dll /usr/bin/cyggmp-10.dll  /usr/bin/cygmpfr-4.dll

#obtained by chgcheck command
DLLS_ox_asir=/bin/cygX11*dll /bin/cygxcb*.dll /bin/cygXau*.dll \
     /bin/cygXdmcp*.dll /bin/cygWaw*dll \
     /bin/cygXext*.dll /bin/cygXmu*.dll /bin/cygXt*.dll \
     /bin/cygIcE*.dll /bin/cygSM*.dll \
    /bin/cygICE*.dll /bin/cygSM*.dll /bin/cyg/bin/cyguuid*.dll \
    /bin/cygintl*.dll /bin/cygiconv*.dll \
    /bin/cyggcc_s-seh*.dll /bin/cygXpm*.dll

usage:
	@echo "make clean ; make oxpari-gen-zip"
	@echo "Targets: all build-tgz clean clean-work link-for-debug copy-to-ftp-area"
	@echo "         remove-experimental gen-cd"
	@echo "  The target all, gen-cd builds the directory OpenXM-win" 
	@echo "Other targets: win-correction3, build-bat"  
	@echo "               asir-book, copy-asir-book-to-ftp-area "

clean: clean-OpenXM-win clean-asir-book
clean-OpenXM-win:
	-rm -rf OpenXM-win .copy-binary-done .copy-binary-asir-done .copy-binary-phc-done OpenXM-win.tgz .OpenXM-win.tgz.done oxasir-win.rr .copy-cygwin-dlls-done .build-binary-openxm-cygwin-done OpenXM-unix OpenXM-doc .OpenXM-win.tgz.copy.done .oxpari-copy-cygwin-dlls-done

clean-work:
	-rm -rf work
clean0:
	-rm -rf .copy-binary-done oxasir-win.rr .copy-cygwin-dlls-done .oxpari-copy-cygwin-dlls-done

all: copy-binary-phc copy-binary copy-binary-asir win-correction win-correction-clean

build-binary-openxm-cygwin:
	if [ ! -f .build-binary-openxm-cygwin-done ] ; then \
		(cd ${OpenXM_TOP}/src ; make install-asir-contrib ) ; \
		(cd ${OpenXM_TOP}/src ; make install-oxpari ) ; \
		touch .build-binary-openxm-cygwin-done ; \
	fi

oxpari-copy-binary-cygwin: build-binary-openxm-cygwin
	-mkdir OpenXM-win
	-mkdir -p OpenXM-win/bin
	if [ ! -f .oxpari-copy-binary-done ] ; then \
	  (cd OpenXM-win ; \
	   cp  ../${OpenXM_TOP}/bin/ox_pari bin ; \
	   cp  ../${OpenXM_TOP}/bin/ox bin ; \
	   cp  ../${OpenXM_TOP}/bin/cyggc*.dll bin ; \
	   cp  ../${OpenXM_TOP}/bin/cygcord*.dll bin ; \
	   cp  ../${OpenXM_TOP}/bin/libpari*.dll bin ; \
	   cp  ../${OpenXM_TOP}/bin/sm1 bin ; \
	   cp  ../${OpenXM_TOP}/bin/ox_sm1 bin ; \
	   cp  ../${OpenXM_TOP}/bin/ox_sm1 bin/ox_sm1_forAsir ; \
	  ) ; \
	   touch .oxpari-copy-binary-oxpari-done ; \
	fi 
oxpari-copy-cygwin-dlls:
	if [ ! -f .oxpari-copy-cygwin-dlls-done ] ; then \
	 cp  ${SYS_DLL_FILES} OpenXM-win/bin ; \
	 touch .oxpari-copy-cygwin-dlls-done ; \
	fi

oxpari-gen-zip: oxpari-copy-binary-cygwin oxpari-copy-cygwin-dlls 
	-mkdir -p OpenXM-win/lib/
	cp ${OpenXM_TOP}/src/asir-contrib/packages/src/oxpari-win.rr OpenXM-win/lib
	zip -r openxm-win-oxpari.zip OpenXM-win
	@echo "Copy openxm-win-oxpari.zip by hand to OpenXM/Current"

copy-binary: copy-binary-openxm-cygwin

copy-binary-openxm-cygwin: build-binary-openxm-cygwin
	-mkdir OpenXM-win
	-mkdir -p OpenXM-win/src
	-mkdir -p OpenXM-win/bin-windows
	if [ ! -f .copy-binary-done ] ; then \
	  (cd OpenXM-win ; \
	   rsync -avz --delete --exclude CVS ../${OpenXM_TOP}/bin . ; \
	   rsync -avz --delete --exclude CVS ../${OpenXM_TOP}/lib . ; \
	   rsync -avz --delete --exclude CVS ../${OpenXM_TOP}/rc . ; \
	   rsync -avz --delete --exclude CVS ../${OpenXM_TOP}/man . ; \
	   rsync -avz --delete --exclude CVS ../${OpenXM_TOP}/src/asir-contrib src ; \
	   find . -name 'CVS' -print | xargs rm -rf ; \
	   find . -name 'DO_NOT_EDIT*' -print | xargs rm -rf ; \
	  ) ; \
	   cp Readme-CD/readme-ja.html OpenXM-win ; \
	   cp Readme-CD/readme-en.html OpenXM-win ; \
	   touch .copy-binary-done ; \
	fi 

copy-binary-asir:
	@echo "Do nothing for copy-binary-asir in the new version."

copy-binary-asir-orig:
	if [ ! -f .copy-binary-asir-done ] ; then \
		make copy-binary-asir-from-internet ; \
	fi

copy-binary-asir-from-internet:
	-mkdir OpenXM-win
	if [ ! -d work ] ; then mkdir work ; fi
	if [ ! -f work/asirwin-${ASIR_LANG}.tgz ] ; then \
		(cd work ; wget ${WGET_ARG} ) ; \
	fi
	(cd OpenXM-win ; tar xzvf ../work/asirwin-${ASIR_LANG}.tgz)
	touch .copy-binary-asir-done

copy-binary-asir-from-local-disk:
	-mkdir OpenXM-win
	-mkdir OpenXM-win/asir
	if [ ! -f .copy-binary-asir-done ] ; then \
	 (cd OpenXM-win ; \
	  (cd ${WINASIR} ; tar czf - bin lib) | (cd asir ; tar xzvf -) ; \
	 ) ; \
	  touch .copy-binary-asir-done ; \
	fi

copy-binary-phc:
	@echo "Do nothing for copy-binary-phc in the new version."

copy-binary-phc-orig:
	-if [ ! -f .copy-binary-phc-done ] ; then \
		make copy-binary-phc-from-internet ; \
	fi

copy-binary-phc-from-internet:
	if [ ! -f ${OpenXM_TOP}/bin/phc.exe ] ; then \
	 (cd ${OpenXM_TOP}/bin ; wget ${WGET_PHC} ) ; \
	fi
	touch .copy-binary-phc-done

copy-cygwin-dlls:
	if [ ! -f .copy-cygwin-dlls-done ] ; then \
	 cp /bin/cygwin1.dll OpenXM-win/bin-windows ; \
	 cp /bin/cygwin1.dll OpenXM-win/bin ; \
	 cp /bin/cygwin1.dll OpenXM-win/lib/sm1/bin ; \
	 touch .copy-cygwin-dlls-done ; \
	fi

#It has not been used.
copy-cygwin-dlls-for-ox_asir :
	for i in ${DLLS_ox_asir} ; do \
	 cp $$i OpenXM-win/asir/bin ; \
	 cp $$i OpenXM-win/bin ; \
        done ; \

win-correction: copy-binary oxasir-win.rr
	make win-correction2 
win-correction1: build-bat copy-cygwin-dlls
	-mkdir OpenXM-win/bin-windows
#	(cd OpenXM-win/rc ; make clean ; make)
win-correction2: win-correction1
	cp oxasir-win.rr OpenXM-win/lib/sm1
	cp oxasir-win.rr $(OpenXM_TOP)/lib/sm1
win-correction2_old:
	cp OpenXM-win/lib/sm1/*.sm1 OpenXM-win/asir/bin
	cp OpenXM-win/lib/sm1/*.sm1 OpenXM-win/bin
	cp OpenXM-win/lib/sm1/*.sm1 OpenXM-win/asir/bin
	cp OpenXM-win/lib/sm1/*.sm1 OpenXM-win/bin
	cp OpenXM-win/lib/sm1/oxasir.asir OpenXM-win/asir/bin
	cp OpenXM-win/lib/sm1/oxasir.asir OpenXM-win/bin
	-cp OpenXM-win/bin/ox_asir OpenXM-win/bin/ox_asir.exe
	cp oxasir-win.rr OpenXM-win/bin
	cp oxasir-win.rr OpenXM-win/asir/bin
	cp oxasir-win.rr OpenXM-win/lib/asir
	cp oxasir-win.rr OpenXM-win/lib/sm1


win-correction-clean:
	-find OpenXM-win -name '*~' -print | xargs rm -f
	-rm -f OpenXM-win/lib/sm1/*.jpeg OpenXM-win/lib/sm1/*.jpg
build-tgz: win-correction-clean
	if [ ! -f .OpenXM-win.tgz.done ]; then \
	tar czvf OpenXM-win.tgz OpenXM-win ; touch .OpenXM-win.tgz.done ; \
	fi

copy-to-orange: build-tgz
	scp OpenXM-win.tgz taka@orange:tmp3
copy-to-ftp-area: build-tgz
	if [ ! -f .OpenXM-win.tgz.copy.done ]; then \
	  scp OpenXM-win.tgz taka@orange2:/home/www/pub/OpenXM/Head/OpenXM-win-${ASIR_LANG}.tgz ; \
	fi
	touch .OpenXM-win.tgz.copy.done
copy-to-ftp-area-stable: build-tgz
	  scp OpenXM-win.tgz taka@orange:/export/home01/ftp/pub/OpenXM/${VERSION}/OpenXM-win-${ASIR_LANG}.tgz 

link-for-debug:
	(cd ${OpenXM_TOP} ; cd bin ; ln -s ../../OpenXM_contrib2/asir2000/asir .)
	(cd ${OpenXM_TOP} ; cd src/kxx ; make install-for-debug)

oxapp : oxapp.c

oxasir-win.rr : oxapp oxpp oxpp2
	rm -f oxasir-win.tmp oxasir-win.tmp2 oxasir-win.tmp3 oxpp2.tmp1 oxpp2.tmp2
	cat ${OpenXM_TOP}/src/kan96xx/Doc/oxasir.asir >>oxasir-win.tmp
	cat ${OpenXM_TOP}/rc/asirrc >>oxasir-win.tmp
	./oxpp2 oxasir-win.tmp >oxasir-win.rr
#	rm -f oxasir-win.tmp oxasir-win.tmp2 oxasir-win.tmp3 oxpp2.tmp1 oxpp2.tmp2

debug-tools/crlf :  debug-tools/crlf.c
	(cd debug-tools; gcc -o crlf crlf.c)
debug-tools/debug-sed : debug-tools/debug-sed.c
	(cd debug-tools; gcc -o debug-sed debug-sed.c)
debug-tools/gcwd : debug-tools/gcwd.c
	(cd debug-tools; gcc -o gcwd gcwd.c)
build-bat: debug-tools/crlf debug-tools/debug-sed debug-tools/gcwd
	-mkdir -p OpenXM-win/bin-windows
	-mkdir -p OpenXM-win/asir
	-mkdir -p OpenXM-win/asir/bin
	-mkdir -p OpenXM-win/asir/lib
	(cd debug-tools; sed -f dist.sed <asirgui-cd.bat >../OpenXM-win/bin-windows/asirgui-cd.bat ; ./crlf ../OpenXM-win/bin-windows/asirgui-cd.bat)
	(cd debug-tools; sed -f dist.sed <sm1-cd.bat >../OpenXM-win/bin-windows/sm1-cd.bat ; ./crlf ../OpenXM-win/bin-windows/sm1-cd.bat)
	(cd debug-tools; sed -f dist.sed <webasir.bat >../OpenXM-win/bin-windows/webasir.bat ; ./crlf ../OpenXM-win/bin-windows/webasir.bat)
	(cd debug-tools; sed -f dist.sed <websm1.bat >../OpenXM-win/bin-windows/websm1.bat ; ./crlf ../OpenXM-win/bin-windows/websm1.bat)
	(rm -f ../OpenXM-win/bin-windows/*.BAK ../OpenXM-win/bin-windows/*.bak)
	cp OpenXM-win/bin-windows/asirgui-cd.bat OpenXM-win/asir/bin
	(cd debug-tools; cp gcwd.exe ../OpenXM-win/bin-windows) 

remove-experimental:
	-rm -f OpenXM-win/bin-windows/*web*.bat


gen-cd:
	-(make ASIR_LANG=${ASIR_LANG} all; make remove-experimental)
	-mkdir OpenXM-unix
	cp Readme-CD/readme-unix-en.html OpenXM-unix
	(cd OpenXM-unix; wget ${WGET_HEAD} )
	@echo "Copy OpenXM-unix and OpenXM-win to the CD."

asir-book: mkdir-asir-book copy-to-asir-book
clean-asir-book:
	-rm -rf asir-book .copy-to-asir-book-asir-bin .copy-to-asir-book-prog .copy-to-asir-book-meadow asir-book-en.tgz asir-book-ja.tgz

mkdir-asir-book: 
	-(mkdir asir-book)
	-(mkdir asir-book/asir)
	-(mkdir asir-book/Meadow)
	-(mkdir asir-book/Prog)

copy-to-asir-book:  copy-to-asir-book-asir-bin copy-to-asir-book-prog copy-to-asir-book-meadow asir-book/readme-ja.html

copy-to-asir-book-asir-bin:  copy-binary-asir
	-if [ ! -f .copy-to-asir-book-asir-bin ]; then  \
	  (cd asir-book ; tar xzvf ../work/asirwin-${ASIR_LANG}.tgz) ;\
	fi
	touch .copy-to-asir-book-asir-bin

copy-to-asir-book-prog:
	if [ ! -f .copy-to-asir-book-prog ]; then \
	  (cd work; cvs-asir-book checkout asir-book) ; \
	  (cp work/asir-book/Prog/*.rr asir-book/Prog) ; \
	fi
	touch .copy-to-asir-book-prog

MEADOW=Meadow-1.14-i386-base.tar.gz
WGET_MEADOW=--passive-ftp ftp://ftp:${USER}.openxm.org@ftp.math.kobe-u.ac.jp/pub/OpenXM/misc/1.2.2/${MEADOW}
WINTAR=tar.exe
WGET_WINTAR=--passive-ftp ftp://ftp:${USER}.openxm.org@ftp.math.kobe-u.ac.jp/pub/OpenXM/misc/1.2.2/${WINTAR}
WINGZIP=gzip.exe
WGET_WINGZIP=--passive-ftp ftp://ftp:${USER}.openxm.org@ftp.math.kobe-u.ac.jp/pub/OpenXM/misc/1.2.2/${WINGZIP}
copy-to-asir-book-meadow:
	if [ ! -f .copy-to-asir-book-meadow ]; then \
	 if [ ! -f work/${MEADOW} ]; then \
		(cd work ; wget ${WGET_MEADOW} ) ; \
		(cd work ; wget ${WGET_WINTAR} ) ; \
		(cd work ; wget ${WGET_WINGZIP} ) ; \
	 fi ; \
	 (cd asir-book ; cp ../work/${MEADOW} Meadow) ; \
	 (cd asir-book ; cp ../work/${WINTAR} Meadow) ; \
	 (cd asir-book ; cp ../work/${WINGZIP} Meadow) ; \
	fi
	touch .copy-to-asir-book-meadow

asir-book/readme-ja.html: Readme-CD/asir-book-index.html
	cp Readme-CD/asir-book-index.html asir-book/readme-ja.html

copy-asir-book-to-ftp-area: asir-book-${ASIR_LANG}.tgz
	scp asir-book-${ASIR_LANG}.tgz taka@orange:/export/home01/ftp/pub/OpenXM/asir-book
asir-book-${ASIR_LANG}.tgz: asir-book 
	tar czvf asir-book-${ASIR_LANG}.tgz --exclude Meadow asir-book


