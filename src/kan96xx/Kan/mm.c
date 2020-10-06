/* $OpenXM: OpenXM/src/kan96xx/Kan/mm.c,v 1.3 2004/07/02 06:41:53 noro Exp $ */
#include <stdio.h>

int main() {
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

