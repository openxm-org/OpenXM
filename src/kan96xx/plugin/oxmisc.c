/*  $OpenXM: OpenXM/src/kan96xx/plugin/oxmisc.c,v 1.31 2016/08/29 01:15:01 takayama Exp $ */
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <setjmp.h>
#include <errno.h>
#include <time.h>
#define SET_MYERROROUT { if (MyErrorOut == NULL) MyErrorOut=stdout; }
/* It is also defined in oxmisc2.c */
FILE *MyErrorOut = NULL;

/* Include files to understand object */
#include "../Kan/datatype.h"
#include "../Kan/stackm.h"
#include "../Kan/extern.h"

#include "ox_kan.h"
#include "mysig.h"

void restoreLockCtrlCForOx(); // defined in Kan/stackmachine.c

#define READBUFSIZE 5000

int OxVersion = 200012030;
int UseOXPacketSerial = 1;
int SerialOX = 1;
extern int Quiet;

static char *OxPortFileName = ".ox.dummy";


int readOneByte(int fd)   /* blocking */
{
  static char data[1];
  int size;
  int ans;
  int watch = 1;

  SET_MYERROROUT;
  if (fd < 0) {
    fprintf(MyErrorOut,"readOneByte fd < 0 ??? .\n");
    return(-1);
  }

  if (oxSocketSelect0(fd,-1)) { /* block */
    size = read(fd,data,1);
    if (size == 0) {
      fprintf(MyErrorOut,"oxSocketSelect0() returns 1, but there is no data. Your peer may be killed.\n");
      return(-1);
    }
    return(data[0]);
  }else{
    fprintf(MyErrorOut,"readOneByte: select error in block mode. Retrying.\n");
    return(-1);
  }
}

int readOneByte_org(int fd)   /* blocking */
{
  static char data[READBUFSIZE];
  static int thisFd = -1;
  static int from = 0;
  static int to = 0;
  int size;
  int ans;
  int watch = 1;

  SET_MYERROROUT;
  if ((thisFd == -1) && (fd >= 0)) {thisFd = fd;}
  if (fd < 0) {
    fprintf(MyErrorOut,"readOneByte fd < 0 ??? .\n");
    return(-1);
  }

  if (fd != thisFd) {
    fprintf(MyErrorOut,"readOneByte can be used only for one fd.\n");
    fflush(NULL);
    return(-1);
  }
  if (to > from) {
    ans = data[from];
    from++;
    if (watch) {
      printf("%2x ",ans);
      fflush(stdout);
    }
    return(ans);
  }

  while (1) {
    if (oxSocketSelect0(fd,-1)) { /* block */
      size = read(fd,data,READBUFSIZE-1);
      if (size == 0) {
        fprintf(MyErrorOut,"oxSocketSelect0() returns 1, but there is no data. Your peer may be killed.\n");
        return(-1);
      }
      from = 0;
      to = size;
      return(readOneByte(fd));
    }else{
      fprintf(MyErrorOut,"readOneByte: select error in block mode. Retrying.\n");  }
  }
}

int oxfdGetInt32(int fd)
{
  char d[4];
  int i;
  for (i=0; i<4; i++) {
    d[i] = readOneByte(fd);
  }
  return(ntohl(* ( (int *)d)));
}

int oxGetInt32(ox_stream ostream)
{
  char d[4];
  int i;
  for (i=0; i<4; i++) {
    d[i] = fp2fgetc(ostream);
  }
  return(ntohl(* ( (int *)d)));
}
int oxGetCMOInt32(ox_stream ostream)
{
  int id;
  SET_MYERROROUT;
  id = oxGetInt32(ostream);
  if (id != CMO_INT32) {
    fprintf(MyErrorOut,"It is not CMO_INT32.\n");
    return(0);
  }
  return(oxGetInt32(ostream));
}
char *oxGetCMOString(ox_stream ostream) {
  int id;
  int size;
  char *r;
  int i;
  SET_MYERROROUT;
  id = oxGetInt32(ostream);
  if (id != CMO_STRING) {
    fprintf(MyErrorOut,"It is not CMO_STRING.\n");
    return(NULL);
  }
  size = oxGetInt32(ostream);
  if (size <0) {
    return(NULL);
  }else{
    r = (char *)mymalloc(size+1);
    for (i=0; i<size; i++) {
      r[i] = fp2fgetc(ostream);
    }
    r[size] = '\0';
    return(r);
  }
}

void oxfdSendSyncBall(int fd) 
{
  oxfdSendOXheader(fd,OX_SYNC_BALL,SerialOX++);
}

void oxSendSyncBall(ox_stream os)
{
  oxSendOXheader(os,OX_SYNC_BALL,SerialOX++);
}

