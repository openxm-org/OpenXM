/* $OpenXM: OpenXM/src/kxx/oxserver00.c,v 1.5 2001/05/06 07:53:01 takayama Exp $ */
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

int OxCritical = 0;
int OxInterruptFlag = 0;
int PacketMonitor = 0;

extern int SerialOX;  /* Serial number of the packets sent. */
extern int SerialCurrent;  /* Current Serial number of the recieved packet. */
extern int OXprintMessage; /* print oxmessages? */

jmp_buf EnvOfChildServer;

int JmpMessage = 0;

static char *getSuffix(char *s);
main(int argc, char *argv[]) {
  char *s;
  char *forAsir[] = {"callsm1.sm1","callsm1b.sm1"};
  char *gnuplot[] = {"callsm1.sm1","gnuplot.sm1"};
  char *phc[] = {"callsm1.sm1","phc.sm1"};
  char *tigers[] = {"callsm1.sm1","tigers.sm1"};
  char *basicCD[] = {"basicCD.sm1"};
  /*  If you change the above, you need to change the argc of Sm1_start below.
   */

  if (argc > 1) {
    if (strcmp(argv[1],"-monitor")==0) {
      fprintf(stderr,"Taking the packet monitor.\n");
      PacketMonitor = 1;
    }else{
      fprintf(stderr,"Unknown option. Possible options are -monitor\n");
    }
  }
  /* Load initialization files if necessary. */
  s = getSuffix(argv[0]);
  if (strcmp(s,"forAsir") == 0) {
    Sm1_start(2,forAsir,"ox_sm1_forAsir");
  }else if (strcmp(s,"gnuplot") == 0) {
    Sm1_start(2,gnuplot,"ox_sm1_gnuplot");
  }else if (strcmp(s,"phc") == 0) {
    Sm1_start(2,phc,"ox_sm1_phc");
  }else if (strcmp(s,"tigers") == 0) {
    Sm1_start(2,tigers,"ox_sm1_tigers");
  }else if (strcmp(s,"basicCD") == 0) {
    Sm1_start(1,basicCD,"basicCD");
  }else {  /* Default */
    Sm1_start(0, NULL, "ox_sm1");
  }

  nullserver(3);
}

