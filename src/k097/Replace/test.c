/*   stackmachin.c */

#include <stdio.h>
#include "datatype.h"
#include "stackm.h"
#include "extern.h"
#include <signal.h>
#include <sys/types.h>
#include <sys/times.h>


#define OPERAND_STACK_SIZE  2000
#define SYSTEM_DICTIONARY_SIZE 200
#define USER_DICTIONARY_SIZE   1223
/* The value of USER_DICTIONARY_SIZE must be prime number, because of hashing
   method */
#define OB_ARRAY_MAX   (AGLIMIT+100)
#define ARGV_WORK_MAX  (AGLIMIT+100)
#define EMPTY (char *)NULL

/* global variables */
struct object OperandStack[OPERAND_STACK_SIZE];
int Osp = 0;   /* OperandStack pointer */
struct dictionary SystemDictionary[SYSTEM_DICTIONARY_SIZE];
int Sdp = 0;   /* SystemDictionary pointer */
struct dictionary UserDictionary[USER_DICTIONARY_SIZE];
int Udp = 0;   /* UserDictionary pointer */

int PrintDollar = 1;         /* flag for printObject() */
int PrintComma  = 1;         /* flag for printObject() */
static struct object ObjTmp; /* for poor compiler */

int StandardMacros = 1;
int StartAFile = 0;
char *StartFile;



static char *SMacros =
#include "smacro.h"

static isInteger(char *);
static strToInteger(char *);
static power(int s,int i);
static struct object pop(void);
static push(struct object);
static char *operatorType(int i);
static void pstack(void);
static struct object executableStringToExecutableArray(char *str);

/****** primitive functions *****************************************
  the values must be greater than 1. 0 is used for special purposes.*/
#define Sadd              1
#define Ssub              2
#define Smult             3
#define Sset_up_ring      4
#define Soptions          6
#define Sgroebner       7
#define Sdef              8
#define Spop              9
#define Sput              10
#define Sprint            11
#define Spstack           12
#define Sshow_ring        13
#define Sprint_options    14
#define Sshow_systemdictionary 15
#define Slength            16
#define Sfor               17
#define Sroll              18
#define Squit              19
#define Stest              20      /* this is used for test of new function*/
#define Ssyzygies        21
#define Sresolution        22
#define Sfileopen      23
#define Sclosefile 24
#define Sidiv      25
#define Sdup       26
#define Smap       27
#define Sreduction 28
#define Sreplace 29
#define SleftBrace 30       /* primitive [ */
#define SrightBrace 31      /* primitive ] */
#define Srun 32            /* run from a file */
#define Sloop 33            
#define Saload 34       
#define Sifelse 35
#define Sequal 36
#define Sexec 37
#define Sset 38
#define Sget 41
#define Scopy 43
#define Sindex 44
#define Ssystem 45
#define Shilbert 47
#define Sset_order_by_matrix 50
#define Sshow_user_dictionary 54
#define Selimination_order 55
#define Sswitch_function 58
#define Sprint_switch_status 59
#define Scat_n 62
#define Sless  63
#define Sgreater  64
#define Swritestring  66
#define Sset_timer 67
#define Sspol 68
#define Susage 69
#define Sto_records 70
#define Scoefficients 71
#define Ssystem_variable 72
#define Sdata_conversion 73
#define Sdegree 74
#define Sinit 75
#define Sload 76
#define Seval 77
#define Shomogenize 78
#define Sprincipal 79
#define Spushfile 80
/***********************************************/

struct object * newObject() 
{
  struct object *r;
  r = (struct object *)GC_malloc(sizeof(struct object));
  if (r == (struct object *)NULL) errorStackmachine("No memory\n");
  r->tag = 0;
  (r->lc).ival = 0;
  (r->rc).ival = 0;
  return(r);
}

struct object newObjectArray(size) 
int size;
{
  struct object rob;
  struct object *op;
  if (size < 0) return(NullObject);
  if (size > 0) {
    op = (struct object *)GC_malloc(size*sizeof(struct object));
    if (op == (struct object *)NULL) errorStackmachine("No memory\n");
  }else{
    op = (struct object *)NULL;
  }
  rob.tag = Sarray;
  rob.lc.ival = size;
  rob.rc.op = op;
  return(rob);
}

isNullObject(obj)
struct object obj;
{
  if (obj.tag == 0) return(1);
  else return(0);
}

int putSystemDictionary(str,ob)
char *str;   /* key */
struct object ob; /* value */
{
  int i;
  int j;
  int flag = 0;

  for (i = Sdp-1; i>=0; i--) {
    /*printf("Add %d %s\n",i,str);*/
    if (strcmp(str,(SystemDictionary[i]).key) > 0) {
      for (j=Sdp-1; j>=i+1; j--) {
	(SystemDictionary[j+1]).key = (SystemDictionary[j]).key;
	(SystemDictionary[j+1]).obj = (SystemDictionary[j]).obj;
      }
      (SystemDictionary[i+1]).key = str;
      (SystemDictionary[i+1]).obj = ob;
      flag = 1;
      break;
    }
  }
  if (!flag) { /* str is the minimum element */
    for (j=Sdp-1; j>=0; j--) {
      (SystemDictionary[j+1]).key = (SystemDictionary[j]).key;
      (SystemDictionary[j+1]).obj = (SystemDictionary[j]).obj;
    }
    (SystemDictionary[0]).key = str;
    (SystemDictionary[0]).obj = ob;
  }
  Sdp++;
  if (Sdp >= SYSTEM_DICTIONARY_SIZE) {
    warningStackmachine("No space for system dictionary area.\n");
    Sdp--;
    return(-1);
  }
  return(Sdp-1);
}

int findSystemDictionary(str)   
     /* only used for primitive functions */
     /* returns 0, if there is no item. */
     /* This function assumes that the dictionary is sorted by strcmp() */
     char *str;    /* key */
{
  int first,last,rr,middle;

  /* binary search */
  first = 0; last = Sdp-1;
  while (1) {
    if (first > last) {
      return(0);
    } else if (first == last) {
      if (strcmp(str,(SystemDictionary[first]).key) == 0) {
	return((SystemDictionary[first]).obj.lc.ival);
      }else {
	return(0);
      }
    } else if (last - first == 1) { /* This case is necessary */
      if (strcmp(str,(SystemDictionary[first]).key) == 0) {
	return((SystemDictionary[first]).obj.lc.ival);
      }else if (strcmp(str,(SystemDictionary[last]).key) == 0) {
	return((SystemDictionary[last]).obj.lc.ival);
      }else return(0);
    }

    middle = (first + last)/2;
    rr = strcmp(str,(SystemDictionary[middle]).key);
    if (rr < 0) { /* str < middle */
      last = middle;
    }else if (rr == 0) {
      return((SystemDictionary[middle]).obj.lc.ival);
    }else {       /* str > middle */
      first = middle;
    }
  }
}

