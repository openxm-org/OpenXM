dnl $OpenXM$

AC_INIT(ox_pari,1.15)
AC_CONFIG_SRCDIR([ox_pari.c])
AC_PREREQ(2.69)
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE
AM_MAINTAINER_MODE

AC_PROG_CC
AC_PROG_INSTALL
AC_PATH_XTRA

if test "${ac_test_CFLAGS}" != set -a "${GCC}" = yes ; then
    if test "${ac_cv_prog_cc_g}" = yes; then
        CFLAGS="-g -O"
    else
        CFLAGS="-O"
    fi
elif test `basename ./"${CC}"` = "icc" ; then
    CFLAGS="-g -O"
fi

AC_ARG_ENABLE([shared],
[  --enable-shared         enable linking with shared libraries.  [[yes]]],
[],[enable_shared=yes])

if test "${enable_shared:=yes}" != no ; then
   true
fi

LIBS='-lox -lmpfr -lgmp -lgc -lpari'
if test "${enable_shared}" = no ; then
    LIBS='${libdir}/libox.a ${libdir}/libmpfr.a ${libdir}/libgmp.a ${libdir}/libgc.a ${libdir}/libpari.a'
fi
AC_SUBST([LIBS])

AC_DEFINE([_POSIX_C_SOURCE])

AC_CHECK_FUNCS(sigaction)
dnl for Solaris 2.x
AC_CHECK_FUNC(socket, , [AC_CHECK_LIB(socket,socket)])
AC_CHECK_FUNC(gethostbyname, , [AC_CHECK_LIB(nsl,gethostbyname)])

AC_CHECK_SIZEOF(long)
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
