/*$OpenXM: OpenXM/src/kan96xx/Kan/scanner.c,v 1.10 2018/09/07 00:15:44 takayama Exp $*/
/*  scanner.c (SM StackMachine) */
/* export: struct tokens getokenSM(actionType kind,char *str);
   scanner.c is used to get tokens from streams.
   files: none
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "datatype.h"
#include "stackm.h"
struct tokens lookupTokens(struct tokens t);
int isLiteral(char *s);
struct object lookupLiteralString(char *s);
char *getLOAD_SM1_PATH();
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
/* #define mygetchar()  getSM() */
/* to use getSM()  ( input from StringSM ),
   setup  StringSM;
   getokenSM(INIT);
*/
/* to use getchar(),
   getokenSM(INIT);
*/



FILE *BaseFp = NULL;       /* Initialized to stdin in stackmachine_init().
                              file pointer of the first file. */

int EchoInScanner = 0;      /* echo in scanner */

#define BUF0LIMIT 20000
static char *StringSM;
static int StrpSM = 0;
static char BufSMorg[BUF0LIMIT];
static char *BufSM = BufSMorg;
static int Buf0limit = BUF0LIMIT;
static int ExistSM = 0;
static int TypeSM = ID;

#define FILESTACK_LIMIT 10
static FILE * Cfp;         /* current input file pointer */
static FILE * FileStack[FILESTACK_LIMIT];
static int FileStackP = 0;
/****************  end of declaration part of lexical analizer ******/

static int getSM();
static void putSM();
static struct tokens flushSM();
static int isSpaceSM();
static int isDollarSM();
static int isBraceSM();
static int isKakkoSM();
static int isSymbolSM();
static int mygetchar();
static int myungetchar();

int ScannerWhich = 0;
unsigned char ScannerBuf[SCANNERBUF_SIZE];
int ScannerPt = 0;

/*
static mygetchar()
{

  return( getc(Cfp) );
}
*/

static int mygetchar()
{ int c;
  c = getc(Cfp);
  if (c > 0) { /* ungetchar is ignored */
    ScannerPt++; if (ScannerPt >= SCANNERBUF_SIZE) ScannerPt = 0;
    ScannerBuf[ScannerPt] = c;
  }
 if (EchoInScanner) {
   if (c==EOF) {
     printf("\n%% EOF of file %p\n",Cfp);
   }else{
     printf("%c",c);
   }
   return( c );
 }else{
   return( c );
 }
}


static int myungetchar(c)
     int c;
{
  return( ungetc(c,Cfp) );
}
  