int putUserDictionary(str,h0,h1,ob)
char *str;   /* key */
int h0,h1;   /* Hash values of the key */
struct object ob; /* value */
{
  int x;
  x = h0;
  if (str[0] == '\0') {
    errorKan1("%s\n","putUserDictionary(): You are defining a value with the null key.");
  }
  while (1) {
    if ((UserDictionary[x]).key == EMPTY) break;
    if (strcmp((UserDictionary[x]).key,str) == 0) break;
    x = (x+h1) % USER_DICTIONARY_SIZE;
    if (x == h0) {
      errorStackmachine("User dictionary is full. loop hashing.\n");
    }
  }
  (UserDictionary[x]).key = str;
  (UserDictionary[x]).obj = ob;
  (UserDictionary[x]).h0 = h0;
  (UserDictionary[x]).h1 = h1;
  return(x);
}

struct object findUserDictionary(str,h0,h1)   
/* returns NoObject, if there is no item. */
char *str;    /* key */
int h0,h1;    /* The hashing values of the key. */
{
  int x;
  x = h0;
  while (1) {
    if ((UserDictionary[x]).key == EMPTY) return(NoObject);
    /* if ((UserDictionary[x]).h1 != h1) return(NoObject); */
    if (strcmp((UserDictionary[x]).key,str) == 0) {
      return( (UserDictionary[x]).obj );
    }
    x = (x+h1) % USER_DICTIONARY_SIZE;
    if (x == h0) {
      errorStackmachine("User dictionary is full. loop hashing in findUserDictionary.\n");
    }
  }

}


int putPrimitiveFunction(str,number)
char *str;
int number;
{
  struct object ob;
  ob.tag = Soperator;
  ob.lc.ival = number;
  return(putSystemDictionary(str,ob));
}

struct tokens lookupTokens(t)
struct tokens t;
{
  struct object *left;
  struct object *right;
  t.object.tag = Slist;
  left = t.object.lc.op = newObject();
  right = t.object.rc.op = newObject();
  left->tag = Sinteger;
  (left->lc).ival = hash0(t.token);
  (left->rc).ival = hash1(t.token);
  right->tag = Sinteger;
  (right->lc).ival = findSystemDictionary(t.token);
  return(t);
}
  
struct object lookupLiteralString(s)
char *s; /* s must be a literal string */
{
  struct object ob;
  ob.tag = Slist;
  ob.lc.op = newObject();
  ob.rc.op = (struct object *)NULL;
  ob.lc.op->tag = Sinteger;
  (ob.lc.op->lc).ival = hash0(&(s[1]));
  (ob.lc.op->rc).ival = hash1(&(s[1]));
  return(ob);
}


int hash0(str)
char *str;
{
  int h=0;
  while (*str != '\0') {
    h = ((h*128)+(*str)) % USER_DICTIONARY_SIZE;
    str++;
  }
  return(h);
}

int hash1(str)
char *str;
{
  return(8-(str[0]%8));
}

void hashInitialize(void)
{
  int i;
  for (i=0; i<USER_DICTIONARY_SIZE; i++) {
    (UserDictionary[i]).key = EMPTY;
  }
}

static isInteger(str)
char *str;
{
  int i;
  int n;
  int start;
  
  n = strlen(str);
  if ((str[0] == '+') ||  (str[0] == '-'))
    start = 1;
  else
    start = 0;
  if (start >= n) return(0);
  
  for (i=start; i<n; i++) {
    if (('0' <= str[i]) && (str[i] <= '9')) ;
    else return(0);
  }
  return(1);
}

static strToInteger(str)
char *str;
{
  int i;
  int n;
  int r;
  int start;

  if ((str[0] == '+') || (str[0] == '-'))
    start = 1;
  else
    start = 0;
  n = strlen(str);
  r = 0;
  for (i=n-1; i>=start ; i--) {
    r += (int)(str[i]-'0') *power(10,n-1-i);
  }
  if (str[0] == '-') r = -r;
  return(r);
}

static power(s,i)
int s;
int i;
{
  if (i == 0) return 1;
  else return( s*power(s,i-1) );
}

static push(ob)
struct object ob;     
{
  OperandStack[Osp++] = ob;
  if (Osp >= OPERAND_STACK_SIZE) {
    warningStackmachine("Operand stack overflow. \n");
    Osp--;
    return(-1);
  }
  return(0);
}

static struct object pop()
{
  if (Osp <= 0) {
    return( NullObject );
  }else{
    return( OperandStack[--Osp]);
  }
}

struct object peek(k)
int k;
{
  if ((Osp-k-1) < 0) {
    return( NullObject );
  }else{
    return( OperandStack[Osp-k-1]);
  }
}

int isLiteral(str)
char *str;
{
  if (strlen(str) <2) return(0);
  else {
    if ((str[0] == '/') && (str[1] != '/')) return(1);
    else return(0);
  }
}

