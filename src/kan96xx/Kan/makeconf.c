/* $OpenXM: OpenXM/src/kan96xx/Kan/makeconf.c,v 1.3 2002/10/24 06:04:07 takayama Exp $ */
#include <stdio.h>
main() {
#if defined(__CYGWIN__)
  printf("s/@@@LIBGMP@@@/ -L$(OpenXM_HOME)\\/lib  -lgmp /\n");
#elif defined(sun)
  printf("s/@@@LIBGMP@@@/ -L$(OpenXM_HOME)\\/lib -lgmp /\n");
#else
  printf("s/@@@LIBGMP@@@/ -L$(OpenXM_HOME)\\/lib -lgmp -Wl,--rpath -Wl,$(OpenXM_HOME)\\/lib/\n");
#endif
}

