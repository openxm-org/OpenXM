/* $OpenXM: OpenXM/misc/packages/Windows/debug-tools/crlf.c,v 1.1 2002/01/16 09:11:59 takayama Exp $ */
#include <stdio.h>
main(int argc,char *argv[]) {
  FILE *fp;
  FILE *fp2;
  char s[1024];
  char fname[1024];
  int cr = 0;
  int c;
  if (argc < 2) {
    fprintf(stderr,"crlf filename\n");
    exit(1);
  }
  fp = fopen(argv[1],"r");
  if (fp == NULL) {
    fprintf(stderr,"Could not open the file %s.\n",argv[1]);
  }
  while ( (c=fgetc(fp)) != EOF) {
    if ( c == 0xd) {
      cr = 1; break;
    }
  }
  if (cr == 1) exit(0);  /* do nothing. Windows file. */
  fclose(fp);

  sprintf(fname,"%s.BAK",argv[1]);
  sprintf(s,"cp %s %s",argv[1],fname);
  system(s);

  fp = fopen(fname,"r");
  fp2 = fopen(argv[1],"w");
  while ((c=fgetc(fp)) != EOF) {
    if (c == 0xa) {
      fputc(0xd,fp2); fputc(0xa,fp2);
    }else{
      fputc(c,fp2);
    }
  }
  fclose(fp); fclose(fp2);  
}


