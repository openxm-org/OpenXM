/* $OpenXM: OpenXM/src/kxx/oxpipe.c,v 1.1 2003/11/19 00:43:18 takayama Exp $ */
#include <stdio.h>
#include <unistd.h>

int
main(int argc,char *argv[]) {
  dup2(0,3);
  dup2(1,4);
  dup2(2,0);
  dup2(2,1);

  if (argc < 2) {
	fprintf(stderr,"oxpipe servername\n");
  }
  if (argc >=2 && argc <= 5) execv(argv[1], argv+1);
  if (argc > 5) {fprintf(stderr,"Too many args\n"); exit(10);}
}
