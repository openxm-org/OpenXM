/* $OpenXM: OpenXM/misc/packages/Windows/debug-tools/debug-sed.c,v 1.1 2002/07/28 08:10:24 takayama Exp $ */
#include <stdio.h>
main(int argc, char *argv[]) {
  char path[1024];
  int i,j;
  sprintf(path,"c:\\\\cygwin");
  if (argc > 1) {
    j = strlen(path);
    for (i=0; i<strlen(argv[1]); i++) {
      if (argv[1][i] == '/') {
	path[j] = '\\'; j++;
	path[j] = '\\'; j++; path[j] = 0;
      }else{
        path[j++] = argv[1][i]; path[j]=0;
      }
    }
  }
  printf("s/set HOME=c:\\\\cygwin\\\\home\\\\nobuki//g\n");
  printf("s/set OpenXM_HOME_WIN=c:\\\\cygwin\\\\home\\\\%USERNAME%\\\\OpenXM\\\\misc\\\\packages\\\\Windows\\\\OpenXM-win/set OpenXM_HOME_WIN=%s\\\\OpenXM-win/g\n",path);
  printf("s/\\\\src\\\\kan96xx\\\\Kan/\\\\bin/g\n");
  exit(0);
}


