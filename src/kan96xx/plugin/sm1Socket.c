/* $OpenXM: OpenXM/src/kan96xx/plugin/sm1Socket.c,v 1.21 2015/10/10 11:29:46 takayama Exp $ */
/* msg0s.c */
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#include "datatype.h"
#include "stackm.h"
#include "extern.h"
#include "sm1Socket.h"

extern int Quiet;
static int Post_debug=0;
static void errorMsg1s(char *s);
static int getContentLength(char *s);
static int getReceivedContentLength(char *s);
#define MAX_LISTEN_QUEUE 3

/* [(sm1.socket) (open) [optional integer port, optional string name] ] extension ; */
struct object KsocketOpen(struct object obj) {
  char serverName[1024];
  int portNumber;
  struct object rob = NullObject;
  struct hostent *myhost;
  struct sockaddr_in me;
  int s_waiting;
  int on;
  int tt;
  extern int Post_debug;

  if ((char *)getenv("OXWEB_DEBUG") != NULL) {
	Post_debug = 1;
  }

  if (obj.tag != Sarray) {
    errorMsg1s("KsocketOpen([optional integer,optional string name])");
  }
  strcpy(serverName,"localhost");
  portNumber = 0;
  if (getoaSize(obj) >= 1) {
    if ((getoa(obj,0)).tag != Sinteger) {
      errorMsg1s("KsocketOpen([optional integer,optional string name]), the first argument must be an integer.");
    }
    portNumber = KopInteger(getoa(obj,0));
  }
  if (getoaSize(obj) >= 2) {
    if ((getoa(obj,1)).tag != Sdollar) {
      errorMsg1s("KsocketOpen([optional integer,optional string name]), the second argument must be a string.");
    }
    if (strlen(KopString(getoa(obj,1))) > 1023) {
      errorMsg1s("Too long server name");
    }
    strcpy(serverName,KopString(getoa(obj,1)));
  }


  /* fprintf(stderr,"Hello from open.\n"); */
  if ((myhost = gethostbyname(serverName)) == NULL) {
    perror("gethostbyname"); errorMsg1s("Bad server name.");
  }
  bzero((char *)&me,sizeof(me));
  me.sin_family = AF_INET;
  me.sin_port = htons(portNumber);
  bcopy(myhost->h_addr,
        &me.sin_addr,myhost->h_length);

  if ((s_waiting = socket(AF_INET,SOCK_STREAM,0)) < 0) {
    perror("socket"); errorMsg1s("Socket allocation is failed.");
  }

  on=1; setsockopt(s_waiting,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on)); 
  /* important */
  if (bind(s_waiting,(struct sockaddr *) &me,sizeof(me)) == -1) {
    fprintf(stderr,"Bind error. Error no is %d. See /usr/include/sys/errno.h.\n",errno);
    perror("bind"); errorMsg1s("cannot bind");
  }

  tt = sizeof(me);
  if (getsockname(s_waiting,(struct sockaddr *)&me,&tt) < 0) {
    fprintf(stderr,"getsockname error. Error no is %d. See /usr/include/sys/errno.h.\n",errno);
    perror("getsockname"); errorMsg1s("cannot getsockname");
  }

  
  if (listen(s_waiting,MAX_LISTEN_QUEUE) < 0) {
    perror("listen"); errorMsg1s("Listen failed");
  }
  /*  
      fprintf(stderr,"Done the initialization. port =%d\n",ntohs(me.sin_port));
      fprintf(stderr,"socket id = %d",accept(s_waiting,NULL,NULL)); */
  rob = newObjectArray(2);
  putoa(rob,0,KpoInteger(s_waiting));
  putoa(rob,1,KpoInteger(ntohs(me.sin_port)));
  return(rob);
  
}

/* [ integer port, optional string host default localhost]
   [ integer socketid, integer port ] */
struct object KsocketConnect(struct object obj) {
  char serverName[1024];
  int portNumber;
  struct hostent *servhost;
  struct sockaddr_in server;
  int socketid;
  struct object robj = NullObject;

