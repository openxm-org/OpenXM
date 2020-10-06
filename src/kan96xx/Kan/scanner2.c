/* $OpenXM: OpenXM/src/kan96xx/Kan/scanner2.c,v 1.9 2005/07/18 10:55:16 takayama Exp $ */
/*  scanner2.c (SM StackMachine) */
/* export: struct tokens decompostToTokens(char *str,int *sizep);
   scanner2.c is for getting tokens from a string.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "datatype.h"
#include "stackm.h"
struct tokens lookupTokens(struct tokens t);
int isLiteral(char *s);
struct object lookupLiteralString(char *s);
/****************  defined in stackm.h ****************************
                   typedef enum {INIT,GET,PUT,OPEN} actionType;

                   struct tokens{
                   char *token;
                   int kind;
                   };


                   #define ID   2
                   #define DOLLAR 3    strings enclosed by dollar sign 
                   #define EXECUTABLE_STRING 4  strings enclosed by {}
                   #define EXECUTABLE_ARRAY  8  Don't set it in this file.
******************************************************************/


/*******   declaration-part of lexical analizer ********************/
#define mygetchar()  getSM() 
/* to use getSM()  ( input from StringSM ),
   setup  StringSM;
   getokenSM(INIT);
*/




#define BUF0LIMIT 40000
static char *StringSM;
static int StrpSM = 0;
static char BufSMorg[BUF0LIMIT];
static char *BufSM = BufSMorg;
static int Buf0limit = BUF0LIMIT ;
static int ExistSM = 0;
static int TypeSM = ID;

/****************  end of declaration part of lexical analizer ******/

static int getSM();
static void putSM();
static struct tokens flushSM();
static int isSpaceSM();
static int isDollarSM();
static int isBraceSM();
static int isKakkoSM();
static int isSymbolSM();
static struct tokens getokenSM2();

void errorScanner2(char *str);

extern int ScannerWhich;
extern unsigned char ScannerBuf[];
extern int ScannerPt;

/****************  code part of lexical analizer ********************/

struct tokens *decomposeToTokens(str,sizep)
     char *str;
     int *sizep;
{
  struct tokens *tArray;
  struct tokens token;
  int k;
  int size;
  
  StringSM = (char *) sGC_malloc((strlen(str)+3)*sizeof(char));
  if (StringSM == (char *)NULL) {
    errorScanner2("I have no memormy.");
  }
  strcpy(StringSM,str);
  getokenSM2(INIT);
  size = 0;
  token = getokenSM2(GET);
  while (token.kind != -1) {
    size++;
    token = getokenSM2(GET);
  }

  tArray = (struct tokens *)sGC_malloc((size+1)*sizeof(struct tokens));
  strcpy(StringSM,str);
  getokenSM2(INIT);
  for (k=0; k<size; k++) {
    tArray[k] = getokenSM2(GET);
  }

  *sizep = size;
  return(tArray);
}



static int getSM()
     /* get a letter from StringSM */
{
  int c;

  if ((StrpSM > 0) && (StringSM[StrpSM] == ',') && (StringSM[StrpSM-1] == ',')) {  int i;
	fprintf(stderr,"Warning: ,, is found: ");
	for (i=(StrpSM-30>0?StrpSM-30:0); i<=StrpSM; i++) {
	  fprintf(stderr,"%c",StringSM[i]);
	}
	fprintf(stderr,"\n");
  }

  c = (unsigned char) StringSM[StrpSM++];
  if (c != 0) {
    ScannerPt++; if (ScannerPt >= SCANNERBUF_SIZE) ScannerPt = 0;
    ScannerBuf[ScannerPt] = c;
  }
  if (c == '\0') {
    StrpSM--;return(EOF);
  } else return(c);
}

static void putSM(c)
     int c;
     /* put a letter on BufSM */
{
  char *new; int i;
  BufSM[ExistSM++] = ((c=='\n')? ' ' : c);
  if (ExistSM >= Buf0limit-1)  {
    new = (char *) sGC_malloc(sizeof(char *)*Buf0limit*2) ;
    if (new == (char *)NULL) {
      fprintf(stderr,"No more memory in parserpass0.c\n");
      exit(18);
    }
    fprintf(stderr,"\nSystem Message: Increasing BufSM to %d in scanner2.c\n",Buf0limit*2);
    for (i=0; i<Buf0limit; i++) {
      new[i] = BufSM[i];
    }
    BufSM = new; Buf0limit *= 2;
  }
}

