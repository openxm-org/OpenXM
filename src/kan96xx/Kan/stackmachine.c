/* $OpenXM: OpenXM/src/kan96xx/Kan/stackmachine.c,v 1.43 2018/09/07 00:15:44 takayama Exp $ */
/*   stackmachin.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "datatype.h"
#include "stackm.h"
#include "extern.h"
#include "gradedset.h"
#include "kclass.h"
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include "mysig.h"

/* The msys2 seems to make a buffer overflow of  EnvOfStackmachine[].
The code
[(x) ring_of_differential_operators 11] define_ring
( Dx*(x+Dx) ) /ff set
causes the segfault because Mp_zero is borken. Is it a bug of msys2?
Anyway, the following definition seems to be a workaround. 2015.09
Singnals do not work properly on msys2. (gcc -dM -E ... to see macros defs)
See stackm.h
*/

/* #define OPERAND_STACK_SIZE  2000 */
#define OPERAND_STACK_SIZE 30000 
#define SYSTEM_DICTIONARY_SIZE 200
/* #define USER_DICTIONARY_SIZE   1223, 3581, 27449 */
#define USER_DICTIONARY_SIZE  59359
/* The value of USER_DICTIONARY_SIZE must be prime number, because of hashing
   method */
#define ARGV_WORK_MAX  (AGLIMIT+100)
#define EMPTY (char *)NULL


/* global variables */
struct object StandardStackA[OPERAND_STACK_SIZE];
int StandardStackP = 0;
int StandardStackMax = OPERAND_STACK_SIZE;
struct operandStack StandardStack;
/* Initialization of operandStack will be done in initSystemDictionary(). */
#define ERROR_STACK_SIZE 100
struct object ErrorStackA[ERROR_STACK_SIZE];
int ErrorStackP = 0;
int ErrorStackMax = ERROR_STACK_SIZE;
struct operandStack ErrorStack;
/* Initialization of ErrorStack will be done in initSystemDictionary(). */

struct operandStack *CurrentOperandStack = &StandardStack;
struct object *OperandStack = StandardStackA;
int Osp = 0;   /* OperandStack pointer */
int OspMax = OPERAND_STACK_SIZE;

struct dictionary SystemDictionary[SYSTEM_DICTIONARY_SIZE];
int Sdp = 0;   /* SystemDictionary pointer */
struct dictionary UserDictionary[USER_DICTIONARY_SIZE];

struct context StandardContext ;
/* Initialization of StructContext will be done in initSystemDictionary(). */
/* hashInitialize is done in global.c (initStackmachine()) */
struct context *StandardContextp = &StandardContext;
struct context *CurrentContextp = &StandardContext;
struct context *PrimitiveContextp = &StandardContext;


static struct object ObjTmp = OINIT; /* for poor compiler */

int Calling_ctrlC_hook = 0;

int StandardMacros = 1;
int StartAFile = 0;
char *StartFile;

int StartAString = 0;
char *StartString;

char *GotoLabel = (char *)NULL;
int GotoP = 0;

static char *SMacros =
#include "smacro.h"

static int isInteger(char *);
static int strToInteger(char *);
static int power(int s,int i);
static void pstack(void);
static struct object executableStringToExecutableArray(char *str);
static int isThereExecutableArrayOnStack(int n);


extern int SerialCurrent;
extern int QuoteMode;

int SGClock = 0;
int UserCtrlC = 0;
int OXlock = 0;
int OXlockSaved = 0;

char *UD_str;
int  UD_attr;

struct object *MsgStackTraceInArrayp = NULL;
char *MsgStackTrace = NULL;
char *MsgSourceTrace = NULL;

struct object * newObject() 
{
  struct object *r;
  r = (struct object *)sGC_malloc(sizeof(struct object));
  if (r == (struct object *)NULL) errorStackmachine("No memory\n");
  r->tag = 0;
  (r->lc).ival = 0;
  (r->rc).ival = 0;
  r->attr = NULL;
  return(r);
}

struct object newObjectArray(size) 
     int size;
{
  struct object rob = OINIT;
  struct object *op;
  if (size < 0) return(NullObject);
  if (size > 0) {
    op = (struct object *)sGC_malloc(size*sizeof(struct object));
    if (op == (struct object *)NULL) errorStackmachine("No memory\n");
  }else{
    op = (struct object *)NULL;
  }
  rob.tag = Sarray;
  rob.lc.ival = size;
  rob.rc.op = op;
  return(rob);
}

int isNullObject(obj)
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

int putUserDictionary(str,h0,h1,ob,dic)
     char *str;   /* key */
     int h0,h1;   /* Hash values of the key */
     struct object ob; /* value */
     struct dictionary *dic;
{
  int x,r;
  extern int Strict2;
  x = h0;
  if (str[0] == '\0') {
    errorKan1("%s\n","putUserDictionary(): You are defining a value with the null key.");
  }
  while (1) {
    if ((dic[x]).key == EMPTY) break;
    if (strcmp((dic[x]).key,str) == 0) break;
    x = (x+h1) % USER_DICTIONARY_SIZE;
    if (x == h0) {
      errorStackmachine("User dictionary is full. loop hashing.\n");
    }
  }
  r = x;
  if (Strict2) {
    switch(((dic[x]).attr) & (PROTECT | ABSOLUTE_PROTECT)) {
    case PROTECT:
      r = -PROTECT;   /* Protected, but we rewrite it. */
      break;
    case ABSOLUTE_PROTECT:
      r = -ABSOLUTE_PROTECT;  /* Protected and we do not rewrite it. */
      return(r);
    default:
      /* (dic[x]).attr = 0; */ /* It is not necesarry, I think. */
      break;
    }
  }
  (dic[x]).key = str;
  (dic[x]).obj = ob;
  (dic[x]).h0 = h0;
  (dic[x]).h1 = h1;
  return(r);
}

struct object KputUserDictionary(char *str,struct object ob)
{
  int r;
  r = putUserDictionary(str,hash0(str),hash1(str),ob,CurrentContextp->userDictionary);
  return(KpoInteger(r));
}

