/* $OpenXM: OpenXM/src/k097/ox_k0.c,v 1.11 2016/03/31 05:27:34 takayama Exp $ */
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>
#include <setjmp.h>
#include <string.h>
#include <stdlib.h>
/* -lnsl -lsocket /usr/ucblib/libucb.a */
#include "../kxx/ox_kan.h"
#include "../kxx/serversm.h"
#include "ox_k0.h"
#include "ki.h"


int OxCritical = 0;
int OxInterruptFlag = 0;
int PacketMonitor = 0;

extern int SerialOX;  /* Serial number of the packets sent. */
extern int SerialCurrent;  /* Current Serial number of the recieved packet. */
extern int OXprintMessage; /* print oxmessages? */
extern int Calling_ctrlC_hook;

#if defined(__CYGWIN__)
sigjmp_buf EnvOfChildServer;
#else
jmp_buf EnvOfChildServer;
#endif

int JmpMessage = 0;

static char *getSuffix(char *s);
void main(int argc, char *argv[]) {
  char *s;

  if (argc > 1) {
    if (strcmp(argv[1],"-monitor")==0) {
      fprintf(stderr,"Taking the packet monitor.\n");
      PacketMonitor = 1;
    }else{
      fprintf(stderr,"Unknown option. Possible options are -monitor\n");
    }
  }
  /* Load initialization files if necessary. */
  /* Sm1_start(0, NULL, "ox_sm1");  */
  K0_start();

  nullserver(3,4);
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
void nullserver(int fdStreamIn,int fdStreamOut) {
  int mtag;
  int message = 1;
  ox_stream ostreamIn;
  ox_stream ostreamOut;
  char sreason[1024];
  extern int RestrictedMode, RestrictedMode_saved;
  extern void controlResetHandler();
#if defined(__CYGWIN__)
  extern sigjmp_buf EnvOfStackMachine;
#else
  extern jmp_buf EnvOfStackMachine;
#endif
  int engineByteOrder;
  extern int InSendmsg2;

  fflush(NULL);
  engineByteOrder = oxTellMyByteOrder(fdStreamOut,fdStreamIn);
  /* Set the network byte order. */
  fprintf(stderr,"engineByteOrder=%x\n",engineByteOrder);
  
  if (fdStreamIn != -1) {
    ostreamIn = fp2open(fdStreamIn);
    if (ostreamIn == NULL) {
      fprintf(stderr,"fp2open(fdStreamIn) failed.\n");
      fdStreamIn = -1;
    }
    if (PacketMonitor) fp2watch(ostreamIn,stdout);
  }
  if (fdStreamOut != -1) {
    ostreamOut = fp2open(fdStreamOut);
    if (ostreamOut == NULL) {
      fprintf(stderr,"fp2open(fdStreamOut) failed.\n");
      fdStreamOut = -1;
    }
    if (PacketMonitor) fp2watch(ostreamOut,stdout);
  }
  aaa : ;
  clearInop(); 
#if defined(__CYGWIN__)
  if (sigsetjmp(EnvOfChildServer,1)) {
#else
  if (setjmp(EnvOfChildServer)) {
#endif
    fprintf(stderr,"childServerMain: jump here.\n");

	{
	  extern int Kpt; 
	  extern char * Kbuff;
	  fprintf(stderr,"Resetting...\n"); fflush(NULL);
	  Kpt = 0; Kbuff[0] = '\0';
	  parseAfile(stdin);
    }

    if (OxInterruptFlag == 0) {
      fprintf(stderr," ?! \n"); fflush(NULL);
    }
    if (!Calling_ctrlC_hook) {
      Calling_ctrlC_hook = 1; RestrictedMode = 0;
      KSexecuteString(" ctrlC-hook "); /* Execute User Defined functions. */
      RestrictedMode = RestrictedMode_saved;
    }
    Calling_ctrlC_hook = 0;
	KSexecuteString(" (Computation is interrupted.) ");
	InSendmsg2 = 0;
    mysignal(SIGUSR1,controlResetHandler); goto aaa;
  } else {  
    if (JmpMessage) fprintf(stderr,"Set EnvOfChildServer.\n");
    mysignal(SIGUSR1,controlResetHandler);
  }
#if defined(__CYGWIN__)
  if (sigsetjmp(EnvOfStackMachine,1)) {
#else  
  if (setjmp(EnvOfStackMachine)) {
#endif
    fprintf(stderr,"childServerMain: jump here by EnvOfStackMachine or timeout.\n");
    if (OxInterruptFlag == 0) {
      fprintf(stderr," ?! \n"); fflush(NULL);
    }
	{
	  extern int Kpt; 
	  extern char * Kbuff;
	  fprintf(stderr,"Resetting...\n"); fflush(NULL);
	  Kpt = 0; Kbuff[0] = '\0';
	  parseAfile(stdin);
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

    if (!Calling_ctrlC_hook) {
      Calling_ctrlC_hook = 1; RestrictedMode = 0;
	  KSexecuteString(" ctrlC-hook "); /* Execute User Defined functions. */
      RestrictedMode = RestrictedMode_saved;
    }
    Calling_ctrlC_hook = 0;
    InSendmsg2=0;
    mysignal(SIGUSR1,controlResetHandler); goto aaa ;
  } else {
    if (JmpMessage) fprintf(stderr,"Set EnvOfStackMachine.\n"); 
    if (mysignal(SIGUSR1,SIG_IGN) != SIG_IGN) {
      mysignal(SIGUSR1,controlResetHandler);
    }
  }
  
  while (1) {
    message = OXprintMessage;
    if (OxInterruptFlag) {
      OxCritical = 1;
      if (message) {fprintf(stderr,"Clearing the read buffer.\n");fflush(NULL); }
      fp2clearReadBuf(ostreamIn); /* clear the read buffer */
      if (message) {fprintf(stderr,"Throwing OX_SYNC_BALL\n"); fflush(NULL);}
      oxSendSyncBall(ostreamOut);
      if (message) {fprintf(stderr,"Waiting for OX_SYNC_BALL\n");fflush(NULL);}
      oxWaitSyncBall(ostreamIn);
      if (message) {fprintf(stderr,"Done changing OX_SYNC_BALL\n"); fflush(NULL);}
      OxInterruptFlag = 0;
      OxCritical = 0;
      goto aaa ;
    }
    OxCritical = 0;
    if (fp2select(ostreamIn,-1)) {
      /* If there is an data in the ostream, then read data in the buffer and
         read data in the communication stream. */
      OxCritical = 1;
    }else{
      /* interrupted system call */
      /* This part is never reached. */
    }
    OxCritical = 1;
    mtag = oxGetOXheader(ostreamIn,&SerialCurrent); /* get the message_tag */
    if (message) {
      fprintf(stderr,"\nmtag is %d (serial=%d) : ",mtag,SerialCurrent);
      switch(mtag) {
      case OX_COMMAND: fprintf(stderr," OX_COMMAND \n"); break;
      case OX_DATA:    fprintf(stderr," OX_DATA \n"); break;
      case OX_SYNC_BALL: fprintf(stderr," OX_SYNC_BALL \n"); break;
      case -1: fprintf(stderr," End of file. Exiting the server child.\n");
        exit(0); break;
      default: fprintf(stderr," ?! \n"); break;
      }
    }
    /*sleep(2);  /* for dubug OX_SYNC_BALL */
    switch(mtag) {
    case OX_COMMAND:
      nullserverCommand(ostreamIn,ostreamOut);
      goto aaa ;  /* We always reset the jump vector. cf. memo1.txt 1998 2/13*/
      break;
    case OX_DATA:
      Sm1_pushCMO(ostreamIn);
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

int nullserverCommand(ox_stream ostreamIn,ox_stream ostreamOut) {
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
  id = oxGetInt32(ostreamIn);   /* get the function_id */
  if (message) {fprintf(stderr,"\nfunction_id is %d\n",id);}
  switch( id ) {
  case SM_mathcap:
    if (message) fprintf(stderr," mathcap\n");
    mathresult = (struct mathCap *)Sm1_mathcap();
    oxPushMathCap(mathresult);
    break;
  case SM_setMathCap:
    if (message) fprintf(stderr," setMathCap\n");
    Sm1_setMathCap(ostreamOut);
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
    iresult = K0_executeStringByLocalParser();
    OxCritical = 1; mysignal(SIGUSR1,controlResetHandler);
    if (iresult < 0) {
      emsg = Sm1_popErrorMessage("executeString: ");
      Sm1_pushError2(SerialCurrent,-1,emsg);
      return(-1);
    }
    break;
  case SM_executeFunction:
    if (message) fprintf(stderr," executeFunction\n");
    OxCritical = 0;
    iresult = K0_executeStringByLocalParser();
    OxCritical = 1; mysignal(SIGUSR1,controlResetHandler);
    if (iresult < 0) {
      emsg = Sm1_popErrorMessage("executeFunction: ");
      Sm1_pushError2(SerialCurrent,-1,emsg);
      return(-1);
    }
    break;
  case SM_popCMO:
    if (message) fprintf(stderr,"popCMO.  Start to sending data %d\n",n);
    oxSendOXheader(ostreamOut,OX_DATA,SerialOX++);
    n=Sm1_popCMO(ostreamOut,SerialCurrent);
    if (message) fprintf(stderr,"Done.\n"); 
    break;
  case SM_popString:
    if (message) fprintf(stderr,"popString. send data from the stack %d\n",n);
    oxSendOXheader(ostreamOut,OX_DATA,SerialOX++);
    oxSendCmoString(ostreamOut,Sm1_popString());
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

      
int nullserver_simplest(int fd) {
  int c;
  while(1) {
    c = readOneByte(fd);
    if (c == '@') { return 0; }
  }
}


void controlResetHandler(sig)
     int sig;
{
  mysignal(sig,SIG_IGN);
  cancelAlarm();
  fprintf(stderr,"From controlResetHandler. OxCritical = %d\n",OxCritical);
  OxInterruptFlag = 1;
  if (OxCritical) {
    return;
  }else{
    (void) traceShowStack(); traceClearStack();
#if defined(__CYGWIN__)
    siglongjmp(EnvOfChildServer,2); /* returns 2 for ctrl-C */
#else
    longjmp(EnvOfChildServer,2); /* returns 2 for ctrl-C */
#endif
  }
}

/* From k2.c */

#define DATE "1998,12/15"
#include <stdio.h>

char *getLOAD_K_PATH();  /* from d.h */


extern int DebugCompiler;  /* 0:   , 1: Displays sendKan[ .... ] */
extern int DebugMode;
extern int K00_verbose;

int Startupk2 = 1;


void K0_start() {
  extern int Saki;
  extern int Interactive;
  int i;
  char *s;
  char tname[1024];
  FILE *fp;

  KSstart();   /********  initialize ***********/
  /* Call setjmp(EnvOfStackMachine) for error handling. cf. scanner() in
   Kan/stackmachine.c */

  fprintf(stderr,"This is kan/k0 Version %s",DATE);
  fprintf(stderr,"\n");
  fprintf(stderr,"WARNING: This is an EXPERIMENTAL version\n");
  if (K00_verbose == 1) {
    KSexecuteString(" /K00_verbose 1 def ");
  }else if (K00_verbose == 2) {
    KSexecuteString(" /K00_verbose 2 def ");
  }else {
    KSexecuteString(" /K00_verbose 0 def ");
  }
  execFile("var.sm1"); KSexecuteString(" strictMode ");
  execFile("incmac.sm1");
  execFile("slib.sm1");

  printf("\n\nIn(1)= ");
  s = "startup.k";
  if (Startupk2) {
    strcpy(tname,s);
    if ((fp = fopen(tname,"r")) == (FILE *) NULL) {
      strcpy(tname,getLOAD_K_PATH());
      strcat(tname,s);
      if ((fp = fopen(tname,"r")) == (FILE *) NULL) {
	strcpy(tname,LOAD_K_PATH);
	strcat(tname,s);
	if ((fp = fopen(tname,"r")) == (FILE *) NULL) {
	  fprintf(stderr,"No startup file.\n");
	}
      }
    }
    if (fp != (FILE *)NULL) {
      if (K00_verbose) fprintf(stderr,"Reading startup.k... ");
      Saki = 0;
      parseAfile(fp);
      KCparse();
      if (K00_verbose) fprintf(stderr,"Done.\n");
    }
  }

  clearInop();
}

/*
  Interactive = 0;
  parseAfile(stdin);
  parseAstring(" sm1(\" 1 1 10 { message } for \" ) "); It works ! 
  KCparse();

  KSstop();  closing 
*/

int K0_executeStringByLocalParser() {
  extern int Interactive;
  char *s;
  s = Sm1_popString();
  fprintf(stderr,"K0_executeStringByLocalParse(): %s\n",s);
  /* fflush(NULL); getchar(); */
  if (s != NULL) {
    clearInop(); Interactive=0; 
	parseAstring(s);
    KCparse();
  }
  return 0;
}





