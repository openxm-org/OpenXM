/* $OpenXM: OpenXM/rc/repl.c,v 1.11 2003/01/17 00:41:05 maekawa Exp $ */

#include <stdio.h>
#include <stdlib.h>

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define BUFSIZE 10000

#define	REPL_IMGFILE	"/tmp/repl_test.img"
#define	REPL_PSFILE	"/tmp/repl_test.ps"

int
main(int argc,char *argv[]) {
  char s[BUFSIZE];
  char cwd[BUFSIZE];
  char *slash;
  char type = 'b';
  FILE *fp;
  int fd;

  if (argc >= 2) {
	if (strcmp(argv[1],"csh")==0) {
	  type = 'c';
	}
  }

  if (getcwd(cwd, sizeof(cwd)) == NULL) {
	fprintf(stderr, "getcwd: %s\n", strerror(errno));
	exit(EXIT_FAILURE);
  }
  if ((slash = strrchr(cwd, '/')) == cwd) {
	fprintf(stderr, "The current working directory is /.\n");
	exit(EXIT_FAILURE);
  }
  *slash = 0;
  while (fgets(s,BUFSIZE,stdin) != NULL) {
	if (strcmp(s,"OpenXM_HOME=$HOME/OpenXM\n") == 0) {
	  printf("OpenXM_HOME=%s\n",cwd);
	}else if (strcmp(s,"setenv OpenXM_HOME $HOME/OpenXM\n") == 0) {
	  printf("setenv OpenXM_HOME %s\n",cwd);
	}else{
	  printf("%s",s);
	}
  }

  /* Configuring environmental variables. */
  /* Check if pstoimg (src/asir-contrib) supports png format. */
  if ((fd = open("/tmp/repl_test.ps", O_WRONLY|O_CREAT|O_EXCL|O_TRUNC,
		 S_IRUSR|S_IWUSR)) < 0) {
	fprintf(stderr, "open: %s: %s\n", REPL_PSFILE, strerror(errno));
	exit(EXIT_FAILURE);
  }
  if ((fp = fdopen(fd, "w")) == NULL) {
	fprintf(stderr, "fdopen: %s", strerror(errno));
	exit(EXIT_FAILURE);
  }
  fprintf(fp,"/Times-Roman findfont 10 scalefont setfont\n");
  fprintf(fp," 390 290 moveto  (F) show \n");
  fprintf(fp,"showpage \n");
  fclose(fp);

  if (!system("pstoimg -type png /tmp/repl_test.ps -out /tmp/repl_test.img >/dev/null")) {
	if (type == 'b') {
	  printf("export OpenXM_PSTOIMG_TYPE=png\n");
	}else{
	  printf("setenv OpenXM_PSTOIMG_TYPE png\n");
	}
  }else if (!system("pstoimg -type gif /tmp/repl_test.ps -out /tmp/repl_test.img >/dev/null")) {
	if (type == 'b') {
	  printf("OpenXM_PSTOIMG_TYPE=gif\n");
      printf("export OpenXM_PSTOIMG_TYPE\n");
	}else{
	  printf("setenv OpenXM_PSTOIMG_TYPE=gif\n");
	}
  }else {
	printf("OpenXM_PSTOIMG_TYPE=no\n");
	printf("export OpenXM_PSTOIMG_TYPE\n");
  }

  while (unlink(REPL_IMGFILE) != 0) {
	if (errno == EINTR)
		continue;
	break;
  }
  while (unlink(REPL_PSFILE) != 0) {
	if (errno == EINTR)
		continue;
	break;
  }

  exit(EXIT_SUCCESS);
}
