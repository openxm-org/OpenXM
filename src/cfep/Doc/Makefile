# temporary for debug. official one is at ../packages
OpenXM_HOME=../../../../OpenXM
usage:
	@echo "copy (copy to OpenXM/src/asir-contrib and OpenXM/doc)"
	@echo "copy-web (copy to HOME/taka/2005/cfep)"

copy:
	rsync -Cavz *.html *.png cfep-asir-install ${OpenXM_HOME}/doc/cfep/
	cp ${OpenXM_HOME}/src/asir-contrib/packages/src/cfep-init.rr ${OpenXM_HOME}/lib/asir-contrib/
	cp ${OpenXM_HOME}/src/asir-contrib/packages/src/glib3.rr ${OpenXM_HOME}/lib/asir-contrib/

copy-web:
	rsync -Cavz -e ssh *.html *.png cfep-asir-install taka@lemon2.math.kobe-u.ac.jp:/home/web/HOME/taka/2005/cfep/