void printOperandStack() {
  int i;
  struct object ob;
  int vs;
  vs = VerboseStack; VerboseStack = 2;
  for (i=Osp-1; i>=0; i--) {
    fprintf(Fstack,"[%d] ",i);
    ob = OperandStack[i];
    printObject(ob,1,Fstack);
  }
  VerboseStack = vs;
}

    
void printObject(ob,nl,fp) 
struct object ob;
int nl;
FILE *fp;
/* print the object on the top of the stack. */
{

  int size;
  int i;
  struct tokens *ta;

  if (VerboseStack >= 2) {
    /*fprintf(fp,"@@@");*/
    switch (ob.tag) {
    case 0:
      fprintf(fp,"<null> "); /* null object */
      break;
    case Sinteger:
      fprintf(fp,"<integer> "); 
      break;
    case Sstring:
      fprintf(fp,"<literal-string> ");
      break;
    case Soperator:
      fprintf(fp,"<operator> ");
      break;
    case Sdollar:
      fprintf(fp,"<string(dollar)> ");
      break;
    case SexecutableArray:
      fprintf(fp,"<executable array> ");
      break;
    case Sarray:
      fprintf(fp,"<array> ");
      break;
    case SleftBraceTag:
      fprintf(fp,"<leftBraceTag> ");
      break;
    case SrightBraceTag:
      fprintf(fp,"<rightBraceTag> ");
      break;
    case Spoly:
      fprintf(fp,"<poly> ");
      break;
    case SarrayOfPOLY:
      fprintf(fp,"<arrayOfPOLY> ");
      break;
    case SmatrixOfPOLY:
      fprintf(fp,"<matrixOfPOLY> ");
      break;
    case Slist:
      fprintf(fp,"<list> ");
      break;
    case Sfile:
      fprintf(fp,"<file> ");
      break;
    case Sring:
      fprintf(fp,"<ring> ");
      break;
    default:
      fprintf(fp,"<Unknown object tag. %d >",ob.tag);
      break;
    }
  }
  switch (ob.tag) {
  case 0:
    fprintf(fp,"%%[null]"); /* null object */
    break;
  case Sinteger:
    fprintf(fp,"%d",ob.lc.ival);
    break;
  case Sstring:
    fprintf(fp,"%s",ob.lc.str);
    break;
  case Soperator:
    fprintf(fp,"%s %%[operator] ",operatorType(ob.lc.ival));
    break;
  case Sdollar:
    if (PrintDollar == 2) {
      fprintf(fp,"(%s)",ob.lc.str);
    } else if (PrintDollar == 0 ) {
      fprintf(fp,ob.lc.str);
    } else {
      fprintf(fp,"$%s$",ob.lc.str);
    }
    break;
  case SexecutableArray:
    size = ob.rc.ival;
    ta = ob.lc.tokenArray;
    fprintf(fp,"{ ");
    for (i=0; i<size; i++) {
      switch ((ta[i]).kind) {
      case ID:
	fprintf(fp,"<<ID>>%s ",(ta[i]).token);
	break;
      case EXECUTABLE_STRING:
	fprintf(fp,"<<EXECUTABLE_STRING>>{%s} ",(ta[i]).token);
	break;
      case EXECUTABLE_ARRAY:
	printObject((ta[i]).object,nl,fp);
	break;
      default:
	fprintf(fp,"Unknown token type\n");
	break;
      }
    }
    fprintf(fp," }");
    break;
  case Sarray:
    printObjectArray(ob,0,fp);
    break;
  case SleftBraceTag:
    fprintf(fp,"[ ");
    break;
  case SrightBraceTag:
    fprintf(fp,"] ");
    break;
  case Spoly:
    fprintf(fp,"%s",KPOLYToString(ob.lc.poly));
    break;
  case SarrayOfPOLY:
    fprintf(fp,"Sorry! The object arrayOfPOLY cannot be printed.");
    break;
  case SmatrixOfPOLY:
    fprintf(fp,"Sorry! The object matrixOfPOLY cannot be printed.");
    break;
  case Slist:
    printObjectList(&ob);
    break;
  case Sfile:
    fprintf(fp,"Name=%s, FILE *=%x ",ob.lc.str,(int) ob.rc.file);
    break;
  case Sring:
    fprintf(fp,"Ring."); KshowRing(KopRingp(ob));
    break;
  default:
    fprintf(fp,"[Unknown object tag.]");
    break;
  }
  if (nl) fprintf(fp,"\n");
}
  

void printObjectArray(ob,nl,fp)
struct object ob;
int nl;
FILE *fp;
{
  int size;
  int i;
  size = ob.lc.ival;
  fprintf(fp,"[  ");
  for (i=0; i<size; i++) {
    if (PrintComma && (i != 0)) {
      fprintf(fp," , ");
    }else{
      fprintf(fp,"  ");
    }
    printObject((ob.rc.op)[i],0,fp);
  }
  fprintf(fp," ] ");
  if (nl) fprintf(fp,"\n");
}

static initSystemDictionary()
 {
   /* It is recommended to sort the follows for performance */
   putPrimitiveFunction("mul",Smult);
   putPrimitiveFunction("add",Sadd);
   putPrimitiveFunction("sub",Ssub);
   putPrimitiveFunction("lt",Sless);
   putPrimitiveFunction("set",Sset);
   putPrimitiveFunction("eq",Sequal);
   putPrimitiveFunction("gt",Sgreater);
   putPrimitiveFunction("QUIT",Squit);
   putPrimitiveFunction("[",SleftBrace);
   putPrimitiveFunction("]",SrightBrace);
   putPrimitiveFunction("bye",Squit);
   putPrimitiveFunction("length",Slength);
   putPrimitiveFunction("for",Sfor);
   putPrimitiveFunction("roll",Sroll);
   putPrimitiveFunction("cat_n",Scat_n);
   putPrimitiveFunction("coefficients",Scoefficients);
   putPrimitiveFunction("copy",Scopy);
   putPrimitiveFunction("data_conversion",Sdata_conversion);
   putPrimitiveFunction("aload",Saload);
   putPrimitiveFunction("def",Sdef);
   putPrimitiveFunction("degree",Sdegree);
   putPrimitiveFunction("elimination_order",Selimination_order);
   putPrimitiveFunction("exec",Sexec);
   putPrimitiveFunction("exit",Squit);
   putPrimitiveFunction("get",Sget);
   putPrimitiveFunction("groebner",Sgroebner);
   putPrimitiveFunction("hilbert",Shilbert);
   putPrimitiveFunction("ifelse",Sifelse);
   putPrimitiveFunction("index",Sindex);
   putPrimitiveFunction("dup",Sdup);
   putPrimitiveFunction("init",Sinit);
   putPrimitiveFunction("loop",Sloop);
   putPrimitiveFunction("options",Soptions);
   putPrimitiveFunction("pop",Spop);
   putPrimitiveFunction("put",Sput);
   putPrimitiveFunction("print",Sprint);
   putPrimitiveFunction("pstack",Spstack);
   putPrimitiveFunction("print_options",Sprint_options);
   putPrimitiveFunction("print_switch_status",Sprint_switch_status);
   putPrimitiveFunction("quit",Squit);
   putPrimitiveFunction("file",Sfileopen);
   putPrimitiveFunction("closefile",Sclosefile);
   putPrimitiveFunction("idiv",Sidiv);
   putPrimitiveFunction("reduction",Sreduction);
   putPrimitiveFunction("replace",Sreplace);
   putPrimitiveFunction("resolution",Sresolution);
   putPrimitiveFunction("run",Srun);
   putPrimitiveFunction("set_order_by_matrix",Sset_order_by_matrix);
   putPrimitiveFunction("set_timer",Sset_timer);
   putPrimitiveFunction("set_up_ring@",Sset_up_ring);
   putPrimitiveFunction("show_ring",Sshow_ring);
   putPrimitiveFunction("show_systemdictionary",Sshow_systemdictionary);
   putPrimitiveFunction("show_user_dictionary",Sshow_user_dictionary);
   putPrimitiveFunction("spol",Sspol);
   putPrimitiveFunction("switch_function",Sswitch_function);
   putPrimitiveFunction("system",Ssystem);
   putPrimitiveFunction("system_variable",Ssystem_variable);
   putPrimitiveFunction("syzygies",Ssyzygies);
   putPrimitiveFunction("test",Stest);
   putPrimitiveFunction("map",Smap);
   putPrimitiveFunction("to_records",Sto_records);
   putPrimitiveFunction("Usage",Susage);
   putPrimitiveFunction("load",Sload);
   putPrimitiveFunction("writestring",Swritestring);
   putPrimitiveFunction("eval",Seval);
   putPrimitiveFunction("homogenize",Shomogenize);
   putPrimitiveFunction("principal",Sprincipal);
   putPrimitiveFunction("pushfile",Spushfile);

 }