int oxWaitSyncBall(ox_stream ostream)
{
  int sss;
  int mtag;
  int com;
  while ((mtag = oxGetOXheader(ostream,&sss)) != OX_SYNC_BALL) {
    switch (mtag) {
    case OX_COMMAND:
      fprintf(stderr,"Waiting for command body: "); fflush(NULL);
      com=oxGetInt32(ostream);
      fprintf(stderr,"%d. Done\n",com); 
      break;
    default:  /* Todo, need support OX_DATA */
      fprintf(stderr,"Looking for the next message tag. Current unknown or unimplented mtag=%d\n",mtag);
      if (UseOXPacketSerial) fprintf(stderr,"Note that we expect the OX message tag with a serial number.\n");
      if (UseOXPacketSerial && (sss == OX_SYNC_BALL)) {
        /* dirty trick, it might cause a trouble. */
	fprintf(stderr,"We assume that the serial number is OX_SYNC_BALL\n");
	oxGetInt32(ostream); // discard the serial of OX_SYNC_BALL
        goto aaa ;
      }
      break;
    }
  aaa:
    /* or stdout */
    fflush(NULL);
	if (mtag == -1) {
	  fprintf(stderr,"Your peer seems to be dead.\n"); return 0;
	}
  }
}

int oxWaitSyncBall_org(ox_stream ostream)
{
  int mtag;
  char data[4];
  int c;
  SET_MYERROROUT;
  data[0] = data[1] = data[2] = data[3] = 0xff;
  while (1) {
    /* This part should be revised so that this part understands
       the cmo format.
    */
       
    if ((c = fp2fgetc(ostream)) < 0) {
      /* never use read directory. readOneByte() is buffers every thing. */
      fprintf(MyErrorOut,"End of file.\n");
      return(-1);
    }
    data[0] = data[1];
    data[1] = data[2];
    data[2] = data[3];
    data[3] = (char)c;
    mtag = ntohl(*((int *)(data)));
    if (mtag == OX_SYNC_BALL) {
      printf("Found the OX_SYNC_BALL. \n");
      fflush(NULL);
      return(mtag);
    }
    fprintf(stderr,"Looking for the next message tag.. %2x, mtag=%d\n",c,mtag);
    /* or stdout */
    fflush(NULL);
  }
}
  

void oxfdSendCmoNull(int fd)
{
  char data[4];
  *((int *)&data[0]) = htonl(CMO_NULL);
  {int r; r=write(fd,data,4);}
  fflush((FILE *)NULL);
}
void oxSendCmoNull(ox_stream os)
{
  char data[4];
  *((int *)&data[0]) = htonl(CMO_NULL);
  fp2write(os,data,4);
  fp2fflush(os);
}

void oxSendCmoError(ox_stream os)
{
  char data[4];
  *((int *)&data[0]) = htonl(CMO_ERROR);
  fp2write(os,data,4);
  fp2fflush(os);
}

void oxSendCmoError2(ox_stream os,char *s)
{
  char data[4];
  *((int *)&data[0]) = htonl(CMO_ERROR2);
  fp2write(os,data,4);
  fp2fflush(os);
  oxSendCmoString(os,s);
}

void oxfdSendInt32(int fd,int k)
{
  char data[4];
  *((int *)&data[0]) = htonl(k);
  {int r; r=write(fd,data,4);}
  fflush((FILE *)NULL);
}
void oxSendInt32(ox_stream os,int k)
{
  char data[4];
  *((int *)&data[0]) = htonl(k);
  fp2write(os,data,4);
  fp2fflush(os);
}

void oxfdSendCmoInt32(int fd,int k)
{
  char data[4*2];
  *((int *)&data[0]) = htonl(CMO_INT32);
  *((int *)&data[4]) = htonl(k);
  {int r; r=write(fd,data,4*2);}
  fflush((FILE *)NULL);
}
void oxSendCmoInt32(ox_stream os,int k)
{
  char data[4*2];
  *((int *)&data[0]) = htonl(CMO_INT32);
  *((int *)&data[4]) = htonl(k);
  fp2write(os,data,4*2);
  fp2fflush(os);
}
void oxfdSendCmoString(int fd,char *s)
{
  char data[4*2];
  int n;
  if (s == NULL) n = 0;
  else {
    n = strlen(s);
  }
  *((int *)&data[0]) = htonl(CMO_STRING);
  *((int *)&data[4]) = htonl(n);
  {int r; r=write(fd,data,4*2);}
  if (s != NULL) {
    {int r; r=write(fd,s,n);}
  }
  fflush((FILE *)NULL);
}

