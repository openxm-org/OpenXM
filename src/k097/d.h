/* $OpenXM: OpenXM/src/k097/d.h,v 1.7 2015/10/10 11:29:46 takayama Exp $ */
/* d.h;*/
/*  from stackm.h */
#include <string.h>
#include <stdlib.h>

/**** data types (class identifiers) ************/
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

#define TYPES            19   /* number of data types. */
/* NOTE! If you change the above, you need to change mklookup.c too. */
/* Change also dr.sm1 : datatype constants. */

/* The following tags are not in stackm.h, but we use them. */
#define CLASSNAME_CONTEXT 258


/*********** fundamental data types ****************/
union cell {
  int ival;
  char *str;
  struct Object *op;
  FILE *file;
  struct stringBuf *sbuf;
  void *voidp;
};

struct Object{
  int tag;                /* class identifier */
  union cell lc;          /* left cell */
  union cell rc;          /* right cell */
  struct object *attr;
};

struct object{         /* must be compatible with stackm.h */
  int tag;                /* class identifier */
  union cell lc;          /* left cell */
  union cell rc;          /* right cell */
  struct object *attr;
};

#define OINIT { .attr = NULL }

typedef struct Object * objectp;   /* cf. 65p of Schreiner. */
#define YYSTYPE objectp


struct stringBuf {
  char *str;
  int limit;
  int ptr;
};

struct object Sm1obj;

/*  for dic.c */
struct dictionary {
  char *key;
  int h0; /* Value of hash functions */
  int h1;
  objectp obj;
  int attr;
};
struct operandStack {
  objectp *ostack;
  int sp;
  int size;
};
struct context {
  struct dictionary *userDictionary;
  struct context *super;
  char *contextName;
  struct operandStack *mystack; /* It is not used for now. */
};
#define USER_DICTIONARY_SIZE   1223
/* The value of USER_DICTIONARY_SIZE must be prime number, because of hashing
   method */
#define EMPTY (char *)NULL
#define SET_ATTR_FOR_ALL_WORDS 0x10
#define PROTECT 0x1
#define ABSOLUTE_PROTECT 0x2
typedef enum {CCPUSH,CCPOP,CCRESTORE} actionOfContextControl;
#define CSTACK_SIZE 1000
typedef enum {IRESET,IPUT,IEXIT} actionOfPutIncetanceVariable;

/* ki.c or dm.c */
void pkkan(char *s);
void pkkanInteger(int k);
void pkdebug(char *s0,char *s1,char *s2,char *s3);
void pkdebug2(void);
char *readstring();
char *newString(int size);

/* object handling */
objectp newObject_d();
void printObject_d(FILE *fp,objectp op);
void printObjectSymbol(objectp op);
char *objectSymbolToString(objectp op);
int K00objectToInteger(objectp op);
objectp K00integerToObject(int k);
objectp K00contextToObject(struct context *cp);
struct context *K00objectToContext(objectp op);
objectp ooAdd(objectp a,objectp b);
objectp ooMult(objectp a,objectp b);

/* memory */
void *mymalloc(int size);

/* protection of symbols */
int isProtectedSymbol(char *s);
int ips(objectp op);
/* read from file */
void parseAfile(FILE *fp);
void parseAstring(char *s);
int fsgetc(objectp op);
int fsungetc(int c,objectp op);
void readLineFromFile(FILE *fp,struct stringBuf *obuf);
struct stringBuf *newStringBuf(char *initstr);
void doublingStringBuf(struct stringBuf *sbuf);
objectp checkIfTheFileExists(objectp name);
void loadFile(objectp op);
void loadFileWithCpp(objectp op);
void showStringBuff(objectp op);
char *getLOAD_K_PATH();
void clearInop();

struct object KSpop();
void testNewFunction(objectp op);

/*  Dictionary and context handling. dic.c */
int K00putUserDictionary(char *str,int h0,int h1,objectp ob,
			 struct context *cp);
int K00putUserDictionary2(char *str,int h0,int h1, int attr,
			 struct context *cp);
objectp K00findUserDictionary(char *str,int h0,int h1,struct context *cp);
objectp K00findUserDictionary0(char *str,int h0,int h1,struct context *cp);
int K00hash0(char *str);
int K00hash1(char *str);
void K00hashInitialize(struct dictionary *dic);
void K00fprintContext(FILE *fp,struct context *cp);
void K00fprintContextAndDictionary(FILE *fp,struct context *cp);
struct context *K00newContext0(struct context *super,char *name);
void K00contextControl(actionOfContextControl ctl);
char *K00getCurrentContextName();
void K00InitDic();
void K00errorDic(char *s);

/* name space control */
int K00putIncetanceVariable(actionOfPutIncetanceVariable action,char *s);
int K00getIncetanceVariable(char *s);
void K00recoverFromError();
int K00declareClass(char *name,char *supername);
void K00toPrimitiveClass();

void *GC_malloc(size_t size);
void *sGC_malloc(size_t size);

int KClex();
int KCerror(char *s);
int KCparse();
int sendKan(int p);
void printTens(objectp op);
void K00foo1();
void K00fooPrimitive();
void repl(FILE *inFile,FILE *outFile);
int readcomment();
int readchar();
void putchar0(int c);
void printf0(char *s);
void printf1(char *s);
int isReserved(char *s);
int shouldReplace(char *s);





