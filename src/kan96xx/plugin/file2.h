/* file2.h */

#define MAGIC2 "Buffered IO by FILE2"
/*  ob.tag == Sfile, ob.lc.str == MAGIC2, ob.lc.voidp = (FILE2 *)fp2; */
#define FILE2BSIZE 1024

typedef struct FILE2 {
  int fd;
  int initialized;
  unsigned char readBuf[FILE2BSIZE];
  unsigned char writeBuf[FILE2BSIZE];
  int readpos;
  int readsize;
  int writepos;
  int limit;
  int watch;
  FILE *watchFile;
  void *mathcapList; /* pointer to struct object */
} FILE2 ;

FILE2 *fp2open(int fd);
int fp2fflush(FILE2 *fp);
int fp2fclose(FILE2 *fp);
int fp2fputc(int c,FILE2 *fp);
int fp2fgetc(FILE2 *fp);
int fp2select(FILE2 *fp2, int t);
int fp2dumpBuffer(FILE2 *fp);
int fp2clearReadBuf(FILE2 *fp);
int fp2write(FILE2 *fp,char *data,int size);
int fp2watch(FILE2 *fp,FILE *log);
int fp2stopWatch(FILE2 *fp);



 
