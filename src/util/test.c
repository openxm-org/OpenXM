/* $OpenXM: OpenXM/src/util/test.c,v 1.4 2003/12/01 03:15:37 takayama Exp $ */
#include <stdio.h>
#include "ox_pathfinder.h"

sGC_malloc(int a) {
  return GC_malloc(a);
}

/* which command */
main(int argc,char *argv[]) {
  char *s;
  if (argc != 2) {
	fprintf(stderr,"test cmdname\n\n");
  }
  printf("%s\n",generateTMPfileName2("hoge","poly",0,0));
  printf("%s\n",generateTMPfileName2("hoge","poly",1,0));
  printf("%s\n",generateTMPfileName2("hoge","poly",0,1));
  printf("%s\n",generateTMPfileName2("hoge","poly",1,1));
  printf("---------------- getCommandPath ----------------\n");
  printf("%s\n",getCommandPath(argv[1]));
  
  printf("%s (cyg) ==> %s (win)\n",argv[1],cygwinPathToWinPath(argv[1]));
  printf("%s (win) ==> %s (cyg)\n",argv[1],winPathToCygwinPath(argv[1]));

  printf("------------------  oxEvalEnvVar ---------------\n");
  s = "/hoge/afo:${PATH}";
  printf("%s ==> %s\n",s,oxEvalEnvVar(s));
  s = "${PATH}:/hoge/afo";
  printf("%s ==> %s\n",s,oxEvalEnvVar(s));
  s = "/hoge/afo:${PATH";
  printf("%s ==> %s\n",s,oxEvalEnvVar(s));
  s = "/hoge/afo:$PATH}";
  printf("%s ==> %s\n",s,oxEvalEnvVar(s));
  s = "${HOME}:${PATH}:/hoge/afo";
  printf("%s ==> %s\n",s,oxEvalEnvVar(s));
}



