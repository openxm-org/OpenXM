## $OpenXM: OpenXM/src/ox_pari/Makefile.am,v 1.3 2016/09/23 02:54:48 ohara Exp $

AUTOMAKE_OPTIONS = foreign no-dependencies 1.5
DISTCLEANFILES     = .configure_done
LN_S = @LN_S@
LIBS = @LIBS@ -L${libdir} @OXLIBS@ -lm
DEFS = @DEFS@
AM_CPPFLAGS = -g -Wall -finline-functions -I${prefix}/include

noinst_HEADERS = ox_pari.h

ox_paridir = ${prefix}/bin
ox_pari_PROGRAMS = ox_pari
ox_pari_SOURCES = ox_pari.c convert.c pari_ftab.c

install-exec-hook: 
	(cd ${libdir} && ${LN_S} -f ${bindir}/ox_pari${EXEEXT} ox_pari${EXEEXT} )
