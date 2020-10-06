/*$OpenXM: OpenXM/src/kan96xx/plugin/file2.c,v 1.16 2016/03/31 03:22:55 takayama Exp $ */
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <stdlib.h>
#include "file2.h"
#include "mysig.h"
#include "sm1Socket.h"

/* If you use file2 standalone to output string,  
    make the following dummy definition;
int KsocketSelect0(int a,int b) { return(0); }
int oxSocketSelect0(int a,int b) { return(0); }
or define FORSTRING
*/
#ifdef  FORSTRING
#define KsocketSelect0(a,b) 0
#define oxSocketSelect0(a,b) 0
#endif

#ifdef KXX
#define sGC_malloc(n) malloc(n)
#else
void *sGC_malloc(int size);
#endif
int WatchStream = 0;
/*  Note:  1997, 12/6   cf. SS475/kxx/openxxx.tex, SS475/memo1.txt
    The functions in file2.c should not accept interruptions
    during its critical operations.  The synchronization mechanism
    has not yet been implemented.
    */

static int debug1 = 0;
static int checkfp2(FILE2 *fp2,char *s);
static int fp2fputcString(int c,FILE2 *fp2);
static int checkfp2(FILE2 *fp2,char *s)
{
  if (fp2 == NULL) {
    fprintf(stderr,"%s:  NULL pointer.\n",s);
    return(-1);
  }
  if (fp2->initialized != 1) {
    fprintf(stderr,"%s:  fp is not initialized.\n",s);
    return(-1);
  }
  return(0);
}

FILE2 *fp2open(int fd) {
  FILE2 *fp2;
  if (debug1) {
    printf("fp2open is called. \n");
  }
  fp2 = (FILE2 *) sGC_malloc(sizeof(FILE2));
  if (fd < -1) {
    fprintf(stderr,"fp2open  Invalid file descriptor %d\n",fd);
    return(NULL);
  }
  /* fd == -1 ==> store in string. */
  if (fp2 == NULL) {
    fprintf(stderr,"fp2open.  No memory.\n");
    return(NULL);
  }
  fp2->fd = fd;
  fp2->initialized = 1;
  checkfp2(fp2,"fp2open ");
  fp2->readpos = 0;
  fp2->readsize = 0;
  fp2->writepos = 0;
  fp2->limit = FILE2BSIZE;
  fp2->readBuf = (char *) sGC_malloc(FILE2BSIZE);
  fp2->writeBuf = (char *) sGC_malloc(FILE2BSIZE);
  if ((fp2->readBuf == NULL) || (fp2->writeBuf == NULL)) {
	fprintf(stderr,"fp2open. No more memory.\n");
	return(NULL);
  }
  fp2->watch = 0;
  fp2->watchFile = NULL;
  fp2->mathcapList = NULL;
  fp2->log_incomming = NULL;
  fp2->log_outgoing = NULL;
  fp2->popened = 0;
  fp2->pfp = NULL;
  return(fp2);
}

void fp2setfp(FILE2 *fp2,FILE *fp,int popened) {
  fp2->pfp = fp;
  fp2->popened = popened;
}
    
int fp2fflush(FILE2 *fp2) {
  int r;
  if (debug1) {
    printf("fp2fflush is called with FILE2 *%lx.\n", (long)fp2);
    fp2dumpBuffer(fp2);
    printf("--------------------------\n");
  }
  if (checkfp2(fp2,"fp2fflush ") == -1) return(-1);
  if (fp2->fd == -1) return(0);
  if (fp2->writepos > 0) {
	mysignal(SIGPIPE,SIG_IGN);
    r = write(fp2->fd,fp2->writeBuf,fp2->writepos);
	mysignal(SIGPIPE,SIG_DFL);
    fp2->writepos = 0;
    if (r <= 0) {
      fprintf(stderr,"fp2fflush(): write failed on %d.\n",fp2->fd);
	  if (errno == EPIPE) {
		fprintf(stderr,"Your peer is closed --- SIGPIPE. Closing this fp2.\n");
		fp2fclose(fp2);
		return r;
	  }
    }
    return(r);
  }else{
    return(0);
  }
}

int fp2fclose(FILE2 *fp2) {
  int r;
  if (checkfp2(fp2," fp2fclose ") == -1) return(-1);
  if (fp2->fd == -1) return(0);
  r = fp2fflush(fp2);
  if (r < 0) {
    fprintf(stderr,"fp2fclose: flush error.\n");
    return(-1);
  }
  if (fp2->pfp != NULL) {
	if (fp2->popened) {
      return pclose(fp2->pfp);
	} else return fclose(fp2->pfp);
  }
  else return(close(fp2->fd));
}

