/* $OpenXM: OpenXM/misc/packages/FreeBSD/openxm-ports/openxm-1.1.2/files/gen-plist.c,v 1.6 2000/03/10 15:09:39 takayama Exp $ */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
/* References are
    /usr/src/bin/ls/print.c   readlink
    /usr/port/japanese/dvipsk-vflib/pkg/PLIST

*/


main() {
  char s[512];
  char s0[512];
  char lnk[1024];
  int i;
  int n;
  int flag = 0;
  struct stat sb;
  int p;

  /* Initial data */
  system("grep \'$OpenXM\' ../pkg/PLIST");
  system("grep \'$OpenXM\' ../../pkg/PLIST");
  printf("bin/asir\n");
  printf("bin/sm1\n");
  printf("bin/oxMathematica\n");
  printf("bin/oxmath\n");
  printf("bin/ox\n");
  printf("bin/oxgp\n");
  printf("bin/openxm\n");
  printf("man/man1/sm1.1.gz\n");
  printf("man/man1/openxm.1.gz\n");
  
  while (fgets(s0,512,stdin) != NULL) {
	n = strlen(s0);
	if (n <= 1) goto LLL;
	if (s0[n-1] == '\n') s0[n-1] = '\0';
	if (s0[0] == '.' && s0[1] == '/' && s0[2] == '.') goto LLL;
	if (s0[0] == '.' && s0[1] == '/' ) {
	  strcpy(s,&(s0[2]));
	}else{
	  strcpy(s,s0);
	}
	n = strlen(s);
	flag = 0;
	if (n == 0) goto LLL;
	if (isExclude(s,"/CVS/")) goto LLL;
	if (isExclude(s,".done")) goto LLL;
	if (isExclude(s,".uuencoded")) goto LLL;
	if (isExclude(s,"DO_NOT_EDIT")) goto LLL;
	if (isExclude(s,"~")) goto LLL;
	if (isExclude(s,".#")) goto LLL;
	if (isExclude(s,"OpenXM/doc/histrical-doc")) goto LLL;
	if (isExclude(s,".log")) goto LLL;
	if (isExclude(s,"fep.linux")) goto LLL;
	if (isExclude(s,"/Old/")) goto LLL;
	if (isExclude(s,"/debug/.sm1")) goto LLL;
	if (isExclude(s,".aux")) goto LLL;
	if (isExclude(s,".toc")) goto LLL;
	if (isExclude(s,".keepme")) goto LLL;
	if (isExclude(s,"/sm1/ole")) goto LLL;
	if (isExclude(s,"OpenXM/lib/sm1/ttt")) goto LLL;
	if (isExclude(s,".sm1.org")) goto LLL;
	if (isExclude(s,"OpenXM/lib/sm1/onlinehelp")) goto LLL;
	if (isExclude(s,"OpenXM/lib/sm1/printOnlineHelp")) goto LLL;
	if (isExclude(s,"OpenXM/lib/sm1/Makefile")) goto LLL;
	if (isExclude(s,"issac2000")) goto LLL;
	if (isExclude(s,"doc/Papers")) goto LLL;
	if (isExclude(s,"doc/OpenXM-web")) goto LLL;
	if (isExclude(s,"doc/Makefile")) goto LLL;
	if (isExclude(s,"doc/install")) goto LLL;
	if (isExclude(s,"doc/compalg")) goto LLL;

	
	if (lstat(s,&sb) < 0) fprintf(stderr,"The file %s is not found.\n",s);
	if (S_ISLNK(sb.st_mode)) {
	  /* symbolic link */
	  p = readlink(s,lnk,1024);
	  if (p > 0) lnk[p] = '\0';
	  else {
		fprintf(stderr,"readlink could not get the symbolic link of %s\n",s);
	  }
	  printf("@exec ln -fs %s %%D/%s\n",lnk,s);
	}else{
	  printf("%s\n",s);
	}
	
  LLL: ;

  }
  printf("@dirrm OpenXM\n");
}

isExclude(char *s,char *pattern) {
  int n;
  int m;
  int i,j;
  n = strlen(pattern);
  m = strlen(s);
  /* printf("%s %s\n",s,pattern); */
  for (i=0; i< m -n +1; i++) {
	for (j=0; j<n; j++) {
	  /* printf("%c %c, ",pattern[j], s[i+j]); */
	  if (pattern[j] != s[i+j]) goto PPP;
	}
	return(1);
  PPP: ;
  }
  return(0);
}
