# $OpenXM: OpenXM/src/sage/Makefile,v 1.1 2018/09/08 05:35:35 takayama Exp $
usage:
	@echo "make install (as root)"
	@echo "     copy-to-ox, copy-from-ox"

install:
	cp asir.py /usr/lib/python2.7/dist-packages/sage/interfaces
	(cd /usr/lib/python2.7/dist-packages/sage/interfaces ; patch ) <all.py.diff

# for misc-2018/A3/sage-asir
copy-to-ox:
	rsync -avzu asir.py ${OpenXM_HOME}/src/sage/asir.py
	rsync -avzu Makefile ${OpenXM_HOME}/src/sage/Makefile
	@echo "commit $OpenXM_HOME and make copy-from-ox, git-misc commit"
copy-from-ox:
	rsync -avzu ${OpenXM_HOME}/src/sage/asir.py asir.py
	rsync -avzu ${OpenXM_HOME}/src/sage/Makefile Makefile