  if (obj.tag != Sarray) {
    errorMsg1s("KsocketConnect( [ integer port, optional string host default localhost])");
  }
  strcpy(serverName,"localhost");
  if (getoaSize(obj) < 1) {
    errorMsg1s("You need to specify port number.\n");
  }
  if (getoaSize(obj) >= 1) {
    if ((getoa(obj,0)).tag != Sinteger) {
      errorMsg1s("KsocketConnect([integer port,optional string host]), the first argument must be an integer.");
    }
    portNumber = KopInteger(getoa(obj,0));
  }
  if (getoaSize(obj) >= 2) {
    if ((getoa(obj,1)).tag != Sdollar) {
      errorMsg1s("KsocketConnect([integer port,optional string host]), the second argument must be a string.");
    }
    if (strlen(KopString(getoa(obj,1))) > 1023) {
      errorMsg1s("Too long server name");
    }
    strcpy(serverName,KopString(getoa(obj,1)));
  }


  if ((servhost = gethostbyname(serverName)) == NULL) {
    perror("gethostbyname"); errorMsg1s("bad server name.\n");
  }
  bzero((char *)&server,sizeof(server));
  server.sin_family = AF_INET;
  server.sin_port = htons(portNumber);
  bcopy(servhost->h_addr,
        (char *)&server.sin_addr,servhost->h_length);

  if ((socketid = socket(AF_INET,SOCK_STREAM,0)) <0) {
    perror("socket"); errorMsg1s("socket allocation is failed.\n");
  }
  if (!Quiet) {
    fprintf(stderr,"Trying to connect port %d, ip=%lx\n",ntohs(server.sin_port),(long) server.sin_addr.s_addr);
  }
  if (connect(socketid,(struct sockaddr *)&server,sizeof(server)) == -1) {
    perror("connect"); errorMsg1s("cannot connect");
  }
  /* fprintf(stderr,"connected.\n"); */
  robj = newObjectArray(2);
  putoa(robj,0,KpoInteger(socketid));
  putoa(robj,1,KpoInteger(portNumber));
  return(robj);
}

/* [ integer socketid ]
   [ integer newsocketid ] */
struct object KsocketAccept(struct object obj) {
  struct object obj1 = OINIT;
  struct object obj2 = OINIT;
  struct object robj = OINIT;
  int s, news;
    
  if (obj.tag != Sarray) {
    errorMsg1s("KsocketAccept([integer socketid])");
  }
  if (getoaSize(obj) < 1) {
    errorMsg1s("KsocketAccept([integer socketid])");
  }
  obj1 = getoa(obj,0);
  if (obj1.tag != Sinteger ) {
    errorMsg1s("KsocketAccept([integer socketid]), argument must be integer.");
  }
  s = KopInteger(obj1);
  if ((news = accept(s,NULL,NULL)) < 0) {
    fprintf(stderr,"Error in accept. Retrying (KsocketAccept).\n");
    /* Code added for strange behavior on cygwin. */
    if ((news = accept(s,NULL,NULL)) < 0) {
      perror("accept"); errorMsg1s("Error in accept. Retry failed.");
    }
  }
  if (close(s) < 0) {
    perror("close"); errorMsg1s("Error in closing the old socket.");
  }
  robj = newObjectArray(1);
  putoa(robj,0,KpoInteger(news));
  return(robj);
}

/* [ integer socketid ]
   [ integer newsocketid ] */
/* It does not close the listening socket. You can call it as
   ls = open.
   fd=accept2(ls).  close(fd).
   fd=accept2(ls).  close(fd).
   ....
   */
struct object KsocketAccept2(struct object obj) {
  struct object obj1 = OINIT;
  struct object obj2 = OINIT;
  struct object robj = OINIT;
  int s, news;
    
  if (obj.tag != Sarray) {
    errorMsg1s("KsocketAccept([integer socketid])");
  }
  if (getoaSize(obj) < 1) {
    errorMsg1s("KsocketAccept([integer socketid])");
  }
  obj1 = getoa(obj,0);
  if (obj1.tag != Sinteger ) {
    errorMsg1s("KsocketAccept([integer socketid]), argument must be integer.");
  }
  s = KopInteger(obj1);
  if ((news = accept(s,NULL,NULL)) < 0) {
    fprintf(stderr,"Error in accept. Retrying (KsocketAccept2).\n");
    /* Code added for strange behavior on cygwin. */
    if ((news = accept(s,NULL,NULL)) < 0) {
      perror("accept"); errorMsg1s("Error in accept. Retry failed.");
    }
  }
  if (close(s) < 0) {
    perror("close"); errorMsg1s("Error in closing the old socket.");
  }
  robj = newObjectArray(1);
  putoa(robj,0,KpoInteger(news));
  return(robj);
}

