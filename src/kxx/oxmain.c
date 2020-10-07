/*  $OpenXM: OpenXM/src/kxx/oxmain.c,v 1.29 2016/09/26 11:43:38 takayama Exp $  */
/*  Note on IntelMac. [2006.06.05]
    SIGINT does not seem to be blocked on the rosetta emulator of ppc
    on the IntelMac's. "ox" should be universal binary.
   A dirty hack to generate a universal binary of ox is as follows.
   (1) Add -arch ppc -arch i386 to CFLAGS in src/kxx/Makefile
       and src/kan96xx/plugin/Makefile
   (2) Build ox
*/
/* nullserver01 */
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>
#include <setjmp.h>
#include <stdlib.h>
/* -lnsl -lsocket /usr/ucblib/libucb.a */
#include "ox_kan.h"
#include "serversm.h"

#define SERVERNAME "ox_sm1"

void oxmainUsage();
void parentServerMain(int fdControl, int fdStream);
int childServerMain(int fdControl, int fdStream);
void unlockCtrlCForOx();
void restoreLockCtrlCForOx();


extern char **environ;
int OxCritical = 0;
int OxInterruptFlag = 0;
int OxTerminateMode = 0;

int SerialCurrentControl;

int MyServerPid;
#define SERVERNAME_SIZE 4096
char ServerName[SERVERNAME_SIZE];
int PacketMonitor = 0;
int Quiet = 0;

int LocalMode = 1;
int NotifyPortnumber = 0;
int Do_not_use_control_stream_to_tell_no_server = 1;
int IgnoreSIGINT = 1;
int Ox_protocol_1999 = 0;
static void errorToStartEngine(void);
static int findOxServer(char *server);
static void couldNotFind(char *s);
/*  gcc -v -c hoge.c */
static void mywait(int m);

void *sGC_malloc(int n) {
  return (void *)malloc(n);
}