/****************  code part of lexical analizer ********************/
static int getSM()
     /* get a letter from StringSM */
{
  int c;

  if ((StrpSM > 0) && (StringSM[StrpSM] == ',') && (StringSM[StrpSM-1] == ',')) {  int i;
	fprintf(stderr,"Warning: ,, is found.");
	for (i=(StrpSM-30>0?StrpSM-30:0); i<=StrpSM; i++) {
	  fprintf(stderr,"%c",StringSM[i]);
	}
	fprintf(stderr,"\n");
  }

  c = StringSM[StrpSM++];
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
    fprintf(stderr,"\nSystem Message: Increasing BufSM to %d in scanner.c\n",Buf0limit*2);
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
    r.token = (char *)NULL; r.kind = -10; /* -1 ==> -10 */
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

struct tokens getokenSM(kind,str)
     actionType kind;
     char *str;
{
  static int c;
  static struct tokens rnull;
  int level;
  char fname[1024];
  
  if (kind == INIT) {
    ScannerWhich = 2;
    ScannerPt = 0;
    ScannerBuf[0] = 0;

    StrpSM = 0;
    ExistSM = 0;
    
    Cfp = BaseFp;  /* set the file name to BaseFp */
    FileStackP = 0; /* Clear the file stack */

    myungetchar('\n'); /* dummy */
    c = mygetchar();  /* Notice that you need at least on input to return
                         from the getokenSM(INIT); ^^^^^^^^*/
    rnull.token = (char *)NULL; rnull.kind = -1;
    return(rnull);
  }

  if (kind == OPEN) {
    rnull.token = (char *)NULL; rnull.kind = -2;
    /* -2: suceeded,  -3: fail */
    myungetchar(c);
    FileStack[FileStackP++] = Cfp;  /* push the current file */
    if (FileStackP >= FILESTACK_LIMIT) {
      fprintf(stderr,"\nError: I cannot open more than %d files.\n",FILESTACK_LIMIT);
      FileStackP--;
      c = mygetchar();
      rnull.kind = -3;
      return(rnull); /* error */
    }else {
      Cfp = fopen(str,"r");
      if (EchoInScanner)
        printf("\n%% I open the file %s.\n",str);
      if (Cfp == (FILE *)NULL) {

        strcpy(fname,getLOAD_SM1_PATH());
        strcat(fname,str);
        Cfp = fopen(fname,"r");
        if (Cfp == (FILE *)NULL) {
          strcpy(fname,LOAD_SM1_PATH);
          strcat(fname,str);
          Cfp = fopen(fname,"r");
          if (Cfp == (FILE *)NULL) {
            fprintf(stderr,"Warning: Cannot open the file <<%s>> for loading in the current directory nor the library directories %s and %s.\n",str,getLOAD_SM1_PATH(),LOAD_SM1_PATH);

            Cfp = FileStack[--FileStackP];
            fprintf(stderr,"\nWarning: I could not  open the file %s\n",str);
            c = mygetchar();
            rnull.kind = -3;
            return(rnull); /* error */
          }else{
            c = mygetchar(); /* input from the new file */
            return(rnull);
          }
        }else{  
          c = mygetchar(); /* input from the new file */
          return(rnull);
        }
      }else{
        c = mygetchar(); /* input from the new file */
        return(rnull);
      }
    }
  }

  for (;;) {
    TypeSM = ID;
    if (c == EOF) {
      if (FileStackP <= 0) {
        if (ExistSM) return(flushSM());
        else return(rnull);
      }else { /* return to the previous file */
        fclose(Cfp); /* close the file */
        Cfp = FileStack[--FileStackP];
        c = mygetchar(Cfp);
      }
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

/*
main() {
  char input[1000];
  struct tokens r;

  getokenSM(INIT);
  r = getokenSM(GET);
  while (r.token != (char *)NULL) {
    printf("%s     %d\n",r.token,r.kind);
    r =getokenSM(GET);
  }

}
*/
  
/*  
  gets(input);
  StringSM = (char *)sGC_malloc((strlen(input)+2)*sizeof(char));
  strcpy(StringSM,input);
  getokenSM(INIT);
*/
  
char *getLOAD_SM1_PATH() {
  char *p;
  char *p2;
  char *getenv(const char *s);
  p = getenv("LOAD_SM1_PATH");
  if (p == NULL) {
    p = getenv("OpenXM_HOME");
    if (p == NULL) {
      return("/usr/local/lib/sm1/");
    }else{
      if (strlen(p) == 0) return(p);
      p2 = (char *) sGC_malloc(sizeof(char)*(strlen(p)+strlen("/lib/sm1/")+3));
      if (p2 == NULL) { fprintf(stderr,"No more memory.\n"); exit(10); }
      if (p[strlen(p)-1] != '/') {
        strcpy(p2,p); strcat(p2,"/lib/sm1/");
      }else{
        strcpy(p2,p); strcat(p2,"lib/sm1/");
      }
      return(p2);
    }
  }else{
    if (strlen(p) == 0) return(p);
    if (p[strlen(p)-1] == '/') return(p);
    /* Add / */
    p2 = (char *) sGC_malloc(sizeof(char)*(strlen(p)+3));
    if (p2 == NULL) { fprintf(stderr,"No more memory.\n"); exit(10); }
    strcpy(p2,p); strcat(p2,"/");
    return(p2);
  }
}

char *traceShowScannerBuf() {
  char *s;
  int i,k;
  s = NULL;
  /*
  printf("ScannerPt=%d\n",ScannerPt);
  for (i=0; i<SCANNERBUF_SIZE; i++) {
	printf("%x ",(int) (ScannerBuf[i]));
  }
  printf("\n");  fflush(NULL); sleep(10);
  */
  if ((ScannerPt == 0) && (ScannerBuf[0] == 0)) {
    s = sGC_malloc(1); s[0] = 0;
    return s;
  }
  if ((ScannerPt > 0) && (ScannerBuf[0] == 0)) {
    s = sGC_malloc(ScannerPt+1);
    for (i=1; i<=ScannerPt; i++) {
      s[i-1] = ScannerBuf[i]; s[i] = 0;
    }
    return s;
  }
  if (ScannerBuf[0] != 0) {
    s = sGC_malloc(SCANNERBUF_SIZE+1);
    k = ScannerPt+1;
    if (k >= SCANNERBUF_SIZE) k = 0;
    for (i=0; i<SCANNERBUF_SIZE; i++) {
      s[i] = ScannerBuf[k]; s[i+1] = 0;
      k++; if (k >= SCANNERBUF_SIZE) k = 0;
    }
    return s;
  }
  return s;
}