static showSystemDictionary() {
  int i;
  int maxl;
  char format[1000];
  int nl;
  maxl = 1;
  for (i=0; i<Sdp; i++) {
    if (strlen((SystemDictionary[i]).key) >maxl)
      maxl = strlen((SystemDictionary[i]).key);
  }
  maxl += 3;
  nl = 80/maxl;
  if (nl < 2) nl = 2;
  sprintf(format,"%%-%ds",maxl);
  for (i=0; i<Sdp; i++) {
    fprintf(Fstack,format,(SystemDictionary[i]).key);
    if (i % nl == nl-1) fprintf(Fstack,"\n");
  }
  fprintf(Fstack,"\n");
}
  
static showUserDictionary() 
{
  int i,j;
  int maxl;
  char format[1000];
  int nl;
  maxl = 1;
  for (i=0; i<USER_DICTIONARY_SIZE; i++) {
    if ((UserDictionary[i]).key != EMPTY) {
      if (strlen((UserDictionary[i]).key) >maxl)
	maxl = strlen((UserDictionary[i]).key);
    }
  }
  maxl += 3;
  nl = 80/maxl;
  if (nl < 2) nl = 2;
  sprintf(format,"%%-%ds",maxl);
  for (i=0,j=0; i<USER_DICTIONARY_SIZE; i++) {
    if ((UserDictionary[i]).key != EMPTY) {
      fprintf(Fstack,format,(UserDictionary[i]).key);
      /*{ char *sss; int ii,h0,h1;
	sss = UserDictionary[i].key;
	h0 = UserDictionary[i].h0;
	h1 = UserDictionary[i].h1;
	for (ii=0; ii<strlen(sss); ii++) fprintf(Fstack,"%x ",sss[ii]);
	fprintf(Fstack,": h0=%d, h1=%d, %d\n",h0,h1,i);
      }*/
      if (j % nl == nl-1) fprintf(Fstack,"\n");
      j++;
    }
  }
  fprintf(Fstack,"\n");
}
  
static char *operatorType(type)
int type;
{ int i;
  for (i=0; i<Sdp; i++) {
    if (type == (SystemDictionary[i]).obj.lc.ival) {
      return((SystemDictionary[i]).key);
    }
  }
  return("Unknown operator");
}

static struct object executableStringToExecutableArray(s)
char *s;
{
  struct tokens *tokenArray;
  struct object ob;
  int i;
  int size;
  tokenArray = decomposeToTokens(s,&size);
  ob.tag = SexecutableArray;
  ob.lc.tokenArray = tokenArray;
  ob.rc.ival = size;
  for (i=0; i<size; i++) {
    if ( ((ob.lc.tokenArray)[i]).kind == EXECUTABLE_STRING) {
      ((ob.lc.tokenArray)[i]).kind = EXECUTABLE_ARRAY;
      ((ob.lc.tokenArray)[i]).object =
	executableStringToExecutableArray(((ob.lc.tokenArray)[i]).token);
    }
  }
  return(ob);
}
/****************  stack machine **************************/
void scanner() {
  struct tokens token;
  struct object ob;
  extern int ctrlC();
  int tmp;
  char *tmp2;
  getokenSM(INIT);
  initSystemDictionary();

  if (setjmp(EnvOfStackMachine)) {
    /* do nothing in the case of error */
  } else {
    if (signal(SIGINT,SIG_IGN) != SIG_IGN) {
      signal(SIGINT,ctrlC);
    }

    KSdefineMacros();

    if (StartAFile) {
      tmp2 = StartFile;
      StartFile = (char *)GC_malloc(sizeof(char)*(strlen(StartFile)+
						  40));
      sprintf(StartFile,"$%s$ run\n",tmp2);
      token.kind = EXECUTABLE_STRING;
      token.token = StartFile;
      executeToken(token); /* execute startup commands */
      token.kind = ID;
      token.token = "exec";
      token = lookupTokens(token); /* set hashing values */
      tmp = findSystemDictionary(token.token);
      ob.tag = Soperator;
      ob.lc.ival = tmp;
      executePrimitive(ob); /* exec */
    }

  }
  
  for (;;) {
    if (setjmp(EnvOfStackMachine)) {
      if (DebugStack >= 1) {
	fprintf(Fstack,"\nscanner> ");
      }
    } else {  }
    if (DebugStack >= 1) { printOperandStack(); }
    token = getokenSM(GET);
    if ((tmp=executeToken(token)) < 0) break;
    /***if (tmp == 1) fprintf(stderr," --- exit --- \n");*/
  }
}


int ctrlC(sig)
int sig;
{
  extern int ctrlC();
  
  signal(sig,SIG_IGN);
  /* see 133p */

  fprintf(Fstack,"User interruption by ctrl-C. We are in the top-level.\n");
  fprintf(Fstack,"Type in quit in order to exit sm1.\n");
  /*fprintf(Fstack,"Warning! The handler of ctrl-C has a bug, so you might have a core-dump.\n");*/
  /*
    $(x0+1)^50$ $x1 x0 + x1^20$ 2 groebner_n
    ctrl-C
    $(x0+1)^50$ $x1 x0 + x1^20$ 2 groebner_n
    It SOMETIMES makes core dump.
  */
  getokenSM(INIT); /* It might fix the bug above. 1992/11/14 */
  signal(SIGINT,ctrlC);
  longjmp(EnvOfStackMachine,1);
}

int executeToken(token)
struct tokens token;
{      
  struct object ob;
  int primitive;
  int size;
  int status;
  struct tokens *tokenArray;
  int i,h0,h1;
  
  if (token.kind == DOLLAR) {
    ob.tag = Sdollar;
    ob.lc.str = token.token;
    push(ob);
  } else if (token.kind == ID) {  /* ID */

    if (strcmp(token.token,"exit") == 0) return(1);
    /* "exit" is not primitive here. */

    if (isLiteral(token.token)) {
      /* literal object */
      ob.tag = Sstring;
      ob.lc.str = (char *)GC_malloc((strlen(token.token)+1)*sizeof(char));
      if (ob.lc.str == (char *)NULL) errorStackmachine("No space.");
      strcpy(ob.lc.str, &((token.token)[1]));

      if (token.object.tag != Slist) {
	fprintf(Fstack,"\n%%Warning: The hashing values for the <<%s>> are not set.\n",token.token);
	token.object = lookupLiteralString(token.token);
      }
      ob.rc.op = token.object.lc.op;
      push(ob);
    } else if (isInteger(token.token)) {
      /* integer object */
      ob.tag = Sinteger ;
      ob.lc.ival = strToInteger(token.token);
      push(ob);
    } else {
      if (token.object.tag != Slist) {
	fprintf(Fstack,"\n%%Warning: The hashing values for the <<%s>> are not set.\n",token.token);
	token = lookupTokens(token);
      }
      h0 = ((token.object.lc.op)->lc).ival;
      h1 = ((token.object.lc.op)->rc).ival;
      ob = findUserDictionary(token.token,h0,h1);
      primitive = ((token.object.rc.op)->lc).ival;
      if (ob.tag >= 0) {
	/* there is a definition in the user dictionary */
	if (ob.tag == SexecutableArray) {
	  tokenArray = ob.lc.tokenArray;
	  size = ob.rc.ival;
	  for (i=0; i<size; i++) {
	    status = executeToken(tokenArray[i]);
	    if (status != 0) return(status);
	  }
	}else {
	  push(ob);
	}
      } else if (primitive) { 
	/* system operator */
	ob.tag = Soperator;
	ob.lc.ival = primitive;
	return(executePrimitive(ob));
      } else {
	fprintf(Fstack,"\n%%Warning: The identifier <<%s>> is not in the system dictionary\n%%   nor in the user dictionary. Push NullObject.\n",token.token);
	/*fprintf(Fstack,"(%d,%d)\n",h0,h1);*/
	push(NullObject); 
      }
    }
  } else if (token.kind == EXECUTABLE_STRING) {
    push(executableStringToExecutableArray(token.token));
  } else if (token.kind == EXECUTABLE_ARRAY) {
    push(token.object);
  } else if ((token.kind == -1) || (token.kind == -2)) { /* eof token */
    return(-1);
  } else {
    /*fprintf(Fstack,"\n%%Error: Unknown token type\n");***/
    fprintf(stderr,"\nUnknown token type = %d\n",token.kind);
    fprintf(stderr,"\ntype in ctrl-\\ if you like to make core-dump.\n");
    fprintf(stderr,"If you like to continue, type in RETURN key.\n");
    fprintf(stderr,"The author expects the bug report.\n");
    getchar();
    errorStackmachine("Error: Unknown token type.\n");
    /**return(-2); /* exit */
  }
  return(0); /* normal exit */
}   

