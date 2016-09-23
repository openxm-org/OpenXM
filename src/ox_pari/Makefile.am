# $OpenXM: OpenXM/src/ox_pari/Makefile.am,v 1.2 2016/09/22 17:12:51 ohara Exp $

AUTOMAKE_OPTIONS = foreign no-dependencies 1.5
DISTCLEANFILES     = .configure_done
LIBS = @LIBS@ -L${libdir} @OXLIBS@ -lm
DEFS = @DEFS@
AM_CPPFLAGS = -g -Wall -finline-functions -I${prefix}/include

noinst_HEADERS = ox_pari.h

ox_paridir = ${prefix}/bin
ox_pari_PROGRAMS = ox_pari
ox_pari_SOURCES = ox_pari.c convert.c pari_ftab.c