void oxSendCmoString(ox_stream os,char *s)
{
  char data[4*2];
  int n;
  if (s == NULL) n = 0;
  else {
    n = strlen(s);
  }
  *((int *)&data[0]) = htonl(CMO_STRING);
  *((int *)&data[4]) = htonl(n);
  fp2write(os,data,4*2);
  if (s != NULL) {
    fp2write(os,s,n);
  }
  fp2fflush(os);
}



void oxReqPushString(ox_stream os, char *s)
{
  oxSendOXheader(os,OX_DATA,SerialOX++);
  oxSendCmoString(os,s);
}

void oxSendResultOfControlInt32(int fd,int i) 
{
  char data[4*2];
  oxfdSendOXheader(fd,OX_DATA,SerialOX++);
  *((int *)&data[0]) = htonl(CMO_INT32);
  *((int *)&data[4]) = htonl(i);
  {int r; r=write(fd,data,4*2);}
  fflush((FILE *)NULL);
}
  
void oxSendResultOfControl(int fd) 
{
  char data[4*1];
  oxfdSendOXheader(fd,OX_DATA,SerialOX++);
  *((int *)&data[0]) = htonl(CMO_NULL);
  {int r; r=write(fd,data,4*1);}
  fflush((FILE *)NULL);
}



void oxSendMathCap(ox_stream os,struct mathCap *mathcap)
{
  int i,n,infosize,ncmo;
  struct object mathinfo = OINIT;
  /* printf("ox sending mathcap\n"); fflush(stdout); */
  mathinfo = *((struct object *)(mathcap->infop));
  infosize = getoaSize(mathinfo);

  oxSendInt32(os,CMO_MATHCAP);

  oxSendInt32(os,CMO_LIST);
  oxSendInt32(os,3);

  /* [0] */
  oxSendInt32(os,CMO_LIST);
  oxSendInt32(os,infosize);
  oxSendCmoInt32(os,KopInteger(getoa(mathinfo,0)));
  for (i=1; i<infosize; i++) {
    oxSendCmoString(os,KopString(getoa(mathinfo,i)));
  }

  /* [1]  */
  oxSendInt32(os,CMO_LIST);
  oxSendInt32(os,mathcap->smSize);
  n = mathcap->smSize;
  for (i=0; i<n; i++) {
    oxSendCmoInt32(os,(mathcap->sm)[i]);
  }

  /* [2] */
  oxSendInt32(os,CMO_LIST);
  n = mathcap->oxSize;
  oxSendInt32(os,n);

  oxSendInt32(os,CMO_LIST);
  oxSendInt32(os,2);
  for (i=0; i<n; i++) {
    /* OX_DATA_xxx */
    oxSendCmoInt32(os,(mathcap->ox)[i]);
    /* OX_DATA_xxx tags. In case of CMO, it is CMO tags. */
    oxSendInt32(os,CMO_LIST);
    oxSendInt32(os,mathcap->n);
    ncmo = mathcap->n;
    for (i=0; i<ncmo; i++) {
      oxSendCmoInt32(os,(mathcap->cmo)[i]);
      /* printf("i=%d %d, ",i,(mathcap->cmo)[i]); */
    }
  }
  /* printf("\n"); fflush(stdout); */
}

void oxReqMathCap(ox_stream os) {
  oxSendOXheader(os,OX_COMMAND,SerialOX++);
  oxSendInt32(os,SM_mathcap);
  fp2fflush(os);
}

void oxReqSetMathCap(ox_stream os,struct mathCap *mathcap) {
  oxSendOXheader(os,OX_DATA,SerialOX++);
  oxSendMathCap(os,mathcap);
  oxSendOXheader(os,OX_COMMAND,SerialOX++);
  oxSendInt32(os,SM_setMathCap);
  fp2fflush(os);
}
void oxReqPops(ox_stream os,int n) { 
  oxSendOXheader(os,OX_DATA,SerialOX++);
  oxSendCmoInt32(os,n);
  oxSendOXheader(os,OX_COMMAND,SerialOX++);
  oxSendInt32(os,SM_pops);
  fp2fflush(os);
}
void oxReqSetName(ox_stream os,char *s) {
  oxSendOXheader(os,OX_DATA,SerialOX++);
  oxSendCmoString(os,s);
  oxSendOXheader(os,OX_COMMAND,SerialOX++);
  oxSendInt32(os,SM_setName);
  fp2fflush(os);
}
void oxReqEvalName(ox_stream os,char *s) {
  oxSendOXheader(os,OX_DATA,SerialOX++);
  oxSendCmoString(os,s);
  oxSendOXheader(os,OX_COMMAND,SerialOX++);
  oxSendInt32(os,SM_evalName);
  fp2fflush(os);
}

