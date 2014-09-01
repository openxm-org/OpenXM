/* $OpenXM: OpenXM/src/asir-port/cgi/webasir2.c,v 1.4 2014/08/31 10:20:33 takayama Exp $
 */
/*
 -(httpd-asir2.sm1) run   webasir2
   >log 2>&1
 - Todo, timer(limit, command, message) implement in sm1.
 - Example.  webasir2 --asir 'oxMessageBody=1%2B3%3B'
 - Example.  webasir2 --asir '3-2'
 - Error handling is not completed. Run src/kan96xx/Doc/httpd-asir2-kill.sh by cron.
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

#define SIZE 0x10000
#define MKEY "oxMessageBody="
char *byteArrayToUrlEncoding(char *s,int size);
static int cgiHex(int p);
char *urlEncodedStringToString(char *s);

int Debug=1;
int SetTimer=0;
int main(int argc,char *argv[]) {
  int dataPort;
  struct hostent *servhost;
  struct sockaddr_in dServer;
  FILE *fp;
  char s[1];
  char fname[SIZE];
  int i,j,c;
  char key[SIZE];
  char comm[SIZE];
  char asircomm[SIZE];
  int quit;
  char workf[SIZE];
  quit = 0;
  asircomm[0] = 0;
  for (i=1; i<argc; i++) {
    if (strcmp(argv[i],"--quit")==0) quit=1;
    else if (strcmp(argv[i],"--asir")==0) {
      i++;
      if (i <argc) strcpy(asircomm,argv[i]);
      else { usage(); return(-1); }
    }else if (strcmp(argv[i],"--debug")==0) {
      i++;
      if (i <argc) sscanf(argv[i],"%d",&Debug);
      else { usage(); return(-1); }
    }else if (strcmp(argv[i],"--settimer")==0) {
      i++;
      if (i <argc) sscanf(argv[i],"%d",&SetTimer);
      else { usage(); return(-1); }
    } else if (strcmp(argv[i],"--stdin")==0) {
      asircomm[0] = 0; j=0;
      while ((c=getchar()) != EOF) {
	asircomm[j] = c; j++; asircomm[j] = 0;
	if (j > SIZE-3) {
	  fprintf(stderr,"error, too big input.\n"); return(-1);
	}
      }
    } else {
      usage(); return(0);
    }
  }

  sprintf(workf,"/tmp/tmp-webasir-%d.txt",(int) getpid());
  sprintf(comm,"ls /tmp/webasir*.txt >%s",workf);
  system(comm);
  fp = fopen(workf,"r");
  if (fp == NULL) {
    fprintf(stderr,"Failed ls\n"); return(-1);
  }
  fgets(fname,SIZE-2,fp);
  for (i=strlen(fname)-1; i>=0; i--) {
    if (fname[i] <= ' ') fname[i]=0; else break;
  }
  fclose(fp);
  if (strlen(fname)==0) {
    if (Debug) fprintf(stderr,"No webasir2 pid file.\n"); 
    if (Debug) {fprintf(stderr,"No webasir2 is running. Start server.\n");}
    startServer();
    sleep(5);
    system(comm);
    fp = fopen(workf,"r");
    if (fp == NULL) {
      fprintf(stderr,"Failed to start the server.\n"); return(-1);
    }
    fgets(fname,SIZE-2,fp);
    for (i=strlen(fname)-1; i>=0; i--) {
      if (fname[i] <= ' ') fname[i]=0; else break;
    }
    if (strlen(fname) == 0) {
      fprintf(stderr,"Failed to start the server. No webasir2 pid file.\n"); return(-1);
    }
    fclose(fp);
  }

  fp = fopen(fname,"r");
  if (fp == NULL) {
    fprintf(stderr,"Open error of %s\n",fname); return(-1);
  }
  mylock(fname);
  fgets(key,SIZE-2,fp); sscanf(key,"%d",&dataPort);
  if (Debug) printf("dataPort=%d\n",dataPort);
  fgets(key,SIZE-2,fp);
  for (i=strlen(key)-1; i>=0; i--) {
    if (key[i] <= ' ') key[i]=0; else break;
  }
  if (Debug) printf("key=%s\n",key);
  fclose(fp);
  sprintf(comm,"rm -f %s",workf);
  system(comm);

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
    fprintf(stderr,"error: cannot connect\n");
  }else{  if (Debug) fprintf(stderr,"Connected\n"); }

  if ((strlen(asircomm)==0) && (!quit)) {myunlock(fname); outputTop(); return(0); }

  /* If the input is MKEY=..., extract ... */
  if (strncmp(asircomm,MKEY,strlen(MKEY))==0) {
    strcpy(comm,&(asircomm[strlen(MKEY)]));
    strcpy(asircomm,comm);
    strcpy(asircomm,urlEncodedStringToString(asircomm));
    if (Debug) fprintf(stderr,"oxMessageBody, asircomm=%s\n",asircomm);
  }

  if (SetTimer) {
    strcpy(comm,asircomm);
    for (i=strlen(comm)-1; i>=0; i--) {
      if ((comm[i] == ';') || (comm[i] <= ' ')) comm[i] = 0;
      else break;
    }
    sprintf(asircomm,"timer(%d,%s,\"error(timeout %d sec)\");",
	    SetTimer,comm,SetTimer);
  }
  if (quit) {
    sprintf(comm,"GET /?msg=httpdAsirMeta+quit HTTP/1.1\n\n");
    write(dataPort,comm,strlen(comm));
    fflush(NULL);
  }else{
    sprintf(comm,"GET /?%s=%s;\n\n",key,byteArrayToUrlEncoding(asircomm,strlen(asircomm)));
    write(dataPort,comm,strlen(comm));
    fflush(NULL);
  }

  /* get result */
  for (i=0; i<SIZE; i++) comm[i]=0;
  read(dataPort,comm,SIZE-2);
  /* remove newline(s) */
  for (i=strlen(comm)-1; i>0; i--) {
    if (comm[i] < ' ') comm[i] = 0;
    else break;
  }
  myunlock(fname);
  outputResult(comm);
}

