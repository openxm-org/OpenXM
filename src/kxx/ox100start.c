/* $OpenXM: OpenXM/src/kxx/ox100start.c,v 1.8 2016/03/31 05:27:34 takayama Exp $ */
/* Moved from misc-2003/07/cygwin/test.c */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <ctype.h>
#include <string.h>
#include "ox_pathfinder.h"
#include "mysig.h"

static void usage(void);
static int forkExec(char **argv);
void *sGC_malloc(int size) {
  return ((void *)malloc(size));
}

int Quiet = 0;
static int EngineLogToStdout = 0;
extern char **environ;

void main(int argc,char *argv[]) {
  char *s;
  char s2[1024];
  char buf[1024];
  FILE *fp;
  char **aaa;
  char **eee;
  int i;
  char *serverName;


  /*
	printf("ostype=%s\n",getOStypes());
	printf("OpenXM_HOME=%s\n",getOpenXM_HOME());
	printf("sm lib =%s\n",get_sm1_lib_path());
	printf("k0 lib =%s\n",get_k0_lib_path());
	printf("ox_asir =%s\n",get_ox_asir_path());
  */
  ox_pathfinderVerbose(1);


  /* Initialize parameters */
  serverName = NULL;
  aaa = NULL;
  
  for (i=1 ; i<argc; i++) {
	if (strcmp(argv[i],"-e")==0) {
	  if (aaa == NULL) {
		fprintf(stderr,"Server is not found or not specified by -oxserver options.\n");
		exit(1);
	  }
	  aaa = catArgv(aaa,&(argv[i+1]));
	  break;
	}else if (strcmp(argv[i],"-oxserver")==0) {
	  i++;
	  serverName = argv[i];
	  aaa = getServerEnv(serverName);
	}else if (strcmp(argv[i],"-nox")==0) {
	  ox_pathfinderNoX(1);
	}else if (strcmp(argv[i],"-engineLogToStdout")==0) {
	  ox_pathfinderEngineLogToStdout(1);
	  EngineLogToStdout = 1; 
	}else if (strcmp(argv[i],"-quiet")==0) {
	  Quiet = 1;  ox_pathfinder_quiet();
	}else{
	  fprintf(stderr,"Unknown option.\n");
	  usage();
	  exit(10);
	}
  }

  forkExec(aaa);
}

#define MYFORKCP_SIZE 100
static int Myforkchildren[MYFORKCP_SIZE];
static int Myforkcp=0;
static void myforkwait() {
  int status;
  int pid;
  int i,j;
  mysignal(SIGCHLD,SIG_IGN);
  pid = wait(&status);
  fprintf(stderr,"Child process %d is exiting.\n",pid);
  for (i=0; i<Myforkcp; i++) {
    if (Myforkchildren[i]  == pid) {
      for (j=i; j<Myforkcp-1; j++) {
        Myforkchildren[j] = Myforkchildren[j+1];
      }
      if (Myforkcp > 0) Myforkcp--;
    }
  }
  mysignal(SIGCHLD,myforkwait);
}

static void usage() {
  fprintf(stderr,"ox100start -oxserver xxx [-e args]\n");
  fprintf(stderr,"Examples: \n");
  fprintf(stderr,"    ox100start -oxserver bin/ox_sm1 -e -reverse -data 3010 --control 3012 -pass 1121343432434 \n");
  fprintf(stderr,"     ox100start -nox -engineLogToStdout -oxserver bin/ox_sm1 -e -data 3010 \n");

}

static int forkExec(char **argv) {
  int pid;
  char **eee;
  int m;
  m = 0;
  if (argv == NULL) {
    fprintf(stderr,"Cannot fork and exec.\n"); return -1;
  }
  if ((pid = fork()) > 0) {
    if (m&2) {
	  /* Do not call singal to turn around a trouble on cygwin. BUG. */
	}else{
	  mysignal(SIGCHLD,myforkwait); /* to kill Zombie */
	}
	Myforkchildren[Myforkcp++] = pid;
	if (Myforkcp >= MYFORKCP_SIZE-1) {
	  fprintf(stderr,"Child process table is full.\n");
	  Myforkcp = 0;
	}
  }else{
    /* close the specified files */
	if (m&1) {
       sigset_t sss;
       sigemptyset(&sss);
       sigaddset(&sss,SIGINT);
       sigprocmask(SIG_BLOCK,&sss,NULL);
	}
	if (ox_pathfinderNoX(-1) && (!EngineLogToStdout)) {
	  FILE *null;
	  null = fopen("/dev/null","wb");
	  dup2(fileno(null),1);
	  dup2(fileno(null),2);
	}

    /*
	{
	  int i;
	  i = 0;
	  fprintf(stderr,"argv-----------\n");
	  while (argv[i] != NULL) {
		fprintf(stderr,"%s  ",argv[i++]);
	  }
      fprintf(stderr,"\n");
	  i = 0;
	  fprintf(stderr,"environ-----------\n");
	  while (environ[i] != NULL) {
		fprintf(stderr,"%s  ",environ[i++]);
	  }
	  fprintf(stderr,"\n");
    }	
	*/

    execve(argv[0],argv,environ);
    /* This place will never be reached unless execv fails. */
    fprintf(stderr,"forkExec fails: ");
  }
}