void oxReqExecuteStringByLocalParser(ox_stream os,char *s)
{
  oxSendOXheader(os,OX_DATA,SerialOX++);
  oxSendCmoString(os,s);
  oxSendOXheader(os,OX_COMMAND,SerialOX++);
  oxSendInt32(os,SM_executeStringByLocalParser);
  fp2fflush(os);
}

void oxReqExecuteFunction(ox_stream os,char *s)
{
  oxSendOXheader(os,OX_DATA,SerialOX++);
  oxSendCmoString(os,s);
  oxSendOXheader(os,OX_COMMAND,SerialOX++);
  oxSendInt32(os,SM_executeFunction);
  fp2fflush(os);
}

void oxReqExecuteFunctionWithOptionalArgument(ox_stream os,char *s)
{
  oxSendOXheader(os,OX_DATA,SerialOX++);
  oxSendCmoString(os,s);
  oxSendOXheader(os,OX_COMMAND,SerialOX++);
  oxSendInt32(os,SM_executeFunctionWithOptionalArgument);
  fp2fflush(os);
}


void oxReqPopString(ox_stream os)
{
  oxSendOXheader(os,OX_COMMAND,SerialOX++);
  oxSendInt32(os,SM_popString);
  fp2fflush(os);
}

void oxReqSingleOperand(ox_stream os,int smtag)
{
  oxSendOXheader(os,OX_COMMAND,SerialOX++);
  oxSendInt32(os,smtag);
  fp2fflush(os);
}


void oxReqControlResetConnection(int fd) {
  oxfdSendOXheader(fd,OX_COMMAND,SerialOX++);
  oxfdSendInt32(fd,SM_control_reset_connection);
  fflush(NULL);
}

void oxReqControlKill(int fd) {
  oxfdSendOXheader(fd,OX_COMMAND,SerialOX++);
  oxfdSendInt32(fd,SM_control_kill);
  fflush(NULL);
}

void oxReqPopCMO(ox_stream os) {
  oxSendOXheader(os,OX_COMMAND,SerialOX++);
  oxSendInt32(os,SM_popCMO);
  fp2fflush(os);
}


int oxGetResultOfControlInt32(int fd) {
  int k; int sss;
  SET_MYERROROUT;
  k = oxfdGetOXheader(fd,&sss);
  if (k != OX_DATA) {
    fprintf(MyErrorOut,"oxGetResultOfControlInt32: wrong header.");
    return(-1);
  }
  k = oxfdGetInt32(fd); /* CMO_INT32 */
  k = oxfdGetInt32(fd);
  return(k);
}

int oxclientMultiSelect(oxclientp clients[],int dataready[],
                        int controlready[], int size, int t)
{
  int i, ddd;
  int fd;
  int humanfd = 0;
  fd_set readfds;
  struct timeval timeout;

  SET_MYERROROUT;
  /** printf("(1)"); fflush(NULL); */
  FD_ZERO(&readfds);
  timeout.tv_sec = 0;
  timeout.tv_usec = (long) t;
  
  ddd = 0;  fd = 0;
  for (i=0; i<size; i++) {
    dataready[i] = controlready[i] = 0;
  }
  for (i=0; i<size; i++) {
    if (clients[i]->humanio) {
      fd = (fd<humanfd?humanfd:fd);
      FD_SET(humanfd,&readfds);
      if (oxSocketSelect0(humanfd,0)) {
        ddd = dataready[i] = 1; controlready[i] = 0;
      }else{
        dataready[i] = 0; controlready[i] = 0;
      }
    }else{
      if (clients[i]->controlport < 0) { /* For RFC_101 */
        controlready[i] = 0;  
      }else{
        fd = (fd<clients[i]->controlfd?clients[i]->controlfd:fd);
        FD_SET(clients[i]->controlfd,&readfds);
        if (oxSocketSelect0(clients[i]->controlfd,0)) {
          ddd = controlready[i] = 1;
        }else{
          controlready[i] = 0;
        }
      }
      if (clients[i]->datafp2 != NULL) {
        fd = (fd<clients[i]->datafp2->fd?clients[i]->datafp2->fd:fd);
        FD_SET(clients[i]->datafp2->fd,&readfds);
        if (fp2select(clients[i]->datafp2,0)) {
          ddd = dataready[i] = 1;
        }else{
          dataready[i] = 0;
        }
      }else{
        dataready[i] = 0;
      }
    }
  }
  if (t > 0 ) {
    if (ddd) return(1);
    if (select(fd+1,&readfds,(fd_set *)NULL,(fd_set *)NULL,&timeout)<0) {
      fprintf(MyErrorOut,"error");
      return(-1);
    }
    return(oxclientMultiSelect(clients, dataready, controlready,size,0));
  }else if (t == 0) {
    return(ddd);
  }else {
    /** printf("(2)"); fflush(NULL); */
    if (select(fd+1,&readfds,(fd_set *)NULL,(fd_set *)NULL,(struct timeval *)NULL)<0) {
      fprintf(MyErrorOut,"error");
      return(-1);
    }
    /** printf("(3)"); fflush(NULL); */
    return(oxclientMultiSelect(clients, dataready, controlready,size,0));
  }
}

