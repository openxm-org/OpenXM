#include <stdio.h>

main() {
  FILE *fp;
  int c;
  char fname[1024];
  int i;
  printf("\\def\\at{\\catcode`@=11{@}\\catcode`@=12 } \n");
  while ((c=getchar()) != EOF) {
    if (c != '@') {
       putchar(c);
    }else{
       i = 0;
       while (1) {
         c = getchar();
	 if (c <= ' ') {
	    fname[i]='\0';
	    break;
	 }
	 fname[i++] = c;
       }
       fp = fopen(fname,"r");
       if (fp == (FILE *)NULL) {
          fprintf(stderr,"I cannot find the file %s.\n",fname);
       }else{
          printf("\\begin{verbatim}\n");
          while ((c =fgetc(fp)) != EOF) putchar(c);
          printf("\n\\end{verbatim}\n");
       }
    }
  }
  exit(0);
}     
