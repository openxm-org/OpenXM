/*  $OpenXM: OpenXM/src/kxx/oxmain.c,v 1.5 2000/07/30 09:55:40 takayama Exp $  */
/* nullserver01 */
#include <stdio.h>
#include <fcntl.h>
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

#define SERVERNAME "ox_sm1"

int OxCritical = 0;
int OxInterruptFlag = 0;

int SerialCurrentControl;

int MyServerPid;
#define SERVERNAME_SIZE 4096
char ServerName[SERVERNAME_SIZE];
int PacketMonitor = 0;
int Quiet = 0;

int LocalMode = 1;
int NotifyPortnumber = 0;
int Do_not_use_control_stream_to_tell_no_server = 1;
static void errorToStartEngine(void);
static int findOxServer(char *server);
static void couldNotFind(char *s);


main(int argc, char *argv[]) {
  int fd;
  int size;
  char sname[1024];
  int tmp[1];
  char *buf;
  int i;
  int fdControl = -1; int portControl = 1200;
  int fdStream = -1;  int portStream = 1300;
  int reverse = 0;
  extern int OpenedSocket;
  char portfile[1024];
  char *pass;
  int result;

  strcpy(sname,"localhost");
  strcpy(ServerName,SERVERNAME);
  i = 1;
  if (argc == 1) {
    oxmainUsage();
    exit();
  }
  while (i<argc) {
    if (strcmp(argv[i],"-host") == 0) {
      i++;
      if (i<argc) strcpy(sname,argv[i]);
    }else if (strcmp(argv[i],"-data")==0) {
      i++;
      if (i<argc) sscanf(argv[i],"%d",&portStream);
    }else if (strcmp(argv[i],"-control")==0) {
      i++;
      if (i<argc) sscanf(argv[i],"%d",&portControl);
    }else if (strcmp(argv[i],"-ox") == 0) {
      i++;
      if (i<argc) strcpy(ServerName,argv[i]);
    }else if (strcmp(argv[i],"-monitor") == 0) {
      PacketMonitor = 1;
    }else if (strcmp(argv[i],"-insecure") == 0) {
      LocalMode = 0;
    }else if (strcmp(argv[i],"-reverse") == 0) {
      reverse = 1;
    }else if (strcmp(argv[i],"-portfile") == 0) {
      i++;
      if (i<argc) {
		sscanf(argv[i],"%s",portfile);
		portControl = 0;
		portStream = 0;
		NotifyPortnumber = 1;
      }
    }else if (strcmp(argv[i],"-pass") == 0) {
      i++;
      if (i<argc) {
		pass = argv[i];
      }
    }else {
      fprintf(stderr,"Unknown option %s.\n",argv[i]);
      oxmainUsage(); exit(10);
    }
    i++;
  }

  if (Do_not_use_control_stream_to_tell_no_server) {
	if (findOxServer(ServerName) < 0) {
	  fprintf(stderr,"Sleeping five seconds...\n");
	  sleep(5);
	  exit(-1);
	}
  }

  if (reverse) {
    /* The order is very important. */
    fdControl = socketConnectWithPass(sname,portControl,pass);
    fdStream = socketConnectWithPass(sname,portStream,pass);

    fprintf(stderr,"Connected: control = %d, data = %d.\n",fdControl,fdStream);
	result = 0;


    if (portControl != -1) {
      MyServerPid = fork();
      if (MyServerPid > 0 ) parentServerMain(fdControl,fdStream);
      else result=childServerMain(fdControl,fdStream);
    }else{
      result=childServerMain(fdControl,fdStream);
    }
    /* This line will be never executed in case of success */
	if (result < 0 ) {
	  errorToStartEngine();
	}
  }

  /* non-reverse case. */
  fprintf(stderr,"Hostname is %s \n",sname);
  fprintf(stderr,"Port for data (-data) = %d \n",portStream);
  fprintf(stderr,"Port for control message (-control) = %d \n",portControl);
  fflush(NULL);

  
  if (LocalMode) {
    if (portControl != -1) {
      fdControl = socketOpen(sname,portControl);
      portControl = OpenedSocket;
      if (NotifyPortnumber) {
		oxWritePortFile(0,portControl,portfile);
      }
      fdControl = socketAcceptLocal(fdControl);
      fprintf(stderr,"\n control port %d : Connected.\n",portControl);
    }
    if (portStream != -1) {
      fdStream = socketOpen(sname,portStream);
      portStream = OpenedSocket;
      if (NotifyPortnumber) {
		oxWritePortFile(1,portStream,portfile);
      }
      fdStream = socketAcceptLocal(fdStream);
      fprintf(stderr,"\n stream port %d : Connected.\n",portStream);
    }
  }else{
    if (portControl != -1) {
      fdControl = socketOpen(sname,portControl);
      portControl = OpenedSocket;
      if (NotifyPortnumber) {
		oxWritePortFile(0,portControl,portfile);
      }
      fdControl = socketAccept(fdControl);
      fprintf(stderr,"\n control port %d : Connected.\n",portControl);
    }
    if (portStream != -1) {
      fdStream = socketOpen(sname,portStream);
      portStream = OpenedSocket;
      if (NotifyPortnumber) {
		oxWritePortFile(1,portStream,portfile);
      }
      fdStream = socketAccept(fdStream);
      fprintf(stderr,"\n stream port %d : Connected.\n",portStream);
    }
  }


  result = 0;
  if (portControl != -1) {
    MyServerPid = fork();
    if (MyServerPid > 0 ) parentServerMain(fdControl,fdStream);
    else result = childServerMain(fdControl,fdStream);
  }else{
    result = childServerMain(fdControl,fdStream);
  }
  if (result < 0) errorToStartEngine();
}

