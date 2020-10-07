/* $OpenXM: OpenXM/src/k097/dic.c,v 1.2 2000/01/21 03:01:25 takayama Exp $ */
#include <stdio.h>
#include "d.h"


static int K00debug0 = 0;  /* If it is set to 1, context will be printed. */

static int K00Initialized = 0;

struct context *K00CurrentContextp = NULL;
struct context *K00PrimitiveContextp = NULL;
/* It is initialize at K00InitDic() */

objectp K00NullObject = NULL;
int K00Strict2 = 1;


static void myerror(char *s) {
  fprintf(stderr,"Error in dic.c : ");
  fprintf(stderr,"%s",s);
}

int K00putUserDictionary(str,h0,h1,ob,cp)
char *str;   /* key */
int h0,h1;   /* Hash values of the key */
objectp ob; /* value */
struct context *cp;
{
  int x,r;
  extern int K00Strict2;
  struct dictionary *dic;
  dic = cp->userDictionary;
  x = h0;
  if (str[0] == '\0') {
    K00errorDic("putUserDictionary(): You are defining a value with the null key.\n");
  }
  while (1) {
    if ((dic[x]).key == EMPTY) break;
    if (strcmp((dic[x]).key,str) == 0) break;
    x = (x+h1) % USER_DICTIONARY_SIZE;
    if (x == h0) {
      K00errorDic("User dictionary is full. loop hashing.\n");
    }
  }
  r = x;
  if (K00Strict2) {
    switch((dic[x]).attr) {
    case PROTECT:
      r = -PROTECT;   /* Protected, but we rewrite it. */
      break;
    case ABSOLUTE_PROTECT:
      r = -ABSOLUTE_PROTECT;  /* Protected and we do not rewrite it. */
      return(r);
    default:
      (dic[x]).attr = 0;
      break;
    }
  }
  (dic[x]).key = str;
  (dic[x]).obj = ob;
  (dic[x]).h0 = h0;
  (dic[x]).h1 = h1;
  return(r);
}

objectp K00findUserDictionary(str,h0,h1,cp)   
/* returns K00NullObject, if there is no item. */
char *str;    /* key */
int h0,h1;    /* The hashing values of the key. */
struct context *cp;
{
  int x;
  struct dictionary *dic;
  dic = cp->userDictionary;
  x = h0;
  while (1) {
    if ((dic[x]).key == EMPTY) { break; }
    if (strcmp((dic[x]).key,str) == 0) {
      return( (dic[x]).obj );
    }
    x = (x+h1) % USER_DICTIONARY_SIZE;
    if (x == h0) {
      K00errorDic("User dictionary is full. loop hashing in findUserDictionary.\n");
    }
  }
  if (cp->super == (struct context *)NULL) return(K00NullObject);
  else return(K00findUserDictionary(str,h0,h1,cp->super));

}

objectp K00findUserDictionary0(str,h0,h1,cp)   
/* returns K00NullObject, if there is no item. */
/* This function does not look up the super class. */
char *str;    /* key */
int h0,h1;    /* The hashing values of the key. */
struct context *cp;
{
  int x;
  struct dictionary *dic;
  dic = cp->userDictionary;
  x = h0;
  while (1) {
    if ((dic[x]).key == EMPTY) { break; }
    if (strcmp((dic[x]).key,str) == 0) {
      return( (dic[x]).obj );
    }
    x = (x+h1) % USER_DICTIONARY_SIZE;
    if (x == h0) {
      K00errorDic("User dictionary is full. loop hashing in findUserDictionary.\n");
    }
  }
  return(K00NullObject);

}

int K00putUserDictionary2(str,h0,h1,attr,cp)
char *str;   /* key */
int h0,h1;   /* Hash values of the key */
int attr;    /* attribute field */
struct context *cp;
{
  int x;
  int i;
  struct dictionary *dic;
  dic = cp->userDictionary;
  if (SET_ATTR_FOR_ALL_WORDS & attr) {
    for (i=0; i<USER_DICTIONARY_SIZE; i++) {
      if ((dic[i]).key !=EMPTY) (dic[i]).attr = attr&(~SET_ATTR_FOR_ALL_WORDS);
    }
    return(0);
  }
  x = h0;
  if (str[0] == '\0') {
    K00errorDic("K00putUserDictionary2(): You are defining a value with the null key.\n");
  }
  while (1) {
    if ((dic[x]).key == EMPTY) return(-1);
    if (strcmp((dic[x]).key,str) == 0) break;
    x = (x+h1) % USER_DICTIONARY_SIZE;
    if (x == h0) {
      K00errorDic("User dictionary is full. loop hashing.\n");
    }
  }
  (dic[x]).attr = attr;
  return(x);
}




int K00hash0(str)
char *str;
{
  int h=0;
  while (*str != '\0') {
    h = ((h*128)+(*str)) % USER_DICTIONARY_SIZE;
    str++;
  }
  return(h);
}

