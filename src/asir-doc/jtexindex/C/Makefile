# Generated automatically from Makefile.in by configure.
# Makefile for GNU Texindex.
# Copyright (C) 1990, 1991, 1992 Free Software Foundation, Inc.

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

#### Start of system configuration section. ####

srcdir = .

CC = gcc
INSTALL = /usr/bin/install -c
INSTALL_PROGRAM = ${INSTALL}
INSTALL_DATA = ${INSTALL} -m 644

LN	= ln
RM	= rm -f
TAR	= tar
MKDIR	= mkdir
RANLIB = ranlib
DEFS = -DEUC  -DSTDC_HEADERS=1 -DHAVE_UNISTD_H=1 -DHAVE_STRING_H=1 -DHAVE_SYS_TIME_H=1 -DHAVE_SYS_FCNTL_H=1 -DHAVE_SYS_FILE_H=1 -DHAVE_ALLOCA_H=1 -DHAVE_ALLOCA=1 -DHAVE_SETVBUF=1 -DHAVE_GETCWD=1 -DHAVE_BZERO=1 -DHAVE_BCOPY=1 -DHAVE_MEMSET=1 -DHAVE_MEMCPY=1 -DHAVE_MEMMOVE=1 -DHAVE_STRCHR=1 -DHAVE_STRCASECMP=1 -DHAVE_STRDUP=1 -DHAVE_VFPRINTF=1 -DHAVE_VSPRINTF=1 -DHAVE_STRERROR=1 -DHAVE_SIGPROCMASK=1 -DHAVE_SIGSETMASK=1 
LIBS = -L./ -ltxi 
LOADLIBES = $(LIBS)

# This is normally inherited from parent make, but if someone wants to
# build libtxi.a alone, this variable will still be properly defined.
ALLOCA = 

SHELL = /bin/sh

CFLAGS = -g -O
LDFLAGS = 

prefix = /usr/local
exec_prefix = ${prefix}
bindir = $(exec_prefix)/bin
# Prefix for each installed program, normally empty or `g'.

#### End of system configuration section. ####

SRCS =  getopt.c getopt.h getopt1.c bzero.c
OBJS =  getopt.o getopt1.o bzero.o $(ALLOCA)

PROGS = libtxi.a

all: texindex $(PROGS)
sub-all: all

.c.o:
	$(CC) -c $(CPPFLAGS) $(DEFS) -I. -I$(srcdir) $(CFLAGS) $<


install: all
	$(INSTALL_PROGRAM) texindex $(bindir)/texindex

uninstall:
	rm -f $(bindir)/texindex

TAGS:
	etags *.c *.h getopt*.c getopt.h

clean:
	rm -f *.o a.out core core.* texindex $(PROGS)

mostlyclean: clean

distclean: clean
	rm -f Makefile config.status

realclean: distclean
	rm -f TAGS

texindex: texindex.o libtxi.a
	$(CC) $(LDFLAGS) -o texindex texindex.o $(LOADLIBES)

libtxi.a: $(OBJS)
	ar qcv libtxi.a $(OBJS)
	$(RANLIB) libtxi.a

texindex.o: texindex.c getopt.h
getopt.o: getopt.c getopt.h
getopt1.o: getopt1.c getopt.h
alloca.o: alloca.c

# Prevent GNU make v3 from overflowing arg limit on SysV.
.NOEXPORT:
