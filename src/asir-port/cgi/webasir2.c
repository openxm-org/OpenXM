/* $OpenXM$
 */
/*
  (httpd-asir2.sm1) run   webasir2
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

#define SIZE 0x10000

int Debug=1;
int main(int argc,char *argv[]) {
  int dataPort;
  struct hostent *servhost;
  struct sockaddr_in dServer;
  FILE *fp;
  char s[1];
  char fname[SIZE];
  int i;
  char key[SIZE];
  char comm[SIZE];
  int quit;
  quit = 0;
  for (i=1; i<argc; i++) {
    if (strcmp(argv[i],"--quit")==0) quit=1;
  }

  system("ls /tmp/webasir*.txt >tmp-webasir.txt");
  fp = fopen("tmp-webasir.txt","r");
  if (fp == NULL) {
    fprintf(stderr,"No webasir2 is running.\n"); return(-1);
  }
  fgets(fname,SIZE-2,fp);
  for (i=strlen(fname)-1; i>=0; i--) {
    if (fname[i] <= ' ') fname[i]=0; else break;
  }
  fclose(fp);
  if (strlen(fname)==0) {
    fprintf(stderr,"No webasir2 pid file.\n"); return(-1);
  }
  fp = fopen(fname,"r");
  if (fp == NULL) {
    fprintf(stderr,"Open error of %s\n",fname); return(-1);
  }
  fgets(key,SIZE-2,fp); sscanf(key,"%d",&dataPort);
  if (Debug) printf("dataPort=%d\n",dataPort);
  fgets(key,SIZE-2,fp);
  for (i=strlen(key)-1; i>=0; i--) {
    if (key[i] <= ' ') key[i]=0; else break;
  }
  if (Debug) printf("key=%s\n",key);
  fclose(fp);

  if ((servhost = gethostbyname("localhost")) == NULL) {
    fprintf(stderr,"bad server name.\n"); return(-1);
  }

  /* Connecting to the data port */  
  bzero((char *)&dServer,sizeof(dServer));
  dServer.sin_family = AF_INET;
  dServer.sin_port = htons(dataPort);
  bcopy(servhost->h_addr,
        (char *)&dServer.sin_addr,servhost->h_length);
  if ((dataPort = socket(AF_INET,SOCK_STREAM,0)) <0) {
    fprintf(stderr,"socket allocation is failed.\n");
  }
  if (Debug) fprintf(stderr,"Trying to connect port %d  ",ntohs(dServer.sin_port));
  
  if (connect(dataPort,(struct sockaddr *)&dServer,sizeof(dServer)) == -1) {
    fprintf(stderr,"cannot connect\n");
  }else{  fprintf(stderr,"Connected\n"); }

  if (quit) {
    sprintf(comm,"GET /?msg=httpdAsirMeta+quit HTTP/1.1\n\n");
    write(dataPort,comm,strlen(comm));
    fflush(NULL);
  }else{
    sprintf(comm,"GET /?%s=10-21;\n\n",key);
    write(dataPort,comm,strlen(comm));
    fflush(NULL);
  }

  /* get result */
  for (i=0; i<SIZE; i++) comm[i]=0;
  read(dataPort,comm,SIZE-2);
  printf("%s\n",comm);
}

