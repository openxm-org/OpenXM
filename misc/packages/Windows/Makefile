# $OpenXM: OpenXM/misc/packages/Windows/Makefile,v 1.12 2002/01/13 07:10:15 takayama Exp $
OpenXM_TOP=../../../../OpenXM
STARTEXE=c:/windows/command/start.exe
WINASIR=c:/home/asir
usage:
	@echo "Targets: binary-tgz build-tgz clean"

clean:
	-rm -rf OpenXM-win .copy-binary-done .copy-binary-asir-done OpenXM-win.tgz .OpenXM-win.tgz.done oxasir-win.rr

binary-tgz: copy-binary copy-binary-asir win-correction

copy-binary:
	-mkdir OpenXM-win
	if [ ! -f .copy-binary-done ] ; then \
	  (cd OpenXM-win ; \
	   (cd ../${OpenXM_TOP}; tar czf - bin lib rc man src/asir-contrib) | tar xzvf -  ; \
	  ) ; \
	   cp readme-ja.html OpenXM-win ; \
	   touch .copy-binary-done ; \
	fi 

copy-binary-asir:
	-mkdir OpenXM-win
	-mkdir OpenXM-win/asir
	if [ ! -f .copy-binary-asir-done ] ; then \
	 (cd OpenXM-win ; \
	  (cd ${WINASIR} ; tar czf - bin lib) | (cd asir ; tar xzvf -) ; \
	 ) ; \
	  touch .copy-binary-asir-done ; \
	fi

win-correction: copy-binary oxasir-win.rr
	make win-correction2 
win-correction1: build-bat
	-mkdir OpenXM-win/bin-windows
	cp ${STARTEXE} OpenXM-win/bin-windows
	cp /bin/cygwin1.dll OpenXM-win/bin-windows
	cp /bin/cygwin1.dll OpenXM-win/bin
	cp /bin/cygwin1.dll OpenXM-win/lib/sm1/bin
#	(cd OpenXM-win/rc ; make clean ; make)
win-correction2: win-correction1
	cp ${STARTEXE} OpenXM-win/bin
	cp ${STARTEXE} OpenXM-win/asir/bin
	cp /usr/X11R6/bin/*.dll OpenXM-win/asir/bin
	cp /usr/X11R6/bin/*.dll OpenXM-win/bin
	cp OpenXM-win/lib/sm1/*.sm1 OpenXM-win/asir/bin
	cp OpenXM-win/lib/sm1/*.sm1 OpenXM-win/bin
	cp OpenXM-win/lib/sm1/*.sm1 OpenXM-win/asir/bin
	cp OpenXM-win/lib/sm1/*.sm1 OpenXM-win/bin
	cp OpenXM-win/lib/sm1/oxasir.asir OpenXM-win/asir/bin
	cp OpenXM-win/lib/sm1/oxasir.asir OpenXM-win/bin
	cp OpenXM-win/bin/ox_asir OpenXM-win/bin/ox_asir.exe
	cp oxasir-win.rr OpenXM-win/bin
	cp oxasir-win.rr OpenXM-win/asir/bin
	cp oxasir-win.rr OpenXM-win/lib/asir
	cp oxasir-win.rr OpenXM-win/lib/sm1
win-correction-clean:
	-find OpenXM-win -name '*~' -print | xargs rm
	-rm OpenXM-win/lib/sm1/*.jpeg OpenXM-win/lib/sm1/*.jpg
build-tgz: win-correction-clean
	if [ ! -f .OpenXM-win.tgz.done ]; then \
	tar czvf OpenXM-win.tgz OpenXM-win ; touch .OpenXM-win.tgz.done ; \
	fi

copy-to-orange: build-tgz
	scp OpenXM-win.tgz taka@orange:tmp3


oxapp : oxapp.c

oxasir-win.rr : oxapp oxpp oxpp2
	rm -f oxasir-win.tmp oxasir-win.tmp2 oxasir-win.tmp3 oxpp2.tmp1 oxpp2.tmp2
	cat ${OpenXM_TOP}/src/kan96xx/Doc/oxasir.asir >>oxasir-win.tmp
	cat ${OpenXM_TOP}/rc/asirrc >>oxasir-win.tmp
	./oxpp2 oxasir-win.tmp >oxasir-win.rr
#	rm -f oxasir-win.tmp oxasir-win.tmp2 oxasir-win.tmp3 oxpp2.tmp1 oxpp2.tmp2

crlf :  debug-tools/crlf.c

	gcc -o crlf debug-tools/crlf.c
build-bat: crlf
	-mkdir OpenXM-win/bin-windows
	(cd debug-tools; sed -f dist.sed <asirgui-cd.bat >../OpenXM-win/bin-windows/asirgui-cd.bat ; ./crlf ../OpenXM-win/bin-windows/asirgui-cd.bat)
	(cd debug-tools; sed -f dist.sed <webasir.bat >../OpenXM-win/bin-windows/webasir.bat ; ./crlf ../OpenXM-win/bin-windows/webasir.bat)
	(cd debug-tools; sed -f dist.sed <websm1.bat >../OpenXM-win/bin-windows/websm1.bat ; ./crlf ../OpenXM-win/bin-windows/websm1.bat)
	(rm -f ../OpenXM-win/bin-windows/*.BAK ../OpenXM-win/bin-windows/*.bak)
	cp OpenXM-win/bin-windows/asirgui-cd.bat OpenXM-win/asir/bin