int KsocketSelect0(int fd,int t) {
  fd_set readfds;
  struct timeval timeout;
  FD_ZERO(&readfds);
  FD_SET(fd,&readfds);
  timeout.tv_sec = 0;
  timeout.tv_usec = (long) t;
  if (t >= 0) {
    if (select(fd+1,&readfds,(fd_set *)NULL,(fd_set *)NULL,&timeout)<0) {
      /* It must be fd+1 !,  Not fd. */
      fprintf(stderr,"Select error. Error no is %d. See /usr/include/sys/errno.h.\n",errno);
      perror("select"); errorMsg1s("KsocketSelect0() : select failed.");
      return(0);
    }
  }else{ /* block */
    if (select(fd+1,&readfds,(fd_set *)NULL,(fd_set *)NULL,(struct timeval *)NULL)<0) {
      perror("select"); errorMsg1s("KsocketSelect0() : select failed.");
      fprintf(stderr,"Select error. Error no is %d. See /usr/include/sys/errno.h.\n",errno);
      return(0);
    }
  }
  if (FD_ISSET(fd,&readfds)) return(1);
  else return(0);
}

/* [ integer socketid optional integer timeout default 0]
   integer true or false
*/
struct object KsocketSelect(struct object obj) {
  struct object robj = OINIT;
  struct object ob1 = OINIT;
  struct object ob2 = OINIT;
  if (obj.tag != Sarray) {
    errorMsg1s("KsocketSelect([ integer socketid optional integer timeout default 0]");
  }
  if (getoaSize(obj) < 1) {
    errorMsg1s("KsocketSelect([ integer socketid optional integer timeout default 0]");
  }
  if (getoaSize(obj) >= 1) {
    ob1 = getoa(obj,0);
    ob2 = KpoInteger(0); /* default value */
  }
  if (getoaSize(obj) >= 2) {
    ob2 = getoa(obj,1);
  }
  if (ob1.tag != Sinteger) {
    errorMsg1s("KsocketSelect([ integer socketid optional integer timeout default 0] : the first argument must be an integer.");
  }
  if (ob2.tag != Sinteger) {
    errorMsg1s("KsocketSelect([ integer socketid optional integer timeout default 0] : the second argument must be an integer.");
  }
  robj = KpoInteger( KsocketSelect0(KopInteger(ob1),KopInteger(ob2)) );
  return(robj);
}