/* from kan96xx/plugin/oxcgi.c */
/* . - _  A-Z a-z 0-9
   space --> +
*/
static int isUrlEncoding3(char s) {
  if ((s == '.') || (s == '-') || (s == '_')) return(0);
  if ((s >= 'A') && (s <= 'Z')) return(0); 
  if ((s >= 'a') && (s <= 'z')) return(0); 
  if ((s >= '0') && (s <= '9')) return(0);
  if (s == ' ') return(0);
  return(1);
} 

char *byteArrayToUrlEncoding(char *s,int size) {
  int n,i,j;
  char *r;
  n = 0;
  /* get Size */
  for (i=0; i<size; i++) {
    if (isUrlEncoding3((char)s[i])) n += 3;
    n++;
  }
  r = malloc(n+1);
  if (r == NULL) {fprintf(stderr,"%s\n","No more memory."); return(NULL); }
  r[0] = 0; r[n] = 0;
  i = 0; j = 0;
  while ((j < n) && (i<size)) {
    if (isUrlEncoding3((char)s[i])) {
      sprintf(&(r[j]),"%%%02X",s[i]); j += 3;
    }else{
      if ((char)s[i] == ' ') r[j]='+';
      else r[j] = s[i]; 
      j++; r[j] = 0;
    }
    i++;
  }
  return(r);
}


char *urlEncodedStringToString(char *s)
{
  char *ts;
  char *ts2;
  int i,j;
  int p;
  int vstart, vend;
  if (s == NULL) return(NULL);
  vstart = 0; vend = strlen(s)-1;
  ts = (char *) malloc(strlen(s)+2);
  if (ts == NULL)  ;
  j = 0; ts[j] = 0;
  for (i=vstart; i<=vend; i++,j++) {
    ts[j] = 0;
    if (s[i] == '+') {
      ts[j] = ' '; ts[j+1] = 0;
    }else if (s[i] == '%') {
      p = cgiHex(s[i+1])*16+cgiHex(s[i+2]);
      i = i+2;
      ts[j] = p; ts[j+1] = 0;
    }else {
      ts[j] = s[i]; ts[j+1] = 0;
    }
  }
  ts2 = (char *) malloc(j);
  if (ts2 == NULL) ;
  for (i=0; i<j; i++) {
    ts2[i] = ts[i];  ts2[i+1] = 0;
  }
  return (ts2);
}

static int cgiHex(int p) {
  if (p >= '0' && p <= '9') return (p-'0');
  if (p >= 'A' && p <= 'F') return (p-'A'+10);
  if (p >= 'a' && p <= 'f') return (p-'a'+10);
  if (Debug) fprintf(stderr,"%s\n","Invalid argument to cgiHex.");
}

outputTop() {
  printf("Content-Type: text/html\n\n");
  printf("<html><body>\nInput <br> asir-command <br> without semicolon. <br><br>\n");
  printf("<form method=\"POST\"> <input type=submit>\n");
  printf("<textarea name=\"oxMessageBody\" rows=10 cols=\"80\" wrap=\"soft\">\n");
  printf("</textarea>\n</form>\n");
  printf("</body></html>\n");
}
outputResult(char *s) {
  printf("Content-Type: text/plain\n\n");
  printf("%s\n",s);
}
startServer() {
  char comm[SIZE];
  char *r;
  r = getenv("CGI_ASIR_ALLOW");
  if (r == NULL) {
    setenv("CGI_ASIR_ALLOW","[(quit) (fctr)]",1);
  }
  sprintf(comm,"%s/src/kan96xx/Doc/httpd-asir2.sh >/dev/null 2>&1 &",getenv("OpenXM_HOME"));
  /* sprintf(comm,"echo $CGI_ASIR_ALLOW\n");  security check. */
  system(comm);
}
mylock(char *fname) {
  char comm[SIZE];
  sprintf(comm,"mv %s /tmp/lock-webasir-%d.txt",fname,getpid());
  system(comm);
}
myunlock(char *fname) {
  char comm[SIZE];
  sprintf(comm,"mv /tmp/lock-webasir-%d.txt %s",getpid(),fname);
  system(comm);
}
usage() {
  fprintf(stderr,"webasir2 [--quit] [--asir command_string]\n");
  fprintf(stderr,"         [--debug level]\n");
  fprintf(stderr,"         [--settimer seconds]\n");
  fprintf(stderr,"webasir2 --stdin  ; command is obtained from the stdin.\n");
}
