/*  $OpenXM: OpenXM/src/kan96xx/plugin/mytcpio.c,v 1.16 2005/07/03 11:08:54 ohara Exp $ */
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <setjmp.h>
#include <errno.h>
#include <unistd.h>
/* -lnsl -lsocket /usr/ucblib/libucb.a */
#include "ox_kan.h"
/* 
   MyEnv_oxmisc is used in oxmisc.c.
   It cannot be put in oxmisc.c for the case of sun.
*/
#if defined(sun)
int MyEnv_oxmisc[2000];
#else
#if defined(__CYGWIN__)
sigjmp_buf MyEnv_oxmisc; /* may cause a trouble in Solaris. */
#else
jmp_buf MyEnv_oxmisc; /* may cause a trouble in Solaris. */
#endif
#endif

#define READBUFSIZE 10000
#define MAX_LISTEN_QUEUE 3
static void errorMsg1s(char *s) {
  fprintf(stderr,"%s\n",s);
}

#define SET_TCPIOERROR  { if (TcpioError == NULL) TcpioError = stdout; }
FILE *TcpioError = NULL;
int OpenedSocket = 0;
extern int Quiet;

int socketOpen(char *serverName,int portNumber) {
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

  if (listen(s_waiting,MAX_LISTEN_QUEUE) < 0) {
    errorMsg1s("Listen failed");
    return(-1);
  }
  if (!Quiet) fprintf(TcpioError,"Done the initialization. port =%d\n",ntohs(me.sin_port));
  OpenedSocket = ntohs(me.sin_port);
  return(s_waiting);
}


int socketAccept(int snum) {
  int s, news;
    
  SET_TCPIOERROR;
  s = snum;
  if (!Quiet) {fprintf(TcpioError,"Trying to accept... "); fflush(TcpioError);}
  if ((news = accept(s,NULL,NULL)) < 0) {
    errorMsg1s("Error in accept. Retrying (socketAccept).");
    /* Code added for strange behavior on cygwin. */
    if ((news = accept(s,NULL,NULL)) < 0) {
      errorMsg1s("Error in accept. Retry failed.");
      return (-1);
    }
  }
  if (!Quiet) {fprintf(TcpioError,"Accepted.\n"); fflush(TcpioError);}
  if (close(s) < 0) {
    errorMsg1s("Error in closing the old socket.");
    return(-1);
  }
  return(news);
}