struct object KsocketSelectMulti(struct object obj) 
     /* [ [integer socketid1, integer socketid2, ...] 
     optional integer timeout default 0]
   [ result1, result2, ....]
*/
{
  struct object robj = OINIT;
  struct object ob1 = OINIT;
  struct object ob2 = OINIT;
  struct object ob3 = OINIT;
  int size,i,fd,p,t;
  fd_set readfds;
  struct timeval timeout;
  if (obj.tag != Sarray) {
    errorMsg1s("KsocketSelectMulti([[sid1, sid2,...] optional integer timeout default 0]");
  }
  if (getoaSize(obj) < 1) {
    errorMsg1s("KsocketSelectMulti([[sid1, sid2,...] optional integer timeout default 0]");
  }
  if (getoaSize(obj) >= 1) {
    ob1 = getoa(obj,0);
    ob2 = KpoInteger(0); /* default value */
  }
  if (getoaSize(obj) >= 2) {
    ob2 = getoa(obj,1);
  }
  if (ob1.tag != Sarray) {
    errorMsg1s("KsocketSelectMulti([[sid1, sid2, ...] optional integer timeout default 0] : the first argument must be an array.");
  }
  if (ob2.tag != Sinteger) {
    errorMsg1s("KsocketSelectMulti([[sid1, sid2, ...] optional integer timeout default 0] : the second argument must be an integer.");
  }
  FD_ZERO(&readfds);
  timeout.tv_sec = 0; t =  KopInteger(ob2);
  timeout.tv_usec = (long)t;

  size = getoaSize(ob1);
  if (size < 1) {
    errorMsg1s("KsocketSelectMulti: the first argument must be a non-empty array of integers.");
  }
  fd = 0;

  for (i=0; i<size; i++) {
    ob3 = getoa(ob1,i);
    if (ob3.tag != Sinteger) {
      errorMsg1s("KsocketSelectMulti: the first argument must be an array of integers.");
    }
    p = KopInteger(ob3);
    if (p > fd) fd = p;
    FD_SET(p,&readfds);
    /* printf("p = %d, fd=%d",p,fd); */
  }

  if (t >= 0) {
    if (select(fd+1,&readfds,(fd_set *)NULL,(fd_set *)NULL,&timeout)<0) {
      /* It must be fd+1 !,  Not fd. */
      fprintf(stderr,"Select error. Error no is %d. See /usr/include/sys/errno.h.\n",errno);
      perror("select"); errorMsg1s("KsocketSelectMulti() : select failed.");
    }
  }else{ /* block */
    if (select(fd+1,&readfds,(fd_set *)NULL,(fd_set *)NULL,(struct timeval *)NULL)<0) {
      fprintf(stderr,"Select error. Error no is %d. See /usr/include/sys/errno.h.\n",errno);
      perror("select"); errorMsg1s("KsocketSelectMulti() : (block) select failed.");
    }
  }
  robj = newObjectArray(size);
  for (i=0; i<size; i++) {
    if (FD_ISSET(KopInteger(getoa(ob1,i)),&readfds)) {
      putoa(robj,i,KpoInteger(1));
    }else{
      putoa(robj,i,KpoInteger(0));
    }
  }

  return(robj);
}




static char Data00[1024];
/* [ integer socketid ]
   string data 
*/
struct object KsocketRead(struct object obj) {
  struct object ob1 = OINIT;
  struct object robj = NullObject;
  static int datasize = 1024;
  static char *data = Data00;
  char *tmp;
  char *r;
  int n;
  int socketid;
  int totalsize;

  if (obj.tag != Sarray) {
    errorMsg1s("KsocketRead([integer socketid])");
  }
  if (getoaSize(obj) < 1) {
    errorMsg1s("KsocketRead([integer socketid])");
  }
  ob1 = getoa(obj,0);
  if (ob1.tag != Sinteger) {
    errorMsg1s("KsocketRead([integer socketid]) : the argument must be integer.");
  }
  socketid = KopInteger(ob1);

  totalsize = 0;

  while (KsocketSelect0(socketid,0)) {
    if (datasize - totalsize > 0) {
      n = read(socketid,data+totalsize,datasize-totalsize);
      if (n < 0) {
		perror("read");
        errorMsg1s("Read error.");
      }
      if (n < datasize-totalsize) {
        totalsize += n;
        break;
      }else{ totalsize += n; }
      if (totalsize == 0) {
        errorMsg1s("Select returns 1, but there is no data to read.");
      }
    }else { /* increase the datasize */
      tmp = (char *)sGC_malloc(sizeof(char)*2*datasize);
      /*I should use sGC_malloc_atomic and free after finishing this function?*/
      if (tmp == (char *)NULL) errorMsg1s("Out of Memory.");
      bcopy(data,tmp,totalsize);
      data = tmp;
      datasize = 2*datasize;
    }
  }

  r = (char *)sGC_malloc(sizeof(char)*(totalsize+1));
  if (r == (char *)NULL) errorMsg1s("Out of Memory.");
  bcopy(data,r,totalsize);
  r[totalsize] = 0;
  robj = KpoString(r);  /* BUG: it works only for reading string from TCP/IP
   					            stream. */

  return(robj);
    
}

/* [ integer socketid, string data ]
   integer  */
