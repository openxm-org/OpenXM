/* $OpenXM: OpenXM/src/util/oxlistusage.c,v 1.2 2017/03/19 12:39:52 takayama Exp $ */
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include "oxlistlocalf.h"

void putchar0(int c);
int fsgetc(FILE *fp);
int readchar(void);
int isReserved(char *name);
void printf0(char *s);
int shouldReplace(char *name);
void printf1(char *name);
int readcomment(void);
int readcomment2(void);

#define MAX_ITEM 1024*10
int Mydebug=0;

objectp KClval;
int Replace = 0;
int Linenumber = 0;
int Saki = 0;
int Debug2 = 0;
FILE *Inop = NULL;
char *Functions[MAX_ITEM];
char *Args[MAX_ITEM];
char *Usages[MAX_ITEM];
int Entries=0;

char *readstring(int endquote);

void *mymalloc(int n) { return malloc(n); }

void myerror(char *s) {
  fprintf(stderr,"Error: %s\n",s);
  exit(-1);
}

objectp newObject_d() {
  objectp obj;
  obj = (objectp) mymalloc(sizeof(struct Object));
  if (obj == (objectp) NULL) {
    fprintf(stderr,"Error: No more memory in newObject_d().\n");
    exit(10);
  }
  obj->tag = Snull;  obj->attr = NULL;
  return(obj);
}

char *newString(int size) {
  char *str;
  if (size <= 0) size=1;
  str = (char *)mymalloc(sizeof(char)*size);
  if (str == (char *)NULL) {
    fprintf(stderr,"Error: No more memory in newObject_d().\n");
    exit(10);
  }
  return(str);
}

void printObject_d(FILE *fp,objectp op)
{
  if (op == (objectp) NULL) {
    fprintf(fp," <null objectp> ");
    return;
  }
  switch(op->tag) {
  case Sinteger:
    fprintf(fp,"%d",op->lc.ival);
    return;
    break;
  case Sstring:
    fprintf(fp,"%s",op->lc.str);
    return;
    break;
  default:
    fprintf(stderr,"printObject_d(): Sorry. Not implemented.");
    break;
  }
}

void printObjectSymbol(objectp op)
{
  static char tmp[1024];
  if (op == (objectp) NULL) {
    printf(" <null objectp> ");
    return;
  }
  switch(op->tag) {
  case Sinteger:
    printf("%d",op->lc.ival);
    return; break;
  case Sstring:
    printf("%s",op->lc.str);
    return; break;
  default:
    fprintf(stderr,"(printObjectSymbol(): Sorry. Not implemented.)");
    break;
  }
}