static char *getSuffix(char *s) {
  /* getSuffix("ox_sm1_forAsir") returns "forAsir" */
  /* getSuffix("ox_sm1_forAsir.exe") returns "forAsir" */
  /* getSuffix("ox_sm1_gnuplot") returns "gnuplot" */
  int n,i,k;
  n = strlen(s);
  if (n > 5 && (strcmp(".exe",&(s[n-4]))==0 || strcmp(".EXE",&(s[n-4]))==0)) {
    s[n-4] = '\0';
  }
  for (i=n-1; i>=0; i--) {
    if (s[i] == '_') {
      return( s+i+1 );
    }
  }
  return(s);
}
nullserver(int fdStream) {
  int mtag;
  int message = 1;
  ox_stream ostream;
  char sreason[1024];
  extern void controlResetHandler();
  extern jmp_buf EnvOfStackMachine;
  int engineByteOrder;

  fflush(NULL);
  engineByteOrder = oxTellMyByteOrder(fdStream);
  /* Set the network byte order. */
  fprintf(stderr,"engineByteOrder=%x\n",engineByteOrder);
  
  if (fdStream != -1) {
    ostream = fp2open(fdStream);
    if (ostream == NULL) {
      fprintf(stderr,"fp2open(fdStream) failed.\n");
      fdStream = -1;
    }
    if (PacketMonitor) fp2watch(ostream,stdout);
  }
  aaa : ;
  if (setjmp(EnvOfChildServer)) {
    fprintf(stderr,"childServerMain: jump here.\n");
    if (OxInterruptFlag == 0) {
      fprintf(stderr," ?! \n"); fflush(NULL);
    }
    signal(SIGUSR1,controlResetHandler); goto aaa;
  } else {  
    if (JmpMessage) fprintf(stderr,"Set EnvOfChildServer.\n");
    signal(SIGUSR1,controlResetHandler);
  }
  
  if (setjmp(EnvOfStackMachine)) {
    fprintf(stderr,"childServerMain: jump here by EnvOfStackMachine.\n");
    if (OxInterruptFlag == 0) {
      fprintf(stderr," ?! \n"); fflush(NULL);
    }
    /* In case of error in the stack machine, pop the error info
       and send the error packet. */
    /* oxSendOXheader(ostream,OX_DATA,SerialOX++);
       oxSendCmoError(ostream); 
       oxSendOXheader(ostream,OX_DATA,SerialOX++);
       sprintf(sreason,"Jump here by sm1 error.");
       oxSendCmoError2(ostream,sreason);
    */
    Sm1_pushError2(SerialCurrent,-1,"Global jump by sm1 error");

    signal(SIGUSR1,controlResetHandler); goto aaa ;
  } else {
    if (JmpMessage) fprintf(stderr,"Set EnvOfStackMachine.\n"); 
    if (signal(SIGUSR1,SIG_IGN) != SIG_IGN) {
      signal(SIGUSR1,controlResetHandler);
    }
  }
  
  while (1) {
    message = OXprintMessage;
    if (OxInterruptFlag) {
      OxCritical = 1;
      if (message) {fprintf(stderr,"Clearing the read buffer.\n");fflush(NULL); }
      fp2clearReadBuf(ostream); /* clear the read buffer */
      if (message) {fprintf(stderr,"Throwing OX_SYNC_BALL\n"); fflush(NULL);}
      oxSendSyncBall(ostream);
      if (message) {fprintf(stderr,"Waiting for OX_SYNC_BALL\n");fflush(NULL);}
      oxWaitSyncBall(ostream);
      if (message) {fprintf(stderr,"Done changing OX_SYNC_BALL\n"); fflush(NULL);}
      OxInterruptFlag = 0;
      OxCritical = 0;
      goto aaa ;
    }
    OxCritical = 0;
    if (fp2select(ostream,-1)) {
      /* If there is an data in the ostream, then read data in the buffer and
         read data in the communication stream. */
      OxCritical = 1;
    }else{
      /* interrupted system call */
      /* This part is never reached. */
    }
    OxCritical = 1;
    mtag = oxGetOXheader(ostream,&SerialCurrent); /* get the message_tag */
    if (message) {
      fprintf(stderr,"\nmtag is %d (serial=%d) : ",mtag,SerialCurrent);
      switch(mtag) {
      case OX_COMMAND: fprintf(stderr," OX_COMMAND \n"); break;
      case OX_DATA:    fprintf(stderr," OX_DATA \n"); break;
      case OX_SYNC_BALL: fprintf(stderr," OX_SYNC_BALL \n"); break;
      case -1: fprintf(stderr," End of file. Exiting the server child.\n");
        exit(); break;
      default: fprintf(stderr," ?! \n"); break;
      }
    }
    /*sleep(2);  /* for dubug OX_SYNC_BALL */
    switch(mtag) {
    case OX_COMMAND:
      nullserverCommand(ostream);
      goto aaa ;  /* We always reset the jump vector. cf. memo1.txt 1998 2/13*/
      break;
    case OX_DATA:
      Sm1_pushCMO(ostream);
      break;
    case OX_SYNC_BALL:
      /* if (OxInterruptFlag)  think about it later. */
      break;
    default:
      fprintf(stderr,"Fatal error in server.\n");
      break;
    }
  }
}

