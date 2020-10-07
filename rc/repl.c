/* $OpenXM: OpenXM/rc/repl.c,v 1.18 2019/03/29 02:49:48 takayama Exp $ */

#include <stdio.h>
#include <stdlib.h>

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define BUFSIZE 10000
#define SSIZE  1024   

/* If you make the following two strings longer, increase the number SSIZE */
#define	REPL_IMGFILE	"repl_test.img"
#define	REPL_PSFILE	    "repl_test.ps"

int
main(int argc,char *argv[]) {
  char s[BUFSIZE];
  char cwd[BUFSIZE];
  char *slash;
  char type = 'b';
  FILE *fp;
  int fd,i;
  char sss_png[SSIZE];
  char sss_gif[SSIZE];

  if (getcwd(cwd, sizeof(cwd)) == NULL) {
	fprintf(stderr, "getcwd: %s\n", strerror(errno));
	exit(EXIT_FAILURE);
  }
  if ((slash = strrchr(cwd, '/')) == cwd) {
	fprintf(stderr, "The current working directory is /.\n");
	exit(EXIT_FAILURE);
  }
  *slash = 0;

  for (i=1; i<argc; i++) {
    if (strcmp(argv[i],"csh")==0) {
      type = 'c';
    }else if (strcmp(argv[i],"bash")==0) {
      type = 'b';
    }else if (strcmp(argv[i],"--prefix")==0) {
      i++;
      strcpy(cwd,argv[i]);
	  strcat(cwd,"/OpenXM");
      if (cwd[0] != '/') {
        fprintf(stderr,"Warning: prefix must start with /\n");
        fprintf(stderr,"Your prefix is %s\n",cwd);
      }
    }else{
      fprintf(stderr,"Warning: Unknown option.\n");
    }
  }

  while (fgets(s,BUFSIZE,stdin) != NULL) {
	if (strcmp(s,"OpenXM_HOME=$HOME/OpenXM\n") == 0) {
	  printf("OpenXM_HOME=%s\n",cwd);
	}else if (strcmp(s,"setenv OpenXM_HOME $HOME/OpenXM\n") == 0) {
	  printf("setenv OpenXM_HOME %s\n",cwd);
	}else if (strcmp(s,"OpenXM_HOME=$(HOME)/OpenXM\n") == 0) {
	  printf("OpenXM_HOME=%s\n",cwd);
	}else if (strcmp(s,"setenv OpenXM_HOME \"$(HOME)\"/OpenXM\n") == 0) {
	  printf("setenv OpenXM_HOME %s\n",cwd);
	}else if (strcmp(s,"OpenXM_HOME=\"${HOME}\"/OpenXM\n") == 0) {
	  printf("OpenXM_HOME=%s\n",cwd);
	}else{
	  printf("%s",s);
	}
  }

  /* Configuring environmental variables. */
  /* Check if pstoimg (src/asir-contrib) supports png format. */
  if ((fp = fopen(REPL_PSFILE,"w")) == NULL) {
	fprintf(stderr, "fopen: %s", strerror(errno));
	exit(EXIT_FAILURE);
  }
  fprintf(fp,"/Times-Roman findfont 10 scalefont setfont\n");
  fprintf(fp," 390 290 moveto  (F) show \n");
  fprintf(fp,"showpage \n");
  while (fclose(fp) != 0) {
	if (errno == EINTR)
		continue;
	break;
  }

  sprintf(sss_png,"pstoimg -type png %s -out %s >/dev/null",REPL_PSFILE,REPL_IMGFILE);
  sprintf(sss_gif,"pstoimg -type gif %s -out %s >/dev/null",REPL_PSFILE,REPL_IMGFILE);
  
  if (!system(sss_png)) {
	if (type == 'b') {
	  printf("export OpenXM_PSTOIMG_TYPE=png\n");
	}else{
	  printf("setenv OpenXM_PSTOIMG_TYPE png\n");
	}
  }else if (!system(sss_gif)) {
	if (type == 'b') {
	  printf("OpenXM_PSTOIMG_TYPE=gif\n");
      printf("export OpenXM_PSTOIMG_TYPE\n");
	}else{
	  printf("setenv OpenXM_PSTOIMG_TYPE=gif\n");
	}
  }else {
    if (type == 'b') {
      printf("OpenXM_PSTOIMG_TYPE=no\n");
      printf("export OpenXM_PSTOIMG_TYPE\n");
    }else{
      printf("setenv OpenXM_PSTOIMG_TYPE no\n");
    }
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
