/*
 $OpenXM$
*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>
#include <setjmp.h>
/* -lnsl -lsocket /usr/ucblib/libucb.a */
#include "ox_kan.h"
#include "serversm.h"

char *getTag(char *s);
char *getKeyValue(char *s,char *key);
char *getBody(char *s);

int MyServerPid;
#define SERVERNAME_SIZE 4096
int Quiet = 0;
int Serial = 0;

int LocalMode = 1;
static int findOxServer(char *server);
static void couldNotFind(char *s);
#if defined(__CYGWIN__)
int errno;
#endif

main(int argc, char *argv[]) {
  char sname[1024];
  int i;
  int fdControl = -1; int portControl = 8089;
  extern int OpenedSocket;
  extern int Serial;
  int sleepingTime = 0;
  int result;
  int fd;

  strcpy(sname,"localhost");
  i = 1;
  while (i<argc) {
    if (strcmp(argv[i],"--control")==0) {
      i++;
      if (i<argc) sscanf(argv[i],"%d",&portControl);
    }else if (strcmp(argv[i],"--insecure") == 0) {
      LocalMode = 0;
    }else if (strcmp(argv[i],"--wait") == 0) {
      i++;
      if (i<argc) {
        sscanf(argv[i],"%d",&sleepingTime);
      }
    }else {
      fprintf(stderr,"Unknown option %s.\n",argv[i]);
      oxdUsage(); exit(10);
    }
    i++;
  }

  if (sleepingTime) {
    fprintf(stderr,"Waiting to connect for %d seconds...\n",sleepingTime);
    sleep(sleepingTime);
    fprintf(stderr,"\nTrying to connect\n");
  }

  
  if (LocalMode) {
    if (portControl != -1) {
      fdControl = socketOpen(sname,portControl);
	  if (fdControl < 0) oxdError("Could not open a socket.");
      portControl = OpenedSocket;
	  while (1) {
		/* fdControl : fd for the wait queue */
		/* fd : accepted fd */
		fprintf(stderr,"Waiting a connection... Serial=%d\n",Serial);
		fflush(NULL);
		fd = socketAcceptLocal2(fdControl);
		if (fd < 0) oxdError("Failed to accept.");
		fprintf(stderr,"\nConnected.\n");
		Serial++;
		MyServerPid = fork();
		if (MyServerPid > 0 ) {
		  /* parent */
		  close(fd);
		} else {
		  result = childServerMain(fd);
		  exit(10);
		}
	  }
    }
  }else{
	oxdError("Non-localmode is not supported.");
  }
}


oxdUsage() {
  fprintf(stderr,"Usage: \n");
  fprintf(stderr,"  oxd \n");
  fprintf(stderr,"\n");
}

void exitServer(int n) {
  extern Serial;
  fprintf(stderr,"\nSerial=%d, Timeout. Exiting.\n",Serial);
  exit(0);
}
void exitServer2(FILE *fp,char *s) {
  extern Serial;
  fprintf(stderr,"\nSerial=%d: %s\n",Serial,s);
  fprintf(fp,"Error: %s\n",s);
  fprintf(fp,"Close connection.\n",s);
  fflush(NULL);
  exit(0);
}
childServerMain(int fd) {
  FILE *fp;
#define SIZE 1024
  char comm[SIZE+2];
  char *status;
  int id;
  char *tag;
  char *key;
  char *home;
  char *body;
  char fname[SIZE*2];
  char fnameBody[SIZE];
  char ccc[SIZE*3];
  /* Starting oxd session */
  signal(SIGALRM,exitServer);
  alarm(60);
  fp = fdopen(fd,"w+");
  if (fp == NULL) oxdError("failed fdopen\n");

#define GET_COMMAND {\
  fprintf(fp,"?"); fflush(fp); \
  status = fgets(comm,SIZE,fp); \
  if (status == NULL) { \
    fprintf(stderr,"End of Input.\n"); \
    exit(0); \
  } \
  /* fprintf(fp,"%s\n",comm); fflush(fp);*/ /* Just echo for debugging. */ \
  fprintf(stderr,"Serial=%d: command=%s\n",Serial,comm); }

  /* Login  */
  GET_COMMAND ;
  tag = getTag(comm);  
  if (tag == NULL) exitServer2(fp,"expecting <login method=\"file\"/>");
  if (strcmp(tag,"login") != 0) exitServer2(fp,"expecting <login method=\"file\"/>");
  key =getKeyValue(comm,"method");
  if (key == NULL || strcmp(key,"file") != 0) exitServer2(fp,"expecting <login method=\"file\"/>");

  /* Out put a challenge. */
  home = getenv("HOME");
  if (home == NULL) oxdError("Set the HOME environmental variable.\n");
  id = Serial*1000+getpid();
  sprintf(fnameBody,".oxd%d",id);
  sprintf(fname,"%s/%s",home,fnameBody);
  fprintf(fp,"<challenge file=\"%s\"/>\n",fname); fflush(fp);

  /* Wait <done/> */
  GET_COMMAND ;
  tag = getTag(comm);
  if ((tag == NULL) || (strcmp(tag,"done") != 0)) exitServer2(fp,"expecting <done>");
  if (access(fname,R_OK) == 0) {
	sprintf(ccc,"rm -f %s",fname);
	system(ccc);
  }else{
	exitServer2(fp,"Challenge file does not exist.");
  }

  /* Expect <launch> */
  GET_COMMAND ;
  tag = getTag(comm);
  if ((tag == NULL) || (strcmp(tag,"launch")!=0))
	exitServer2(fp,"expecting <launch> ox -ox ox_asir -reverse -data dport -control cport </launch>");

  body = getBody(comm);
  if (strlen(body) > SIZE*2) exitServer2(fp,"too big body.");
  sprintf(ccc,"/usr/local/bin/openxm %s",body);
  fprintf(stderr,"Serial=%d : Executing command=%s\n",ccc);
  fprintf(fp,"<bye/>\n"); fflush(NULL);
  fclose(fp); /* close the connection */
  system(ccc);
  fprintf(stderr,"Serial=%d : The following command is finished : %s\n",ccc);
}


/* These are dummy.  It is defined in stackmachine.c */
unlockCtrlCForOx() { ; }
restoreLockCtrlCForOx() { ; }

oxdError(char *s) {
  fprintf(stderr,"%s\n",s);
  exit(10);
}



