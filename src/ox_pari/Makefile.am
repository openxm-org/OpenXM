# $OpenXM$

AUTOMAKE_OPTIONS = foreign no-dependencies 1.5
DISTCLEANFILES     = .configure_done
LIBS = -L${libdir} @LIBS@ -lm
DEFS = @DEFS@
AM_CPPFLAGS = -g -Wall -finline-functions -I${prefix}/include

noinst_HEADERS = ox_pari.h mysig.h

ox_paridir = ${prefix}/bin
ox_pari_PROGRAMS = ox_pari
ox_pari_SOURCES = ox_pari.c convert.c pari_ftab.c

