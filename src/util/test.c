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
}