int fp2fputc(int c,FILE2 *fp2) {
  FILE *fp;
  if (debug1) {
    printf("fp2fputc is called with %2x, fp2->writepos=%d, ",c,fp2->writepos);
    printf("fp2 = %lx.\n",(long) fp2);
  }
  if (fp2->watch || WatchStream) {
    if (fp2->watch) fp = fp2->watchFile;
    else fp = stderr;
	fprintf(stderr,"put to <%x> ",fp2->fd); /* output the channel for debug */
    if (c >= ' ' && c <='z') {
      fprintf(fp," %2x(%c)-> ",c& 0xff,c);
    }else{
      fprintf(fp," %2x( )-> ",c& 0xff);
    }
    fflush(NULL);
  }
  if (fp2->log_outgoing != NULL) fputc(c,fp2->log_outgoing);
  if (checkfp2(fp2," fp2fputc ") == -1) return(-1);
  (fp2->writeBuf)[fp2->writepos] = c;
  (fp2->writepos)++;
  if (fp2->fd == -1) return(fp2fputcString(c,fp2));
  if (fp2->writepos < fp2->limit) {
    return(c);
  }else{
    if (fp2fflush(fp2) <0) return(-1);
    else return(c);
  }
}

int fp2fgetc(FILE2 *fp2) {
  int c;
  FILE *fp;
  /* printf("fp2fgetc is called. "); fflush(NULL); */
  if (checkfp2(fp2," fp2fgetc ") == -1) return(-1);
  if (fp2->readpos < fp2->readsize) {
    fp2->readpos++;
    c = fp2->readBuf[fp2->readpos -1];
    if (fp2->watch || WatchStream) {
      if (fp2->watch) fp = fp2->watchFile;
      else fp = stderr;
      fprintf(fp,"get from <%x> ",fp2->fd); /* output the channel for debug*/
      if (c >= ' ' && c <= 'z') {
        fprintf(fp," %2x(%c) ",c,c);
      }else{
        fprintf(fp," %2x( ) ",c);
      }
      fflush(NULL);
    }
    if (fp2->log_incomming != NULL) fputc(c,fp2->log_incomming);
    return(c); 
  }else{
	if (fp2->fd == -1) return(-1);
    fp2->readpos = 0;
    fp2 ->readsize =
      read(fp2->fd, fp2->readBuf, fp2->limit);
    if (fp2->readsize == 0) {
      if (fp2->watch || WatchStream) {
        if (fp2->watch) fp = fp2->watchFile;
        else fp = stderr;
        fprintf(fp," <%2x ",c);
        fflush(NULL);
      }
      return(-1);
    }
    else {
      return(fp2fgetc(fp2));
    }
  }
}

int fp2select(FILE2 *fp2, int t) {
  if (fp2->fd == -1) {
	if (fp2->readpos < fp2->readsize) return(1);
	else return(0);
  }
  if (fp2->readpos < fp2->readsize) return(1);
  else {
#ifdef KXX
    return(oxSocketSelect0(fp2->fd,t));
#else
    return(KsocketSelect0(fp2->fd,t));
#endif
  }
}

int fp2dumpBuffer(FILE2 *fp2) {
  int i;
  if (checkfp2(fp2," f2pdumpBuf ") == -1) {
    return(-1);
  }
  printf("fd=%d\n",fp2->fd);
  printf("initialied=%d\n",fp2->initialized);
  printf("readpos=%d\n",fp2->readpos);
  printf("readsize=%d\n",fp2->readsize);
  printf("writepos=%d\n",fp2->writepos);
  printf("limit=%d\n",fp2->limit);
  for (i=0; i<fp2->readsize; i++) {
    printf("readBuf[%d]=%2x ",i,fp2->readBuf[i]);
  }
  for (i=0; i<fp2->writepos; i++) {
    printf("writeBuf[%d]=%2x ",i,fp2->writeBuf[i]);
  }
  printf("\n");
  return(0);
}

