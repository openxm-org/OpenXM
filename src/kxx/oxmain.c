/*  $OpenXM: OpenXM/src/kxx/oxmain.c,v 1.2 1999/10/30 02:24:27 takayama Exp $  */
/* nullserver01 */
#include <stdio.h>
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

#define SERVERNAME "/usr/local/OpenXM/bin/ox_sm1"

int OxCritical = 0;
int OxInterruptFlag = 0;

int SerialCurrentControl;

int MyServerPid;
char ServerName[1024];
int PacketMonitor = 0;
int Quiet = 0;

int LocalMode = 1;
int NotifyPortnumber = 0;


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

  if (reverse) {
    /* The order is very important. */
    fdControl = socketConnectWithPass(sname,portControl,pass);
    fdStream = socketConnectWithPass(sname,portStream,pass);

    fprintf(stderr,"Connected: control = %d, data = %d.\n",fdControl,fdStream);
    if (portControl != -1) {
      MyServerPid = fork();
      if (MyServerPid > 0 ) parentServerMain(fdControl,fdStream);
      else childServerMain(fdControl,fdStream);
    }else{
      childServerMain(fdControl,fdStream);
    }
    /* This line will be never executed. */
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



  if (portControl != -1) {
    MyServerPid = fork();
    if (MyServerPid > 0 ) parentServerMain(fdControl,fdStream);
    else childServerMain(fdControl,fdStream);
  }else{
    childServerMain(fdControl,fdStream);
  }

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
      oxSendResultOfControlInt32(fdControl,0);
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
  if (PacketMonitor) {
    if (execl(ServerName,ServerName,"-monitor",NULL)) {
      fprintf(stderr,"%s cannot be executed with -monitor.\n",ServerName);
    }
  }else {
    if (execl(ServerName,ServerName,NULL)) {
      fprintf(stderr,"%s cannot be executed.\n",ServerName);
    }
  }
}


/* These are dummy.  It is defined in stackmachine.c */
unlockCtrlCForOx() { ; }
restoreLockCtrlCForOx() { ; }