int executePrimitive(ob) 
struct object ob;
{
  struct object ob1;
  struct object ob2;
  struct object ob3;
  struct object ob4;
  struct object ob5;
  struct object rob;
  struct object obArray[OB_ARRAY_MAX];
  struct object obArray2[OB_ARRAY_MAX];
  int size;
  int i,j,k,n;
  int status;
  struct tokens *tokenArray;
  struct tokens token;
  FILE *fp;
  char *fname;
  int rank;
  struct object oMat;
  static int timerStart = 1;
  static struct tms before, after;
  struct object oInput;
  char *str;
  extern int KeepInput;
  extern int History;
  extern struct ring *CurrentRingp;

  if (DebugStack >= 2) {
    fprintf(Fstack,"In execute\n"); printOperandStack();
  }
  
  switch (ob.lc.ival) {
    /* Postscript primitives :stack */
  case Spop:
    ob1 = pop();
    break;

  case Sdup:
    ob1 = pop();
    push(ob1); push(ob1);
    break;
  case Scopy:  /* copy values. cf. dup */
    ob1 = pop();
    switch(ob1.tag) {
    case Sinteger: break;
    default: errorStackmachine("Usage:copy");
    }
    size = ob1.lc.ival;
    k = 0;
    for (i=size-1; i>=0; i--) {
      ob2 = peek(i+k);
      switch(ob2.tag) {
      case Sdollar: /* copy by value */
	str = (char *)GC_malloc(strlen(ob2.lc.str)+3);
	if (str == (char *)NULL) errorStackmachine("No memory (copy)");
	strcpy(str,ob2.lc.str);
	push(KpoString(str));
	break;
      case Spoly:
	errorStackmachine("no pCopy (copy)");
	break;
      case Sarray:
	n = ob2.lc.ival;
	ob3 = newObjectArray(n);
	for (j=0; j<n; j++) {
	  putoa(ob3,j,getoa(ob2,j));
	}
	push(ob3);
	break;
      default:
	push(ob2);
	break;
      }
      k++;
    }
    break;
  case Sroll:
    ob1 = pop();
    ob2 = pop();
    switch(ob1.tag) {
    case Sinteger:
      j = ob1.lc.ival;
      break;
    default: errorStackmachine("Usage:roll");
    }
    switch(ob2.tag) {
    case Sinteger:
      n = ob2.lc.ival;
      break;
    default: errorStackmachine("Usage:roll");
    }
    for (i=0; i<n; i++) {
      if (i < OB_ARRAY_MAX) {
	obArray[i] = pop();
      }else{
	errorStackmachine("exceeded OB_ARRAY_MAX (roll)\n");
      }
    }
    for (i=0; i<n; i++) {
      k = (j-1)%n;
      k = (k>=0?k: k+n);
      push(obArray[k]);
      j--;
    }
    break;
  case Spstack:
    printOperandStack();
    break;

    /* Postscript primitives :arithmetic */
  case Sadd:
    ob1 = pop();
    ob2 = pop();
    rob = KooAdd(ob1,ob2);
    push(rob);
    break;
  case Ssub:
    ob2 = pop();
    ob1 = pop();
    rob = KooSub(ob1,ob2);
    push(rob);
    break;
  case Smult:
    ob2 = pop();
    ob1 = pop();
    rob = KooMult(ob1,ob2);
    push(rob);
    break;
  case Sidiv:
    ob2 = pop(); ob1 = pop();
    rob = KooDiv(ob1,ob2);
    push(rob);
    break;

    /* Postscript primitives :array */
  case SleftBrace:
    rob.tag = SleftBraceTag;
    push(rob);
    break;
    
  case SrightBrace:
    size = 0;
    ob1 = peek(size);
    while (!(Osp-size-1 < 0)) { /* while the stack is not underflow */
      if (ob1.tag == SleftBraceTag) {
	rob = newObjectArray(size);
	for (i=0; i<size; i++) {
	  (rob.rc.op)[i] = peek(size-1-i);
	}
	for (i=0; i<size+1; i++) {
	  pop();
	}
	break;
      }
      size++;
      ob1 = peek(size);
    }
    push(rob);
    break;
    
  case Sget:
    /* [a_0 ... a_{n-1}] i   get a_i */
    /* ob2               ob1 get     */
    ob1 = pop();
    ob2 = pop();
    switch(ob2.tag) {
    case Sarray: break;
    default: errorStackmachine("Usage:get");
    }
    switch(ob1.tag) {
    case Sinteger: break;
    default: errorStackmachine("Usage:get");
    }
    i =ob1.lc.ival;
    size = getoaSize(ob2);
    if ((0 <= i) && (i<size)) {
      push(getoa(ob2,i));
    }else{
      errorStackmachine("Index is out of bound. (get)\n");
    }
    break;

  case Sput:
    /* [a_0 ... a_{n-1}] index any put */
    /* ob3               ob2   ob1 put     */
    ob1 = pop(); ob2 = pop(); ob3 = pop();
    switch(ob2.tag) {
    case Sinteger: break;
    default: errorStackmachine("Usage:put");
    }
    switch(ob3.tag) {
    case Sarray:
      i = ob2.lc.ival;
      size = getoaSize(ob3);
      if ((0 <= i) && (i<size)) {
	getoa(ob3,i) = ob1;
      }else{
	errorStackmachine("Index is out of bound. (put)\n");
      }
      break;
    case Sdollar:
      i = ob2.lc.ival;
      size = strlen(ob3.lc.str);
      if ((0 <= i) && (i<size)) {
	if (ob1.tag == Sdollar) {
	  (ob3.lc.str)[i] = (ob1.lc.str)[0];
	}else{
	  (ob3.lc.str)[i] = ob1.lc.ival;
	}
      }else{
	errorStackmachine("Index is out of bound. (put)\n");
      }
      break;
    default: errorStackmachine("Usage:put");
    }
    break;

  case Sindex:
    ob1 = pop();
    switch(ob1.tag) {
    case Sinteger: break;
    default: errorStackmachine("Usage:index");
    }
    size = ob1.lc.ival;
    push(peek(size-1));
    break;

  case Saload:
    /* [a1 a2 ... an] aload a1 a2 ... an [a1 ... an] */
    ob1 = pop();
    switch(ob1.tag) {
    case Sarray: break;
    default:
      errorStackmachine("Usage:aload");
    }
    size = getoaSize(ob1);
    for (i=0; i<size; i++) {
      push(getoa(ob1,i));
    }
    push(ob1);
    
    break;

  case Slength:
    /* [a_0 ... a_{n-1}] length n */
    /*               ob1 length rob */
    ob1 = pop();
    switch(ob1.tag) {
    case Sarray:
      size = getoaSize(ob1);
      push(KpoInteger(size));
      break;
    case Sdollar:
      push(KpoInteger(strlen(ob1.lc.str)));
      break;
    default: errorStackmachine("Usage:length");
    }
    break;

    /* Postscript primitives :relation */
  case Sequal:
    /* obj1 obj2 == bool */
    ob2 = pop();
    ob1 = pop();
    if(KooEqualQ(ob1,ob2)) {
      push(KpoInteger(1));
    }else{
      push(KpoInteger(0));
    }
    break;

  case Sless:
    /* obj1 obj2 < bool */
    ob2 = pop();
    ob1 = pop();
    push(KooLess(ob1,ob2));
    break;

  case Sgreater:
    /* obj1 obj2 < bool */
    ob2 = pop();
    ob1 = pop();
    push(KooGreater(ob1,ob2));
    break;


    /* Postscript primitives :controle */
  case Sloop:
    /* { .... exit .....} loop */
    ob1 = pop();
    switch(ob1.tag) {
    case SexecutableArray: break;
    default:
      errorStackmachine("Usage:loop");
      break;
    }
    tokenArray = ob1.lc.tokenArray;
    size = ob1.rc.ival;
    i = 0;
    while (1) {
      token = tokenArray[i];
      /***printf("[token %d]%s\n",i,token.token);*/
      i++;
      if (i >= size) i=0;
      status = executeToken(token);
      if (status != 0) break;
      /* here, do not return 1. Do not propagate exit signal outside of the
	 loop. */
    }
    break;

  case Sfor:
    /* init inc limit { } for */
    /* ob4  ob3 ob2   ob1 */
    ob1 =pop(); ob2 = pop(); ob3 = pop(); ob4 = pop();
    switch(ob1.tag) {
    case SexecutableArray: break;
    default: errorStackmachine("Usage:for");
    }
    switch(ob2.tag) {
    case Sinteger: break;
    default:
      errorStackmachine("Usage:for The 3rd argument must be integer.");
    }
    switch(ob3.tag) {
    case Sinteger: break;
    default: errorStackmachine("Usage:for The 2nd argument must be integer.");
    }
    switch(ob4.tag) {
    case Sinteger: break;
    default: errorStackmachine("Usage:for The 1st argument must be integer.");
    }
    {
      int i,lim,inc,j;
      i = ob4.lc.ival;
      lim = ob2.lc.ival;
      inc = ob3.lc.ival;
      if (inc > 0) {
        /*
	if (lim < i) errorStackmachine("The initial value must not be greater than limit value (for).\n"); 
        */
	for ( ; i<=lim; i += inc) {
	  push(KpoInteger(i));
	  tokenArray = ob1.lc.tokenArray;
	  size = ob1.rc.ival;
	  for (j=0; j<size; j++) {
	    status = executeToken(tokenArray[j]);
	    if (status) goto xyz;
	  }
	}
      }else{
        /*
	if (lim > i) errorStackmachine("The initial value must not be less than limit value (for).\n");
        */
	for ( ; i>=lim; i += inc) {
	  push(KpoInteger(i));
	  tokenArray = ob1.lc.tokenArray;
	  size = ob1.rc.ival;
	  for (j=0; j<size; j++) {
	    status = executeToken(tokenArray[j]);
	    if (status) goto xyz;
	  }
	}
      }
      xyz:  ;
    }
    break;

  case Smap:
    ob2 = pop(); ob1 = pop();
    switch(ob1.tag) {
    case Sarray: break;
    default:
      errorStackmachine("Usage:map The 1st argument must be an array.");
      break;
    }
    switch(ob2.tag) {
    case SexecutableArray: break;
    default:
      errorStackmachine("Usage:map The 2nd argument must be an executable array.");
      break;
    }
    { int osize,size;
      int i,j;
      osize = getoaSize(ob1);

      /*KSexecuteString("[");*/
      rob.tag = SleftBraceTag;
      push(rob);

      for (i=0; i<osize; i++) {
	push(getoa(ob1,i));
	tokenArray = ob2.lc.tokenArray;
	size = ob2.rc.ival;
	for (j=0; j<size; j++) {
	  status = executeToken(tokenArray[j]);
	  if (status) goto foor;
	}
      }
    foor: ;
      /*KSexecuteString("]");*/
      {
	size = 0;
	ob1 = peek(size);
	while (!(Osp-size-1 < 0)) { /* while the stack is not underflow */
	  if (ob1.tag == SleftBraceTag) {
	    rob = newObjectArray(size);
	    for (i=0; i<size; i++) {
	      (rob.rc.op)[i] = peek(size-1-i);
	    }
	    for (i=0; i<size+1; i++) {
	      pop();
	    }
	    break;
	  }
	  size++;
	  ob1 = peek(size);
	}
	push(rob);
      }
    }
    break;


  case Sifelse:
    /* bool { } { } ifelse */
    ob1 = pop();
    ob2 = pop();
    ob3 = pop();
    switch (ob1.tag) {
    case SexecutableArray: break;
    default: errorStackmachine("Usage:ifelse");
    }
    switch (ob2.tag) {
    case SexecutableArray: break;
    default: errorStackmachine("Usage:ifelse");
    }
    switch (ob3.tag) {
    case Sinteger: break;
    default: errorStackmachine("Usage:ifelse");
    }
    if (ob3.lc.ival) {
      /* execute ob2 */
      ob1 = ob2;
    }
    /* execute ob1 */
    tokenArray = ob1.lc.tokenArray;
    size = ob1.rc.ival;
    for (i=0; i<size; i++) {
      token = tokenArray[i];
      status = executeToken(token);
      if (status != 0) return(status);
    }
    
    break;

  case Sexec:
    /* { .........} exec */
    ob1 = pop();
    switch(ob1.tag) {
    case SexecutableArray: break;
    default: errorStackmachine("Usage:exec");
    }
    tokenArray = ob1.lc.tokenArray;
    size = ob1.rc.ival;
    for (i=0; i<size; i++) {
      token = tokenArray[i];
      /***printf("[token %d]%s\n",i,token.token);*/
      status = executeToken(token);
      if (status != 0) break;
    }
    break;

   /* Postscript primitives :dictionary */    
  case Sdef:
    ob2 = pop();
    ob1 = pop();
    /* type check */
    switch(ob1.tag) {
    case Sstring: break;
    default:
      errorStackmachine("Usage:def");
      break;
    }
    putUserDictionary(ob1.lc.str,(ob1.rc.op->lc).ival,
		      (ob1.rc.op->rc).ival,ob2);
    break;
    
  case Sload:
    ob1 = pop();
    switch(ob1.tag) {
    case Sstring: break;
    default: errorStackmachine("Usage:load");
    }
    ob1 =  findUserDictionary(ob1.lc.str,
			     (ob1.rc.op->lc).ival,
			     (ob1.rc.op->rc).ival);
    if (ob1.tag == -1) push(NullObject);
    else push(ob1);
    
    break;

  case Sset:
    ob1 = pop();
    ob2 = pop();
    switch(ob1.tag) {
    case Sstring: break;
    default: errorStackmachine("Usage:set");
    }
    putUserDictionary(ob1.lc.str,(ob1.rc.op->lc).ival,
		      (ob1.rc.op->rc).ival,ob2);
    break;

  case Sshow_systemdictionary:
    fprintf(Fstack,"------------- system dictionary -------------------\n");
    showSystemDictionary();
    break;

  case Sshow_user_dictionary:
    showUserDictionary("");
    break;



    /* Postscript primitives : convert */
  case Sdata_conversion:
    ob2 = pop();
    ob1 = pop();
    switch(ob2.tag) {
    case Sdollar: break;
    default: errorStackmachine("Usage:data_conversion");
    }
    rob = KdataConversion(ob1,ob2.lc.str);
    push(rob);
    break;
    

    /* Postscript ptimitives :file */
  case Srun:
    ob1 = pop();
    switch(ob1.tag) {
    case Sdollar: break;
    case Sstring: break;
    default:
      errorStackmachine("Usage:run");
      break;
    }
    getokenSM(OPEN,ob1.lc.str);  /* open the file, $filename$ run */
    break;

  case Sprint:
    ob1 = pop();
    printObject(ob1,0,Fstack);
    break;

  case Sfileopen: /* filename  mode   file  descripter */
              /* ob2       ob1  */
    ob1 = pop();
    ob2 = pop();
    switch(ob1.tag) {
    case Sdollar: break;
    default: errorStackmachine("Usage:file");
    }
    switch(ob2.tag) {
    case Sdollar: break;
    default:errorStackmachine("Usage:file");
    }
    rob = NullObject;
    if (strcmp(ob2.lc.str,"%stdin") == 0) {
      rob.tag = Sfile; rob.lc.str="%stdin"; rob.rc.file = stdin;
    }else if (strcmp(ob2.lc.str,"%stdout") == 0) {
      rob.tag = Sfile; rob.lc.str="%stdout"; rob.rc.file = stdout;
    }else if (strcmp(ob2.lc.str,"%stderr") == 0) {
      rob.tag = Sfile; rob.lc.str="%stderr"; rob.rc.file = stderr;
    }else if ( (rob.rc.file = fopen(ob2.lc.str,ob1.lc.str)) != (FILE *)NULL) {
      rob.tag = Sfile; rob.lc.str = ob2.lc.str;
    }else {
      errorStackmachine("I cannot open the file."); 
    }
    push(rob);
    break;


  case Swritestring:
    /* file string writestring
       ob2  ob1
    */
    ob1 = pop();
    ob2 = pop();
    switch(ob2.tag) {
    case Sfile: break;
    default: errorStackmachine("Usage:writestring");
    }
    switch(ob1.tag) {
    case Sdollar: break;
    default:  errorStackmachine("Usage:writestring");
    }
    fprintf(ob2.rc.file,"%s",ob1.lc.str);
    break;

  case Sclosefile:
    ob1 = pop();
    switch(ob1.tag) {
    case Sfile: break;
    default: errorStackmachine("Usage:closefile");
    }
    if (fclose(ob1.rc.file) == EOF) {
      errorStackmachine("I couldn't close the file.\n");
    }
    break;

  case Spushfile: /* filename pushfile  string */
                  /* ob2       */
    ob2 = pop();
    switch(ob2.tag) {
    case Sdollar: break;
    default:errorStackmachine("Usage:pushfile");
    }
    rob = NullObject;
    if (strcmp(ob2.lc.str,"%stdin") == 0) {
      ob1.tag = Sfile; ob1.lc.str="%stdin"; ob1.rc.file = stdin;
    }else if (strcmp(ob2.lc.str,"%stdout") == 0) {
      ob1.tag = Sfile; ob1.lc.str="%stdout"; ob1.rc.file = stdout;
    }else if (strcmp(ob2.lc.str,"%stderr") == 0) {
      ob1.tag = Sfile; ob1.lc.str="%stderr"; ob1.rc.file = stderr;
    }else if ( (ob1.rc.file = fopen(ob2.lc.str,"r")) != (FILE *)NULL) {
      ob1.tag = Sfile; ob1.lc.str = ob2.lc.str;
    }else {
      errorStackmachine("I cannot open the file."); 
    }

    /* read the strings
    */
    n = 256; j=0;
    rob.tag = Sdollar; rob.lc.str = (char *) GC_malloc(sizeof(char)*n);
    if (rob.lc.str == (char *)NULL) errorStackmachine("No more memory.");
    while ((i = fgetc(ob1.rc.file)) != EOF) {
      if (j >= n-1) {
	n = 2*n;
	if (n <= 0) errorStackmachine("Too large file to put on the stack.");
        str = (char *)GC_malloc(sizeof(char)*n);
	if (str == (char *)NULL) errorStackmachine("No more memory.");
	for (k=0; k< n/2; k++) str[k] = (rob.lc.str)[k];
	rob.lc.str = str;
      }
      (rob.lc.str)[j] = i; (rob.lc.str)[j+1] = '\0';
      j++;
    }
      
    fclose(ob1.rc.file);
    push(rob);
    break;

    /* Postscript primitives :misc */
  case Squit:
    Kclose(); stackmachine_close();
    exit(0);
    break;

  case Ssystem:
    ob1 = pop();
    switch(ob1.tag) {
    case Sdollar: break;
    case Sstring: break;
    default: errorStackmachine("Usage:system");
    }
    system( ob1.lc.str );
    break;

  case Scat_n:
    ob1 = pop();
    switch(ob1.tag) {
    case Sinteger: break;
    default: errorStackmachine("Usage:cat_n");
    }
    size = ob1.lc.ival;
    k = 0;
    for (i=size-1; i>=0; i--) {
      ob2 = peek(i);
      switch(ob2.tag) {
      case Sdollar: break;
      default:	errorStackmachine("Usage:cat_n");
      }
      k += strlen(ob2.lc.str);
    }
    ob1.tag = Sdollar;
    ob1.lc.str = (char *)GC_malloc(sizeof(char)*(k+1));
    if (ob1.lc.str == (char *)NULL) {
      errorStackmachine("No more memory.\n");
    }
    /* concatnate */
    k = 0;
    for (i=size-1; i>=0; i--) {
      ob2 = peek(i);
      strcpy(&((ob1.lc.str)[k]),ob2.lc.str);
      k = strlen(ob1.lc.str);
    }
    /* clear the arguments */
    for (i=size-1; i>=0; i--) {
      ob2 = pop();
    }
    push(ob1);
    break;

  case Sset_timer:
    /* 118p */
    if (timerStart) {
      times(&before);
      timerStart = 0;
    }else{
      times(&after);
      printf("User time: %f seconds, System time: %f seconds\n",
	     ((double)(after.tms_utime - before.tms_utime)) /60.0,
	     ((double)(after.tms_stime - before.tms_stime)) /60.0);
      timerStart = 1;
    }
    break;

  case Susage:
    ob1 = pop();
    Kusage(ob1);
    break;

  case Sto_records:
    ob1 = pop();
    switch(ob1.tag) {
    case Sdollar: break;
    default:  errorStackmachine("Usage:to_records");
    }
    ob2 = KtoRecords(ob1);
    size = getoaSize(ob2);
    for (i=0; i<size; i++) {
      push(getoa(ob2,i));
    }
    rob.tag = Sinteger;
    rob.lc.ival = size;
    push(rob);
    break;

  case Ssystem_variable:
    ob1 = pop();
    switch(ob1.tag) {
    case Sarray: break;
    default: errorStackmachine("Usage:system_variable");
    }
    push(KsystemVariable(ob1));
    break;

    /* kan primitives :kan :ring */
  case Sset_order_by_matrix:
    ob1 = pop();
    KsetOrderByObjArray(ob1);
    break;
  case Sset_up_ring:
    ob5 = pop(); ob4=pop(); ob3=pop(); ob2=pop(); ob1=pop();
    KsetUpRing(ob1,ob2,ob3,ob4,ob5);
    break;
  case Sshow_ring:
    KshowRing(CurrentRingp);
    break;
  case Sswitch_function:
    ob1 = pop();
    ob2 = pop();
    KswitchFunction(ob2,ob1);
    break;
  case Sprint_switch_status:
    KprintSwitchStatus();
    break;
  case Sreplace:
    ob2 = pop();
    ob1 = pop();
    push(KoReplace(ob1,ob2));
    break;
    
  case Scoefficients:
    ob2 = pop();
    ob1 = pop();
    push(Kparts(ob1,ob2));
    break;

  case Sdegree:
    ob2 = pop();
    ob1 = pop();
    push(Kdegree(ob1,ob2));
    break;
  case Sspol:
    ob2 = pop();
    ob1 = pop();
    push(Ksp(ob1,ob2));
    break;

  case Seval:
    ob1 = pop();
    push(Keval(ob1));
    break;

  case Sreduction:
    ob2 = pop();
    ob1 = pop();
    push(Kreduction(ob1,ob2));
    break;
    
  case Sgroebner :
    ob1 = pop();
    push(Kgroebner(ob1));
    break;

  case Shomogenize :
    ob1 = pop();
    push(homogenizeObject(ob1,&i));
    break;

  case Sprincipal :
    ob1 = pop();
    push(oPrincipalPart(ob1));
    break;

  case Sinit:
    ob1 = pop();
    push(Khead(ob1));
    break;
    
  case Stest:
    /* test is used for a test of a new function. */
    ob1 = pop();
    push(test(ob1));
    /*
    {

    }
    */
    break;
    
    
  default:
    errorStackmachine("Unknown Soperator type. \n");
  }
  return(0);  /* normal exit */
}

    

      

