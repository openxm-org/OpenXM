/* $OpenXM: OpenXM/src/kan96xx/Kan/makeconf.c,v 1.2 2002/10/24 05:53:37 takayama Exp $ */
#include <stdio.h>
main() {
#if defined(__CYGWIN__)
  printf("s/@@@LIBGMP@@@/$(OpenXM_HOME)\\/lib\\/libgmp.a/\n");
#elif defined(sun)
  printf("s/@@@LIBGMP@@@/$(OpenXM_HOME)\\/lib\\/libgmp.a -L $(OpenXM_HOME)\\/lib/\n");
#else
  printf("s/@@@LIBGMP@@@/$(OpenXM_HOME)\\/lib\\/libgmp.so -Wl,--rpath -Wl,$(OpenXM_HOME)\\/lib/\n");
#endif
}

