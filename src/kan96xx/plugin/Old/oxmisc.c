#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
FILE *MyErrorOut = stdout;
#include "ox_kan.h"

#define READBUFSIZE 5000

int UseOXPacketSerial = 0;


int readOneByte(int fd)   /* blocking */
{
  static char data[1];
  int size;
  int ans;
  int watch = 1;

  if (fd < 0) {
    fprintf(MyErrorOut,"readOneByte fd < 0 ??? .\n");
    return(-1);
  }

  if (oxSocketSelect0(fd,-1)) { /* block */
    size = read(fd,data,1);
    if (size == 0) {
      fprintf(MyErrorOut,"oxSocketSelect0() returns 1, but there is no data. You peer may be killed.\n");
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
	fprintf(MyErrorOut,"oxSocketSelect0() returns 1, but there is no data. You peer may be killed.\n");
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
  /*
  char data[4*2];
  *((int *)&data[0]) = htonl(OX_SYNC_BALL);
  write(fd,data,4);
  fflush((FILE *)NULL);
  */
}

void oxSendSyncBall(ox_stream os)
{
  char data[4*2];
  *((int *)&data[0]) = htonl(OX_SYNC_BALL);
  fp2write(os,data,4);
  fp2fflush(os);
}

int oxWaitSyncBall(ox_stream ostream)
{
  int mtag;
  char data[4];
  int c;
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
  write(fd,data,4);
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
  write(fd,data,4);
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
  write(fd,data,4*2);
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
  write(fd,data,4*2);
  if (s != NULL) {
    write(fd,s,n);
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
  oxSendInt32(os,OX_DATA);
  oxSendCmoString(os,s);
}

void oxSendResultOfControlInt32(int fd,int i) 
{
  char data[4*3];
  *((int *)&data[0]) = htonl(OX_DATA);
  *((int *)&data[4]) = htonl(CMO_INT32);
  *((int *)&data[8]) = htonl(i);
  write(fd,data,4*3);
  fflush((FILE *)NULL);
}
  
void oxSendResultOfControl(int fd) 
{
  char data[4*2];
  *((int *)&data[0]) = htonl(OX_DATA);
  *((int *)&data[4]) = htonl(CMO_NULL);
  write(fd,data,4*2);
  fflush((FILE *)NULL);
}

void oxReqMathCap(ox_stream os) {
  oxSendInt32(os,OX_COMMAND);
  oxSendInt32(os,SM_mathcap);
  fp2fflush(os);
}
void oxReqPops(ox_stream os,int n) { 
  oxSendInt32(os,OX_DATA);
  oxSendCmoInt32(os,n);
  oxSendInt32(os,OX_COMMAND);
  oxSendInt32(os,SM_pops);
  fp2fflush(os);
}
void oxReqSetName(ox_stream os,char *s) {
  oxSendInt32(os,OX_DATA);
  oxSendCmoString(os,s);
  oxSendInt32(os,OX_COMMAND);
  oxSendInt32(os,SM_setName);
  fp2fflush(os);
}
void oxReqEvalName(ox_stream os,char *s) {
  oxSendInt32(os,OX_DATA);
  oxSendCmoString(os,s);
  oxSendInt32(os,OX_COMMAND);
  oxSendInt32(os,SM_evalName);
  fp2fflush(os);
}

void oxReqExecuteStringByLocalParser(ox_stream os,char *s)
{
  oxSendInt32(os,OX_DATA);
  oxSendCmoString(os,s);
  oxSendInt32(os,OX_COMMAND);
  oxSendInt32(os,SM_executeStringByLocalParser);
  fp2fflush(os);
}

void oxReqExecuteFunction(ox_stream os,char *s)
{
  oxSendInt32(os,OX_DATA);
  oxSendCmoString(os,s);
  oxSendInt32(os,OX_COMMAND);
  oxSendInt32(os,SM_executeFunction);
  fp2fflush(os);
}


void oxReqPopString(ox_stream os)
{
  oxSendInt32(os,OX_COMMAND);
  oxSendInt32(os,SM_popString);
  fp2fflush(os);
}


void oxReqControlResetConnection(int fd) {
  oxfdSendInt32(fd,OX_COMMAND);
  oxfdSendInt32(fd,SM_control_reset_connection);
  fflush(NULL);
}

void oxReqControlKill(int fd) {
  oxfdSendInt32(fd,OX_COMMAND);
  oxfdSendInt32(fd,SM_control_kill);
  fflush(NULL);
}

void oxReqPopCMO(ox_stream os) {
  oxSendInt32(os,OX_COMMAND);
  oxSendInt32(os,SM_popCMO);
  fp2fflush(os);
}


int oxGetResultOfControlInt32(int fd) {
  int k;
  k = oxfdGetInt32(fd);
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
  extern int errno;

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
      fd = (fd<clients[i]->controlfd?clients[i]->controlfd:fd);
      FD_SET(clients[i]->controlfd,&readfds);
      if (oxSocketSelect0(clients[i]->controlfd,0)) {
	ddd = controlready[i] = 1;
      }else{
	controlready[i] = 0;
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
  return(0);
}

int oxIsThereErrorClient(oxclientp client) {
  if (client == NULL) return(1);
  if (client->dstate == -1) return(1);
  if (client->cstate == -1) return(1);
  return(0);
}

oxclientp oxCreateClient(char *sname,int portStream,int portControl)
{
  static int clnum = 0;
  int v = 0;
  int fdControl = -1;
  int fdStream = -1;
  oxclientp client;
  v = 1;
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
  client->id = clnum; clnum++;
  client->type = CLIENT_SOCKET; /* socket */
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




  