int KClex() {
  extern int Saki; /* Saki-yomi */
  int d;
  int state = 0;
  int i;
  static char nametmp[1024];
  char *name = nametmp;
  char *str;
  KClval = (objectp) NULL;
  while (1) {
    switch( state ) {
    case 0:
      /* printf(" <%x> ",Saki); */
      if (Saki == EOF) {
        return(Saki);
      }
      if (Saki <= ' ') {
        if (Saki == '\n') ++Linenumber;
        if (Replace) putchar0(Saki); 
        Saki = fsgetc(Inop); break;
      }
      if ( Saki == '\"' ) {
        str = readstring('\"');
        if (Mydebug) printf("[string: %s]",str); 

        KClval = newObject_d();
        KClval->tag = Sstring;
        KClval->lc.str = newString(strlen(str)+1);
        strcpy(KClval->lc.str,str);
    
        Saki = fsgetc(Inop);
        return(QUOTE);
        break;
      }
      if (Saki == '\'' ) {
        str = readstring('\'');
        if (strlen(str)==1) d=str[0];
        else d='E';
        Saki = fsgetc(Inop);

        KClval = newObject_d();
        KClval->tag = Sinteger;
        KClval->lc.ival = d;

        return(SINGLEQUOTE);
      }
      /* single */
      if ( Saki == '(' || Saki == ')' || Saki == ';' ||
           Saki == '{' || Saki == '}' || Saki == ',' ||
           Saki == '[' || Saki == ']' || Saki == '~' ||
            Saki == '?' || Saki == '.' || Saki == '$') {
        d = Saki; Saki = fsgetc(Inop);
        if (Replace) putchar0(d);
        return(d);
      }

      /* single or combination =, == */
      if ( Saki == '=') {
        state = 51; Saki = fsgetc(Inop); break;
      } else if ( Saki == '<' ) {
        state = 52; Saki = fsgetc(Inop); break;
      } else if ( Saki == '>' ) {
        state = 53; Saki = fsgetc(Inop); break;
      } else if ( Saki == '/' ) {
        state = 54; Saki = fsgetc(Inop); break;
      } else if ( Saki == '&' ) {
        state = 55; Saki = fsgetc(Inop); break;
      } else if ( Saki == '|' ) {
        state = 56; Saki = fsgetc(Inop); break;
      } else if ( Saki == '!' ) {
        state = 57; Saki = fsgetc(Inop); break;
      } else if ( Saki == ':' ) {
        state = 58; Saki = fsgetc(Inop); break;
      } else if ( Saki == '+' ) {
        state = 59; Saki = fsgetc(Inop); break;
      } else if ( Saki == '-' ) {
        state = 60; Saki = fsgetc(Inop); break;
      } else if ( Saki == '%' ) {
        state = 61; Saki = fsgetc(Inop); break;
      } else if ( Saki == '^' ) {
        state = 62; Saki = fsgetc(Inop); break;
      } else if ( Saki == '*' ) {
        state = 63; Saki = fsgetc(Inop); break;
      } else ;


      /* else : Identifier or function names. */
      name[0] = Saki; i=1; name[i] = '\0';
      Saki = fsgetc(Inop);
      if (isdigit(name[0])) {
        /*while (isdigit(Saki) || isalpha(Saki) || Saki=='.') */
        while (isdigit(Saki) || isalpha(Saki)) {
          name[i++] = Saki; name[i] = '\0';
          Saki = fsgetc(Inop);
        }
      }else{
        while (isdigit(Saki) || isalpha(Saki) || (Saki=='_') ||
               ( Saki>= 256)) {
          name[i++] = Saki; name[i] = '\0';
          Saki = fsgetc(Inop);
        }
      }
      if (Mydebug) printf("identifier string=[%s]",name);
      if (isdigit(name[0])) {
        /****************************
      case : machine integer. 
    KClval = newObject_d();
    KClval->tag = Sinteger;
    sscanf(name,"%d",&(KClval->lc.ival));*************/
        /* Other cases.  */
        KClval = newObject_d();
        KClval->tag = Sstring;
        KClval->lc.str = newString(strlen(name)+1);
        strcpy(KClval->lc.str,name);
        return(NUMBER);
        break;
      } /* else : Identifier case.*/
      
      if ((d = isReserved(name))) {
        if (Replace) printf0(name);
        return(d);
      } else {
        if (Replace) {
          if (shouldReplace(name))
            printf1(name); /* do your own replacement in printf1*/
          else
            printf0(name);
        }
        KClval = newObject_d();
        KClval->tag = Sstring;
        KClval->lc.str = newString(strlen(name)+1);
        strcpy(KClval->lc.str,name);
        return(ID);
      }
      break;

    case 51:
      if (Replace) putchar0('=');
      if ( Saki == '=') {
        if (Replace) putchar0('=');
        Saki = fsgetc(Inop);state = 0;return(EQUAL); /* == */
      }else{
        state = 0;return('=');
      }
      break;
    case 52: /* 52 --- 60 tmporary return values */
      if (Replace) putchar0('<');
      if ( Saki == '=') {
        if (Replace) putchar0('=');
        Saki = fsgetc(Inop);state = 0;return(LESSEQUAL); /* <= */
      } else if ( Saki == '<') {
        if (Replace) putchar0('<');
        Saki = fsgetc(Inop);state = 0;return(LEFTSHIFT); /* << */
      }else{
        state = 0;return('<');
      }
      break;
    case 53: 
      if (Replace) putchar0('>');
      if ( Saki == '=') {
        if (Replace) putchar0('=');
        Saki = fsgetc(Inop);state = 0;return(GREATEREQUAL); /* >= */
      } else if ( Saki == '>') {
        if (Replace) putchar0('>');
        Saki = fsgetc(Inop);state = 0;return(RIGHTSHIFT); /* >> */
      }else{
        state = 0;return('>');
      }
      break;
    case 54: 
      if ( Saki == '*') {
        readcomment();
        Saki = fsgetc(Inop);state = 0;break; /* clike-comment */
      }else if (Saki == '/') {
        readcomment2();
        Saki = fsgetc(Inop);state = 0;break; /* comment by // */
      }else {
        if (Replace) putchar0('/');
        state = 0;return('/');
      }
      break;
    case 55: 
      if (Replace) putchar0('&');
      if ( Saki == '&') {
        if (Replace) putchar0('&');
        Saki = fsgetc(Inop);state = 0;return(AND); /* && */
      }else{
        state = 0;return('&');
      }
      break;
    case 56: 
      if (Replace) putchar0('|');
      if ( Saki == '|') {
        if (Replace) putchar0('|');
        Saki = fsgetc(Inop);state = 0;return(OR); /* || */
      }else{
        state = 0;return('|');
      }
      break;
    case 57: 
      if (Replace) putchar0('!');
      if ( Saki == '=') {
        if (Replace) putchar0('=');
        Saki = fsgetc(Inop);state = 0;return(NOTEQUAL); /* != */
      }else{
        state = 0;return('!');
      }
      break;
    case 58: 
      if (Replace) putchar0(':');
      if ( Saki == '=') {
        if (Replace) putchar0('=');
        Saki = fsgetc(Inop);state = 0;return(PUT); /* := */
      }else{
        state = 0;return(':');
      }
      break;
    case 59: 
      if (Replace) putchar0('+');
      if ( Saki == '+') {
        if (Replace) putchar0('+');
        Saki = fsgetc(Inop);state = 0;return(INCREMENT); /* ++ */
      }else{
        state = 0;return('+');
      }
      break;
    case 60: 
      if (Replace) putchar0('-');
      if ( Saki == '-') {
        if (Replace) putchar0('-');
        Saki = fsgetc(Inop);state = 0;return(DECREMENT); /* -- */
      }else if (Saki == '>') {
        if (Replace) putchar0('>');
        Saki = fsgetc(Inop);state = 0;return(MEMBER); /* -> */
      }else{
        state = 0;return('-');
      }
      break;
    case 61: 
      if (Replace) putchar0('%');
      if ( Saki == '=') {
        if (Replace) putchar0('=');
        Saki = fsgetc(Inop);state = 0;return(RESIDUEPUT); /* %= */
      }else{
        state = 0;return('%');
      }
      break;
    case 62: 
      if (Replace) putchar0('^');
      if ( Saki == '=') {
        if (Replace) putchar0('=');
        Saki = fsgetc(Inop);state = 0;return(NEGATEPUT); /* ^= */
      }else{
        state = 0;return('^');
      }
      break;
    case 63: 
      if (Replace) putchar0('*');
      if ( Saki == '=') {
        if (Replace) putchar0('=');
        Saki = fsgetc(Inop);state = 0;return(MULTPUT); /* *= */
      }else{
        state = 0;return('*');
      }
      break;


    default:
      fprintf(stderr,"%d: Error in KClex().\n",Linenumber);
    }
  }

}


