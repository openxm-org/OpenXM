# $OpenXM: OpenXM/misc/packages/Windows/Makefile,v 1.8 2001/12/28 09:54:47 takayama Exp $
OpenXM_TOP=../../../../OpenXM
STARTEXE=c:/windows/command/start.exe
WINASIR=c:/home/asir
usage:
	@echo "Targets: binary-tgz build-tgz clean"

clean:
	-rm -rf OpenXM-win .copy-binary-done .copy-binary-asir-done OpenXM-win.tgz .OpenXM-win.tgz.done

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

win-correction: copy-binary 
	make win-correction2 
win-correction1:
	-mkdir OpenXM-win/bin-windows
	cp ${STARTEXE} OpenXM-win/bin-windows
	cp OpenXM-win/src/asir-contrib/win/asirgui*.bat OpenXM-win/bin-windows
	cp OpenXM-win/src/asir-contrib/win/asirgui*.bat OpenXM-win/asir/bin
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
win-correction-clean:
	-find OpenXM-win -name '*~' -print | xargs rm
	-rm OpenXM-win/lib/sm1/*.jpeg OpenXM-win/lib/sm1/*.jpg
build-tgz: win-correction-clean
	if [ ! -f .OpenXM-win.tgz.done ]; then \
	tar czvf OpenXM-win.tgz OpenXM-win ; touch .OpenXM-win.tgz.done ; \
	fi

copy-to-orange: build-tgz
	scp OpenXM-win.tgz taka@orange:tmp3

