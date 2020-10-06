/* $OpenXM: OpenXM/src/kan96xx/Kan/parserpass0.c,v 1.5 2013/11/06 06:23:24 takayama Exp $ */
/* parserpass0.c */
/*  In this preprocessor, for example, the expression
      x^2+y^2-4+x y;
    is transformed into
      x3^2+x4^2-4+x3*x4;
*/
/*  1992/03/05 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "datatype.h"
#include "extern2.h"

static int isSymbol0();
static int isNumber0();
typedef enum {INIT,GET,PUT} actionType;

struct tokens{
  char *token;
  int kind;
};

#define KAZU 1  /* used in kind of tokens */
#define ID   2

#define BUF0LIMIT 2000
#define STRBUFMAX 20000
static char *String0;
static int Strp0 = 0;
static char Buf0org[BUF0LIMIT];
static char *Buf0 = Buf0org ;
static int Buf0limit = BUF0LIMIT ;
static int Exist0 = 0;

static struct tokens getoken_pass0();
static struct tokens flush();


static int get0()
     /* get a letter from String0 */
{
  int c;
  c = String0[Strp0++];
  if (c == '\0') {
    Strp0--;return(EOF);
  } else return(c);
}

static void put0(c)
     int c;
     /* put a letter on Buf0 */
{
  char *new; int i;
  Buf0[Exist0++] = c;
  if (Exist0 >= Buf0limit-1)  {
    new = (char *) sGC_malloc(sizeof(char *)*Buf0limit*2) ;
    if (new == (char *)NULL) {
      fprintf(stderr,"No more memory in parserpass0.c\n");
      exit(18);
    }
    fprintf(stderr,"\nSystem Message: Increasing Buf0 to %d in parserpass0.c\n",Buf0limit*2);
    for (i=0; i<Buf0limit; i++) {
      new[i] = Buf0[i];
    }
    Buf0 = new; Buf0limit *= 2;
  }
}

static struct tokens flush()
{
  char *token;
  struct tokens r;
  if (Exist0<=0) {
    fprintf(stderr,"\n flush() is called without data. \n");
    r.token = (char *)NULL; r.kind = -1;
    return(r);
  }
  Buf0[Exist0] = '\0';
  Exist0 = 0;
  token = (char *)sGC_malloc((strlen(Buf0)+1)*sizeof(char));
  strcpy(token,Buf0);
  r.token = token;
  if (isSymbol0(token[0])) r.kind = token[0];
  else if (isNumber0(token[0])) r.kind =KAZU;
  else r.kind = ID;
  return(r);
}

static int isSpace0(c)
     int c;
{
  if (c <= ' ') return(1);
  else return(0);
}

static int isSymbol0(c)
     int c;
{
  if ((c == '+') ||
      (c == '-') ||
      (c == '*') ||
      (c == '/') ||
      (c == '^') ||
      (c == ';') ||
      (c == '(') ||
      (c == ')'))
    return(1);
  else return(0);
}

static int isNumber0(c)
     int c;
{
  if ((c>='0') && (c<='9')) return(1);
  else return(0);
}

static struct tokens getoken_pass0(kind)
     actionType kind;
{
  static int c;
  static struct tokens rnull;
  if (kind == INIT) {
    Strp0 = 0;
    Exist0 = 0;
    c = get0();
    rnull.token = (char *)NULL; rnull.kind = -1;
    return(rnull);
  }

  for (;;) {
    if (c == EOF) {
      if (Exist0) return(flush());
      else return(rnull);
    } else if (isSpace0(c)) {
      if (Exist0) {
        c = get0(); return(flush());
      }else {
        while (isSpace0(c=get0())) ;
      }
    } else if (isSymbol0(c)) {
      if (Exist0) return(flush());
      else {
        put0(c);
        c = get0();
        return(flush());
      }
    } else if (isNumber0(c)) {
      put0(c);
      c = get0();
      while (isNumber0(c)) {
        put0(c);
        c = get0();
      }
      return(flush());
    } else { /* identifier */
      put0(c);
      c =get0();
      while ((!isSymbol0(c)) &&
             (!isSpace0(c))  &&
             (c != EOF)) {
        put0(c);
        c = get0();
      }
      return(flush());
    }
  }
}

/* This function put the string into the Buf */
static char Buftmp[STRBUFMAX];
static char *Buf = Buftmp;
static int Buflimit = STRBUFMAX;

