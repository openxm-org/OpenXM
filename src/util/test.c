/* $OpenXM: OpenXM/src/util/test.c,v 1.2 2003/11/16 07:14:44 takayama Exp $ */
#include <stdio.h>
#include "ox_pathfinder.h"

sGC_malloc(int a) {
  return GC_malloc(a);
}

/* which command */
main(int argc,char *argv[]) {
  if (argc != 2) {
	fprintf(stderr,"test cmdname");
  }
  printf("%s\n",getCommandPath(argv[1]));
  
  printf("%s (cyg) ==> %s (win)\n",argv[1],cygwinPathToWinPath(argv[1]));
  printf("%s (win) ==> %s (cyg)\n",argv[1],winPathToCygwinPath(argv[1]));
}



