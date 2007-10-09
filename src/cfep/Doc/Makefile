# temporary for debug. official one is at ../packages
usage:
	@echo "copy (copy to OpenXM/src/asir-contrib and OpenXM/doc)"
	@echo "copy-web (copy to HOME/taka/2005/cfep)"

copy:
	cp intro-ja.html ~/OX4/OpenXM/doc/cfep
	cp ~/OX4/OpenXM/src/asir-contrib/packages/src/cfep-init.rr ~/OX4/OpenXM/lib/asir-contrib
	cp ~/OX4/OpenXM/src/asir-contrib/packages/src/glib3.rr ~/OX4/OpenXM/lib/asir-contrib

copy-web:
	rsync -Cavz -e ssh *.html *.png cfep-asir-install taka@lemon2.math.kobe-u.ac.jp:/home/web/HOME/taka/2005/cfep/