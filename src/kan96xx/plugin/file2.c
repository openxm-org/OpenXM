#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include "file2.h"

#ifdef KXX
#define GC_malloc(n) malloc(n)
#else
void *GC_malloc(int size);
#endif

/*  Note:  1997, 12/6   cf. SS475/kxx/openxxx.tex, SS475/memo1.txt
    The functions in file2.c should not accept interruptions
    during its critical operations.  The synchronization mechanism
    has not yet been implemented.
    */

static int debug1 = 0;
static int checkfp2(FILE2 *fp2,char *s);
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
  fp2 = (FILE2 *) GC_malloc(sizeof(FILE2));
  if (fd < 0) {
    fprintf(stderr,"fp2open  Invalid file descriptor %d\n",fd);
    return(NULL);
  }
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
  fp2->watch = 0;
  fp2->watchFile = NULL;
  fp2->mathcapList = NULL;
  return(fp2);
}

    
int fp2fflush(FILE2 *fp2) {
  int r;
  if (debug1) {
    printf("fp2fflush is called with FILE2 *%x.\n", (int )fp2);
    fp2dumpBuffer(fp2);
    printf("--------------------------\n");
  }
  if (checkfp2(fp2,"fp2fflush ") == -1) return(-1);
  if (fp2->writepos > 0) {
    r = write(fp2->fd,fp2->writeBuf,fp2->writepos);
    fp2->writepos = 0;
    if (r <= 0) {
      fprintf(stderr,"fp2fflush(): write failed on %d.\n",fp2->fd);
    }
    return(r);
  }else{
    return(0);
  }
}

int fp2fclose(FILE2 *fp2) {
  int r;
  if (checkfp2(fp2," fp2fclose ") == -1) return(-1);
  r = fp2fflush(fp2);
  if (r < 0) {
    fprintf(stderr,"fp2fclose: flush error.\n");
    return(-1);
  }
  return(close(fp2->fd));
}

int fp2fputc(int c,FILE2 *fp2) {
  if (debug1) {
    printf("fp2fputc is called with %2x, fp2->writepos=%d, ",c,fp2->writepos);
    printf("fp2 = %x.\n",(int) fp2);
  }
  if (fp2->watch) {
    if (c >= ' ' && c <='z') {
      fprintf(fp2->watchFile," %2x(%c)-> ",c,c);
    }else{
      fprintf(fp2->watchFile," %2x( )-> ",c);
    }
    fflush(NULL);
  }
  if (checkfp2(fp2," fp2fputc ") == -1) return(-1);
  (fp2->writeBuf)[fp2->writepos] = c;
  (fp2->writepos)++;
  if (fp2->writepos < fp2->limit) {
    return(c);
  }else{
    if (fp2fflush(fp2) <0) return(-1);
    else return(c);
  }
}

int fp2fgetc(FILE2 *fp2) {
  int c;
  /* printf("fp2fgetc is called. "); fflush(NULL); */
  if (checkfp2(fp2," fp2fgetc ") == -1) return(-1);
  if (fp2->readpos < fp2->readsize) {
    fp2->readpos++;
    c = fp2->readBuf[fp2->readpos -1];
    if (fp2->watch) {
      if (c >= ' ' && c <= 'z') {
	fprintf(fp2->watchFile," %2x(%c) ",c,c);
      }else{
	fprintf(fp2->watchFile," %2x( ) ",c);
      }
      fflush(NULL);
    }
    return(c); 
  }else{
    fp2->readpos = 0;
    fp2 ->readsize =
      read(fp2->fd, fp2->readBuf, fp2->limit);
    if (fp2->readsize == 0) {
      if (fp2->watch) {
	fprintf(fp2->watchFile," <%2x ",c);
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
  extern int errno;
  int fd;
#define TMP00SIZE  2000
  char tmp00[TMP00SIZE];
  int n;

  if (checkfp2(fp2," fp2clearReadBuf ") == -1) {
    return(-1);
  }

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
    r = fp2fputc(data[i],os);
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
  