struct object KsocketWrite(struct object obj) {
  struct object ob1 = OINIT;
  struct object ob2 = OINIT;
  int socketid;
  int r;
  int k,k0;
  char *s;
  if (obj.tag != Sarray) {
    errorMsg1s("KsocketWrite([integer socketid, string data])");
  }
  if (getoaSize(obj) < 2) {
    errorMsg1s("KsocketWrite([integer socketid, string data])");
  }
  ob1 = getoa(obj,0);
  ob2 = getoa(obj,1);
  if (ob1.tag != Sinteger) {
    errorMsg1s("KsocketWrite([integer socketid, string data]) : the first argument must be an integer.");
  }
  socketid = KopInteger(ob1);
  if (ob2.tag != Sdollar) {
    errorMsg1s("KsocketWrite([integer socketid, string data]) : the second argument must be a string.");
  }
  s = KopString(ob2);
  k0 = k = strlen(s);
  while (1) {
    r = write(socketid,s,k);
    if (r < 0) {
      perror("write"); errorMsg1s("KsocketWrite: write failed.");
    }
    if (r >= k) break;
    k -= r;
    s = &(s[r]);
  }
  return(KpoInteger(k0));

}
struct object KsocketClose(struct object obj) {
  int socketid;
  struct object ob1 = OINIT;
  if (obj.tag != Sarray) {
    errorMsg1s("KsocketClose([ integer socketid ])");
  }
  if (getoaSize(obj) != 1) {
    errorMsg1s("KsocketClose([ integer socketid ])");
  }
  ob1 = getoa(obj,0);
  if (ob1.tag != Sinteger) {
    errorMsg1s("KsocketClose([ INTEGER socketid ])");
  }
  socketid = KopInteger(ob1);
  return(KpoInteger(close(socketid)));
}


static void errorMsg1s(char *s) {
  fprintf(stderr,"%s\n",s);
  errorKan1("msg1s.c: %s\n",s);
}


/**************** new 1997, 11/23 *******************/
struct object KsocketReadByte(struct object obj);
struct object KsocketWriteByte(struct object obj);

struct object KsocketReadByte(struct object obj) {
  struct object ob1 = OINIT;
  struct object robj = NullObject;
  char data[2];
  char *tmp;
  char *r;
  int n;
  int socketid;


  if (obj.tag != Sarray) {
    errorMsg1s("KsocketReadByte([integer socketid])");
  }
  if (getoaSize(obj) < 1) {
    errorMsg1s("KsocketReadByte([integer socketid])");
  }
  ob1 = getoa(obj,0);
  if (ob1.tag != Sinteger) {
    errorMsg1s("KsocketReadByte([integer socketid]) : the argument must be integer.");
  }
  socketid = KopInteger(ob1);

  
  n = read(socketid,data,1);
  if (n < 0) {
	perror("read");
    errorMsg1s("Read error.");
    robj = KpoInteger(-1);
    return(robj);
  }
  if (n == 0) {
    errorMsg1s("Read returned without data.");
  }

  robj = KpoInteger((int)((unsigned char)data[0]));
  return(robj);
    
}

/* [ integer socketid, int ]
   integer  */
struct object KsocketWriteByte(struct object obj) {
  struct object ob1 = OINIT;
  struct object ob2 = OINIT;
  int socketid;
  int r,i,n,kk,r0;
#define DATA_SIZE 1024
  char data[DATA_SIZE];
  if (obj.tag != Sarray) {
    errorMsg1s("KsocketWriteByte([integer socketid, int | array of int])");
  }
  if (getoaSize(obj) < 2) {
    errorMsg1s("KsocketWriteByte([integer socketid, int | array of int])");
  }
  ob1 = getoa(obj,0);
  ob2 = getoa(obj,1);
  if (ob1.tag != Sinteger) {
    errorMsg1s("KsocketWriteByte([integer socketid, int | array of int]) : the first argument must be an integer.");
  }
  socketid = KopInteger(ob1);
  if (ob2.tag != Sinteger && ob2.tag != Sarray) {
    errorMsg1s("KsocketWriteByte([integer socketid, int | array of int]) : the second argument must be an integer or an array of integers.");
  }
  if (ob2.tag == Sinteger) {
    data[0] = KopInteger(ob2);
    r = write(socketid,data, 1);
    if (r < 0) {
	  perror("write"); errorMsg1s("KsocketWriteByte: write error");
    }
  }else{
    n = getoaSize(ob2); kk = 0; r = 0;
    for (i=0; i<n; i++) {
      if (getoa(ob2,i).tag != Sinteger)
        errorMsg1s("KsocketWriteByte([integer socketid, int | array of int]) : elements of the second argument must be integers.");
      data[kk] = KopInteger(getoa(ob2,i));
      kk++;
      if (kk >= DATA_SIZE) {
        r0 = write(socketid,data,kk);
        if (r0 < 0) { perror("write"); errorMsg1s("write failed."); }
        if (r0 != kk) {
          fprintf(stderr,"Warning: Could not write to the socket.\n");
          return(KpoInteger(r+r0));  /* bug: we should retry. */
        }
        r += r0;
        kk = 0;
      }
    }
    if (kk > 0) {
      r0 = write(socketid,data,kk);
      if (r0 < 0) { perror("write"); errorMsg1s("write failed."); }
      if (r0 != kk) {
        fprintf(stderr,"Warning: Could not write to the socket.\n");
        return(KpoInteger(r+r0));
      }
      r += r0;
    }
  }
  return(KpoInteger(r));
}