int readcomment() {
  int c;
  while (1) {
    c = fsgetc(Inop);
    if (c == EOF) {
      fprintf(stderr,"%d: Unexpected end of file in a comment.\n",Linenumber);
      return 0;
    }
    if (c == '*') {
      c = fsgetc(Inop);
      if (c == '/') return 0;
    }
  }
}

int readcomment2() {
  int c;
  while (1) {
    c = fsgetc(Inop);
    if (c == EOF) {
      fprintf(stderr,"%d: Unexpected end of file in a comment.\n",Linenumber);
      return 0;
    }
    if (c == '\n') {
      return 0;
    }
  }
}
      

char *readstring(int endquote) {
  static char strtmp[1024]; /* temporary */
  static int size = 1024;
  static char *str = strtmp;
  int i=0;
  int c;
  static char *strwork;


  if (Replace) putchar0('\"'); /* output " */
  while (1) {
    c = fsgetc(Inop);
    if (Replace) putchar0(c);
    if (c == endquote) {
      str[i] = '\0';
      return(str);
    }
    if (c == EOF) {
      fprintf(stderr,"%d: Unexpected end of file in the string.\n",Linenumber);
      str[i]= '\0';
      return(str);
    }
    if (c == '\\') {
      c = fsgetc(Inop);
      if (Replace) {
	putchar0(c); 
      }
      switch(c) {  /* interprete as escape sequence. */
      case 'n': c='\n'; break;
      case 't': c='\t'; break;
      default: break;
      }
      if (c == EOF) {
        fprintf(stderr,"%d: Unexpected end of file in the escape sequence.\n",Linenumber);
        str[i]= '\0';
        return(str);
      }
    }

    str[i++] = c;
    if (i >= size-10) {
      size = size*2;
      strwork = newString(size);
      strcpy(strwork,str);
      str = strwork;
    }
  }
}
      

