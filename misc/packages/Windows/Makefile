# $OpenXM: OpenXM/misc/packages/Windows/Makefile,v 1.2 2001/12/22 05:59:22 takayama Exp $
OpenXM_TOP=../../../../OpenXM
STARTEXE=c:/windows/command/start.exe
WINASIR=c:/home/asir
usage:
	@echo "Targets: binary-tgz clean"

clean:
	-rm -rf OpenXM-win .copy-binary-done .copy-binary-asir-done

binary-tgz: copy-binary copy-binary-asir win-correction

copy-binary:
	-mkdir OpenXM-win
	if [ ! -f .copy-binary-done ] ; then \
	  (cd OpenXM-win ; \
	   (cd ../${OpenXM_TOP}; tar czf - bin lib rc man src/asir-contrib) | tar xzvf -  ; \
	  ) ; \
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
	(cd OpenXM-win/lib/sm1/bin ; rm -f ox_sm1_forAsir ; cp ../../../bin/ox_sm1.exe ox_sm1_forAsir.exe)
	-mkdir OpenXM-win/bin-windows
	cp ${STARTEXE} OpenXM-win/bin-windows
	cp OpenXM-win/src/asir-contrib/win/asirgui.bat OpenXM-win/bin-windows
	cp OpenXM-win/src/asir-contrib/win/asirgui2.bat OpenXM-win/bin-windows
	cp /bin/cygwin1.dll OpenXM-win/bin-windows
	cp /bin/cygwin1.dll OpenXM-win/bin
	cp /bin/cygwin1.dll OpenXM-win/lib/sm1/bin
	(cd OpenXM-win/rc ; make clean ; make)
