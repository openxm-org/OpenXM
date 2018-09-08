# $OpenXM$  
# This is a Makefile to test the module asir.py on mathlibre2018.
# su ; make install
usage:
	"make diff, make install (as root)"

SAGE_ASIR_DIR=${OpenXM_HOME}/src/sage

diff:
	-(cd /usr/lib/python2.7/dist-packages/sage/interfaces ; diff -c all.py.orig all.py) >all.py.diff

install:
	(cd /usr/lib/python2.7/dist-packages/sage/interfaces ; ln -s ${SAGE_ASIR_DIR}/asir.py . ; patch <${SAGE_ASIR_DIR}/all.py.diff)