int oxGetControl(oxclientp client) 
     /* synchronized. */
{
  int ans;
  ox_stream os;
  switch (client->cstate) {
  case 1:
    ans = oxGetResultOfControlInt32(client->controlfd);
    client->cstate = 0;
    return(ans);
  default:
    fprintf(MyErrorOut,"oxGet: unknown cstate.\n");
    client->cstate = -1;
    return(-1);
  }
  
  return(-1);
}

int oxInitClient(oxclientp client)
{
  client->datafp2 = NULL;
  client->dataport = 0;
  client->controlport = 0;
  client->controlfd = 0;
  client->humanio = 0;
  client->dstate = 0;
  client->cstate = 0;
  client->id = -1;
  client->mathcapObjp = NULL;
  client->controlByteOrder = OX_BYTE_NETWORK_BYTE_ORDER;
  client->engineByteOrder = OX_BYTE_NETWORK_BYTE_ORDER;
  client->engineID = -1;
  return(0);
}

int oxIsThereErrorClient(oxclientp client) {
  if (client == NULL) return(1);
  if (client->dstate == -1) return(1);
  if (client->cstate == -1) return(1);
  return(0);
}

oxclientp oxCreateClient(char *sname,int portStream,int portControl,
                         char *passControl, char *passData)
     /* you also need to change oxCreateClient2. */
{
  int v = 0;
  int fdControl = -1;
  int fdStream = -1;
  oxclientp client;
  int controlByteOrder, engineByteOrder;
  v = !Quiet;
  if (portControl != -1) {
    fdControl = socketConnect(sname,portControl);
    if (v) fprintf(stderr,"\nControl port %d : Connected.\n",portControl);
  }
  if (portStream != -1) {
    sleep(1); /* wait */
    fdStream = socketConnect(sname,portStream);
    if (v) fprintf(stderr,"\nStream port %d : Connected.\n",portStream);
  }

  if (fdStream == -1 || fdControl == -1) {
    fprintf(stderr,"\nOpen error in oxCreateClient.\n");
    return(NULL);
  }

  if (passControl != NULL) {
    if (v) fprintf(stderr,"Sending password %s for the control channel.\n",
                   passControl);
    if (write(fdControl,passControl,strlen(passControl)+1) < 0) {
      fprintf(stderr,"oxCreateClient(): failed to send passControl.\n");
      return(NULL);
    }
  }
  if (passData != NULL) {
    if (v) fprintf(stderr,"Sending password %s for the data channel.\n",
                   passData);
    if (write(fdStream,passData,strlen(passData)+1) < 0) {
      fprintf(stderr,"oxCreateClient(): failed to send passData.\n");
      return(NULL);
    }
  }

  controlByteOrder = oxSetByteOrder(fdControl);
  if (v) fprintf(stderr,"Byte order for control process is %s.\n",
                 (controlByteOrder == 0? "network byte order":
                  (controlByteOrder == 1? "little indican":
                   "big indian")));
  engineByteOrder = oxSetByteOrder(fdStream);
  if (v) fprintf(stderr,"Byte order for engine process is %s.\n",
                 (engineByteOrder == 0? "network byte order":
                  (engineByteOrder == 1? "little indican":
                   "big indian")));
  
  client = (oxclientp) mymalloc(sizeof(oxclient));
  oxInitClient(client);
  client->datafp2 = fp2open(fdStream);
  if (client->datafp2 == NULL) {
    fprintf(stderr,"oxCreateClient(): fp2open(fd) failed.\n");
    return(NULL);
  }
  client->dataport = portStream;
  client->controlport = portControl;
  client->controlfd = fdControl;
  client->id = oxGetClientID();
  client->type = CLIENT_SOCKET; /* socket */
  client->engineByteOrder = engineByteOrder;
  client->controlByteOrder = controlByteOrder;
  return(client);
}

