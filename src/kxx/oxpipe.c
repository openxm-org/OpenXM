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

  if (argc == 2) execl(argv[1],argv[1],NULL);
  if (argc == 3) execl(argv[1],argv[1],argv[2],NULL);
  if (argc == 4) execl(argv[1],argv[1],argv[2],argv[3],NULL);
  if (argc == 5) execl(argv[1],argv[1],argv[2],argv[3],argv[4],NULL);
  if (argc > 5) {fprintf(stderr,"Too many args\n"); exit(10);}
}