void main(int argc, char *argv[]) {
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
  char *pass = NULL;
  char *passControl = NULL;
  char *passData = NULL;
  int result;
  int sleepingTime = 0;
  int authEncoding=0;
  FILE *fp;
  char *stmp;
  extern int OxTerminateMode;

  int sighup[]={SIGHUP,-1};
  block_signal(sighup);  /* ignore x of xterm */
  strcpy(sname,"localhost");
  strcpy(ServerName,SERVERNAME);
  i = 1;
  if (argc == 1) {
    oxmainUsage();
    exit(10);
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
    }else if (strcmp(argv[i],"-finish") == 0) {
      OxTerminateMode = 1;
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
    }else if (strcmp(argv[i],"-passData") == 0) {
      i++;
      if (i<argc) {
        passData = argv[i];
      }
    }else if (strcmp(argv[i],"-passControl") == 0) {
      i++;
      if (i<argc) {
        passControl = argv[i];
      }
    }else if (strcmp(argv[i],"-wait") == 0) {
      i++;
      if (i<argc) {
        sscanf(argv[i],"%d",&sleepingTime);
      }
    }else if (strcmp(argv[i],"-authEncoding") == 0) {
      i++;
      if (strcmp(argv[i],"file") == 0) {
        authEncoding = 1;
      }else{
        fprintf(stderr,"Unknown -authEncoding %s.\n",argv[i]);
        oxmainUsage(); exit(10);
      }
    }else if (strcmp(argv[i],"-ignoreSIGINT") == 0) {
      i++;
      if (i<argc) {
        sscanf(argv[i],"%d",&IgnoreSIGINT);
      }
    }else if (strcmp(argv[i],"-protocol_1999") == 0) {
      Ox_protocol_1999=1;
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

  if (sleepingTime) {
    fprintf(stderr,"Waiting to connect for %d seconds...\n",sleepingTime);
    sleep(sleepingTime);
    fprintf(stderr,"\nTrying to connect\n");
  }

  if ((pass != NULL) && (passData == NULL)) {
	passData = pass;
  }
  if ((pass != NULL) && (passControl == NULL)) {
    passControl = pass;
  }

  /* Decrypt passControl and passData, here. Lookup cryptmethod. */
  if (authEncoding == 1) {
	stmp = (char *)sGC_malloc(strlen(getenv("HOME"))+strlen(passControl)+
							  strlen(passData)+128);
	sprintf(stmp,"%s/.openxm/tmp.opt/%s",(char *)getenv("HOME"),passControl);
	fp = fopen(stmp,"r");
	if (fp == NULL) { fprintf(stderr,"passControl file %s is not found.\n",stmp); exit(1);}
	{char *rrr; rrr=fgets(stmp,127,fp);} passControl = stmp; fclose(fp);

	stmp = (char *)sGC_malloc(strlen(getenv("HOME"))+strlen(passControl)+
							  strlen(passData)+128);
	sprintf(stmp,"%s/.openxm/tmp.opt/%s",(char *)getenv("HOME"),passData);
	fp = fopen(stmp,"r");
	if (fp == NULL) { fprintf(stderr,"passData file %s is not found.\n",stmp); exit(1);}
	{char *rrr; rrr=fgets(stmp,127,fp);} passData = stmp; fclose(fp);
  }

  if (reverse) {
    /* The order is very important. */
    fdControl = socketConnectWithPass(sname,portControl,passControl);
    fdStream = socketConnectWithPass(sname,portStream,passData);

    fprintf(stderr,"Connected: control = %d, data = %d.\n",fdControl,fdStream);
    result = 0;


    if ((fdControl < 0) ||  (fdStream < 0)) {
      fprintf(stderr,"Waiting for 10 seconds to show an error.\n");
      sleep(10);
    }

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

  if (passControl != NULL) {
	char *s; int mm;
	fprintf(stderr,"passControl\n");
	mm = strlen(passControl);
	s = (char *) malloc(mm+1);
    if (s == NULL) {fprintf(stderr,"No more memory.\n"); exit(1); }
	if (read(fdControl,s,mm+1) < 0) {
	  fprintf(stderr,"Read error to read passControl\n"); sleep(5); exit(1);
	}
    s[mm] = 0;
	if (strcmp(s,passControl) != 0) {
	  fprintf(stderr,"s=%s and passControl=%s  do not match.\n",s,passControl); sleep(5); exit(1);
	}
	free(s);
  }
  if (passData != NULL) {
	char *s; int mm;
	mm = strlen(passData);
	fprintf(stderr,"passData\n");
	s = (char *) malloc(mm+1);
    if (s == NULL) {fprintf(stderr,"No more memory.\n"); exit(1); }
	if (read(fdStream,s,mm+1) < 0) {
	  fprintf(stderr,"Read error to read passData\n");
	  errorToStartEngine();
	}
	if (strcmp(s,passData) != 0) {
	  fprintf(stderr,"s=%s and passData=%s  do not match.\n",s,passData); 
	  errorToStartEngine();
	}
    free(s);
  }

  if ((fdControl < 0) ||  (fdStream < 0)) {
    fprintf(stderr,"Waiting for 10 seconds to show an error.\n");
    sleep(10);
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
  sleep(10);
  exit(-1);
}

void oxmainUsage() {
  fprintf(stderr,"Usage: \n");
  fprintf(stderr,"  ox [-ox serverprogram -host name -data portnum -control portnum -monitor]\n");
  fprintf(stderr," [-insecure -portfile fname -reverse -passControl xxxyyyzzz -passData pppqqqrrr]");
  fprintf(stderr," [-finish]  [-wait seconds] [-authEncoding [file]]");
  fprintf(stderr," [-ignoreSIGINT [1|0]]");
  fprintf(stderr,"\n");
  fprintf(stderr,"-reverse: ox server connects to the client.\n");
  fprintf(stderr,"          The client must give a one time password to ox server to connect to the client with -pass* option.\n");
  fprintf(stderr,"          The one time password can be seen by ps command, so you must not use this one time password system on an untrustful host.\n");
  fprintf(stderr,"          The one time password should be sent by a safe communication line like ssh and the ox server should be started by ssh. Do not use rsh\n");
  fprintf(stderr,"          (The option -pass is obsolete.)\n");
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

void parentServerMain(int fdControl, int fdStream) {
  int id;
  int mtag;
  int size;
  int n;
  int r;
  int message = 1;
  int controlByteOrder;
  extern int OxTerminateMode;
  extern void myServerExit(int m);

  int sigchld[]={SIGCHLD,-1};
  unblock_signal(sigchld);
  if (OxTerminateMode) {
	/*
	  OxTerminateMode cannot be used if you run ox by xterm -exec ox ...
	 */
	if (fork()) {
	  close(fdControl); close(fdStream);
	  /* Parent */
	  exit(0);  /*Tell the caller that launching is successfully finished.*/
	}
  }
  
  controlByteOrder = oxTellMyByteOrder(fdControl,fdControl);
  /* Set the network byte order. */
  fprintf(stderr,"controlByteOrder=%x\n",controlByteOrder);

  /*  int sigint[]={SIGINT,-1};
  set_signal(sigint[0],myServerExit);
  unblock_signal(sigint);
  */
  mysignal(SIGINT,SIG_IGN);
  while(1) {
    mtag = oxfdGetOXheader(fdControl,&SerialCurrentControl);
    /* get the message_tag */
    /* message_body */
    id = oxfdGetInt32(fdControl);   /* get the function_id */
    if (message) {printf("\n[control] control function_id is %d\n",id);}
    switch( id ) {
    case SM_control_kill:
      if (message) printf("[control] control_kill\n");
      /* oxSendResultOfControl(fdControl); */
      sleep(2);
      myServerExit(0);
      break;
    case SM_control_reset_connection:
      if (message) printf("[control] control_reset_connection.\n");
      if (message) printf("Sending the SIGUSR1 signal to %d:  ",MyServerPid);
      r=kill(MyServerPid,SIGUSR1);
      if (message) printf("Result = %d\n",r);
      if (Ox_protocol_1999) {
	if (message) printf("[obsolete protocol of ox-rfc-100 in 1999] Sending the result packet to the control channel. r=%d\n",r);
        oxSendResultOfControlInt32(fdControl,0); 
      }
      fflush(NULL);
      /*      oxSendResultOfControlInt32(fdControl,0); */
      break;
    default:
      fprintf(stderr,"[control] Unknown control message.\n");
      fprintf(stderr,"Shutdown the server.");
      myServerExit(0);
      break;
    }
  }
}

void myServerExit(int m) {
  printf("Sending the kill signal to the child.\n");
  kill(MyServerPid,SIGKILL);
  exit(0);
}

int childServerMain(int fdControl, int fdStream) {
  int i;
  struct rlimit res;
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

  /*  
  {
	int i;
	i=0;
	while (environ[i] != NULL) {
	  fprintf(stderr,"%s ",environ[i++]);
	}
	fprintf(stderr,"\n");
  }
  */
  /* bug: xterm of potato does not seem to pass the LD_LIBRARY_PATH.
      So, the new gc does not work.
     it is an workaround for OpenXM */
  if (getenv("LD_LIBRARY_PATH") == (char *)NULL) {
	char *s,*o;
    fprintf(stderr,"Hmm... LD_LIBRARY_PATH does not seem to be set.\n");
    o = getenv("OpenXM_HOME");
	if (o == NULL) {
	  fprintf(stderr,"Giving up to set the LD_LIBRARY_PATH variable.\n");
	}else{
	  s = (char *)malloc(strlen(o)+64);
	  sprintf(s,"LD_LIBRARY_PATH=%s/lib",o);
	  putenv(s);
	}
  }
  getrlimit(RLIMIT_STACK,&res);
  if (res.rlim_cur < 65536000) {
    fprintf(stderr,"RLIMIT_STACK is increased to 65Mbytes by setrlimit.\n");
	res.rlim_cur = 65536000;
	setrlimit(RLIMIT_STACK,&res);
  }

  //  int sigint[]={SIGINT,-1};
  //  if (IgnoreSIGINT) { block_signal(sigint); fprintf(stderr,"SIGING\n");}
  if (IgnoreSIGINT) { mysignal(SIGINT,SIG_IGN); fprintf(stderr,"SIG_ING\n");}

  if (PacketMonitor) {
    if (execle(ServerName,ServerName,"-monitor",NULL,environ)) {
      fprintf(stderr,"%s cannot be executed with -monitor.\n",ServerName);
      fflush(NULL);
      return(-1);
    }
  }else {
    if (execle(ServerName,ServerName,NULL,environ)) {
      fprintf(stderr,"%s cannot be executed.\n",ServerName);
      fflush(NULL);
      return(-1);
    }
  }
  /* never reached. */
}


/* These are dummy.  It is defined in stackmachine.c */
void unlockCtrlCForOx() { ; }
void restoreLockCtrlCForOx() { ; }

static int findOxServer(char *server) {
  char *p;
  char *p2;
  if (strlen(server) == 0) return(-1);
  /* fd = open(server,O_RDONLY); */
  if (access(server,X_OK&R_OK) == 0) {
    fprintf(stderr,"Starting OX server : %s\n",server);
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
  /* fd = open(p2,O_RDONLY); */
  if (access(p2,X_OK&R_OK) == 0) {
    fprintf(stderr,"Starting OX server : %s\n",p2);
    if (strlen(p2) < SERVERNAME_SIZE) strcpy(server,p2);
    else {
      couldNotFind("Too long ox server name.");
      return(-1);
    }
    return(0);
  }
  couldNotFind(p2);
  return(-1);
}

static void couldNotFind(char *s) {
  fprintf(stderr,"OX server %s could not be found.\n",s);
}


static void mywait(int m) {
  int status;
  int pid;
  int i,j;
  /* signal(SIGCHLD,SIG_IGN); */
  pid = wait(&status);
  fprintf(stderr,"Control: child process %d is exiting.\n",pid);
  fprintf(stderr,"Control: Shutting down the control server.\n");
  sleep(2);
  exit(0);
}