struct object KsocketReadHTTP(struct object socketObj) {
  /* Read until two empty line appears. */
  struct object ob = OINIT;
  struct object ob1 = OINIT;
  struct object nob = OINIT;
  char *s;
  char *sss;
  char *tmp;
  int i;
  int flag;
  int flagmax = 1;
  int datasize;
  int last;
  int contentLength=-1;
  int socketid;
  extern int Post_debug;
  nob = NullObject;

  if (socketObj.tag != Sarray) {
    errorMsg1s("KsocketReadHTTP([integer socketid])");
  }
  if (getoaSize(socketObj) < 1) {
    errorMsg1s("KsocketReadHTTP([integer socketid])");
  }
  ob1 = getoa(socketObj,0);
  if (ob1.tag != Sinteger) {
    errorMsg1s("KsocketReadHTTP([integer socketid]) : the argument must be integer.");
  }
  socketid = KopInteger(ob1);

  if (KsocketSelect0(socketid,-1) != 1) {
	return(nob);
  }
  ob = KsocketRead(socketObj);
  s = KopString(ob);
  if (strncmp(s,"POST",4) == 0) flagmax=2; /* for IE */
  else flagmax=1;
  flag = 0;
  for (i=strlen(s)-1; i>=0; i--) {
	if ((s[i] == '\n') && (i==0)) {
	  ++flag;
	}else if ((s[i] == '\n') && (s[i-1] == '\n')) {
	  ++flag;
	}else if ((s[i] == 0xd) && (s[i+1] == 0xa) && (i == 0)) {
	  ++flag;
	}else if ((s[i] == 0xa) && (s[i-1] == 0xd) && (s[i+1] == 0xd) && (s[i+2] == 0xa)) {
	  ++flag;
	}
  }
  if (flag >= flagmax) return ob;
  datasize = strlen(s);
  sss = s;
  if ((s[strlen(s)-1] == '\n') ||
      (s[strlen(s)-2] == 0xd) && (s[strlen(s)-1] == 0xa)) {
	last = 1;
  }else last = 0;

  while (flag < flagmax) {
	contentLength = getContentLength(sss);
	if (contentLength != -1) {
	  if (contentLength <= getReceivedContentLength(sss)) {
		break;
	  }
	}
	if (Post_debug) {
	  fprintf(stderr,"Waiting in socketReadBlock. flagmax(0d,0a)=%d, content-length=%d, received content-length=%d\n",flagmax,contentLength,getReceivedContentLength(sss));
	}
	if (strlen(s) == 0) {
	  fprintf(stderr,"No data. Perhaps connection is closed by foreign host.\n");
	  return nob;
	}else{
	  /* for debugging. */
	  if (Post_debug) {
		for (i=0; i<strlen(sss); i++) {
		  if ((sss[i] >= ' ') && (sss[i] < 0x7f)) {
			fprintf(stderr,"%c",sss[i]);
		  }else{
			fprintf(stderr,"(%3x)",sss[i]);
			if (sss[i] == 0xa) fprintf(stderr,"\n");
		  }

		}
		fprintf(stderr,"\n");
	  }
	}

    if (KsocketSelect0(socketid,-1) != 1) {
	  return nob;
	}
	ob = KsocketRead(socketObj);
	s = KopString(ob);
	for (i=strlen(s)-1; i>=0; i--) {
	  if ((s[i] == '\n') && (i==0) && last) {
		++flag;
	  }else if ((s[i] == '\n') && (s[i-1] == '\n')) {
		++flag;
	  }else if ((s[i] == 0xd) && (s[i+1] == 0xa) && (i==0) && last) {
		++flag;
	  }else if ((s[i] == 0xa) && (s[i-1] == 0xd) && (s[i+1] == 0xd) && (s[i+2] == 0xa)) {
		++flag;
	  }
	}
	if (datasize-1 <= strlen(sss)+strlen(s)) {
      tmp = (char *)sGC_malloc(sizeof(char)*2*(datasize+strlen(s))+1);
      if (tmp == (char *)NULL) errorMsg1s("Out of Memory.");
	  strcpy(tmp,sss);
	  strcat(tmp,s);
      datasize = 2*(datasize+strlen(s));
	  sss = tmp;
	}else{
	  strcat(sss,s);
	}

	if ((s[strlen(s)-1] == '\n') ||
		(s[strlen(s)-2] == 0xd) && (s[strlen(s)-1] == 0xa)) {
	  last = 1;
	}else last = 0;

  }

