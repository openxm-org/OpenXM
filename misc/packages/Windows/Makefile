# $OpenXM$
OpenXM_TOP=../../../../OpenXM
STARTEXE=c:/windows/command/start.exe
WINASIR=c:/home/asir
usage:
	@echo "Targets: binary-tgz clean"

clean:
	-rm -rf OpenXM-win .copy-binary-done

binary-tgz: copy-binary win-correction

copy-binary:
	-mkdir OpenXM-win
	if [ ! -f .copy-binary-done ] ; then \
	  (cd OpenXM-win ; \
	   (cd ../${OpenXM_TOP}; tar czf - bin lib man src/asir-contrib) | tar xzvf -  ; \
	  ) ; \
	   touch .copy-binary-done ; \
	fi 

win-correction: copy-binary
	(cd OpenXM-win/lib/sm1/bin ; rm -f ox_sm1_forAsir ; cp ../../../bin/ox_sm1.exe ox_sm1_forAsir.exe)
	-mkdir OpenXM-win/bin-windows
	cp ${STARTEXE} OpenXM-win/bin-windows
	cp OpenXM-win/src/asir-contrib/win/asirgui.bat OpenXM-win/bin-windows
	cp /bin/cygwin1.dll OpenXM-win/bin-windows
	cp /bin/cygwin1.dll OpenXM-win/bin
	cp /bin/cygwin1.dll OpenXM-win/lib/sm1/bin
	-mkdir OpenXM-win/asir
	(cd ${WINASIR} ; tar czf - bin lib) | (cd OpenXM-win/asir ; tar xzvf -)