errorStackmachine(str)
char *str;
{
  int i,j,k;
  static char *u="Usage:";
  char message0[1024];
  char *message;
  message = message0;
  i = 0;
  while (i<6 && str[i]!='0') {
    if (str[i] != u[i]) break;
    i++;
  }
  if (i==6) {
    fprintf(stderr,"ERROR(sm): \n");
    while (str[i] != '\0' && str[i] != ' ') {
      i++;
    }
    if (str[i] == ' ') {
      fprintf(stderr,"  %s\n",&(str[i+1]));
      k = 0;
      if (i-6 > 1022) message = (char *)GC_malloc(sizeof(char)*i);
      for (j=6; j<i ; j++) {
	message[k] = str[j];
	message[k+1] = '\0';
	k++;
      }
      Kusage2(stderr,message);
    }else{
      Kusage2(stderr,&(str[6]));
    }
  }else {
    fprintf(stderr,"ERROR(sm): ");
    fprintf(stderr,str);
  }
  fprintf(stderr,"\n");
  longjmp(EnvOfStackMachine,1);
}

warningStackmachine(str)
char *str;
{
  fprintf(stderr,"WARNING(sm): ");
  fprintf(stderr,str);
  return(0);
}


/* exports */
KSexecuteString(s)
char *s;
{
  struct tokens token;
  struct object ob;
  int tmp;

  
  if (setjmp(EnvOfStackMachine)) {
    return(-1);
  }else{
    token.token = s;
    token.kind = EXECUTABLE_STRING;
    executeToken(token);
    token.kind = ID;
    token.token = "exec";
    token = lookupTokens(token); /* no use */
    tmp = findSystemDictionary(token.token);
    ob.tag = Soperator;
    ob.lc.ival = tmp;
    executePrimitive(ob);
    return(0);
  }
}

KSdefineMacros() {
  struct tokens token;
  int tmp;
  struct object ob;

  if (setjmp(EnvOfStackMachine)) {
    return(-1);
  }else{
    if (StandardMacros && (strlen(SMacros))) {
      token.kind = EXECUTABLE_STRING;
      token.token = SMacros;
      executeToken(token); /* execute startup commands */
      token.kind = ID;
      token.token = "exec";
      token = lookupTokens(token); /* no use */
      tmp = findSystemDictionary(token.token);
      ob.tag = Soperator;
      ob.lc.ival = tmp;
      executePrimitive(ob); /* exec */
    }
    return(0);
  }
}

void KSstart() {
  stackmachine_init(); KinitKan();
  getokenSM(INIT); initSystemDictionary();
  KSdefineMacros();
  /* setjmp(EnvOfStackMachine) */
}

void KSstop() {
  Kclose(); stackmachine_close();
}


struct object KSpop() {
  return(pop());
}

void KSpush(ob)
struct object ob;
{
  push(ob);
}
