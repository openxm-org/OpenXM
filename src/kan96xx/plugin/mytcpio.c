/*  $OpenXM: OpenXM/src/kan96xx/plugin/mytcpio.c,v 1.4 2000/09/08 16:08:42 takayama Exp $ */
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <setjmp.h>
/* -lnsl -lsocket /usr/ucblib/libucb.a */
#include "ox_kan.h"
/* 
   MyEnv_oxmisc is used in oxmisc.c.
   It cannot be put in oxmisc.c for the case of sun.
*/
#if defined(sun)
int MyEnv_oxmisc[2000];
#else
jmp_buf MyEnv_oxmisc; /* may cause a trouble in Solaris. */
#endif

#define READBUFSIZE 10000
static void errorMsg1s(char *s) {
  fprintf(stderr,"%s\n",s);
}

#define SET_TCPIOERROR  { if (TcpioError == NULL) TcpioError = stdout; }
FILE *TcpioError = NULL;
int OpenedSocket = 0;
extern int Quiet;

socketOpen(char *serverName,int portNumber) {
	struct addrinfo hints, *res, *ai;
	struct sockaddr_storage ss;
	socklen_t len;
	char pstr[BUFSIZ], *errstr;
	int s, p, opt, error;

	SET_TCPIOERROR;
	fprintf(TcpioError, "Hello from open. serverName is %s and "
			    "portnumber is %d\n", serverName, portNumber);

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = PF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	memset(pstr, 0, sizeof(pstr));
	snprintf(pstr, sizeof(pstr), "%u", portNumber);

	error = getaddrinfo(serverName, pstr, &hints, &res);
	if (error) {
		errorMsg1s("Bad server name.");
		return (-1);
	}

	s = -1;
	p = -1;
	errstr = NULL;

	for (ai = res ; ai != NULL ; ai = ai->ai_next) {
		s = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
		if (s < 0) {
			errstr = "socket";
			continue;
		}

		opt = 1;
		setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

		if (bind(s, ai->ai_addr, ai->ai_addrlen) < 0) {
			errstr = "bind";
			close(s);
			s = -1;
			continue;
		}

		len = sizeof(ss);
		if (getsockname(s, (struct sockaddr *)&ss, &len) < 0) {
			errstr = "getsockname";
			close(s);
			s = -1;
			continue;
		}

		switch (ss.ss_family) {
		case AF_INET:
		{
			struct sockaddr_in *sin;
			sin = (struct sockaddr_in *)&ss;
			p = ntohs(sin->sin_port);
			break;
		}
		case AF_INET6:
		{
			struct sockaddr_in6 *sin6;
			sin6 = (struct sockaddr_in6 *)&ss;
			p = ntohs(sin6->sin6_port);
			break;
		}
		default:
			p = -1;
		}
		if (p < 0) {
			errstr = "AF";
			close(s);
			s = -1;
			continue;
		}

		if (listen(s, 1) < 0) {
			errstr = "listen";
			close(s);
			s = -1;
			continue;
		}

		break;
	}

	freeaddrinfo(res);

	if (s < 0) {
		fprintf(TcpioError, "Error: %s", errstr);
		errorMsg1s(errstr);
	} else {
		OpenedSocket = p;
	}

	return (s);
}


socketAccept(int snum) {
  int s, news;
    
  SET_TCPIOERROR;
  s = snum;
  fprintf(TcpioError,"Trying to accept... "); fflush(TcpioError);
  if ((news = accept(s,NULL,NULL)) < 0) {
    errorMsg1s("Error in accept.");
    return(-1);
  }
  fprintf(TcpioError,"Accepted.\n"); fflush(TcpioError);
  if (close(s) < 0) {
    errorMsg1s("Error in closing the old socket.");
    return(-1);
  }
  return(news);
}

socketAcceptLocal(int snum) {
  int s, news;
  struct sockaddr_storage ss;
  int len;
  int i;

  SET_TCPIOERROR;
  s = snum;
  fprintf(TcpioError,"Trying to accept from localhost... "); fflush(TcpioError);
  len = sizeof(ss);
  if ((news = accept(s, (struct sockaddr *)&ss,&len)) < 0) {
    errorMsg1s("Error in accept.");
    return(-1);
  }

  len = sizeof(ss);
  getpeername(news, (struct sockaddr *)&ss,&len);
#if 0
  printf("len= %d\n",len);
  for (i=0; i<len; i++) {
    printf(" %x ", peer.sa_data[i]);
  }
  printf("\n");
  if (peer.sa_data[2] == 0x7f && peer.sa_data[3] == 0 &&
      peer.sa_data[4] == 0    && peer.sa_data[5] == 1) {
    fprintf(stderr,"Authentication: localhost is allowed to be accepted.\n");
  }else{
    errorMsg1s("Authentication: The connection is not from the localhost.");
    close(s);
    fprintf(stderr,"The connection is refused.");
    return(-1);
  }
#endif

  fprintf(TcpioError,"Accepted.\n"); fflush(TcpioError);
  if (close(s) < 0) {
    errorMsg1s("Error in closing the old socket.");
    return(-1);
  }
  return(news);
}

int oxSocketSelect0(int fd,int t) {
  fd_set readfds;
  struct timeval timeout;
  int debug = 0;
  extern int errno;
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

oxSocketMultiSelect(int sid[],int size,int t,int result[])
{
  int i,fd,p;
  fd_set readfds;
  struct timeval timeout;
  int isdata = 0;
  extern errno;

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


socketConnect(char *serverName,int portNumber) {
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
    fprintf(TcpioError,"Trying to connect port %d, ip=%x\n",ntohs(server.sin_port),server.sin_addr);
  }
  if (connect(socketid,(struct sockaddr *)&server,sizeof(server)) == -1) {
    errorMsg1s("cannot connect");
    return(-1);
  }
  if (!Quiet) fprintf(TcpioError,"connected.\n");
  return(socketid);
}

socketConnectWithPass(char *servername,int port,char *pass)
{
  int fd;
  int m;
  SET_TCPIOERROR;
  fd = socketConnect(servername,port);
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
  