int fp2clearReadBuf(FILE2 *fp2) {
  fd_set readfds;
  struct timeval timeout;
  int fd;
#define TMP00SIZE  2000
  char tmp00[TMP00SIZE];
  int n;

  if (checkfp2(fp2," fp2clearReadBuf ") == -1) {
    return(-1);
  }
  if (fp2->fd == -1) return(0);  

  fp2->readsize=0; fp2->readpos = 0;  /* Clear the buffer. */
  
  fd = fp2->fd;
  while (1) {
    FD_ZERO(&readfds);
    FD_SET(fd,&readfds);
    timeout.tv_sec = 0;
    timeout.tv_usec = (long) 0;
    fp2->readpos = fp2->readsize = 0;
    if (select(fd+1,&readfds,(fd_set *)NULL,(fd_set *)NULL,&timeout)<0) {
      fprintf(stderr,"fp2clearReadBuf: Select error. Error no is %d. See /usr/include/sys/errno.h.\n",errno);
      return(-1);
    }
    if (FD_ISSET(fd,&readfds)) {
      n = read(fd,tmp00, TMP00SIZE);
      if (n <=  0) {
        fprintf(stderr,"fp2clearReadBuf: File is closed or read error.\n");
        return(-1);
      }
      if ( n < TMP00SIZE ) {
        return(0);
      }
    }else {
      return(0);
    }
  }
}
int fp2write(FILE2 *os, char *data, int size) {
  int i,r;
  for (i=0; i<size; i++) {
    r = fp2fputc((unsigned char)(data[i]),os);
  }
  return(r);
}

int fp2watch(FILE2 *fp2,FILE *f)
{
  if (f == NULL) {
    fprintf(stderr,"fp2watch FILE *f is NULL.\n");
    return(-1);
  }
  if (fp2 == NULL) {
    fprintf(stderr,"fp2watch FILE2 *fp2 is NULL.\n");
    return(-1);
  }
  if (fp2->watch) {
    fprintf(stderr,"fp2watch: fp2 is already taking a log.");
    return(-1);
  }
  fp2->watch = 1;
  fp2->watchFile = f;
  return(0);
}

int fp2stopWatch(FILE2 *fp2)
{
  if (fp2 == NULL) {
    fprintf(stderr,"fp2stopWatch FILE2 *fp2 is NULL.\n");
    return(-1);
  }
  if (!fp2->watch) {
    fprintf(stderr,"fp2stopWatch: fp2 is not taking a log.");
    return(-1);
  }
  fp2->watch=0;
  if (fp2->watchFile != stdout) {
    return(fclose(fp2->watchFile));
  }
}
  
int fp2log(FILE2 *fp,FILE *incomming,FILE *outgoing) {
  fp->log_incomming = incomming;
  fp->log_outgoing = outgoing;
  return 0;
}
int fp2stopLog(FILE2 *fp) {
  if (fp->log_incomming != NULL) fclose(fp->log_incomming);
  if (fp->log_outgoing != NULL) fclose(fp->log_outgoing);
  fp->log_incomming = fp->log_outgoing = NULL;
  return 0;
}

static int fp2fputcString(int c,FILE2 *fp2) {
  unsigned char *newwrite,*newread;
  int newsize;
  int i;
  (fp2->readBuf)[fp2->readsize] = c;
  (fp2->readsize)++;
  if ((fp2->writepos < fp2->limit) && (fp2->readsize < fp2->limit)) return(c);
  if ((fp2->limit)*2 >=0x3000000) {
	fprintf(stderr,"Too big output string.\n");
	return(-1);
  }
  newsize = (fp2->limit)*2;
  newwrite = (char *)sGC_malloc(newsize);
  newread = (char *)sGC_malloc(newsize);
  if ((newwrite == NULL) || (newread == NULL)) {
	fprintf(stderr,"fp2fputcString: No more memory.\n");
	return(-1);
  }
  for (i=0; i<fp2->writepos; i++) {
	newwrite[i] = fp2->writeBuf[i];
  }
  for (i=0; i<fp2->readsize; i++) {
	newread[i] = fp2->readBuf[i];
  }
  fp2->writeBuf = newwrite;
  fp2->readBuf = newread;
  fp2->limit = newsize;
  return(c);
}

char *fp2fcloseInString(FILE2 *fp2, int *sizep)
{
  if (fp2->fd == -1) {
	fp2fputc(0,fp2);
	*sizep = fp2->writepos-1;
	return(fp2->writeBuf);
  }else{
	fprintf(stderr,"fp2fcloseInString is called for a file stream that is not associated to a string.\n");
  }
}

int fp2fputs(char *s,FILE2 *fp) {
  int i,n;
  n = strlen(s);
  for (i=0; i<n; i++) {
	if (fp2fputc((unsigned char)(s[i]),fp) < 0) return(-1);
  }
  return(0);
}

/* Sample program  FORSTRING
 FILE2 *fp2;
 int c;
 char *s;
 int size;
 fp2 = fp2fopen(-1);
 while ((c = getchar()) != EOF) {
   fp2fputc(c,fp2);
 }
 s = fp2fcloseInString(fp2,&size);

 or

 while ((c = fp2fgetc(fp2)) != EOF) {
    ....
 }
*/

