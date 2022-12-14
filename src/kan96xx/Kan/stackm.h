/* $OpenXM: OpenXM/src/kan96xx/Kan/stackm.h,v 1.13 2015/10/08 08:14:25 takayama Exp $ */
#define LOAD_SM1_PATH "/usr/local/lib/sm1/"
/* Do not forget to put / at the tail.
   "/usr/local/lib/sm1" does not work.
*/


/******************* stackm.h ******************************/
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
 
/**** data types (class identifiers) ************/
/* Never change the following orders.  
   If you add a new class, add that class at the bottom and give the
   max number. */
#define Snull             0
#define Sinteger          1     /* integer */
#define Sstring           2     /* pointer to a string */
#define SexecutableArray  3     /* executable array */
#define Soperator         4     /* operators defined in the system dic */
#define Sdollar           5     /* pointer to a string obtained from $...$ */
#define Sarray            6     /* lc.ival is the size of array,
				   (rc.op)[0], ..., (rc.op)[k] is the array
				   of object */
#define SleftBraceTag    7     /* [ */
#define SrightBraceTag   8     /* ] */
#define Spoly            9
#define SarrayOfPOLY     10
#define SmatrixOfPOLY    11
#define Slist            12   /* list of object */
#define Sfile            13
#define Sring            14
#define SuniversalNumber 15
#define SrationalFunction 16
#define Sclass          17   /* class, for extension */
#define Sdouble         18
#define SbyteArray      19

#define TYPES            20   /* number of data types. */
/* NOTE! If you change the above, you need to change mklookup.c too. */
/* Change also dr.sm1 : datatype constants. */

#define     CMO   1024    /* cf. plugin cmo.h. Object tag used in cmo. */
/*  CMO --- CMO+1024*3 
    CMO + LARGEID ( cf.cmo.h ) --- CMO + LARGEID+256
    CMO + PRIVATEID ( cf.cmo.h ) --- CMO + PRIVATEID+256
    are reserved area.
*/

typedef struct object * objectp;
/*********** fundamental data types ****************/
union cell {
  int ival;
  char *str;
  struct object *op;
  POLY poly;    
  struct arrayOfPOLY *arrayp;
  struct matrixOfPOLY *matrixp;
  struct tokens *tokenArray;
  FILE *file;
  struct ring *ringp;
  struct coeff *universalNumber;
  void *voidp;
  double *dbl;
  unsigned char *bytes;
};

struct object{
  int tag;                /* class identifier */
  union cell lc;          /* left cell */
  union cell rc;          /* right cell */
  struct object *attr;    /* Attribute list. See misc-2005/06/gfan/test1.sm1
                             as to performance test. */
};

#define OINIT  { .attr = NULL}

struct dictionary {
  char *key;
  int h0; /* Value of hash functions */
  int h1;
  struct object obj;
  int attr;
};

struct operandStack {
  struct object *ostack;
  int sp;
  int size;
};

struct context {
  struct dictionary *userDictionary;
  struct context *super;
  char *contextName;
  struct operandStack *mystack; /* It is not used for now. */
};

/* for the scanner */
typedef enum {INIT,GET,PUT,OPEN} actionType;

struct tokens{
  char *token;
  int kind;
  struct object object;
  int tflag;
};

/* used in kind of tokens */ 
#define ID   2
#define DOLLAR 3   /* strings enclosed by dollar sign */
#define EXECUTABLE_STRING 4 /* strings enclosed by {} */
#define EXECUTABLE_ARRAY  8

/* Used in tflag of tokens, bit wise */
#define NO_DELAY  0x2


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

#define isObjectArray(ob) ((ob).tag == Sarray)

#define isDollar(ob) ((ob).tag == Sdollar)

/******** macros for lists ************/
#define isNullList(list) ((struct object *)NULL == list)
#define NULLLIST (struct object *)NULL

/* For dictionary, flag bit */
#define SET_ATTR_FOR_ALL_WORDS 0x10
#define OR_ATTR_FOR_ALL_WORDS  0x20
#define PROTECT 0x1
#define ABSOLUTE_PROTECT 0x2
#define ATTR_INFIX       0x4
#define ATTR_EXPORT      0x8

/* For status, flag bit */
#define STATUS_EOF   -1
#define STATUS_BREAK 0x1
#define STATUS_INFIX 0x2
#define DO_QUOTE     0x10

typedef enum {CCPUSH,CCPOP,CCRESTORE} actionOfContextControl;

#define SCANNERBUF_SIZE 240