int readchar() {
  int c;
  if (Replace) putchar0('\'');
  c = fsgetc(Inop); /* 'c.'   '\.c' */
  if (Replace) putchar0(c);
  if ( c == '\\') {
    c = fsgetc(Inop); /* '\c.' */
    if (Replace) putchar0(c);
    if (c == EOF) {
      fprintf(stderr,"%d: Unexpected end of file in the escape sequence.\n",Linenumber);
      return(-1);
    }
    if (fsgetc(Inop) != '\'') {
      fprintf(stderr,"%d: Error in single quote string (escape seq)\n",Linenumber);
      return(c);
    }
    if (Replace) putchar0('\'');
    return(c);
  }

  if (fsgetc(Inop) != '\'') {
    fprintf(stderr,"%d: Error in single quote string\n",Linenumber);
  }
  if (Replace) putchar0('\'');
  return(c);
}
  
void putchar0(c)
  int c;
{
  if (c > 0) putchar(c);
}

void printf0(s)
  char *s;
{
  int i = 0;
  while (s[i] != '\0') putchar0(s[i++]);
}

void printf1(s)
  char *s;
{
  int i = 0;
  /* putchar0('K'); */   /* do your own replacement */
  while (s[i] != '\0') putchar0(s[i++]);
}

int isReserved(s)
  char *s;
{
  char *r[] = {"auto","break","case","char","const","continue",
               "default","do","double","else","enum","extern",
               "float","for","goto","if","int","long","register",
               "return","short","signed","sizeof","static","struct",
               "switch","typedef","union","unsigned","volatile",
               "void","while",
               "print","module","local","def","sm1","load","Test","special",
               "class","super","final","extends",
               "incetanceVariable","this","new","startOfThisClass",
               "sizeOfThisClass","PSfor","OutputPrompt"};
           
  int  val[]= {AUTO, BREAK, CASE, CHAR, CONST, CONTINUE, DEFAULT, DO, DOUBLE,
               ELSE, ENUM,
               EXTERN, FLOAT, FOR, GOTO, IF, INT, LONG, REGISTER,
               RETURN, SHORT, SIGNED, SIZEOF, STATIC, STRUCT, SWITCH,
               TYPEDEF, UNION,
               UNSIGNED, VOLATILE, VOID, WHILE,
               PRINT,MODULE,LOCAL,DEF,SM1,LOAD,TEST,SPECIAL,
               CLASS,SUPER,FINAL,EXTENDS,INCETANCEVARIABLE,
               THIS, NEW, STARTOFTHISCLASS, SIZEOFTHISCLASS,PSFOR,PROMPT}; 
  int  n = 51; /* Length of the list above */
  /* You have to change simple.y, too. */

  int i;
  for (i=0; i<n; i++) {
    if (strcmp(r[i],s) == 0) {
      if (Debug2) printf("\nReserved word: %s ---\n",s); 
      return(val[i]);
    }
  }
  return(0);

}

int shouldReplace(s)
  char *s;
{
  char *r[] = {"dummy"};
  int n = 1;
  int i;
  for (i=0; i<n; i++) {
    if (strcmp(r[i],s) == 0) {
      /* printf("\n--- %s ---\n",s); */
      return(1);
    }
  }
  return(1); /* change to 0. */
}