int K00hash1(str)
char *str;
{
  return(8-(str[0]%8));
}

void K00hashInitialize(struct dictionary *dic)
{
  int i;
  for (i=0; i<USER_DICTIONARY_SIZE; i++) {
    (dic[i]).key = EMPTY; (dic[i]).attr = 0;
  }
}

/* functions to handle contexts. */
void K00fprintContext(FILE *fp,struct context *cp) {
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

void K00fprintContextAndDictionary(FILE *fp,struct context *cp) {
  int i,j;
  int maxl;
  char format[1000];
  int nl;
  struct dictionary *dic;

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

  dic = cp->userDictionary;
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
      fprintf(fp,format,(dic[i]).key);
      /*{ char *sss; int ii,h0,h1;
	sss = dic[i].key;
	h0 = dic[i].h0;
	h1 = dic[i].h1;
	for (ii=0; ii<strlen(sss); ii++) fprintf(fp,"%x ",sss[ii]);
	fprintf(fp,": h0=%d, h1=%d, %d\n",h0,h1,i);
      }*/
      if (j % nl == nl-1) fprintf(fp,"\n");
      j++;
    }
  }
  fprintf(fp,"\n");
  for (i=0; i<USER_DICTIONARY_SIZE; i++) {
    if ((dic[i]).key != EMPTY) {
      fprintf(fp,"%s ",(dic[i]).key);
      printObject_d(fp,(dic[i]).obj);
      fprintf(fp," \n");
    }
  }
  fprintf(fp,"\n");
}

struct context *K00newContext0(struct context *super,char *name) {
  struct context *cp;
  cp = mymalloc(sizeof(struct context));
  if (cp == (struct context *)NULL) K00errorDic("No memory (newContext0)");
  cp->userDictionary=mymalloc(sizeof(struct dictionary)*USER_DICTIONARY_SIZE);
  if (cp->userDictionary==(struct dictionary *)NULL)
    K00errorDic("No memory (newContext0)");
  K00hashInitialize(cp->userDictionary);
  cp->contextName = name;
  cp->super = super;
  return(cp);
}

void K00contextControl(actionOfContextControl ctl) {
  static struct context *cstack[CSTACK_SIZE];
  static int cstackp = 0;
  switch(ctl) {
  case CCRESTORE:
    if (cstackp == 0) return;
    else {
      K00CurrentContextp = cstack[0];
      cstackp = 0;
    }
    break;
  case CCPUSH:
    if (cstackp < CSTACK_SIZE) {
      cstack[cstackp] = K00CurrentContextp;
      cstackp++;
    }else{
      K00contextControl(CCRESTORE);
      K00errorDic("Context stack (cstack) is overflow. CurrentContext is restored.\n");
    }
    break;
  case CCPOP:
    if (cstackp > 0) {
      cstackp--;
      K00CurrentContextp = cstack[cstackp];
    }
    break;
  default:
    break;
  }
  return;
}

void K00InitDic() {
  extern struct context *K00CurrentContextp;
  extern struct context *K00PrimitiveContextp;
  char *start = "K00start";
  char *size = "K00sizeof";
  char *primitiveObject = "PrimitiveObject";
  K00CurrentContextp = K00newContext0((struct context *)NULL,primitiveObject);
  K00PrimitiveContextp = K00CurrentContextp;
  K00putUserDictionary(start,K00hash0(start),K00hash1(start),
		       K00integerToObject(1),
		       K00CurrentContextp);
  K00putUserDictionary2(start,K00hash0(start),K00hash1(start),
		       ABSOLUTE_PROTECT,
		       K00CurrentContextp);
  K00putUserDictionary(size,K00hash0(size),K00hash1(size),
		       K00integerToObject(0),
		       K00CurrentContextp);
  K00putUserDictionary2(size,K00hash0(size),K00hash1(size),
		       ABSOLUTE_PROTECT,
		       K00CurrentContextp);
  K00putUserDictionary(primitiveObject,K00hash0(primitiveObject),
		       K00hash1("primitiveObject"),
		       K00contextToObject(K00CurrentContextp),
		       K00CurrentContextp);
  K00putUserDictionary2(primitiveObject,K00hash0(primitiveObject),
		       K00hash1("primitiveObject"),
		       ABSOLUTE_PROTECT,
		       K00CurrentContextp);

}


void K00errorDic(char *s) {
  fprintf(stderr,"Error in dic.c: %s",s);
  exit(10);
}

int K00objectToInteger(objectp op) {
  return(op->lc.ival);
}

objectp K00integerToObject(int k) {
  objectp op;
  op = newObject_d();
  op->tag = Sinteger;
  op->lc.ival = k;
  return(op);
}

objectp K00contextToObject(struct context *cp) {
  objectp op;
  op = newObject_d();
  op->tag = op->lc.ival = CLASSNAME_CONTEXT;
  op->rc.voidp = cp;
  return(op);
}

