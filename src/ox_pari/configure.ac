dnl $OpenXM: OpenXM/src/ox_pari/configure.ac,v 1.3 2016/09/26 13:05:17 ohara Exp $

AC_INIT(ox_pari,1.15)
AC_CONFIG_SRCDIR([ox_pari.c])
AC_PREREQ(2.69)
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE
AM_MAINTAINER_MODE

AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S

AC_ARG_ENABLE([shared],
[  --enable-shared         enable linking with shared libraries.  [[yes]]],
[],[enable_shared=yes])

if test "${enable_shared:=yes}" != no ; then
   true
fi

OXLIBS='-lox -lmpfr -lgmp -lgc -lpari'
if test "${enable_shared}" = no ; then
    OXLIBS='${libdir}/libox.a ${libdir}/libmpfr.a ${libdir}/libgmp.a ${libdir}/libgc.a ${libdir}/libpari.a'
fi
AC_SUBST([OXLIBS])

dnl AC_DEFINE([_POSIX_C_SOURCE])

AC_CHECK_FUNCS(sigaction)
dnl dnl for Solaris 2.x
dnl AC_CHECK_FUNC(socket, , [AC_CHECK_LIB(socket,socket)])
dnl AC_CHECK_FUNC(gethostbyname, , [AC_CHECK_LIB(nsl,gethostbyname)])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
