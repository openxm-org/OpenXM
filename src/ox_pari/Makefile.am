## $OpenXM: OpenXM/src/ox_pari/Makefile.am,v 1.6 2017/07/10 02:43:49 noro Exp $

AUTOMAKE_OPTIONS = foreign no-dependencies 1.5
DISTCLEANFILES     = .configure_done
LN_S = @LN_S@
LIBS = @LIBS@ -L${libdir} @OXLIBS@ -lm
DEFS = @DEFS@
AM_CPPFLAGS = -g -Wall -finline-functions -I${prefix}/include

noinst_HEADERS = ox_pari.h mysig.h

ox_paridir = ${prefix}/bin
ox_pari_PROGRAMS = ox_pari
ox_pari_SOURCES = ox_pari.c convert.c pari_ftab.c mysig.c

install-exec-hook: 
	-@${MKDIR_P} ${libdir}/asir
	-@rm -f ${libdir}/asir/ox_pari
	(cd ${libdir}/asir && ${LN_S} ../../bin/ox_pari${EXEEXT} ox_pari${EXEEXT} )