struct object findUserDictionary(str,h0,h1,cp)   
     /* returns NoObject, if there is no item. */
     char *str;    /* key */
     int h0,h1;    /* The hashing values of the key. */
     struct context *cp;
	 /* Set char *UD_str, int UD_attr (attributes) */
{
  int x;
  struct dictionary *dic;
  extern char *UD_str;
  extern int UD_attr;
  UD_str = NULL; UD_attr = -1;
  dic = cp->userDictionary;
  x = h0;
  while (1) {
    if ((dic[x]).key == EMPTY) { break; }
    if (strcmp((dic[x]).key,str) == 0) {
	  UD_str = (dic[x]).key; UD_attr = (dic[x]).attr;
      return( (dic[x]).obj );
    }
    x = (x+h1) % USER_DICTIONARY_SIZE;
    if (x == h0) {
      errorStackmachine("User dictionary is full. loop hashing in findUserDictionary.\n");
    }
  }
  if (cp->super == (struct context *)NULL) return(NoObject);
  else return(findUserDictionary(str,h0,h1,cp->super));

}

struct object KfindUserDictionary(char *str) {
  return(findUserDictionary(str,hash0(str),hash1(str),CurrentContextp));
}

int putUserDictionary2(str,h0,h1,attr,dic)
     char *str;   /* key */
     int h0,h1;   /* Hash values of the key */
     int attr;    /* attribute field */
     struct dictionary *dic;
{
  int x;
  int i;
  if (SET_ATTR_FOR_ALL_WORDS & attr) {
    for (i=0; i<USER_DICTIONARY_SIZE; i++) {
      if ((dic[i]).key !=EMPTY) (dic[i]).attr = attr&(~SET_ATTR_FOR_ALL_WORDS);
    }
    return(0);
  }
  if (OR_ATTR_FOR_ALL_WORDS & attr) {
    for (i=0; i<USER_DICTIONARY_SIZE; i++) {
      if ((dic[i]).key !=EMPTY) (dic[i]).attr |= attr&(~OR_ATTR_FOR_ALL_WORDS);
    }
    return(0);
  }
  x = h0;
  if (str[0] == '\0') {
    errorKan1("%s\n","putUserDictionary2(): You are defining a value with the null key.");
  }
  while (1) {
    if ((dic[x]).key == EMPTY) return(-1);
    if (strcmp((dic[x]).key,str) == 0) break;
    x = (x+h1) % USER_DICTIONARY_SIZE;
    if (x == h0) {
      errorStackmachine("User dictionary is full. loop hashing.\n");
    }
  }
  (dic[x]).attr = attr;
  return(x);
}


int putPrimitiveFunction(str,number)
     char *str;
     int number;
{
  struct object ob = OINIT;
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
    h = ((h*128)+((unsigned char)(*str))) % USER_DICTIONARY_SIZE;
    str++;
  }
  return(h);
}

int hash1(str)
     char *str;
{
  return(8-((unsigned char)(str[0])%8));
}

void hashInitialize(struct dictionary *dic)
{
  int i;
  for (i=0; i<USER_DICTIONARY_SIZE; i++) {
    (dic[i]).key = EMPTY; (dic[i]).attr = 0;
  }
}

static int isInteger(str)
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

static int strToInteger(str)
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

static int power(s,i)
     int s;
     int i;
{
  if (i == 0) return 1;
  else return( s*power(s,i-1) );
}

int Kpush(ob)
     struct object ob;     
{
  OperandStack[Osp++] = ob;
  if (Osp >= OspMax) {
    warningStackmachine("Operand stack overflow. \n");
    Osp--;
    return(-1);
  }
  return(0);
}

struct object Kpop()
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

static int isThereExecutableArray(struct object ob) {
  int n,i;
  struct object otmp = OINIT;
  if (ob.tag == SexecutableArray) return(1);
  if (ob.tag == Sarray) {
    n = getoaSize(ob);
    for (i=0; i<n; i++) {
      otmp = getoa(ob,i);
      if (isThereExecutableArray(otmp)) return(1);
    }
    return(0);
  }
  /* Class and list is not checked, since there is no parser
     to directory translte these objects. */
  return(0);
}
static int isThereExecutableArrayOnStack(int n) {
  int i;
  struct object ob = OINIT;
  for (i=0; i<n; i++) {
    if (Osp-i-1 < 0) return(0);
    ob = peek(i);
    if (isThereExecutableArray(ob)) return(1); 
  }
  return(0);
}

struct object newOperandStack(int size)
{
  struct operandStack *os ;
  struct object ob = OINIT;
  os = (struct operandStack *)sGC_malloc(sizeof(struct operandStack));
  if (os == (void *)NULL) errorStackmachine("No more memory.");
  if (size <= 0) errorStackmachine("Size of stack must be more than 1.");
  os->size = size;
  os->sp = 0;
  os->ostack = (struct object *)sGC_malloc(sizeof(struct object)*(size+1));
  if (os->ostack == (void *)NULL) errorStackmachine("No more memory.");
  ob.tag = Sclass;
  ob.lc.ival = CLASSNAME_OPERANDSTACK;
  ob.rc.voidp = os;
  return(ob);
}

void setOperandStack(struct object ob) {
  if (ob.tag != Sclass) errorStackmachine("The argument must be class.");
  if (ob.lc.ival != CLASSNAME_OPERANDSTACK)
    errorStackmachine("The argument must be class.OperandStack.");
  CurrentOperandStack->ostack = OperandStack;
  CurrentOperandStack->sp = Osp;
  CurrentOperandStack->size = OspMax;
  OperandStack = ((struct operandStack *)(ob.rc.voidp))->ostack;
  Osp = ((struct operandStack *)(ob.rc.voidp))->sp;
  OspMax = ((struct operandStack *)(ob.rc.voidp))->size;
  CurrentOperandStack = ob.rc.voidp;
}

void stdOperandStack(void) {
  CurrentOperandStack->ostack = OperandStack;
  CurrentOperandStack->sp = Osp;
  CurrentOperandStack->size = OspMax;

  CurrentOperandStack = &StandardStack;
  OperandStack =   CurrentOperandStack->ostack;
  Osp =  CurrentOperandStack->sp;
  OspMax = CurrentOperandStack->size;
}

/* functions to handle contexts. */
void fprintContext(FILE *fp,struct context *cp) {
  if (cp == (struct context *)NULL) {
    fprintf(fp," Context=NIL \n");
    return;
  }
  fprintf(fp,"  ContextName = %s, ",cp->contextName);
  fprintf(fp,"Super = ");
  if (cp->super == (struct context *)NULL) fprintf(fp,"NIL");
  else {
    fprintf(fp,"%s",cp->super->contextName);
  }
  fprintf(fp,"\n");
}