struct context *K00objectToContext(objectp op) {
  return((struct context *)(op->rc.voidp));
}

int K00putIncetanceVariable(actionOfPutIncetanceVariable action,char *s) {
/*  typedef enum {IRESET,IPUT,IEXIT} actionOfPutIncetanceVariable; */
  extern struct context *K00CurrentContextp;
  static int K00start;
  static int serial;
  static int initialized = 0;
  objectp op1;
  objectp op2;
  if (K00CurrentContextp == K00PrimitiveContextp) {
    myerror("You cannot define incetanceVariables in PrimitiveContext.\n");
    return(-1);
  }
  if (action == IRESET) {
    serial = 0;
    op1 = K00findUserDictionary0("K00start",K00hash0("K00start"),
				 K00hash1("K00start"),
				 K00CurrentContextp->super);
    if (op1 == K00NullObject) {
      myerror("K00start is not found.\n");
      return(-1);
    }
    op2 = K00findUserDictionary0("K00sizeof",K00hash0("K00sizeof"),
			      K00hash1("K00sizeof"),
			     K00CurrentContextp->super);
    if (op2 == K00NullObject) {
      myerror("K00sizeof is not found.\n");
      return(-1);
    }
    K00start = K00objectToInteger(op1)+K00objectToInteger(op2);
    initialized = 1;
    return(0);
  }else if (action == IEXIT) {
    if (initialized) {
      K00putUserDictionary("K00start",K00hash0("K00start"), K00hash1("K00start"),
			   K00integerToObject(K00start),
			   K00CurrentContextp);
      K00putUserDictionary("K00sizeof",K00hash0("K00sizeof"), K00hash1("K00sizeof"),
			   K00integerToObject(serial),
			   K00CurrentContextp);
    }
    initialized = 0;
    return(0);

  }else if (action == IPUT) {
    if (initialized) {
      K00putUserDictionary(s,K00hash0(s),K00hash1(s),
			   K00integerToObject(K00start+serial),
			   K00CurrentContextp);
      serial++;
    }else{
      myerror("K00putIncetanceVariable() is not initialized.\n");
    }
    return(0);
  }
  return(-1);
}

int K00getIncetanceVariable(char *s) {
  objectp op;
  extern struct context *K00CurrentContextp;
  extern int K00Initialized;
  if (!K00Initialized) {K00InitDic(); K00Initialized = 1;}
  op = K00findUserDictionary0(s,K00hash0(s),K00hash1(s),K00CurrentContextp);
  if (op == K00NullObject) {
    return(-1);
  }else if (op->tag != Sinteger) {
    return(-1);
  }else{
    return( K00objectToInteger(op) );
  }
}

void K00recoverFromError() {
  K00toPrimitiveClass();
}

int K00declareClass(char *name,char *supername) {
  extern struct context *K00CurrentContextp;
  extern struct context *K00PrimitiveContextp;
  struct context *sup;
  objectp op1;
  extern int K00Initialized;
  if (!K00Initialized) { K00InitDic(); K00Initialized = 1;}
  op1 = K00findUserDictionary0(supername,K00hash0(supername),K00hash1(supername),
			       K00PrimitiveContextp);
  if (op1 == K00NullObject) {
    myerror("Super class is not in the top dictionary.\n");
    /* debug */ K00fooPrimitive();
    return(-1);
  }
  if (op1->tag != CLASSNAME_CONTEXT) {
    myerror("It is not class.context.\n");
    /* debug */ K00fooPrimitive();
    return(-1);
  }
  K00CurrentContextp = K00newContext0(K00objectToContext(op1),name);
  K00putUserDictionary(name,K00hash0(name),K00hash1(name),
		       K00contextToObject(K00CurrentContextp),
		       K00PrimitiveContextp);
  K00putIncetanceVariable(IRESET," ");
  return(0);
}

void K00toPrimitiveClass() {
  extern struct context *K00CurrentContextp;
  extern struct context *K00PrimitiveContextp;
  K00CurrentContextp = K00PrimitiveContextp;
}

char *K00getCurrentContextName() {
  extern struct context *K00CurrentContextp;
  if (!K00Initialized) {
    K00InitDic(); K00Initialized = 1;
  }
  return(K00CurrentContextp->contextName);
}

void pkkanInteger(int k) {
  char tmp[256]; /* pkkan creates a new memory area. */
  sprintf(tmp," %d ",k);
  pkkan(tmp);
}
  

void K00foo1() {
  extern struct context *K00CurrentContextp;
  extern int K00debug0;
  if (K00debug0) K00fprintContextAndDictionary(stderr,K00CurrentContextp);
}

void K00fooPrimitive() {
  extern struct context *K00PrimitiveContextp;
  extern int K00debug0;
  if (K00debug0) {
    printf("--------- PrimitiveContext ------------\n");
    K00fprintContextAndDictionary(stderr,K00PrimitiveContextp);
  }
}

