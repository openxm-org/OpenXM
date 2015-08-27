# $OpenXM$
MYRSYNC=rsync -avzuC -e ssh
OXWEB=${USER}@lemon.math.kobe-u.ac.jp:/home/web/OpenXM/Current/doc/other-docs

OS_MULDIFF_DOC=3DGraph.pdf mtoupper.pdf os_muldif.pdf
OS_MULDIFF_PROG=os_muldif.rr

usage:
	@echo "fetch, install"

fetch: fetch-os_muldiff

install: install-os_muldiff


fetch-os_muldiff:
	mkdir -p tmp-os_muldiff
	(cd tmp-os_muldiff ; ../curl-os_muldiff.sh )

install-os_muldiff:
	-(cd tmp-os_muldiff ; ${MYRSYNC} ${OS_MULDIFF_DOC} ${OXWEB})
	(cd tmp-os_muldiff ; ${MYRSYNC} ${OS_MULDIFF_PROG} ../../packages/src)
	@echo "Do cvs-openxm commit in packages/src"
