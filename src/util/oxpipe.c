/* $OpenXM$ */
#include <stdio.h>

main(int argc,char *argv[]) {
  dup2(0,3);
  dup2(1,4);
  dup2(2,0);
  dup2(2,1);

  if (argc < 2) {
	fprintf(stderr,"oxpipe servername\n");
  }

  execl(argv[1],argv[1],NULL);
}
