# -*- mode: Makefile -*-
## $OpenXM: OpenXM/src/ox_toolkit/Makefile.am,v 1.3 2004/07/02 08:19:07 noro Exp $

AUTOMAKE_OPTIONS = foreign no-dependencies 1.5
OXWEAVE   = ${prefix}/bin/oxweave
INCLUDES  = @X_CFLAGS@ -I${prefix}/include
OXLIBS    = -L${prefix}/lib ./libox.la -lgc -lgmp
XLIBS     = ${X_LIBS} ${X_PRE_LIBS} -lX11 ${X_EXTRA_LIBS}
DOCUMENTS = readme-en.txt readme-ja.txt bnf-en.txt bnf-ja.txt

CLEANFILES         = ${DOCUMENTS}
DISTCLEANFILES     = .configure_done

docdir    = ${prefix}/doc/ox_toolkit
doc_DATA  = ${DOCUMENTS}

include_HEADERS    = ox_toolkit.h
noinst_HEADERS     = mysocket.h parse.h
noinst_PROGRAMS    = bconv testclient sample1 sample2 zclient ox_Xsample

bconv_SOURCES      = bconv.c ox_toolkit.h
sample1_SOURCES    = sample1.c ox_toolkit.h
sample2_SOURCES    = sample2.c ox_toolkit.h
zclient_SOURCES    = zclient.c ox_toolkit.h
testclient_SOURCES = testclient.c ox_toolkit.h
ox_Xsample_SOURCES = ox_Xsample.c glib.c ox_toolkit.h

bconv_LDADD        = ${OXLIBS}
sample1_LDADD      = ${OXLIBS}
sample2_LDADD      = ${OXLIBS}
zclient_LDADD      = ${OXLIBS}
testclient_LDADD   = ${OXLIBS}
ox_Xsample_LDADD   = ${OXLIBS} ${XLIBS}

lib_LTLIBRARIES    = libox.la
libox_la_LDFLAGS   = --version-info 0:0:0
libox_la_SOURCES   = ox.c cmo.c parse.c mysocket.c mathcap.c dump.c \
                     cmolen.c print.c oxf.c oxf_old.c \
                     ox_toolkit.h mysocket.h parse.h

readme-ja.txt: README
	${OXWEAVE} ja common < README > readme-ja.txt

readme-en.txt: README
	${OXWEAVE} en common < README > readme-en.txt

bnf-ja.txt: bnf.oxweave
	${OXWEAVE} ja common < bnf.oxweave > bnf-ja.txt

bnf-en.txt: bnf.oxweave
	${OXWEAVE} en common < bnf.oxweave > bnf-en.txt