static struct tokens flushSM()
{
  char *token;
  struct tokens r;
  if (ExistSM<=0) {
    fprintf(stderr,"\n flushSM() is called without data. Don't use the empty string $$. \n");
    r.token = (char *)NULL; r.kind = -10; /* -1 ==> -10 ***/
    return(r);
  }
  BufSM[ExistSM] = '\0';
  ExistSM = 0;
  token = (char *)sGC_malloc((strlen(BufSM)+1)*sizeof(char));
  strcpy(token,BufSM);
  r.token = token;
  r.kind = TypeSM;
  r.tflag = 0;
  if (r.kind == ID) {
    if (isLiteral(r.token)) {
      r.object = lookupLiteralString(r.token);
    }else{
      r = lookupTokens(r); /* Compute hashing values */
    }
  }
  return(r);
}

static int isSpaceSM(c)
     int c;
{
  if (((c <= ' ') || c == ',') && (c!= EOF)) return(1);
  else return(0);
}

static int isDollarSM(c)
     int c;
{
  if (c == '$') return(1);
  else return(0);
}

static int isBraceSM(c)
     int c;
{
  if (c == '{') return(1);
  else return(0);
}

static int isKakkoSM(c)
     int c;
{
  if (c == '(') return(1);
  else return(0);
}

static int isSymbolSM(c)
     int c;
{
  if ((c == '{') ||
      (c == '}') ||
      (c == '[') ||
      (c == ']') ||
      (c == '(') ||
      (c == ')'))
    return(1);
  else return(0);
}

static struct tokens getokenSM2(kind,str)
     actionType kind;
     char *str;
{
  static int c;
  static struct tokens rnull;
  int level;
  
  if (kind == INIT) {
    ScannerWhich = 2;
    ScannerPt = 0;
    ScannerBuf[0] = 0;

    StrpSM = 0;
    ExistSM = 0;
    
    c = mygetchar();
    rnull.token = (char *)NULL; rnull.kind = -1;
    return(rnull);
  }



  for (;;) {
    TypeSM = ID;
    if (c == EOF) {
      if (ExistSM) return(flushSM());
      else return(rnull);
    } else if (isSpaceSM(c)) {
      if (ExistSM) {
        c = mygetchar(); return(flushSM());
      }else {
        while (isSpaceSM(c=mygetchar())) ;
      }
    } else if (isDollarSM(c)) { /* output contents in dollar signs. */
      if (ExistSM) return(flushSM());
      else {
        c = mygetchar();
        while ((c != EOF) && (c != '$')) {
          putSM(c);
          c = mygetchar();
        }
        if (c=='$') c=mygetchar();
        TypeSM = DOLLAR;
        return(flushSM());
      }
    } else if (isBraceSM(c)) { /* output contents in { }  */
      /*  { {  } } */
      level = 0;
      if (ExistSM) return(flushSM());
      else {
        c = mygetchar();
        while (1) {
          if (c == '%') { /* skip the comment in the brace. */
            while (((c=mygetchar()) != '\n') && (c != EOF))  ;
          }
          if (c == EOF) break;
          if ((c == '}') && (level <= 0)) break;
          if ( c == '{') ++level;
          if ( c == '}') --level;
          putSM(c);
          c = mygetchar();
        }
        if (c=='}') c=mygetchar();
        TypeSM = EXECUTABLE_STRING;
        return(flushSM());
      }
    } else if (isKakkoSM(c)) { /* output contents in (  )  */
      level = 0;
      if (ExistSM) return(flushSM());
      else {
        c = mygetchar();
        while (1) {
          if (c == EOF) break;
          if (c == '\\') { /* e.g. \(  */
            putSM(c);
            c = mygetchar();
            if (c == EOF) break;
          }else{
            if ((c == ')') && (level <= 0)) break;
            if ( c == '(') ++level;
            if ( c == ')') --level;
          }
          putSM(c);
          c = mygetchar();
        }
        if (c==')') c=mygetchar();
        TypeSM = DOLLAR;
        return(flushSM());
      }
    } else if (c=='%') { /* comment */
      while (((c=mygetchar()) != '\n') && (c != EOF))  ;
      if(ExistSM) return(flushSM());
    } else if (isSymbolSM(c)) { /* symbols. {,} etc */
      if(ExistSM) return(flushSM());
      else {
        putSM(c);
        c = mygetchar();
        return(flushSM());
      }
    } else { /* identifier */
      putSM(c);
      c =mygetchar();
      while ((!isDollarSM(c)) &&
             (!isSpaceSM(c))  &&
             (!isSymbolSM(c)) &&
             (c != EOF)) {
        putSM(c);
        c = mygetchar();
      }
      return(flushSM());
    }
  }
}

/*********** end of code part of lexical analizer ********************/


void errorScanner2(str)
     char *str;
{
  fprintf(stderr,"Error (scanner2.c): %s\n",str);
  exit(10);
}

