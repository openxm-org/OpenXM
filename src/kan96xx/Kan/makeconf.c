/* $OpenXM$ */
#include <stdio.h>
main() {
#if defined(__CYGWIN__)
  printf("s/@@@LIBGMP@@@/$(OpenXM_HOME)\\/lib\\/libgmp.a/\n");
#else
  printf("s/@@@LIBGMP@@@/$(OpenXM_HOME)\\/lib\\/libgmp.so -Wl,--rpath -Wl,$(OpenXM_HOME)\\/lib/\n");
#endif
}

