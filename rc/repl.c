/* $OpenXM: OpenXM/rc/repl.c,v 1.1 2000/01/18 09:38:58 takayama Exp $ */
#include <stdio.h>
#include <unistd.h>

#define BUFSIZE 10000
main() {
  char s[BUFSIZE];
  char cwd[BUFSIZE];
  getcwd(cwd,BUFSIZE);
  while (fgets(s,BUFSIZE,stdin) != NULL) {
	if (strcmp(s,"OpenXM_HOME=$HOME/OpenXM\n") == 0) {
	  printf("OpenXM_HOME=%s/../../OpenXM\n",cwd);
	}else if (strcmp(s,"setenv OpenXM_HOME $HOME/OpenXM\n") == 0) {
	  printf("setenv OpenXM_HOME %s/../../OpenXM\n",cwd);
	}else{
	  printf("%s",s);
	}
  }
  exit(0);
}

	  
  