static int putstr(str,kind)
     char str[]; /* string to be outputted */
     actionType kind; /* kind is INIT or PUT */
{
  static int ptr;
  int i;
  int k; char *newbuf;
  if (kind == INIT) {
    ptr = 0;
    return 0;
  }

  i=0;
  while (str[i]!='\0') {
    Buf[ptr++] = str[i++];
    Buf[ptr] = '\0';
    if (ptr >= Buflimit-2) {
      fprintf(stderr,"\nSystem Message: Limit is exceeded in putstr() (parserpass0.c) \n");
      fprintf(stderr,"Increasing Buf to %d.\n",2*Buflimit);
      newbuf = (char *) sGC_malloc(2*Buflimit);
      if (newbuf == (char *)NULL) {
        fprintf(stderr,"\nNo memory in putstr() in parserpass0.c\n");
        exit(20);
      }
      for (k=0; k<Buflimit; k++) {
        newbuf[k] = Buf[k];
      }
      Buf = newbuf;
      Buflimit *= 2;
    }
  }
  return 0;
}
  


char *str2strPass0(string,ringp)
     char *string;
     struct ring *ringp;
{

  struct tokens ptoken;  /* previous token */
  struct tokens ctoken;  /* current token */
  struct tokens nulltoken;
  char *result;
  int i;
  char work[100];
  int inputTranslation = 1;
  int n;

  n = ringp->n;

  nulltoken.token = (char *)NULL;
  nulltoken.kind  = -1;
  putstr("",INIT);
  String0 = (char *)sGC_malloc((strlen(string)+2)*sizeof(char));
  strcpy(String0,string);
  getoken_pass0(INIT);

  ptoken = nulltoken;
  for (;;) {
    ctoken = getoken_pass0(GET);

    /*   ** ----> ^ */
    if (ctoken.kind == '*' && ptoken.kind == '*') {
      ptoken.kind = '^';
      ptoken.token = (char *)sGC_malloc(sizeof(char)*5);
      strcpy(ptoken.token,"^");
      ctoken = getoken_pass0(GET);
    }
      
    if (ctoken.token == (char *)NULL) {
      fprintf(stderr,"\n Unexpected eof in str2strPass0() (parserpass0.c).   ; is expected. \n");
      return((char *)NULL);
    }
    /* output the ptoken.token */
    if (inputTranslation && (ptoken.kind == ID)) {
      /* Do the input translation for identifier*/
      for (i=0; i<n; i++) {
        if (strcmp(ptoken.token,ringp->x[i]) == 0) {
          sprintf(work,"x%d",i);
          putstr(work,PUT);
          i = -1;
          break; /* break for */
        }
        if (strcmp(ptoken.token,ringp->D[i]) == 0) {
          sprintf(work,"d%d",i);
          putstr(work,PUT);
          i = -1;
          break; /* break for */
        }
      }
      if (strlen(ptoken.token)>0  && (ptoken.token)[0] == '@') {
        putstr(ptoken.token,PUT);
        i = -1;
      }
      if (i != -1) { /* very dirty code */
        fprintf(stderr,"\n Undefined identifier %s. \n",ptoken.token);
        putstr(ptoken.token,PUT);
      }
      /* end of input translations */
    }else { 
      if (ptoken.token != (char *)NULL) {
        putstr(ptoken.token,PUT);
      }
    }
    /* if ctoken.token is ";" then end */
    if (strcmp(ctoken.token,";") == 0) {
      putstr(ctoken.token,PUT);
      if (ptoken.token != (char *)NULL) sGC_free(ptoken.token);
      if (ctoken.token != (char *)NULL) sGC_free(ctoken.token);
      result = (char *)sGC_malloc((strlen(Buf)+2)*sizeof(char));
      strcpy(result,Buf);
      return(result);
    }
    
    if (((ptoken.kind == ID) || (ptoken.kind == KAZU) || (ptoken.kind == ')'))
        &&
        ((ctoken.kind == ID) || (ctoken.kind == KAZU) || (ctoken.kind == '(')))
      {
        putstr(" * ",PUT);
      }
    if (ptoken.token != (char *)NULL) sGC_free(ptoken.token);
    ptoken = ctoken;
  }
}

  
  
/*  
    main() {
    char str[200];
    struct ring r;
    static char *x[]={"x","y","z"};
    static char *D[]={"Dx","Dy","Dz"};
    r.n = 3;
    r.x = x; r.D = D;
    gets(str);
    printf("%s\n",str2strPass0(str,&r));
    }
*/
