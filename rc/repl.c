/* $OpenXM: OpenXM/rc/repl.c,v 1.2 2000/01/19 06:10:33 noro Exp $ */
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define BUFSIZE 10000
main() {
  char s[BUFSIZE];
  char cwd[BUFSIZE];
  char *slash;

  getcwd(cwd,BUFSIZE);
  slash = strrchr(cwd,'/');
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
  exit(0);
}

	  
  
