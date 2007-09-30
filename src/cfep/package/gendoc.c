/*
   Generates html files for help under
     ${OpenXM_HOME}/doc/cfep/html-ja_JP.utf8
   cf. ~/.cfep/html-ja_JP.utf8

   mdimport ${OpenXM_HOME}/doc/cfep/html-ja_JP.utf8
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <locale.h>
  

char *extractFileName(char *s);
char *extractFolderName(char *s);
int translate(char *dir,char *outputdirTop);
int Sjis = 0;

main(int argc, char *argv[]) {
  char com[1024];
  char workdir[1024];
  char oxhome[1024];
  int i;
  if (argc < 2) {
	fprintf(stderr,"gendoc foldernameOfHtmlHelp ... \n");
	if (Sjis) fprintf(stderr,"  output to ${OpenXM_HOME}/doc/cfep/html-ja_JP.sjis\n");
	else fprintf(stderr,"  output to ${OpenXM_HOME}/doc/cfep/html-ja_JP.utf8\n");
	fprintf(stderr,"nkf is used.\n");
	fprintf(stderr,"Example: gendoc ~/OX4/OpenXM/src/asir-doc/html-jp\n");
	exit(0);
  }
  setlocale(LC_ALL,"C");
  /* if ja_JP.eucJP is set, 01 is added to each bytes of argv[i] on mac. cf. misc/argvtest* */
  /* A strange things will happen when japanese file names are used. */
  if (!getenv("OpenXM_HOME")) {
	fprintf(stderr,"OpenXM_HOME is not set.\n"); exit(1);
  }
  strcpy(oxhome,getenv("OpenXM_HOME"));
  if (Sjis) sprintf(workdir,"%s/doc/cfep/html-ja_JP.sjis",oxhome);
  else sprintf(workdir,"%s/doc/cfep/html-ja_JP.utf8",oxhome);

  sprintf(com,"mkdir -p %s",workdir);
  system(com);

  for (i=1; i<argc; i++) {
	translate(argv[i],workdir);
  }
  exit(0);
}

int translate(char *dir,char *outputdirTop) {
  char com[1024];
  FILE *fp;
  FILE *fp2;
  int first,i;
  first = 1;
  char *folder;
  char oname[1024*3];
  char *fname;
  char longFname[1024];
  sprintf(com,"ls %s/*.html >/tmp/tmp-gendoc.txt",dir);
  system(com);

  fp = fopen("/tmp/tmp-gendoc.txt","r");
  while (fgets(longFname,1000,fp)) {
	for (i=0; i<strlen(longFname); i++) {
	  if (longFname[i] == '\n') longFname[i] = 0;
	}
	if (first) {
	  folder = extractFolderName(longFname);
	  sprintf(com,"mkdir -p %s/%s",outputdirTop,folder);
	  fprintf(stderr,"%s\n",com);
	  system(com);
	  first = 0;
	}
	fname = extractFileName(longFname);
    sprintf(oname,"%s/%s/%s",outputdirTop,folder,fname);
	fp2 = fopen(oname,"w");
	if (Sjis) fprintf(fp2,"<HTML>\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=SJIS\">\n");
	else fprintf(fp2,"<HTML>\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">\n");
	fclose(fp2);
	if (Sjis) sprintf(com,"nkf -s %s | sed -e 's/<HTML>//g' >>%s",longFname,oname);
    else sprintf(com,"nkf -w %s | sed -e 's/<HTML>//g' >>%s",longFname,oname);
    printf("%s\n",com);
	system(com);
  }
  system("rm -f /tmp/tmp-gendoc.txt");
  return 0;
}

char *extractFileName(char *s) {
  int i,n,start;
  char *ans;
  start = 0;
  n = strlen(s);
  ans = (char *)malloc(n+10);
  strcpy(ans,"tmp.html");
  for (i=n-1; i>=0; i--) {
	if (s[i] == '/') {
	  start = i+1;
	  break;
	}
  }
  if (start < n) strcpy(ans,&(s[start]));
  for (i=0; i<strlen(ans); i++) {
	if (ans[i] < ' ') ans[i]=0; 
  }
  return ans;
}

char *extractFolderName(char *s) {
  int i,n,start,end;
  char *ans;
  start = end = -1;
  n = strlen(s);
  ans = (char *)malloc(n+5);
  strcpy(ans,"tmp");
  for (i=n-1; i>=0; i--) {
	if (s[i] == '/') {
	  i--; end = i;
	  for ( ; i>=0; i--) {
		if (s[i] == '/') {
		  start = i+1;
		  break;
		}
	  }
	  break;
	}
  }
  if ((start>=0) && (end >=0)) strcpy(ans,&(s[start]));
  ans[end-start+1] = 0;
  for (i=0; i<strlen(ans); i++) {
	if (ans[i] < ' ') ans[i]=0; 
  }
  return ans;
}

