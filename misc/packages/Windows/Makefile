# $OpenXM: OpenXM/misc/packages/Windows/Makefile,v 1.19 2002/07/31 00:39:44 takayama Exp $
OpenXM_TOP=../../../../OpenXM
ASIR_LANG=ja
#ASIR_LANG=en
WGET_ARG=--passive-ftp ftp://ftp:${USER}.openxm.org@ftp.math.kobe-u.ac.jp/pub/asir/asirwin-${ASIR_LANG}.tgz

WINASIR=c:/home/asir

usage:
	@echo "Targets: all build-tgz clean clean-work link-for-debug"
	@echo "         remove-experimental "

clean:
	-rm -rf OpenXM-win .copy-binary-done .copy-binary-asir-done OpenXM-win.tgz .OpenXM-win.tgz.done oxasir-win.rr .copy-cygwin-dlls-done .build-binary-openxm-cygwin-done
clean-work:
	-rm -rf work
clean0:
	-rm -rf .copy-binary-done oxasir-win.rr .copy-cygwin-dlls-done

all: copy-binary copy-binary-asir win-correction win-correction-clean

build-binary-openxm-cygwin:
	if [ ! -f .build-binary-openxm-cygwin-done ] ; then \
		(cd ${OpenXM_TOP}/src ; make install-kan-ox ) ; \
		touch .build-binary-openxm-cygwin-done ; \
	fi

copy-binary: copy-binary-openxm-cygwin
copy-binary-openxm-cygwin: build-binary-openxm-cygwin
	-mkdir OpenXM-win
	-mkdir OpenXM-win/src
	if [ ! -f .copy-binary-done ] ; then \
	  (cd OpenXM-win ; \
	   rsync -avz --delete --exclude CVS ../${OpenXM_TOP}/bin . ; \
	   rsync -avz --delete --exclude CVS ../${OpenXM_TOP}/lib . ; \
	   rsync -avz --delete --exclude CVS ../${OpenXM_TOP}/rc . ; \
	   rsync -avz --delete --exclude CVS ../${OpenXM_TOP}/man . ; \
	   rsync -avz --delete --exclude CVS ../${OpenXM_TOP}/src/asir-contrib src ; \
	   find . -name 'CVS' -print | xargs rm -rf ; \
	  ) ; \
	   cp readme-ja.html OpenXM-win ; \
	   touch .copy-binary-done ; \
	fi 

copy-binary-asir:
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

copy-cygwin-dlls:
	if [ ! -f .copy-cygwin-dlls-done ] ; then \
	 cp /bin/cygwin1.dll OpenXM-win/bin-windows ; \
	 cp /bin/cygwin1.dll OpenXM-win/bin ; \
	 cp /bin/cygwin1.dll OpenXM-win/lib/sm1/bin ; \
	 cp /usr/X11R6/bin/*.dll OpenXM-win/asir/bin ; \
	 cp /usr/X11R6/bin/*.dll OpenXM-win/bin ; \
	 touch .copy-cygwin-dlls-done ; \
	fi

win-correction: copy-binary oxasir-win.rr
	make win-correction2 
win-correction1: build-bat copy-cygwin-dlls
	-mkdir OpenXM-win/bin-windows
#	(cd OpenXM-win/rc ; make clean ; make)
win-correction2: win-correction1
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
build-bat: debug-tools/crlf debug-tools/debug-sed
	-mkdir OpenXM-win/bin-windows
	(cd debug-tools; sed -f dist.sed <asirgui-cd.bat >../OpenXM-win/bin-windows/asirgui-cd.bat ; ./crlf ../OpenXM-win/bin-windows/asirgui-cd.bat)
	(cd debug-tools; sed -f dist.sed <webasir.bat >../OpenXM-win/bin-windows/webasir.bat ; ./crlf ../OpenXM-win/bin-windows/webasir.bat)
	(cd debug-tools; sed -f dist.sed <websm1.bat >../OpenXM-win/bin-windows/websm1.bat ; ./crlf ../OpenXM-win/bin-windows/websm1.bat)
	(rm -f ../OpenXM-win/bin-windows/*.BAK ../OpenXM-win/bin-windows/*.bak)
	cp OpenXM-win/bin-windows/asirgui-cd.bat OpenXM-win/asir/bin
	rm -f debug-tools/debug.sed ; debug-tools/debug-sed `pwd` >debug-tools/debug.sed
	(cd debug-tools; sed -f debug.sed <asirgui-cd.bat >asirgui-debug.bat ; ./crlf asirgui-debug.bat)

remove-experimental:
	-rm -f OpenXM-win/bin-windows/*web*.bat


