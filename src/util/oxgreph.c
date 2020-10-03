/* $OpenXM: OpenXM/src/util/oxgreph.c,v 1.4 2009/02/22 17:30:03 ohara Exp $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define mymalloc(n)  malloc(n)

/* 
   It greps files and outputs an html file to browse the files.
   Keyword should be given in euc or utf-8 depending on the coding system of the files. 
   It does not add meta section  of html file.
   Locale must be "C" to use this program or use -f option to read keyword.

   Example:
   export LC_ALL="C"
   s.txt contains "insubunkai"
   oxgreph  "`nkf -w s.txt`" $OpenXM_HOME/doc/cfep/html-ja_JP.utf8/html-ja/*.html >t.html
   oxgreph --sjis "`nkf -s s.txt`" $OpenXM_HOME/doc/cfep/html-ja_JP.sjis/html-ja/*.html >t.html
*/

int Sjis = 0;
int LeaveTag = 0;
int main(int argc, char *argv[]) {
  int i,m,pid,n, cpos,start;
  char *com;
  char workf[256];
#define SSIZE 1024
  char s[SSIZE];
  char ss[SSIZE*2];
  FILE *fp;
  int r;
  m = 256;
  pid = getpid();
  if (argc < 2) {
	fprintf(stderr,"oxgreph [--leaveTag] args_to_grep \n");
	fprintf(stderr,"Note: LC_ALL should be C on some systems or use -f to give the pattern.\n");
	fprintf(stderr,"Example 1:  oxgreph \"`nkf -w s.txt`\" $OpenXM_HOME/doc/cfep/html-ja_JP.utf8/*.html >t.html\n");
	fprintf(stderr,"Example 2:  oxgreph -f s.txt $OpenXM_HOME/doc/cfep/html-ja_JP.utf8/*.html >t.html\n");
	fprintf(stderr,"  The keyword is stored in s.txt.\n");
	exit(0);
  }
  for (i=1; i<argc; i++) {
    m += strlen(argv[i])+1;
  }
  com = (char *)mymalloc(m);
  sprintf(com,"grep ");
  for (i=1; i<argc; i++) {
    if (strcmp(argv[i],"--sjis")==0) {
      Sjis = 1;
    }else if (strcmp(argv[i],"--leaveTag")==0) {
      LeaveTag = 1;
    }else {
      sprintf(&(com[strlen(com)]),"%s ",argv[i]);
    }
  }
  sprintf(workf,"/tmp/tmp-oxgreph-%d.txt",pid);
  sprintf(&(com[strlen(com)]),">%s",workf);
  r=system(com);

  printf("<ol>\n");
  fp = fopen(workf,"r");
  if (fp == NULL) {
    fprintf(stderr,"Open error of the workfile.\n"); exit(1);
  }
  while (fgets(s,SSIZE-1,fp)) {
    cpos = -1; n = strlen(s);
    for (i=0; i< n; i++) {
      if (s[i] == ':') {
        cpos = i; break;
      }
    }
    if (cpos >= 0) {
      s[cpos] = 0; start = 0;
      for (i=cpos+1; i<n; i++) {
        if (Sjis && (((unsigned char)s[i]) >= 0x80)) {
          i++;
          continue; /* skip the next byte. */
        }
        if (s[i] == 0x1b) { /* skip escape sequence */ 
          i++;
          while (s[i] != 0x1b) i++;
        }
        if (LeaveTag) ;
        else {
          if (s[i] == '<') { s[i]='{'; }
          else if (s[i] == '>') { s[i]='}'; }
        }
        if (s[i] == '\n') s[i] = ' ';
      }
      printf("<li><a href=\"%s\">%s</a> <pre>%s </pre>\n",s,s,&(s[cpos+1]));
    }
  }
  printf("</ol>\n");
  sprintf(com,"rm -f %s",workf);
  r=system(com);
  return 0;
}
