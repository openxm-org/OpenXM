/* $OpenXM: OpenXM/doc/oxlib/test1-tcp.c,v 1.2 2000/09/07 21:55:48 takayama Exp $ */
/* A sample code to explain how to use ox_asir by TCP/IP and
   OpenXM control protocol.
   It computes the gcd of 12 and 8 by calling ox_asir server.
   */

/*
   Asir code that does the same computation on ox_asir server.
   Try the following lines on asir.

   P = ox_launch();
   ox_push_cmo(P,12);
   ox_push_cmo(P,8);
   ox_push_cmo(P,ntoint32(2));
   ox_push_cmo(P,"igcd");
   ox_push_cmd(P,269);
   ox_pop_string(P);
   
   Then, you get 
     4

   */

#include <stdio.h>
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

#define DATAPORT 4321
#define CONTROLPORT 1234

static int Serial = 0;

main() {
  int dataPort, controlPort;
  struct hostent *servhost;
  struct sockaddr_in cServer;
  struct sockaddr_in dServer;
  char s[1];

  /* Starting the ox_asir server */
  system("xterm -geometry 80x25+0+0 -e ../../bin/ox -ox ../../bin/ox_asir -control 1234 -data 4321 &");
  sleep(3);
  if ((servhost = gethostbyname("localhost")) == NULL) {
    fprintf(stderr,"bad server name.\n"); exit(1);
  }

  /* Connecting to the control port */
  bzero((char *)&cServer,sizeof(cServer));
  cServer.sin_family = AF_INET;
  cServer.sin_port = htons(CONTROLPORT);
  bcopy(servhost->h_addr,
        (char *)&cServer.sin_addr,servhost->h_length);
  if ((controlPort = socket(AF_INET,SOCK_STREAM,0)) <0) {
    fprintf(stderr,"socket allocation is failed.\n");
  }
  fprintf(stderr,"Trying to connect port %d  ",ntohs(cServer.sin_port));
  
  if (connect(controlPort,(struct sockaddr *)&cServer,sizeof(cServer)) == -1) {
    fprintf(stderr,"cannot connect\n"); 
  }else{  fprintf(stderr,"Connected\n"); }
  sleep(1);

  /* Connecting to the data port */  
  bzero((char *)&dServer,sizeof(dServer));
  dServer.sin_family = AF_INET;
  dServer.sin_port = htons(DATAPORT);
  bcopy(servhost->h_addr,
        (char *)&dServer.sin_addr,servhost->h_length);
  if ((dataPort = socket(AF_INET,SOCK_STREAM,0)) <0) {
    fprintf(stderr,"socket allocation is failed.\n");
  }
  fprintf(stderr,"Trying to connect port %d  ",ntohs(dServer.sin_port));
  
  if (connect(dataPort,(struct sockaddr *)&dServer,sizeof(dServer)) == -1) {
    fprintf(stderr,"cannot connect\n");
  }else{  fprintf(stderr,"Connected\n"); }

  /* Byte order negotiation */
  read(controlPort,s,1);
  read(dataPort,s,1);
  s[0] = 0;
  write(controlPort,s,1);
  write(dataPort,s,1);
  fflush(NULL);

  /* Computation by the engine */
  hoge(dataPort);

  printf("\nType in ctrl-C to quit.\n"); 
  sleep(10000);
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


