/* server stack machine, serversm.h  */
void *Sm1_mathcap(void);
int Sm1_setMathCap(ox_stream os);
void Sm1_pops(void);
int Sm1_executeStringByLocalParser(void);
char *Sm1_popString(void);
int Sm1_setName(void);
int Sm1_evalName(void);
int Sm1_start(int argc, char *fnames[],char *myname);
int Sm1_pushCMO(ox_stream fp);  /* read data from the stream and push it */
int Sm1_popCMO(ox_stream fp,int serial);         /* pop and send the data to stream. */
int Sm1_pushError2(int serial,int no,char *message);
char *Sm1_popErrorMessage(char *s);
void Sm1_getsp(void);
void Sm1_dupErrors(void);
void Sm1_pushCMOtag(int serial);

#include <setjmp.h>

#if defined(__CYGWIN__) || defined(__MSYS__)
#define MYSETJMP(e) _setjmp(e)
#define MYSIGSETJMP(e,f) _setjmp(e)
#define MYLONGJMP(e,f) _longjmp(e,f)
#define MYSIGLONGJMP(e,f) _longjmp(e,f)
#else
#define MYSETJMP(e)  setjmp(e)
#define MYSIGSETJMP(e,f) sigsetjmp(e,f)
#define MYLONGJMP(e,f) longjmp(e,f)
#define MYSIGLONGJMP(e,f) siglongjmp(e,f)
#endif

/********************  Object from Kan/stackm.h *************************/
#define Snull             0
#define Sinteger          1     /* integer */
#define Sdollar           5     /* pointer to a string obtained from $...$ */
#define Sarray            6     /* lc.ival is the size of array,
				   (rc.op)[0], ..., (rc.op)[k] is the array
				   of object */
#define Sclass          17   /* class, for extension */


union cell {
  int ival;
  char *str;
  struct object *op;
  void *voidp;
};
struct object{
  int tag;                /* class identifier */
  union cell lc;          /* left cell */
  union cell rc;          /* right cell */
  struct object *attr;
};
#define OINIT  { .attr = NULL }

/********** macros to use Sarray **********************/
/* put to Object Array */
#define putoa(ob,i,cc) {\
if ((ob).tag != Sarray) {fprintf(stderr,"Warning: PUTOA is for an array of objects\n");} else \
{if ((0 <= (i)) && ((i) < (ob).lc.ival)) {\
  (ob.rc.op)[i] = cc;\
}else{\
  fprintf(stderr,"Warning: PUTOA, the size is %d.\n",(ob).lc.ival);\
}}}

#define getoa(ob,i) ((ob.rc.op)[i])

#define getoaSize(ob) ((ob).lc.ival)
/**************** end of macros from Kan/stackm.h *******************/

/*   Interface functions from kanlib.a.  */
struct object KSpop();
void KSpush(struct object ob);
struct object KfindUserDictionary(char *s);
struct object KputUserDictionary(char *s,struct object ob);
struct object KnewErrorPacket(int serial,int no,char *s);
struct object KnewErrorPacketObj(struct object);
struct object KSmathCap(void);  /* defined plugin/cmo.c */
void *KSmathCapByStruct(void);  /* defined plugin/cmo.c */
char *popErrorStackByString(void);
struct object KSdupErrors(void);
struct object KpoInteger(int i);
struct object KpoString(char *s);
struct object KSpeek(int k);
struct object KSnewObjectArray(int k);