nullserverCommand(ox_stream ostream) {
  int id;
  int mtag;
  int n;
  char *name;
  char *sresult;
  struct mathCap *mathresult;
  int iresult;
  int message = 1;
  char *emsg;
  extern void controlResetHandler();

  message = OXprintMessage;
  /* message_body */
  id = oxGetInt32(ostream);   /* get the function_id */
  if (message) {fprintf(stderr,"\nfunction_id is %d\n",id);}
  switch( id ) {
  case SM_mathcap:
    if (message) fprintf(stderr," mathcap\n");
    mathresult = (struct mathCap *)Sm1_mathcap();
    oxPushMathCap(mathresult);
    break;
  case SM_setMathCap:
    if (message) fprintf(stderr," setMathCap\n");
    Sm1_setMathCap(ostream);
    break;
  case SM_pops:
    if (message) fprintf(stderr," pops \n");
    Sm1_pops();
    break;
  case SM_getsp:
    if (message) fprintf(stderr," getsp \n");
    Sm1_getsp();
    break;
  case SM_dupErrors:
    if (message) fprintf(stderr," dupErrors \n");
    Sm1_dupErrors();
    break;
  case SM_pushCMOtag:
    if (message) fprintf(stderr," pushCMOtag \n");
    Sm1_pushCMOtag(SerialCurrent);
    break;
  case SM_setName:
    if (message) fprintf(stderr," setName \n");
    iresult = Sm1_setName();
    if (iresult < 0) {
      Sm1_pushError2(SerialCurrent,-1,"setName");
    }
    break;
  case SM_evalName:
    if (message) fprintf(stderr," evalName \n");
    iresult = Sm1_evalName();
    if (iresult < 0) {
      Sm1_pushError2(SerialCurrent,-1,"evalName");
    }
    break;
  case SM_executeStringByLocalParser:
    if (message) fprintf(stderr," executeStringByLocalParser\n");
    OxCritical = 0;
    iresult = Sm1_executeStringByLocalParser();
    OxCritical = 1; signal(SIGUSR1,controlResetHandler);
    if (iresult < 0) {
      emsg = Sm1_popErrorMessage("executeString: ");
      Sm1_pushError2(SerialCurrent,-1,emsg);
      return(-1);
    }
    break;
  case SM_executeFunction:
    if (message) fprintf(stderr," executeFunction\n");
    OxCritical = 0;
    iresult = Sm1_executeStringByLocalParser();
    OxCritical = 1; signal(SIGUSR1,controlResetHandler);
    if (iresult < 0) {
      emsg = Sm1_popErrorMessage("executeFunction: ");
      Sm1_pushError2(SerialCurrent,-1,emsg);
      return(-1);
    }
    break;
  case SM_popCMO:
    if (message) fprintf(stderr,"popCMO.  Start to sending data.\n",n);
    oxSendOXheader(ostream,OX_DATA,SerialOX++);
    n=Sm1_popCMO(ostream,SerialCurrent);
    if (message) fprintf(stderr,"Done.\n"); 
    break;
  case SM_popString:
    if (message) fprintf(stderr,"popString. send data from the stack.\n",n);
    oxSendOXheader(ostream,OX_DATA,SerialOX++);
    oxSendCmoString(ostream,Sm1_popString());
    if (message) fprintf(stderr,"Done.\n");
    break;
  case SM_shutdown:
    fprintf(stderr,"Shutting down the engine.\n");
    exit(0);
    break;
  case SM_beginBlock:
  case SM_endBlock:
    fprintf(stderr,"This command has not yet been implemented.\n");
    return(-1);
    break;
  default:
    fprintf(stderr,"Fatal error. Unknown function_id %d\n",id);
    return(-1);
    break;
  }
  return(0);
}

      
nullserver_simplest(int fd) {
  int c;
  while(1) {
    c = readOneByte(fd);
    if (c == '@') { return; }
  }
}


void controlResetHandler(sig)
     int sig;
{
  signal(sig,SIG_IGN);
  fprintf(stderr,"From controlResetHandler. OxCritical = %d\n",OxCritical);
  OxInterruptFlag = 1;
  if (OxCritical) {
    return;
  }else{
    longjmp(EnvOfChildServer,2); /* returns 2 for ctrl-C */
  }
}