int socketAcceptLocal(int snum) {
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

/* It does not close the socket for listening. */
int socketAcceptLocal2(int snum) {
  int s, news;
  struct sockaddr peer;
  int len;
  int i;

  SET_TCPIOERROR;
  s = snum;
  if (!Quiet) {fprintf(TcpioError,"Trying to accept from localhost... "); fflush(TcpioError);}
  len = sizeof(struct sockaddr);
  if ((news = accept(s,&peer,&len)) < 0) {
    errorMsg1s("Error in accept. Retrying (socketAcceptLocal2).");
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
    fprintf(stderr,"The connection is refused.");
    return(-1);
  }

  if (!Quiet) {fprintf(TcpioError,"Accepted.\n"); fflush(TcpioError);}
  return(news);
}

int oxSocketSelect0(int fd,int t) {
  fd_set readfds;
  struct timeval timeout;
  int debug = 0;
  SET_TCPIOERROR;
  FD_ZERO(&readfds);
  FD_SET(fd,&readfds);
  timeout.tv_sec = 0;
  timeout.tv_usec = (long) t;
  if (t >= 0) {
    if (debug) {printf("select t>=0 for fd = %d\n",fd); fflush(NULL);}
    if (select(fd+1,&readfds,(fd_set *)NULL,(fd_set *)NULL,&timeout)<0) {
      /* It must be fd+1 !,  Not fd. */
      fprintf(TcpioError,"select (non-block) error. Error no is %d. See /usr/include/sys/errno.h (asm/errno.h).\n",errno);
      errorMsg1s("oxSocketSelect0() : select failed.");
      return(0);
    }
    if(debug){printf("Return from select t>=0 for fd = %d\n",fd);fflush(NULL);}
  }else{ /* block */
    if (select(fd+1,&readfds,(fd_set *)NULL,(fd_set *)NULL,(struct timeval *)NULL)<0) {
      fprintf(TcpioError,"select (block) error. Error no is %d. See /usr/include/sys/errno.h (asm/errno.h).\n",errno);
      errorMsg1s("socketSelect0() : select failed.");
      return(0);
    }
  }
  if (FD_ISSET(fd,&readfds)) return(1);
  else return(0);
}

int oxSocketMultiSelect(int sid[],int size,int t,int result[])
{
  int i,fd,p;
  fd_set readfds;
  struct timeval timeout;
  int isdata = 0;

  SET_TCPIOERROR;
  FD_ZERO(&readfds);
  timeout.tv_sec = 0;
  timeout.tv_usec = (long)t;

  fd = 0;

  for (i=0; i<size; i++) {
    if (sid[i] >= 0) {
      p = sid[i];
      if (p > fd) fd = p;
      FD_SET(p,&readfds);
      /* printf("p = %d, fd=%d",p,fd); */
    }
  }

  /* printf("MultiSelect..\n"); fflush(NULL); */
  if (t >= 0) {
    if (select(fd+1,&readfds,(fd_set *)NULL,(fd_set *)NULL,&timeout)<0) {
      /* It must be fd+1 !,  Not fd. */
      fprintf(TcpioError,"Select error. Error no is %d. See /usr/include/sys/errno.h (asm/errno.h).\n",errno);
      errorMsg1s("oxSocketMultiSelect() : select failed.");
      return(0);
    }
  }else{ /* block */
    if (select(fd+1,&readfds,(fd_set *)NULL,(fd_set *)NULL,(struct timeval *)NULL)<0) {
      fprintf(TcpioError,"Select error. Error no is %d. See /usr/include/sys/errno.h (asm/errno.h).\n",errno);
      errorMsg1s("oxSocketMultiSelect() : (block) select failed.");
      return(0);
    }
  }
  /* printf("Done. MultiSelect.\n"); fflush(NULL); */
  for (i=0; i<size; i++) {
    p = sid[i];
    if ((sid[i] >=0) && FD_ISSET(p,&readfds)) {
      result[i] = 1; isdata = 1;
    }else{
      result[i] = 0;
    }
  }
  return(isdata);
}


int socketConnect(char *serverName,int portNumber) {
  struct hostent *servhost;
  struct sockaddr_in server;
  int socketid;
  int on;

  SET_TCPIOERROR;
  if ((servhost = gethostbyname(serverName)) == NULL) {
    errorMsg1s("bad server name.\n");
    return(-1);
  }
  bzero((char *)&server,sizeof(server));
  server.sin_family = AF_INET;
  server.sin_port = htons(portNumber);
  bcopy(servhost->h_addr,
        (char *)&server.sin_addr,servhost->h_length);

  if ((socketid = socket(AF_INET,SOCK_STREAM,0)) <0) {
    errorMsg1s("socket allocation is failed.\n");
    return(-1);
  }
  /* on=1; setsockopt(socketid,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));  */
  if (!Quiet) {
    fprintf(TcpioError,"Trying to connect port %d, ip=%lx\n",ntohs(server.sin_port),(long) server.sin_addr.s_addr);
  }
  if (connect(socketid,(struct sockaddr *)&server,sizeof(server)) == -1) {
    errorMsg1s("cannot connect");
    return(-1);
  }
  if (!Quiet) fprintf(TcpioError,"connected.\n");
  return(socketid);
}

int socketConnectWithPass(char *servername,int port,char *pass)
{
  int fd;
  int m;
  SET_TCPIOERROR;
  fd = socketConnect(servername,port);
  if ((pass == NULL) && (fd >= 0)) return fd;
  if ((pass == NULL) && (fd < 0)) return -1;
  if (fd >= 0) {
    m = write(fd,pass,strlen(pass)+1);
    if (m != strlen(pass)+1) {
      fprintf(TcpioError,"Fail to send password to fd=%d.\n",fd);
      return(-1);
    }
    return(fd);
  }else{
    return(-1);
  }
}
  