struct context *newContext0(struct context *super,char *name) {
  struct context *cp;
  cp = sGC_malloc(sizeof(struct context));
  if (cp == (struct context *)NULL) errorStackmachine("No memory (newContext0)");
  cp->userDictionary=sGC_malloc(sizeof(struct dictionary)*USER_DICTIONARY_SIZE);
  if (cp->userDictionary==(struct dictionary *)NULL)
    errorStackmachine("No memory (newContext0)");
  hashInitialize(cp->userDictionary);
  cp->contextName = name;
  cp->super = super;
  return(cp);
}

void KsetContext(struct object contextObj)  {
  if (contextObj.tag != Sclass) {
    errorStackmachine("Usage:setcontext");
  }
  if (contextObj.lc.ival != CLASSNAME_CONTEXT) {
    errorStackmachine("Usage:setcontext");
  }
  if (contextObj.rc.voidp == NULL) {
    errorStackmachine("You cannot set NullContext to the CurrentContext.");
  }
  CurrentContextp = (struct context *)(contextObj.rc.voidp);
}


struct object getSuperContext(struct object contextObj) {
  struct object rob = OINIT;
  struct context *cp;
  if (contextObj.tag != Sclass) {
    errorStackmachine("Usage:supercontext");
  }
  if (contextObj.lc.ival != CLASSNAME_CONTEXT) {
    errorStackmachine("Usage:supercontext");
  }
  cp = (struct context *)(contextObj.rc.voidp);
  if (cp->super == (struct context *)NULL) {
    return(NullObject);
  }else{
    rob.tag = Sclass;
    rob.lc.ival = CLASSNAME_CONTEXT;
    rob.rc.voidp = cp->super;
  }
  return(rob);
}

#define CSTACK_SIZE 1000
void contextControl(actionOfContextControl ctl) {
  static struct context *cstack[CSTACK_SIZE];
  static int cstackp = 0;
  switch(ctl) {
  case CCRESTORE:
    if (cstackp == 0) return;
    else {
      CurrentContextp = cstack[0];
      cstackp = 0;
    }
    break;
  case CCPUSH:
    if (cstackp < CSTACK_SIZE) {
      cstack[cstackp] = CurrentContextp;
      cstackp++;
    }else{
      contextControl(CCRESTORE);
      errorStackmachine("Context stack (cstack) is overflow. CurrentContext is restored.\n");
    }
    break;
  case CCPOP:
    if (cstackp > 0) {
      cstackp--;
      CurrentContextp = cstack[cstackp];
    }
    break;
  default:
    break;
  }
  return;
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
  struct object ob = OINIT;
  int vs;
  vs = VerboseStack; VerboseStack = 2;
  for (i=Osp-1; i>=0; i--) {
    fprintf(Fstack,"[%d] ",i);
    ob = OperandStack[i];
    printObject(ob,1,Fstack);
  }
  VerboseStack = vs;
}

    

static int initSystemDictionary()
{
  StandardStack.ostack = StandardStackA;
  StandardStack.sp = StandardStackP;
  StandardStack.size = OPERAND_STACK_SIZE;

  ErrorStack.ostack = ErrorStackA;
  ErrorStack.sp = ErrorStackP;
  ErrorStack.size = ErrorStackMax;

  StandardContext.userDictionary = UserDictionary;
  StandardContext.contextName = "StandardContext";
  StandardContext.super = (struct context *)NULL;

  KdefinePrimitiveFunctions();

}

struct object showSystemDictionary(int f) {
  int i;
  int maxl;
  char format[1000];
  int nl;
  struct object rob = OINIT;
  rob = NullObject;
  if (f != 0) {
    rob = newObjectArray(Sdp);
    for (i=0; i<Sdp; i++) {
      putoa(rob,i,KpoString((SystemDictionary[i]).key));
    }
    return(rob);
  }
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
  return(rob);
}
  
int showUserDictionary() 
{
  int i,j;
  int maxl;
  char format[1000];
  int nl;
  struct dictionary *dic;
  dic = CurrentContextp->userDictionary;
  fprintf(Fstack,"DictionaryName=%s, super= ",CurrentContextp->contextName);
  if (CurrentContextp->super == (struct context *)NULL) {
    fprintf(Fstack,"NIL\n");
  }else{
    fprintf(Fstack,"%s\n",CurrentContextp->super->contextName);
  }
  maxl = 1;
  for (i=0; i<USER_DICTIONARY_SIZE; i++) {
    if ((dic[i]).key != EMPTY) {
      if (strlen((dic[i]).key) >maxl)
        maxl = strlen((dic[i]).key);
    }
  }
  maxl += 3;
  nl = 80/maxl;
  if (nl < 2) nl = 2;
  sprintf(format,"%%-%ds",maxl);
  for (i=0,j=0; i<USER_DICTIONARY_SIZE; i++) {
    if ((dic[i]).key != EMPTY) {
      fprintf(Fstack,format,(dic[i]).key);
      /*{ char *sss; int ii,h0,h1;
        sss = dic[i].key;
        h0 = dic[i].h0;
        h1 = dic[i].h1;
        for (ii=0; ii<strlen(sss); ii++) fprintf(Fstack,"%x ",sss[ii]);
        fprintf(Fstack,": h0=%d, h1=%d, %d\n",h0,h1,i);
        }*/
      if (j % nl == nl-1) fprintf(Fstack,"\n");
      j++;
    }
  }
  fprintf(Fstack,"\n");
}
  