oxclientp oxCreateClientFile(char *fname,char *mode,char *controlName,char *cmode)
{
  static int clnum = 0x8000;
  int v = 0;
  int fdControl = -1;
  int fdStream = -1;
  oxclientp client;
  v = 1;
  if (strcmp(mode,"w") == 0) {
    fdStream = creat(fname,S_IWRITE|S_IREAD|S_IRGRP|S_IROTH);
    if (fdStream < 0) {
      fprintf(stderr,"\nCreat failed for %s\n",fname); return(NULL);
    }
  }else if (strcmp(mode,"r")==0) {
    fdStream = open(fname,O_RDONLY);
    if (fdStream < 0) {
      fprintf(stderr,"\nOpen failed for %s\n",fname); return(NULL);
    }
  }else {
    fprintf(stderr,"\nThe mode %s is not supported.\n",mode); return(NULL);
  }

  if (strcmp(cmode,"w") == 0) {
    fdControl = creat(controlName,S_IWRITE|S_IREAD|S_IRGRP|S_IROTH);
    if (fdControl < 0) {
      fprintf(stderr,"\nCreat failed for %s\n",controlName); return(NULL);
    }
  }else if (strcmp(cmode,"r")==0) {
    fdControl = open(controlName,O_RDONLY);
    if (fdControl < 0) {
      fprintf(stderr,"\nOpen failed for %s\n",controlName); return(NULL);
    }
  }else if (strcmp(cmode,"rw")==0) {
    fdControl = open(controlName,O_RDWR);
    if (fdControl < 0) {
      fprintf(stderr,"\nOpen failed for %s\n",controlName); return(NULL);
    }
  }else {
    fprintf(stderr,"\nThe mode %s is not supported.\n",cmode); return(NULL);
  }


  client = (oxclientp) mymalloc(sizeof(oxclient));
  oxInitClient(client);
  client->datafp2 = fp2open(fdStream);
  if (client->datafp2 == NULL) {
    fprintf(stderr,"oxCreateClientFile(): fp2open(fd) failed.\n");
    return(NULL);
  }
  client->dataport = 0;
  client->controlport = 0;
  client->controlfd = fdControl;
  client->id = clnum; clnum++;
  client->type = CLIENT_FILE;
  client->engineByteOrder = OX_BYTE_NETWORK_BYTE_ORDER;
  client->controlByteOrder = OX_BYTE_NETWORK_BYTE_ORDER;
  return(client);
}

void oxSendOXheader_generic(int type,int fd,ox_stream ox,
                            int k,int serial)
{
  static int ss = 0;
  extern int UseOXPacketSerial;
  if (serial >= 0) ss = serial;
  else ss++;
  if (ss < 0) ss=0;
  if (type == 0) { /* fd */
    oxfdSendInt32(fd,k);
    if (UseOXPacketSerial) oxfdSendInt32(fd,ss);
  }else {
    oxSendInt32(ox,k);
    if (UseOXPacketSerial) oxSendInt32(ox,ss);
  }
}
void oxSendOXheader(ox_stream ostream,int k,int serial) {
  oxSendOXheader_generic(1,-1,ostream,k,serial);
}
void oxfdSendOXheader(int fd,int k,int serial) {
  oxSendOXheader_generic(0,fd,(ox_stream) NULL,k,serial);
}

int oxfdGetOXheader(int fd,int *sss)
{
  char d[4];
  int i;
  int m;

  for (i=0; i<4; i++) {
    d[i] = readOneByte(fd);
  }
  m = ntohl(* ( (int *)d));
  *sss = -1;
  if (UseOXPacketSerial) {
    for (i=0; i<4; i++) {
      d[i] = readOneByte(fd);
    }
    *sss = ntohl(* ( (int *)d));
  }
  return(m);
}

int oxGetOXheader(ox_stream ostream,int *sss)
{
  char d[4];
  int m;
  int i;

  unlockCtrlCForOx();  /* temporary unlock */
  while (fp2select(ostream,1000) == 0) ;
  restoreLockCtrlCForOx(); 

  for (i=0; i<4; i++) {
    d[i] = fp2fgetc(ostream);
  }
  m = ntohl(* ( (int *)d));
  *sss = -1;
  if (UseOXPacketSerial) {
    for (i=0; i<4; i++) {
      d[i] = fp2fgetc(ostream);
    }
    *sss = ntohl(* ( (int *)d));
  }
  return(m);
}


