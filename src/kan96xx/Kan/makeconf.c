/* $OpenXM: OpenXM/src/kan96xx/Kan/makeconf.c,v 1.1 2001/05/20 07:48:19 takayama Exp $ */
#include <stdio.h>
main() {
#if defined(__CYGWIN__)
  printf("s/@@@LIBGMP@@@/$(OpenXM_HOME)\\/lib\\/libgmp.a/\n");
#elif defined(sun)
  printf("s/@@@LIBGMP@@@/$(OpenXM_HOME)\\/lib\\/libgmp.so -L $(OpenXM_HOME)\\/lib/\n");
#else
  printf("s/@@@LIBGMP@@@/$(OpenXM_HOME)\\/lib\\/libgmp.so -Wl,--rpath -Wl,$(OpenXM_HOME)\\/lib/\n");
#endif
}