static void errorToStartEngine(void) {
  fprintf(stderr,"Failed to start the engine. Childing process is terminating.\n");
  /* You have to tell to the control server that there is no engine.
	 And, the control server must tell the client that there is no
	 engine.
	 This part has not yet been implemented.
	 If you implement this, set Do_not_use_control_stream_to_tell_no_server to
	 zero.
	 */
  sleep(2);
  exit(-1);
}

oxmainUsage() {
  fprintf(stderr,"Usage: \n");
  fprintf(stderr,"  ox [-ox serverprogram -host name -data portnum -control portnum -monitor]\n");
  fprintf(stderr," [-insecure -portfile fname -reverse -pass xxxyyyzzz]");
  fprintf(stderr,"\n");
  fprintf(stderr,"-reverse: ox server connects to the client.\n");
  fprintf(stderr,"          The client must give a one time password to ox server to connect to the client with -pass option.\n");
  fprintf(stderr,"          The one time password can be seen by ps command, so you must not use this one time password system on an untrustful host.\n");
  fprintf(stderr,"          The one time password should be sent by a safe communication line like ssh and the ox server should be started by ssh. Do not use rsh\n");
  fprintf(stderr,"          If -reverse is not given, the client connect to the ox server\n");
  fprintf(stderr,"          See OpenXM/src/SSkan/Doc/ox.sm1, /sm1connectr\n");
  fprintf(stderr,"-insecure : \n");
  fprintf(stderr,"          If you access to the server from a localhost, you do not need one time password. However, if you access outside of the localhost, a one time password is required. To turn off this restriction, -insecure option is used.\n");
  fprintf(stderr,"\n");
  fprintf(stderr,"If ox fails to find the serverprogram, it tries to look for it in /usr/local/OpenXM/bin and $OpenXM_HOME/bin.\n");
  fprintf(stderr,"\n");
  fprintf(stderr,"Example 1:\n");
  fprintf(stderr,"(Start the ox server): dc1%% ox -ox ~/OpenXM/bin/ox_sm1 -host dc1.math.kobe-u.ac.jp -insecure -control 1200 -data 1300\n");
  fprintf(stderr,"(client):  sm1\n ");
  fprintf(stderr,"           (ox.sm1) run ; \n");
  fprintf(stderr,"           sm1>[(dc1.math.kobe-u.ac.jp) 1300 1200] oxconnect /ox.ccc set\n");
  fprintf(stderr,"Example 2:\n");
  fprintf(stderr,"(Start the ox server): dc1%% ox -ox ~/OpenXM/bin/ox_sm1\n");
  fprintf(stderr,"(client): dc1%% sm1\n ");
  fprintf(stderr,"           (ox.sm1) run ; \n");
  fprintf(stderr,"           sm1>[(localhost) 1300 1200] oxconnect /ox.ccc set\n");
  fprintf(stderr,"\n");
}

