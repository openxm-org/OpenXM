/* $OpenXM: OpenXM/src/kan96xx/Kan/mm.c,v 1.2 2000/01/16 07:55:39 takayama Exp $ */
#include <stdio.h>

main() {
  int c;
  printf("\" ");
  while ((c=getchar()) != EOF) {
    if (c == '%') {
      while((c=getchar()) != EOF && (c != '\n')) ;
    }else if (c != '\n' && c != '\r') {
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

