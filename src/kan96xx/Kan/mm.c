/* $OpenXM$ */
#include <stdio.h>

main() {
  int c;
  printf("\" ");
  while ((c=getchar()) != EOF) {
    if (c == '%') {
      while((c=getchar()) != EOF && (c != '\n')) ;
    }else if (c != '\n') {
      putchar(c);
    }else{
      /*printf("\\\n");*/
      printf(" ");
    }
  }
  /*printf(" \";");*/
  printf(" \";\n");
  return(0);
}