  return KpoString(sss);

}

struct object Kplugin_sm1Socket(char *key,struct object obj) {
  struct object robj = NullObject;
  if (strcmp(key,"open") == 0) {
    robj = KsocketOpen(obj);
  }else if (strcmp(key,"connect") == 0) {
    robj = KsocketConnect(obj);
  }else if (strcmp(key,"accept") == 0) {
    robj = KsocketAccept(obj);
  }else if (strcmp(key,"accept2") == 0) {
    robj = KsocketAccept2(obj);
  }else if (strcmp(key,"select") == 0) {
    robj = KsocketSelect(obj);
  }else if (strcmp(key,"mselect") == 0) {
    robj = KsocketSelectMulti(obj);
  }else if (strcmp(key,"read") == 0) {
    robj = KsocketRead(obj);
  }else if (strcmp(key,"readHTTP") == 0) {
    robj = KsocketReadHTTP(obj);
  }else if (strcmp(key,"gethostname") == 0) {
    robj = KsocketGetHostName();
  }else if (strcmp(key,"write") == 0) {
    robj = KsocketWrite(obj);
  }else if (strcmp(key,"read") == 0) {
    robj = KsocketRead(obj);
  }else if (strcmp(key,"readByte") == 0) {
    robj = KsocketReadByte(obj);
  }else if (strcmp(key,"writeByte") == 0) {
    robj = KsocketWriteByte(obj);
  }else if (strcmp(key,"close") == 0) {
    robj = KsocketClose(obj);
  }else {
    errorMsg1s("Unknown tag for sm1.socket");
  }
  return(robj);
}


static int getContentLength(char *s) {
  int n;
  int i,j;
  int len = -1;
  char *s1 = "content-length:";
  char s0[256];
  int m;
  m = strlen(s1);
  n = strlen(s);
  for (i=0; i<n; i++) {
    strncpy(s0,&(s[i]),m+1);
    for (j=0; j<m; j++) {
      if ((s0[j] >= 'A') && (s0[j] <= 'Z')) s0[j] = s0[j]+0x20;
    }
    if (strncmp(s0,s1,strlen(s1)) == 0) {
      sscanf(&(s[i+strlen(s1)]),"%d",&len);
      break;
    }
  }
  return len;
}
static int getReceivedContentLength(char *s) {
  int n;
  int i;
  int start;
  start = -1;
  n = strlen(s);
  for (i=0; i<n; i++) {
	if ((s[i] == '\n') && (s[i+1] == '\n')) {
	  start = i+2; break;
	}else if ((s[i] == 0xd) && (s[i+1] == 0xa) && (s[i+2] == 0xd) && (s[i+3] == 0xa)) {
	  start = i+4;
	}
  }
  if (start == -1) return 0;
  return (n-start);
}


struct object KsocketGetHostName(void) {
  char name[1024];
  char *s;
  struct object rob = NullObject;
  if (gethostname(name,1023) != 0) {
    return rob;	
  }
  s = (char *)sGC_malloc(sizeof(char)*(strlen(name)+2));
  if (s == (char *)NULL) errorMsg1s("Out of Memory.");
  strcpy(s,name);
  return(KpoString(s));
}