int oxWritePortFile(int func,int port,char *fname) {
  char name[1024];
  FILE *fp;
  strcpy(name,fname);
  if (func == 0) {
    strcat(name,".control");
    fp = fopen(name,"w");
    fprintf(fp,"%05d\n",port);
    fclose(fp);
  }else {
    strcat(name,".data");
    fp = fopen(name,"w");
    fprintf(fp,"%05d\n",port);
    fclose(fp);
  }
}
int oxReadPortFile(int func,char *fname) {
  int port = 0;
  char name[1024];
  FILE *fp;
  strcpy(name,fname);
  if (func == 0) {
    strcat(name,".control");
    fp = fopen(name,"r");
    {int r; r=fscanf(fp,"%d",&port);}
    fclose(fp);
  }else {
    strcat(name,".data");
    fp = fopen(name,"r");
    {int r; r=fscanf(fp,"%d",&port);}
    fclose(fp);
  }
  return(port);
}
char *oxGenPortFile(void) {
  char *fname;
  time_t tt;
  char sstime[512];

  fname = (char *)malloc(1024*sizeof(char));
  strcpy(fname,getenv("HOME"));
  strcat(fname,"/.ox.");
  tt = time(NULL);
  sprintf(sstime,"%ld",(long) tt);
  strcat(fname,sstime);
  if (fname[strlen(fname)-1] == '\n') {
    fname[strlen(fname)-1] = '\0';
  }
  /* fprintf(stderr,"OxPortFileName=%s\n",fname); */
  OxPortFileName = fname;
  return(fname);
}
int oxRemovePortFile(void) {
  char fname[1024];
  FILE *fp;
  SET_MYERROROUT;
  strcpy(fname,OxPortFileName);
  strcat(fname,".control");
  if ((fp=fopen(fname,"r")) == NULL) {
  }{
    fclose(fp);
    if (unlink(fname)) {
      fprintf(MyErrorOut,"fail unlink.\n");
    }

  }
  strcpy(fname,OxPortFileName);
  strcat(fname,".data");
  if ((fp=fopen(fname,"r")) == NULL) {
  }{
    fclose(fp);
    if (unlink(fname)) {
      fprintf(MyErrorOut,"fail unlink.\n");
    }
  }
}
  
char *oxGenPass(void) {
  static int seed = 0;
  long p;
  char *s;
  int i,n;
  if (seed == 0) {
    seed = (int) time(NULL) + (int) ((long) &p);
    srandom((unsigned int) seed);
  }
  s = (char *)malloc(128*sizeof(char));
  if (s == NULL) { fprintf(stderr,"No more memory.\n"); return(s); }
  n = (((int)((long) s)) + (int) time(NULL)) % 100;
  for (i=0; i < n ; i++) random();
  p = random();
  sprintf(s,"%ld",p);
  return(s);
}


static void cancelConnection() {
#if defined(__CYGWIN__)
  extern sigjmp_buf MyEnv_oxmisc;
#else
  extern jmp_buf MyEnv_oxmisc;
#endif
  mysignal(SIGALRM,SIG_IGN);
  fprintf(stderr,"Time out in TCP/IP connection.\n");
#if defined(__CYGWIN__)
  MYSIGLONGJMP(MyEnv_oxmisc,1);
#else
  MYLONGJMP(MyEnv_oxmisc,1);
#endif
}

