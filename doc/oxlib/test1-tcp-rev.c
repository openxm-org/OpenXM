/* $OpenXM: OpenXM/doc/oxlib/test1-tcp-rev.c,v 1.1 2005/11/12 02:38:01 takayama Exp $
/* A sample code to explain how to use ox_asir by TCP/IP and
   OpenXM control protocol. ox_asir is called by the -revese option.
   It computes the gcd of 12 and 8 by calling ox_asir server.
   */

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>

#define OX_COMMAND          513
#define OX_DATA             514
#define SM_executeFunction  269
#define SM_popString        263
#define CMO_STRING            4

static int Serial = 0;

main() {
  int fdControl = -1;
  int fdStream = -1;
  int portControl;
  int portStream;
  int m;
  char com[1024];
  char *s;
  int controlByteOrder=0;
  int engineByteOrder=0;
  char *passControl = "1111";
  char *passData = "2222";
  int v = 1;

  fdControl = socketOpen("localhost",0,&portControl);
  fdStream = socketOpen("localhost",0,&portStream);

  /* Starting the ox_asir server */
  sprintf(com,"../../bin/ox -ox ../../bin/ox_asir -reverse -control %d -data %d -passControl 1111 -passData 2222 &",portControl,portStream);
  fprintf(stderr,"system: %s\n",com);
  system(com); 


  fdControl = socketAcceptLocal(fdControl);
  fdStream  = socketAcceptLocal(fdStream);

  fprintf(stderr,"\nControl fd %d : Connected.\n",fdControl);
  fprintf(stderr,"\nStream fd %d : Connected.\n",fdStream);

  if (fdStream == -1 || fdControl == -1) {
    fprintf(stderr,"\nOpen error in oxCreateClient2.\n");
    fprintf(stderr,"fdStream=%d, fdControl=%d\n",fdStream,fdControl);
    return(0);
  }

  /* Authentication by password. */
  m = strlen(passControl)+strlen(passData);
  if (m > 0) {
    s = (char *)malloc(sizeof(char)*(m+1));
    m = strlen(passControl); s[0] = 0;
    read(fdControl,s,m+1); s[m] = '\0';
    if (strcmp(s,passControl) != 0) {
      fprintf(stderr,"s=%s, passControl=%s\n",s,passControl);
      fprintf(stderr,"password authentication failed for control channel.\n");
      close(fdControl);
      return(0);
    }
    m = strlen(passData); s[0] = 0;
    read(fdStream,s,m+1); s[m] = '\0';
    if (strcmp(s,passData) != 0) {
      fprintf(stderr,"s=%s, passData=%s\n",s,passData);
      fprintf(stderr,"password authentication failed for data channel.\n");
      close(fdStream);
      return(0);
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
  

  hoge(fdStream);
  fprintf(stderr,"Type in ctrl-C"); fflush(NULL);
  sleep(1000);
}

ox_push_cmo(int dataPort,char *cmo,int length) {
  extern int Serial;
  char oxtag[4];
  char oxserial[4];
  *((int *)oxtag) = (int) htonl(OX_DATA);
  *((int *)oxserial) = (int) htonl(Serial++);
  write(dataPort,oxtag,4);
  write(dataPort,oxserial,4);
  write(dataPort,cmo,length);
  fflush(NULL);
}

ox_push_cmd(int dataPort, int cmd) { 
  extern int Serial;
  char oxtag[4];
  char oxserial[4];
  char oxcmd[4];
  *((int *)oxtag) = (int) htonl(OX_COMMAND);
  *((int *)oxserial) = (int) htonl(Serial++);
  *((int *)oxcmd) = (int) htonl(cmd);
  write(dataPort,oxtag,4);
  write(dataPort,oxserial,4);
  write(dataPort,oxcmd,4);
  fflush(NULL);
}

ox_pop_int32(int dataPort) {
  char s[4];
  read(dataPort,s,4);
  return((int) ntohl( *((int *)s)));
}

ox_pop_string(int dataPort,char *buf,int limit) {
  int oxtag, serial, cmotag, length ;
  
  ox_push_cmd(dataPort,SM_popString);  
  oxtag = ox_pop_int32(dataPort);  /* It must be CMO_DATA */
  serial = ox_pop_int32(dataPort); /* Read the serial number */
  cmotag = ox_pop_int32(dataPort); /* Read the CMO tag */
  if (cmotag != CMO_STRING) {
	fprintf(stderr,"The tag should be CMO_STRING.\n");
	exit(1);
  }
  length = ox_pop_int32(dataPort);
  if (length > limit-1) {
	fprintf(stderr,"Too long string to read.\n");
	exit(1);
  }
  read(dataPort,buf,length);
  buf[length]=0;
}

#define SIZE 1024
hoge(int dataPort) {
  char s[SIZE];
  /*  (CMO_ZZ,12); */
  unsigned char cmo0[]=
  {00, 00, 00, 0x14,
   00, 00, 00, 01, 00, 00, 00, 0xc};

  /* (CMO_ZZ,8) */
  unsigned char cmo1[] =
  {00, 00, 00, 0x14,
   00, 00, 00, 01, 00, 00, 00, 8};  

  /* (CMO_INT32,2); */
  unsigned char cmo2[] =
  { 00, 00, 00, 02, 00, 00, 00, 02}; 

  /* (CMO_STRING,"igcd") */
  unsigned char cmo3[] =
  {00, 00, 00, 04, 00, 00, 00, 04,
   0x69,0x67,0x63,0x64 };


  ox_push_cmo(dataPort,cmo0,12);
  ox_push_cmo(dataPort,cmo1,12);
  ox_push_cmo(dataPort,cmo2,8);
  ox_push_cmo(dataPort,cmo3,12);

  ox_push_cmd(dataPort,SM_executeFunction);  
  ox_pop_string(dataPort,s,SIZE);

  printf("gcd of 12 and 8, in the string format, is \n");
  printf("%s",s);
  printf("\n");
}


#define SET_TCPIOERROR  { if (TcpioError == NULL) TcpioError = stdout; }
FILE *TcpioError = NULL;
int Quiet = 0;
errorMsg1s(char *s) { fprintf(stderr,"%s\n",s); }
socketAcceptLocal(int snum) {
  int s, news;
  struct sockaddr peer;
  int len;
  int i;

  SET_TCPIOERROR;
  s = snum;
  if (!Quiet) {fprintf(TcpioError,"Trying to accept from localhost... "); fflush(TcpioError);}
  len = sizeof(struct sockaddr);
  if ((news = accept(s,&peer,&len)) < 0) {
    errorMsg1s("Error in accept. Retrying");
    /* Code added for strange behavior on cygwin. */
   if ((news = accept(s,&peer,&len)) < 0) {
      errorMsg1s("Error in accept. Retry failed.");
      return (-1);
   }
  }

  len = sizeof(struct sockaddr);
  getpeername(news,&peer,&len);
  if (!Quiet) printf("len= %d\n",len);
  for (i=0; i<len; i++) {
    if (!Quiet) printf(" %x ",peer.sa_data[i]);
  }
  if (!Quiet) printf("\n");
  if (peer.sa_data[2] == 0x7f && peer.sa_data[3] == 0 &&
      peer.sa_data[4] == 0    && peer.sa_data[5] == 1) {
    if (!Quiet) fprintf(stderr,"Authentication: localhost is allowed to be accepted.\n");
  }else{
    errorMsg1s("Authentication: The connection is not from the localhost.");
    close(s);
    fprintf(stderr,"The connection is refused.");
    return(-1);
  }

  if (!Quiet) {fprintf(TcpioError,"Accepted.\n"); fflush(TcpioError);}
  if (close(s) < 0) {
    errorMsg1s("Error in closing the old socket.");
    return(-1);
  }
  return(news);
}

socketOpen(char *serverName,int portNumber,int *portNumberp) {
  static struct hostent *myhost;
  static struct sockaddr_in me;
  static int s_waiting;
  static int on;
  int tt;

  SET_TCPIOERROR;
  if (!Quiet) fprintf(TcpioError,"Hello from open. serverName is %s and portnumber is %d\n",
          serverName,portNumber);
  if ((myhost = gethostbyname(serverName)) == NULL) {
    errorMsg1s("Bad server name.");
    return(-1);
  }
  bzero((char *)&me,sizeof(me));
  me.sin_family = AF_INET;
  me.sin_port = htons(portNumber);
  bcopy(myhost->h_addr,
        &me.sin_addr,myhost->h_length);

  if ((s_waiting = socket(AF_INET,SOCK_STREAM,0)) < 0) {
    errorMsg1s("Socket allocation is failed.");
    return(-1);
  }

  on=1; setsockopt(s_waiting,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on)); 
  /* important */
  if ((tt = bind(s_waiting,(struct sockaddr *) &me,sizeof(me))) == -1) {
    fprintf(TcpioError,"Bind error. Error no is %d. See /usr/include/sys/errno.h. (asm/errno.h)\n",errno);
    errorMsg1s("cannot bind");
    return(-1);
  }
  /* printf("bind returns %d\n",tt); */
  tt = sizeof(me);
  if (getsockname(s_waiting,(struct sockaddr *)&me,&tt) < 0) {
    fprintf(TcpioError,"getsockname error. Error no is %d. See /usr/include/sys/errno.h (asm/errno.h).\n",errno);
    errorMsg1s("cannot getsockname");
    return(-1);
  }

  if (listen(s_waiting,16) < 0) {
    errorMsg1s("Listen failed");
    return(-1);
  }
  if (!Quiet) fprintf(TcpioError,"Done the initialization. port =%d\n",ntohs(me.sin_port));
  *portNumberp = ntohs(me.sin_port);
  return(s_waiting);
}

int oxSetByteOrder(int fd) {
  char data[1];
  int peertype;
  read(fd,data,1);
  peertype = (unsigned char) data[0];

  /* We support only Network byte order */
  data[0] = 0;
  write(fd,data,1);

  return(0);
}