int fsgetc(FILE *fp) {
  int c;
  return(fgetc(fp));
}

int oxgOut(char *inFname) {
  FILE *fp;
  int i;
  fp = stdout;
  for (i=0; i<Entries; i++) {
    fprintf(fp,"/*&usage begin:\n");
    fprintf(fp,"%s%s\n",Functions[i],Args[i]);
    fprintf(fp,"description: %s\n",Usages[i]);
    fprintf(fp,"end: */\n\n");
  }
  return(0);
}

int outUsage(char *inFname,int oxg) {
  int c;
  char usage[1024*100];
  char fname[1024];
  char *args[1024];
  char outbuf[1024*10];
  char *tt;
  int prevtype;
  int i;
  Entries=0;
  while ((c=KClex()) != EOF) {
    if (c == DEF) {
      c=KClex();
      if (c != ID) {
        fprintf(stderr,"ID (identifier) is expected, but the token type is %d. See isReserved() in oxlistusage.c\n",c);
      }else {
        if (Mydebug) printf("ID=%s",(KClval->lc).str);
        strcpy(fname,(KClval->lc).str); // def ... 
        if (Mydebug) printf("fname=%s,",fname);
      }
      prevtype=0;
      i=0; args[i]=NULL;
      while (c != '{') {
        prevtype=c;
        c=KClex(); 
        if (c == QUOTE) {
          strcpy(usage,(KClval->lc).str);
          if (Mydebug) printf("usage=%s,",usage);
        }else if (c == ID) {
          if (Mydebug) { printf("tag=%d ",KClval->tag); fflush(NULL);}
          if (KClval->tag == Sstring) {
            tt = (char *) mymalloc(strlen((KClval->lc).str)+2);
            if (tt == NULL) myerror("No memory\n");
            strcpy(tt,(KClval->lc).str);
            args[i++] = tt;
            args[i] = NULL;
            if (i >1024-2) {fprintf(stderr,"def %s: ",fname); myerror("Too many args.\n");
            }
          }
        }
        if (prevtype == QUOTE) {
          /**/printf("\nFunction: %s(",fname);
          Functions[Entries]=(char *)mymalloc(strlen(fname)+2);
          strcpy(Functions[Entries],fname);
          sprintf(outbuf,"(");
          for (i=0; args[i] != NULL; i++) {
            /**/printf("%s",args[i]);
            sprintf(&(outbuf[strlen(outbuf)]),"%s",args[i]);
            if (args[i+1] != NULL) {
              /**/printf(",");
	      sprintf(&(outbuf[strlen(outbuf)]),",");
             }
          }
          /**/printf(")\n");
	  sprintf(&(outbuf[strlen(outbuf)]),")");
	  Args[Entries] = (char *)mymalloc(strlen(outbuf)+2);
	  strcpy(Args[Entries],outbuf);
          /**/printf("Usage: %s\n",usage);
	  Usages[Entries] = (char *)mymalloc(strlen(usage)+2);
	  strcpy(Usages[Entries],usage);
          Entries++;
        }
      }
    }
  }
  if (oxg) { oxgOut(inFname); Entries=0;}
  return(0);
}

int show_help() {
  printf("oxlistusage [--oxgentexi --help] filename1 ...\n");
  printf("  See also oxgentexi, keys:  Example:\n");
}

int main(int argc,char *argv[]) {
  int i;
  int oxg=0;
  Inop = stdin;
  for (i=1; i<argc; i++) {
    if (argv[i][0] == '-') {
      if (strcmp(argv[i],"--oxgentexi")==0) oxg=1;
      else if (strcmp(argv[i],"--help")==0) {
	show_help(); return(0);
      }
    }else {
      Inop = fopen(argv[i],"r");
      if (Inop == NULL) {
	fprintf(stderr,"File %s is not found.\n",argv[1]); return(-1);
      }
      outUsage(argv[i],oxg);
      fclose(Inop);
    }
  }
  if (Inop == stdin) outUsage("stdin",oxg);
}