oxclientp oxCreateClient2(int fdstream,int portStream,
                          int fdcontrol,int portControl,int ipmask,
                          char *passControl, char *passData)
{
  int v = 0;
  int fdControl = -1;
  int fdStream = -1;
  int m;

  char *s;
  oxclientp client;
#if defined(__CYGWIN__)
  extern sigjmp_buf MyEnv_oxmisc;
#else
  extern jmp_buf MyEnv_oxmisc;
#endif
  int controlByteOrder, engineByteOrder;

  v = !Quiet;
#if defined(__CYGWIN__)
  if (MYSIGSETJMP(MyEnv_oxmisc,1)) {
#else
  if (MYSETJMP(MyEnv_oxmisc)) {
#endif
    return(NULL);
  }else{
  }
  alarm((unsigned int) 20);  /* setup timeout. */
  mysignal(SIGALRM,cancelConnection);
  
  switch(ipmask) {
  case 0:/* only local */
    fdControl = socketAcceptLocal(fdcontrol);
    fdStream  = socketAcceptLocal(fdstream);
    break;
  default:/* any */
    fdControl = socketAccept(fdcontrol);
    fdStream  = socketAccept(fdstream);
    break;
  }
  if (v) fprintf(stderr,"\nControl port %d : Connected.\n",portControl);
  if (v) fprintf(stderr,"\nStream port %d : Connected.\n",portStream);

  if (fdStream == -1 || fdControl == -1) {
    fprintf(stderr,"\nOpen error in oxCreateClient2.\n");
    fprintf(stderr,"fdStream=%d, fdControl=%d\n",fdStream,fdControl);
    return(NULL);
  }

  /* Authentication by password. */
  m = strlen(passControl)+strlen(passData);
  if (m > 0) {
    s = (char *)mymalloc(sizeof(char)*(m+1));
    m = strlen(passControl); s[0] = 0;
    {int r; r=read(fdControl,s,m+1);} s[m] = '\0';
    if (strcmp(s,passControl) != 0) {
      fprintf(stderr,"s=%s, passControl=%s\n",s,passControl);
      fprintf(stderr,"oxCreateClient2(): password authentication failed for control channel.\n");
      close(fdControl);
      return(NULL);
    }
    m = strlen(passData); s[0] = 0;
    {int r; r=read(fdStream,s,m+1);} s[m] = '\0';
    if (strcmp(s,passData) != 0) {
      fprintf(stderr,"s=%s, passData=%s\n",s,passData);
      fprintf(stderr,"oxCreateClient2(): password authentication failed for data channel.\n");
      close(fdStream);
      return(NULL);
    }
  }
  mysignal(SIGALRM,SIG_IGN);

  controlByteOrder = oxSetByteOrder(fdControl);
  if (v) fprintf(stderr,"Byte order for control process is %s.\n",
                 (controlByteOrder == 0? "network byte order":
                  (controlByteOrder == 1? "little indican":
                   "big indian")));
  engineByteOrder = oxSetByteOrder(fdStream);
  if (v) fprintf(stderr,"Byte order for engine process is %s.\n",
                 (engineByteOrder == 0? "network byte order":
                  (engineByteOrder == 1? "little indican":
                   "big indian")));
  

  client = (oxclientp) mymalloc(sizeof(oxclient));
  oxInitClient(client);
  client->datafp2 = fp2open(fdStream);
  if (client->datafp2 == NULL) {
    fprintf(stderr,"oxCreateClient2(): fp2open(fd) failed.\n");
    return(NULL);
  }
  client->dataport = portStream;
  client->controlport = portControl;
  client->controlfd = fdControl;
  client->id = oxGetClientID();
  client->type = CLIENT_SOCKET; /* socket */
  client->engineByteOrder = engineByteOrder;
  client->controlByteOrder = controlByteOrder;
  return(client);
}

int oxSetByteOrder(int fd) {
  char data[1];
  int peertype;
  /* It is for client. read and next write. */
  /* oxSocketSelect0(fd,10);  wait. */
  {int r; r=read(fd,data,1);}
  peertype = (unsigned char) data[0];

  /* We support only Network byte order */
  data[0] = OX_BYTE_NETWORK_BYTE_ORDER;
  {int r; r=write(fd,data,1);}

  return(OX_BYTE_NETWORK_BYTE_ORDER);
}

int oxTellMyByteOrder(int fdOut, int fdIn) {  
  char data[1];
  int peertype;
  /* It is for server. read and next write. */

  /* We support only Network byte order */
  data[0] = OX_BYTE_NETWORK_BYTE_ORDER;
  {int r; r=write(fdOut,data,1);}
  fsync(fdOut);  /* returns 0 if normal. Does it work for socket? */

  {int r; r=read(fdIn,data,1);} /* Read pear's byte order */

  return(OX_BYTE_NETWORK_BYTE_ORDER);
}


struct object OxClientList[MAX_N_OF_CLIENT];
int OxClientListn = 0;

int oxGetClientID() {
  extern struct object OxClientList[];
  extern int OxClientListn;
  extern struct object Nobj;
  int i;
  for (i=0; i<OxClientListn; i++) {
	if ((OxClientList[i]).tag == Snull) {
	  return i;
	}
  }
  i = OxClientListn;
  (OxClientList[i]).tag = Snull;
  if (OxClientListn < MAX_N_OF_CLIENT-1) {
	OxClientListn++;
	return i;
  }else{
	fprintf(MyErrorOut,"oxGetClientID(): the client table is full. Returns ID = 0.\n");
	return 0;
  }
}

char *oxFIDtoStr(int id) {
  switch( id ) {
  case SM_mathcap:
    return "SM_mathcap"; break;
  case SM_setMathCap:
    return "SM_setMathCap"; break;
  case SM_pops:
    return "SM_pops"; break;
  case SM_getsp:
    return "SM_getsp"; break;
  case SM_dupErrors:
    return "SM_dupErrors"; break;
  case SM_pushCMOtag:
    return "SM_pushCMOtag"; break;
  case SM_setName:
    return "SM_setName"; break;
  case SM_evalName:
    return "SM_evalName"; break;
  case SM_executeStringByLocalParser:
    return "SM_executeStringByLocalParser"; break;
  case SM_executeFunction:
    return "SM_executeFunction"; break;
  case SM_executeFunctionWithOptionalArgument:
    return "SM_executeFunctionWithOptionalArgument"; break;
  case SM_popCMO:
    return "SM_popCMO"; break;
  case SM_popString:
    return "SM_popString"; break;
  case SM_shutdown:
    return "SM_shutdown"; break;
  case SM_beginBlock:
    return "SM_beginBlock"; break;
  case SM_endBlock:
    return "SM_endBlock"; break;
  default:
    return "Unknown to oxFIDtoStr"; break;
  }
}