parentServerMain(int fdControl, int fdStream) {
  int id;
  int mtag;
  int size;
  int n;
  int r;
  int message = 1;
  int controlByteOrder;

  extern void myServerExit();

  controlByteOrder = oxTellMyByteOrder(fdControl);
  /* Set the network byte order. */
  fprintf(stderr,"controlByteOrder=%x\n",controlByteOrder);


  signal(SIGINT,myServerExit);
  while(1) {
    mtag = oxfdGetOXheader(fdControl,&SerialCurrentControl);
    /* get the message_tag */
    /* message_body */
    id = oxfdGetInt32(fdControl);   /* get the function_id */
    if (message) {printf("\n[control] control function_id is %d\n",id);}
    switch( id ) {
    case SM_control_kill:
      if (message) printf("[control] control_kill\n");
      oxSendResultOfControl(fdControl);
      sleep(2);
      myServerExit();
      break;
    case SM_control_reset_connection:
      if (message) printf("[control] control_reset_connection.\n");
      if (message) printf("Sending the SIGUSR1 signal to %d:  ",MyServerPid);
      r=kill(MyServerPid,SIGUSR1);
      if (message) printf("Result = %d\n",r);
      fflush(NULL);
/*      oxSendResultOfControlInt32(fdControl,0); */
      break;
    default:
      fprintf(stderr,"[control] Unknown control message.\n");
      fprintf(stderr,"Shutdown the server.");
      myServerExit();
      break;
    }
  }
}

void myServerExit() {
  printf("Sending the kill signal to the child.\n");
  kill(MyServerPid,SIGKILL);
  exit();
}

childServerMain(int fdControl, int fdStream) {
  int i;
  close(fdControl);   /* close(0); dup(fdStream); */
  dup2(fdStream,3);
  dup2(fdStream,4);  
  /*close(0);
	#include <sys/param.h>
	for (i=5; i<NOFILE; i++) close(i); 
	*/
  if (!Do_not_use_control_stream_to_tell_no_server) {
	if (findOxServer(ServerName) < 0) {
	  return(-1);
	}
  }
  fprintf(stderr,"childServerMain: Starting the server %s\n",ServerName); fflush(NULL);
  if (PacketMonitor) {
    if (execl(ServerName,ServerName,"-monitor",NULL)) {
      fprintf(stderr,"%s cannot be executed with -monitor.\n",ServerName);
	  fflush(NULL);
	  return(-1);
    }
  }else {
    if (execl(ServerName,ServerName,NULL)) {
      fprintf(stderr,"%s cannot be executed.\n",ServerName);
	  fflush(NULL);
	  return(-1);
    }
  }
  /* never reached. */
}


/* These are dummy.  It is defined in stackmachine.c */
unlockCtrlCForOx() { ; }
restoreLockCtrlCForOx() { ; }

static int findOxServer(char *server) {
  char *p;
  char *p2;
  int fd;
  char *getenv(char *s);
  if (strlen(server) == 0) return(-1);
  fd = open(server,O_RDONLY);
  if (fd >= 0) {
	fprintf(stderr,"Starting OX server : %s\n",server);
	close(fd);
	return(0);
  }
  if (server[0] == '/') {
	couldNotFind(server);
	return(-1);
  }
  fprintf(stderr,"The server %s was not found. Trying to find it under OpenXM/bin\n",server);
  p = getenv("OpenXM_HOME");
  if (p == NULL) {
    p = "/usr/local/OpenXM";
  }
  p2 = (char *) malloc(sizeof(char)*(strlen(p)+strlen("/bin/")+3+strlen(server)));
  if (p2 == NULL) { fprintf(stderr,"No more memory.\n"); exit(10); }
  strcpy(p2,p); strcat(p2,"/bin/"); strcat(p2,server);
  fd = open(p2,O_RDONLY);
  if (fd >= 0) {
	fprintf(stderr,"Starting OX server : %s\n",p2);
	if (strlen(p2) < SERVERNAME_SIZE) strcpy(server,p2);
	else {
	  couldNotFind("Too long ox server name.");
	  return(-1);
	}
	close(fd);
	return(0);
  }
  couldNotFind(p2);
  return(-1);
}

static void couldNotFind(char *s) {
  fprintf(stderr,"OX server %s could not be found.\n",s);
}




