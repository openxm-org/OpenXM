/*
 $OpenXM: OpenXM/src/kxx/oxd.c,v 1.4 2002/10/27 10:39:33 takayama Exp $
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

/* #define DEBUG */

char *getTag(char *s);
char *getKeyValue(char *s,char *key);
char *getBody(char *s);
char *getOpenXMpath(void);
void *xtagMalloc(int n);

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
#define NOBODY 65534

main(int argc, char *argv[]) {
  char sname[1024];
  int i;
  int fdControl = -1; int portControl = 8089;
  extern int OpenedSocket;
  extern int Serial;
  int result;
  int fd;
  int uid;

  strcpy(sname,"localhost");
  i = 1;
  while (i<argc) {
    if (strcmp(argv[i],"--port")==0) {
      i++;
      if (i<argc) sscanf(argv[i],"%d",&portControl);
    }else if (strcmp(argv[i],"--insecure") == 0) {
      LocalMode = 0;
    }else {
      fprintf(stderr,"Unknown option %s.\n",argv[i]);
      oxdUsage(); exit(10);
    }
    i++;
  }

  uid = getuid();
  if (uid == 0) {
	/* If I'm a super user, then change uid to nobody. */
	if (setuid(NOBODY) != 0) {
	  oxdError("Failed to change uid to nobody (%d)\n",NOBODY);
	}
	fprintf(stderr,"uid is changed to nobody (%d).\n",NOBODY);
  }
  
  if (LocalMode) {
    if (portControl != -1) {
      fdControl = socketOpen(sname,portControl);
	  if (fdControl < 0) oxdError("Could not open a socket. \n\nPerhaps, oxd is already running on your machine.\nTo start oxd on a different port xyz, start oxd by oxd --port xyz");
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
  fprintf(stderr,"  oxd [--port xyz]\n");
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
  extern int Serial;
  char *openxm;
  int resultCode;
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
  openxm = getOpenXMpath();
  sprintf(ccc,"%s %s",openxm,body);
  fprintf(stderr,"Serial=%d : Executing command=%s\n",Serial,ccc);
  /*  Old code.
  fprintf(fp,"<bye/>\n"); fflush(NULL);
  fclose(fp); 
  system(ccc);
  fprintf(stderr,"Serial=%d : The following command is finished : %s\n",Serial,ccc);
   */
  /* New code. It requires  ox with -finish option. */
  resultCode = system(ccc);
  fprintf(stderr,"Serial=%d : The following command is finished : %s, resultCode=%d\n",Serial,ccc,resultCode);
  if (resultCode == 0) {
	fprintf(fp,"<suceeded/>\n"); 
  }else{
	fprintf(fp,"<failed code=\"%d\"/>\n",resultCode); 
  }

  GET_COMMAND   /* expect <login/> */

  fclose(fp); /* close the connection */
  exit(0);
}

char *getOpenXMpath() {
  char *s;
  char *sss;
  s = getenv("OpenXM_HOME");
  if (s == NULL) {
	s = getenv("OPENXM_HOME");
  }
  if (s == NULL) sss="/usr/local/bin/openxm";
  else {
	sss = (char *) xtagMalloc(strlen(s)+20);
	sprintf(sss,"%s/bin/openxm",s);
  }
  if (access(sss,X_OK&R_OK) == 0) {
  }else{
	oxdError("The shell script openxm does not exists. It is usually generated under OpenXM/rc");
  }
  return sss;
}

/* These are dummy.  It is defined in stackmachine.c */
unlockCtrlCForOx() { ; }
restoreLockCtrlCForOx() { ; }

oxdError(char *s) {
  fprintf(stderr,"%s\n",s);
  exit(10);
}