static struct object executableStringToExecutableArray(s)
     char *s;
{
  struct tokens *tokenArray;
  struct object ob = OINIT;
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
  struct object ob = OINIT;
  extern int Quiet;
  extern void ctrlC();
  int tmp, status;
  char *tmp2;
  extern int ErrorMessageMode;
  int jval;
  extern int InSendmsg2;
  int infixOn = 0;
  struct tokens infixToken;
  extern int RestrictedMode, RestrictedMode_saved;
  getokenSM(INIT);
  initSystemDictionary();

#if defined(__CYGWIN__)
  if (MYSIGSETJMP(EnvOfStackMachine,1)) {
#else
  if (MYSETJMP(EnvOfStackMachine)) {
#endif
    /* do nothing in the case of error */
    fprintf(stderr,"An error or interrupt in reading macros, files and command strings.\n");
    exit(10);
  } else {  }
  if (mysignal(SIGINT,SIG_IGN) != SIG_IGN) {
    mysignal(SIGINT,ctrlC);
  }
  
  /* setup quiet mode or not */
  token.kind = EXECUTABLE_STRING; token.tflag = 0;
  if (Quiet) {
    token.token = " /@@@.quiet 1 def ";
  }else {
    token.token = " /@@@.quiet 0 def ";
  }
  executeToken(token); /* execute startup commands */
  token.kind = ID; token.tflag = 0;
  token.token = "exec";
  token = lookupTokens(token); /* set hashing values */
  tmp = findSystemDictionary(token.token);
  ob.tag = Soperator;
  ob.lc.ival = tmp;
  executePrimitive(ob); /* exec */
  
  
  KSdefineMacros();
  
  if (StartAFile) {
    tmp2 = StartFile;
    StartFile = (char *)sGC_malloc(sizeof(char)*(strlen(StartFile)+
                                                 40));
    sprintf(StartFile,"$%s$ run\n",tmp2);
    token.kind = EXECUTABLE_STRING; token.tflag = 0;
    token.token = StartFile;
    executeToken(token);    /* execute startup commands */
    token.kind = ID; token.tflag = 0;
    token.token = "exec";
    token = lookupTokens(token); /* set hashing values */
    tmp = findSystemDictionary(token.token);
    ob.tag = Soperator;
    ob.lc.ival = tmp;
    executePrimitive(ob);   /* exec */
  }
  
  if (StartAString) {
    token.kind = EXECUTABLE_STRING;  token.tflag = 0;
    token.token = StartString;
    executeToken(token);    /* execute startup commands */
    token.kind = ID; token.tflag = 0;
    token.token = "exec";
    token = lookupTokens(token); /* set hashing values */
    tmp = findSystemDictionary(token.token);
    ob.tag = Soperator;
    ob.lc.ival = tmp;
    executePrimitive(ob);   /* exec */
  }
  
  
  for (;;) {
#if defined(__CYGWIN__)
    if (jval=MYSIGSETJMP(EnvOfStackMachine,1)) {
#else
    if (jval=MYSETJMP(EnvOfStackMachine)) {
#endif
      /* ***  The following does not work properly.  ****
         if (jval == 2) {
         if (ErrorMessageMode == 1 || ErrorMessageMode == 2) {
         pushErrorStack(KnewErrorPacket(SerialCurrent,-1,"User interrupt by ctrl-C."));
         }
         }
         **** */
      if (DebugStack >= 1) {
        fprintf(Fstack,"\nscanner> ");
      }
      if (!Calling_ctrlC_hook) { /* to avoid recursive call of ctrlC-hook. */
        Calling_ctrlC_hook = 1; RestrictedMode = 0;
        KSexecuteString(" ctrlC-hook "); /* Execute User Defined functions. */
        RestrictedMode = RestrictedMode_saved;
      }
      Calling_ctrlC_hook = 0;  
      KSexecuteString(" (Computation is interrupted.) "); /* move to ctrlC-hook? */
      InSendmsg2 = 0;
      infixOn = 0;
      continue ;
    } else {  }
    if (DebugStack >= 1) { printOperandStack(); }
    token = getokenSM(GET); 
    if ((status=executeToken(token)) < 0) break;
    /***if (status == 1) fprintf(stderr," --- exit --- \n");*/
    /* fprintf(stderr,"token.token=%s, status=%d, infixOn=%d\n",token.token,status,infixOn); */
    if (status & STATUS_INFIX) {
      infixOn = 1;  infixToken = token; infixToken.tflag |= NO_DELAY;
    }else if (infixOn) {
      infixOn = 0;
      if ((status=executeToken(infixToken)) < 0) break;
    }
  }
}


void ctrlC(sig)
     int sig;
{
  extern void ctrlC();
  extern int ErrorMessageMode;
  extern int SGClock;
  extern int UserCtrlC;
  extern int OXlock;
  extern int RestrictedMode, RestrictedMode_saved;

  mysignal(sig,SIG_IGN);
  /* see 133p */
  RestrictedMode = RestrictedMode_saved;
  cancelAlarm();
  if (sig == SIGALRM) {
    fprintf(stderr,"ctrlC by SIGALRM\n");
  }

  if (SGClock) {
    UserCtrlC = 1;
    fprintf(stderr,"ctrl-c is locked because of gc.\n");
    mysignal(sig,ctrlC);  if (sig == SIGALRM) alarm((unsigned int)10);
    return;
  }
  if (OXlock) {
    if (UserCtrlC > 0) UserCtrlC++;
    else UserCtrlC = 1;
    if (UserCtrlC > 3) {
      fprintf(stderr,"OK. You are eager to cancel the computation.\n");
      fprintf(stderr,"You should close the ox communication cannel.\n");
      mysignal(SIGINT,ctrlC);
      unlockCtrlCForOx();
    }
    fprintf(stderr,"ctrl-c is locked because of ox lock %d.\n",UserCtrlC);
    mysignal(sig,ctrlC);  if (sig == SIGALRM) alarm((unsigned int)10);
    return;
  }
  if (ErrorMessageMode != 1) {
    (void *) traceShowStack();
    fprintf(Fstack,"User interruption by ctrl-C. We are in the top-level.\n");
    fprintf(Fstack,"Type in quit in order to exit sm1.\n");
  }
  traceClearStack();
  if (GotoP) {
    fprintf(Fstack,"The interpreter was looking for the label <<%s>>. It is also aborted.\n",GotoLabel);
    GotoP = 0;
  }
  stdOperandStack(); contextControl(CCRESTORE);
  /*fprintf(Fstack,"Warning! The handler of ctrl-C has a bug, so you might have a core-dump.\n");*/
  /*
    $(x0+1)^50$ $x1 x0 + x1^20$ 2 groebner_n
    ctrl-C
    $(x0+1)^50$ $x1 x0 + x1^20$ 2 groebner_n
    It SOMETIMES makes core dump.
  */
  getokenSM(INIT); /* It might fix the bug above. 1992/11/14 */
  mysignal(SIGINT,ctrlC);
#if defined(__CYGWIN__)
  MYSIGLONGJMP(EnvOfStackMachine,2);
#else
  MYLONGJMP(EnvOfStackMachine,2); /* returns 2 for ctrl-C */
#endif
}

int executeToken(token)
     struct tokens token;
{      
  struct object ob = OINIT;
  int primitive;
  int size;
  int status;
  int i,h0,h1;
  extern int WarningMessageMode;
  extern int Strict;
  extern int InSendmsg2;
  extern int RestrictedMode, RestrictedMode_saved;
  int localRestrictedMode_saved;

  localRestrictedMode_saved = 0;
  if (GotoP) { /* for goto */
    if (token.kind == ID && isLiteral(token.token)) {
      if (strcmp(&((token.token)[1]),GotoLabel) == 0) {
        GotoP = 0;
        return(0); /* normal exit */
      }
    }
    return(0);  /* normal exit */
  }
  if (token.kind == DOLLAR) {
    ob.tag = Sdollar;
    ob.lc.str = token.token;
    Kpush(ob);
  } else if (token.kind == ID) {  /* ID */

    if (strcmp(token.token,"exit") == 0) return(1);
    /* "exit" is not primitive here. */

    if (isLiteral(token.token)) {
      /* literal object */
      ob.tag = Sstring;
      ob.lc.str = (char *)sGC_malloc((strlen(token.token)+1)*sizeof(char));
      if (ob.lc.str == (char *)NULL) errorStackmachine("No space.");
      strcpy(ob.lc.str, &((token.token)[1]));

      if (token.object.tag != Slist) {
        fprintf(Fstack,"\n%%Warning: The hashing values for the <<%s>> are not set.\n",token.token);
        token.object = lookupLiteralString(token.token);
      }
      ob.rc.op = token.object.lc.op;
      Kpush(ob);
    } else if (isInteger(token.token)) {
      /* integer object */
      ob.tag = Sinteger ;
      ob.lc.ival = strToInteger(token.token);
      Kpush(ob);
    } else {
      if (token.object.tag != Slist) {
        fprintf(Fstack,"\n%%Warning: The hashing values for the <<%s>> are not set.\n",token.token);
        token = lookupTokens(token);
      }
      h0 = ((token.object.lc.op)->lc).ival;
      h1 = ((token.object.lc.op)->rc).ival;
      ob=findUserDictionary(token.token,h0,h1,CurrentContextp);
      primitive = ((token.object.rc.op)->lc).ival;
      if (!(token.tflag & NO_DELAY)) {
        if ((ob.tag >= 0) && (UD_attr & ATTR_INFIX)) {
          return STATUS_INFIX; 
        }
      }
      if (ob.tag >= 0) {
        /* there is a definition in the user dictionary */
        if (ob.tag == SexecutableArray) {
          if (RestrictedMode) {
            if (UD_attr & ATTR_EXPORT) {
              localRestrictedMode_saved = RestrictedMode; RestrictedMode = 0;
              if (isThereExecutableArrayOnStack(5)) {
				int i;
                for (i=0; i<5; i++) { (void) Kpop(); }
                errorStackmachine("Executable array is on the argument stack (restricted mode). They are automatically removed.\n");
			  }
            }else{
              tracePushName(token.token);
              errorStackmachine("You cannot execute this function in restricted mode.\n");
            }
          }

          status = executeExecutableArray(ob,token.token,0);

          if (localRestrictedMode_saved) RestrictedMode = localRestrictedMode_saved;
          if ((status & STATUS_BREAK) || (status < 0)) return status;
        }else {
          Kpush(ob);
        }
      } else if (primitive) { 
        tracePushName(token.token);
        /* system operator */
        ob.tag = Soperator;
        ob.lc.ival = primitive;
        status = executePrimitive(ob);
        tracePopName(); 
        return(status);
      } else {
        if (QuoteMode) {
          if (InSendmsg2) return(DO_QUOTE);
          else {
            Kpush(KpoString(token.token));
            return(0); /* normal exit.*/
          }
		}
        {    
          char tmpc[1024];
          if (strlen(token.token) < 900) {
            sprintf(tmpc,"\n>>Warning: The identifier <<%s>> is not in the system dictionary\n>>   nor in the user dictionaries. Push NullObject.\n",token.token);
          }else {strcpy(tmpc,"\n>>Warning: identifier is not in the dictionaries.\n");}  
          /* do not use %% in a string. tmpc will be used as  fprintf(stderr,tmpc); */
          if (WarningMessageMode == 1 || WarningMessageMode == 2) {
            pushErrorStack(KnewErrorPacket(SerialCurrent,-1,tmpc));
          }
          if (WarningMessageMode != 1) {
            fprintf(Fstack,"%s",tmpc);
            /*fprintf(Fstack,"(%d,%d)\n",h0,h1);*/
          }
          if (Strict) {
            errorStackmachine(tmpc);
          }
          Kpush(NullObject); 
        }
      }
    }
  } else if (token.kind == EXECUTABLE_STRING) {
    Kpush(executableStringToExecutableArray(token.token));
  } else if (token.kind == EXECUTABLE_ARRAY) {
    Kpush(token.object);
  } else if ((token.kind == -1) || (token.kind == -2)) { /* eof token */
    return(-1);
  } else {
    /*fprintf(Fstack,"\n%%Error: Unknown token type\n");***/
    fprintf(stderr,"\nUnknown token type = %d\n",token.kind);
    fprintf(stderr,"\ntype in ctrl-\\ if you like to make core-dump.\n");
    fprintf(stderr,"If you like to continue, type in RETURN key.\n");
    fprintf(stderr,"Note that you cannot input null string.\n");
    getchar();
    errorStackmachine("Error: Unknown token type.\n");
    /* return(-2); /* exit */
  }
  return(0); /* normal exit */
}   


      

void errorStackmachine(char *str)
{
  int i,j,k;
  static char *u="Usage:";
  char message0[1024];
  char *message;
  extern int ErrorMessageMode;
  extern int RestrictedMode, RestrictedMode_saved;
  RestrictedMode = RestrictedMode_saved;
  cancelAlarm();
  MsgStackTrace = NULL;
  MsgSourceTrace = NULL;
  if (ErrorMessageMode == 1 || ErrorMessageMode == 2) {
    pushErrorStack(KnewErrorPacket(SerialCurrent,-1,str));
  }
  if (ErrorMessageMode != 1) {
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
        if (i-6 > 1022) message = (char *)sGC_malloc(sizeof(char)*i);
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
      fprintf(stderr,"%s",str);
    }
    fprintf(stderr,"\n");
	MsgStackTraceInArrayp = traceNameStackToArrayp();
    MsgStackTrace = traceShowStack(); 
    MsgSourceTrace = traceShowScannerBuf();
  }
  traceClearStack();
  if (GotoP) {
    fprintf(Fstack,"The interpreter was looking for the label <<%s>>. It is also aborted.\n",GotoLabel);
    GotoP = 0;
  }
  stdOperandStack(); contextControl(CCRESTORE);
  getokenSM(INIT); /* It might fix the bug. 1996/3/10 */
  /* fprintf(stderr,"Now, Long jump!\n"); */
  MYLONGJMP(EnvOfStackMachine,1);
}

int warningStackmachine(char *str)
{
  extern int WarningMessageMode;
  extern int Strict;
  if (WarningMessageMode == 1 || WarningMessageMode == 2) {
    pushErrorStack(KnewErrorPacket(SerialCurrent,-1,str));
  }
  if (WarningMessageMode != 1) {
    fprintf(stderr,"%s","WARNING(sm): ");
    fprintf(stderr,"%s",str);
  }
  if (Strict) errorStackmachine(" ");
  return(0);
}


/* exports */ 
/* NOTE:  If you call this function and an error occured,
   you have to reset the jump buffer by setjmp(EnvOfStackMachine).
   cf. kxx/memo1.txt, kxx/stdserver00.c 1998, 2/6 */
int KSexecuteString(s)
     char *s;
{
  struct tokens token;
  struct object ob = OINIT;
  int tmp;
  extern int CatchCtrlC;
  int jval;
  static int recursive = 0;
  extern int ErrorMessageMode;
  extern int KSPushEnvMode;
  jmp_buf saved_EnvOfStackMachine;
  void (*sigfunc)();
  int localCatchCtrlC ;
  extern int RestrictedMode, RestrictedMode_saved;

  localCatchCtrlC = CatchCtrlC;
  /* If CatchCtrlC is rewrited in this program,
     we crash. So, we use localCatchCtrlC. */

  if (localCatchCtrlC) {
    sigfunc = mysignal(SIGINT,SIG_IGN);
    mysignal(SIGINT,ctrlC);
  }

  if (KSPushEnvMode) {
    *saved_EnvOfStackMachine = *EnvOfStackMachine;
#if defined(__CYGWIN__)
    if (jval = MYSIGSETJMP(EnvOfStackMachine,1)) {
#else
    if (jval = MYSETJMP(EnvOfStackMachine)) {
#endif
      *EnvOfStackMachine = *saved_EnvOfStackMachine;
      if (jval == 2) {
        if (ErrorMessageMode == 1 || ErrorMessageMode == 2) {
          pushErrorStack(KnewErrorPacket(SerialCurrent,-1,"User interrupt by ctrl-C."));
        }
      }
      recursive--;
      if (localCatchCtrlC) { mysignal(SIGINT, sigfunc); }
      if (!Calling_ctrlC_hook) {
        Calling_ctrlC_hook = 1; RestrictedMode = 0;
        KSexecuteString(" ctrlC-hook "); /* Execute User Defined functions. */
        RestrictedMode_saved;
      }
      Calling_ctrlC_hook = 0;
      KSexecuteString(" (Computation is interrupted.) "); /* move to ctrlC-hook?*/
      /* fprintf(stderr,"result code=-1 for %s\n",s); */
      return(-1);
    }else{ }
  }else{
    if (recursive == 0) {
#if defined(__CYGWIN__)
      if (jval=MYSIGSETJMP(EnvOfStackMachine,1)) { 
#else
      if (jval=MYSETJMP(EnvOfStackMachine)) { 
#endif
        if (jval == 2) {
          if (ErrorMessageMode == 1 || ErrorMessageMode == 2) {
            pushErrorStack(KnewErrorPacket(SerialCurrent,-1,"User interrupt by ctrl-C."));
          }
        }
        recursive = 0;
        if (localCatchCtrlC) { mysignal(SIGINT, sigfunc); }
        if (!Calling_ctrlC_hook) {
          Calling_ctrlC_hook = 1; RestrictedMode = 0;
          KSexecuteString(" ctrlC-hook "); /* Execute User Defined functions. */
          RestrictedMode = RestrictedMode_saved;
        }
        Calling_ctrlC_hook = 0;
        Calling_ctrlC_hook = 0;
		KSexecuteString(" (Computation is interrupted.) ");
		/* fprintf(stderr,"result code=-1 for %s\n",s);*/
        return(-1);
      }else { }
    }
  }

  recursive++;
  token.token = s;
  token.kind = EXECUTABLE_STRING; token.tflag = 0;
  executeToken(token);
  token.kind = ID; token.tflag = 0;
  token.token = "exec";
  token = lookupTokens(token); /* no use */
  tmp = findSystemDictionary(token.token);
  ob.tag = Soperator;
  ob.lc.ival = tmp;
  executePrimitive(ob);
  recursive--;
  if (KSPushEnvMode) *EnvOfStackMachine = *saved_EnvOfStackMachine;
  if (localCatchCtrlC) { mysignal(SIGINT, sigfunc); }
  return(0);
}

int KSdefineMacros() {
  struct tokens token;
  int tmp;
  struct object ob = OINIT;

  if (StandardMacros && (strlen(SMacros))) {
    token.kind = EXECUTABLE_STRING; token.tflag = 0;
    token.token = SMacros;
    executeToken(token);    /* execute startup commands */
    token.kind = ID; token.tflag = 0;
    token.token = "exec";
    token = lookupTokens(token); /* no use */
    tmp = findSystemDictionary(token.token);
    ob.tag = Soperator;
    ob.lc.ival = tmp;
    executePrimitive(ob);   /* exec */
  }
  return(0);

}

void KSstart_quiet() {
  extern int Quiet;
  Quiet=1;
  KSstart();
}
void KSstart() {
  struct tokens token;
  int tmp;
  struct object ob = OINIT;
  extern int Quiet;

  stackmachine_init(); KinitKan();
  getokenSM(INIT); initSystemDictionary();

  /* The following line may cause a core dump, if you do not setjmp properly
     after calling KSstart().*/
  /*
    if (MYSETJMP(EnvOfStackMachine)) {
    fprintf(stderr,"KSstart(): An error or interrupt in reading macros, files and command strings.\n");
    exit(10);
    } else {  }  */

  /* setup quiet mode or not */
  token.kind = EXECUTABLE_STRING; token.tflag = 0;
  if (Quiet) {
    token.token = " /@@@.quiet 1 def ";
  }else {
    token.token = " /@@@.quiet 0 def ";
  }
  executeToken(token); /* execute startup commands */
  token.kind = ID; token.tflag = 0;
  token.token = "exec";
  token = lookupTokens(token); /* set hashing values */
  tmp = findSystemDictionary(token.token);
  ob.tag = Soperator;
  ob.lc.ival = tmp;
  executePrimitive(ob); /* exec */
  
  KSdefineMacros();
}

void KSstop() {
  Kclose(); stackmachine_close();
}


struct object KSpop() {
  return(Kpop());
}

void KSpush(ob)
     struct object ob;
{
  Kpush(ob);
}

struct object KSpeek(int k) {
  return(peek(k));
}

char *KSstringPop() {
  /* pop a string */
  struct object rob = OINIT;
  rob = Kpop();
  if (rob.tag == Sdollar) {
    return(rob.lc.str);
  }else{
    return((char *)NULL);
  }
}

char *KSpopString() {
  return(KSstringPop());
}

int KSset(char *name) {
  char *tmp2;
  char tmp[1024];
  tmp2 = tmp;
  if (strlen(name) < 1000) {
    sprintf(tmp2," /%s set ",name);
  }else{
    tmp2 = sGC_malloc(sizeof(char)*(strlen(name)+20));
    if (tmp2 == (char *)NULL) errorStackmachine("Out of memory.");
    sprintf(tmp2," /%s set ",name);
  }
  return( KSexecuteString(tmp2) );
}

int KSpushBinary(int size,char *data) {
  /* struct object KbinaryToObject(int size, char *data); */
  errorStackmachine("KSpushBinary is not implemented.\n");
  return(-1);
}

char *KSpopBinary(int *size) {
  /* char *KobjectToBinary(struct object ob,int *size); */
  errorStackmachine("KSpopBinary is not implemented.\n");
  *size = 0;
  return((char *)NULL);
}

struct object KSnewObjectArray(int k) {
   return newObjectArray(k);
}

int pushErrorStack(struct object obj)
{
  if (CurrentOperandStack == &ErrorStack) {
    fprintf(stderr,"You cannot call pushErrorStack when ErrorStack is the CurrentOperandStack. \n");
    return(-1);
  }
  (ErrorStack.ostack)[(ErrorStack.sp)++] = obj;
  /* printf("ErrorStack.sp = %d\n",ErrorStack.sp); */
  if ((ErrorStack.sp) >= (ErrorStack.size)) {
    ErrorStack.sp = 0;
    fprintf(stderr,"pushErrorStack():ErrorStack overflow. It is reset.\n");
    /* Note that it avoids recursive call.*/
    return(-1);
  }
  return(0);
}

struct object popErrorStack(void) {
  if (CurrentOperandStack == &ErrorStack) {
    fprintf(stderr,"You cannot call popErrorStack when ErrorStack is the CurrentOperandStack. \n");
    return(NullObject);
  }
  if ((ErrorStack.sp) <= 0) {
    return( NullObject );
  }else{
    return( (ErrorStack.ostack)[--(ErrorStack.sp)]);
  }
}

char *popErrorStackByString(void) {
  struct object obj = OINIT;
  struct object eobj = OINIT;
  eobj = popErrorStack();
  if (ectag(eobj) != CLASSNAME_ERROR_PACKET) {
    return(NULL);
  }else{
    obj = *(KopErrorPacket(eobj));
  }
  if (obj.tag != Sarray || getoaSize(obj) != 3) {
    fprintf(stderr,"errorPacket format error.\n");
    printObject(eobj,0,stderr); fflush(stderr);
    return("class errorPacket format error. Bug of sm1.");
  }
  obj = getoa(obj,2);
  if (obj.tag != Sdollar) {
    fprintf(stderr,"errorPacket format error at position 2..\n");
    printObject(eobj,0,stderr); fflush(stderr);
    return("class errorPacket format error at the position 2. Bug of sm1.");
  }
  return(KopString(obj));
}
    

int KScheckErrorStack(void)
{
  return(ErrorStack.sp);
}

struct object KnewErrorPacket(int serial,int no,char *message)
{
  struct object obj = OINIT;
  struct object *myop;
  char *s;
  /* Set extended tag. */
  obj.tag = Sclass;  obj.lc.ival = CLASSNAME_ERROR_PACKET ;
  myop = (struct object *)sGC_malloc(sizeof(struct object));
  if (myop == (struct object *)NULL) errorStackmachine("No memory\n");
  *myop = newObjectArray(3);
  /*fprintf(stderr,"newErrorPacket() in stackmachine.c: [%d, %d, %s] \n",serial,no,message);  **kxx:CMO_ERROR  */
  putoa((*myop),0,KpoInteger(serial)); 
  putoa((*myop),1,KpoInteger(no));
  s = (char *)sGC_malloc(sizeof(char)*(strlen(message)+2));
  if (s == (char *)NULL) errorStackmachine("No memory\n");
  strcpy(s,message);
  putoa((*myop),2,KpoString(s));
  obj.rc.op = myop;
  return(obj);
}


struct object KnewErrorPacketObj(struct object ob1)
{
  struct object obj = OINIT;
  struct object *myop;
  char *s;
  /* Set extended tag. */
  obj.tag = Sclass;  obj.lc.ival = CLASSNAME_ERROR_PACKET ;
  myop = (struct object *)sGC_malloc(sizeof(struct object));
  if (myop == (struct object *)NULL) errorStackmachine("No memory\n");
  *myop = ob1;
  obj.rc.op = myop;
  return(obj);
}

void *sGC_malloc(size_t n) { /* synchronized function */
  void *c;
  int id;
  extern int SGClock, UserCtrlC;

  SGClock = 1;
  c = GC_malloc(n);
  SGClock = 0;
  if (UserCtrlC) {
    UserCtrlC = 0;
    id = getpid();
    kill(id,SIGINT);
    return(c);
  }else{
    return(c);
  }
}

void *sGC_realloc(void *p,size_t new) { /* synchronized function */
  void *c;
  int id;
  extern int SGClock, UserCtrlC;

  SGClock = 1;
  c = GC_realloc(p,new);
  SGClock = 0;
  if (UserCtrlC) {
    UserCtrlC = 0;
    id = getpid();
    kill(id,SIGINT);
    return(c);
  }else{
    return(c);
  }
}

void sGC_free(void *c) { /* synchronized function */
  int id;
  extern int SGClock, UserCtrlC;

  SGClock = 1;
  GC_free(c);
  SGClock = 0;
  if (UserCtrlC) {
    UserCtrlC = 0;
    id = getpid();
    kill(id,SIGINT);
    return;
  }else{
    return;
  }
}

void lockCtrlCForOx() {
  extern int OXlock;
  extern int OXlockSaved;
  OXlockSaved = OXlock;
  OXlock = 1;
}

void unlockCtrlCForOx() {
  int id;
  extern int OXlock, UserCtrlC;
  extern int OXlockSaved;
  OXlockSaved = OXlock;
  OXlock = 0;
  if (UserCtrlC) {
    UserCtrlC = 0;
    id = getpid();
    kill(id,SIGINT);
    return;
  }else{
    return;
  }
}  

void restoreLockCtrlCForOx() {
  extern int OXlock;
  extern int OXlockSaved;
  OXlock = OXlockSaved;
}

int KSstackPointer() {
  return(Osp);
}

struct object KSdupErrors() {
  struct object rob = OINIT;
  struct object ob = OINIT;
  int i;
  int n;
  int m;

  n = KSstackPointer();
  m = 0;
  for (i=0; i<n; i++) {
    ob = peek(i);
    if (ob.tag == Sclass && ectag(ob) == CLASSNAME_ERROR_PACKET) {
      m++;
    }
  }
  rob = newObjectArray(m);
  m = 0;
  for (i=0; i<n; i++) {
    ob = peek(i);
    if (ob.tag == Sclass && ectag(ob) == CLASSNAME_ERROR_PACKET) {
      putoa(rob, m, ob);
      m++;
    }
  }
  return(rob);
}

void cancelAlarm() {
  alarm((unsigned int) 0);
  mysignal(SIGALRM,SIG_DFL);
}

/* back-trace */
#define TraceNameStackSize 3000
char *TraceNameStack[TraceNameStackSize];
int TraceNameStackp = 0;
void tracePushName(char *s) {
  char *t;
  /*
  t = (char *)sGC_malloc(strlen(s)+1);
  if (t == NULL) {
    fprintf(stderr,"No more memory.\n"); return;
  }
  strcpy(t,s);
  */
  t = s;
  TraceNameStack[TraceNameStackp++] = t;
  if (TraceNameStackp >= TraceNameStackSize) {
    fprintf(stderr,"Warning: TraceNameStack overflow. Clearing the stack.\n");
    TraceNameStackp = 0;
  }
}
void traceClearStack(void) {
  TraceNameStackp = 0;
}
char *tracePopName(void) {
  if (TraceNameStackp <= 0) return (char *) NULL;
  return TraceNameStack[--TraceNameStackp];
}
struct object *traceNameStackToArrayp(void) {
  int n,i;
  struct object *op;
  op = sGC_malloc(sizeof(struct object));
  n = TraceNameStackp; if (n < 0) n = 0;
  *op = newObjectArray(n);
  for (i=0; i<n; i++) {
	putoa((*op),i, KpoString(TraceNameStack[i]));
  }
  return op;
}
#define TRACE_MSG_SIZE 320
char *traceShowStack(void) {
  char *s;
  char *t;
  int p;
  s = (char *) sGC_malloc(TRACE_MSG_SIZE);
  if (s == NULL) {
    fprintf(stderr,"No more memory.\n"); return NULL;
  }
  sprintf(s,"Trace: ");
  p = strlen(s);
  do {
    t = tracePopName();
    if (t == NULL) {
      s[p] = ';'; s[p+1] = 0;
      break;
    }else if ((strlen(t) + p) > (TRACE_MSG_SIZE-10)) {
	  /* fprintf(stderr,"p=%d, TraceNameStackp=%d, strlen(t)=%d, t=%s\n",p,TraceNameStackp,strlen(t),t); */
      strcpy(&(s[p])," ...");
      break;
    }
    strcpy(&(s[p]),t); p += strlen(t);
    strcpy(&(s[p]),"<-"); p += 2;
  } while (t != (char *)NULL);
  fprintf(stderr,"%s\n",s);
  return s;
}

/*
  if (fname != NULL) fname is pushed to the trace stack.
 */ 
int executeExecutableArray(struct object ob,char *fname,int withGotoP) {
  struct tokens *tokenArray;
  int size,i;
  int status;
  int infixOn;
  struct tokens infixToken;
  extern int GotoP;
  
  infixOn = 0;
  if (ob.tag != SexecutableArray) errorStackmachine("Error (executeTokenArray): the argument is not a token array.");

  if (fname != NULL) tracePushName(fname);
  tokenArray = ob.lc.tokenArray;
  size = ob.rc.ival;
  for (i=0; i<size; i++) {
    status = executeToken(tokenArray[i]);
    if ((status & STATUS_BREAK) || (status < 0) || (withGotoP && GotoP)) {
      if (fname != NULL) tracePopName();
      return(status);
    }
    
    if (status & STATUS_INFIX) {
      if (i == size-1) errorStackmachine("Infix operator at the end of an executable array.");
      infixOn = 1; infixToken = tokenArray[i]; 
      infixToken.tflag |= NO_DELAY;
      continue;
    }else if (infixOn) {
      infixOn = 0;
      status = executeToken(infixToken);
      if ((status & STATUS_BREAK) || (status < 0) || (withGotoP && GotoP)) {
        if (fname != NULL) tracePopName();
        return(status);
      }
    }
  }
  if (fname != NULL) tracePopName();
  return(0); /* normal exit */
}
