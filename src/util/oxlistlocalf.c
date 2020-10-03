/* $OpenXM: OpenXM/src/util/oxlistlocalf.c,v 1.2 2015/10/13 06:18:26 takayama Exp $ */
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include "oxlistlocalf.h"

int readcomment();
int readcomment2();
char *readstring();
int readchar();
void putchar0(int c);
void printf0(char *s);
void printf1(char *s);
int isReserved(char *s);
int shouldReplace(char *s);
int fsgetc(FILE *fp);





objectp KClval;
int Replace = 0;
int Linenumber = 0;
int Saki = 0;
int Debug2 = 0;
FILE *Inop = NULL;

char *readstring(void);

void *mymalloc(int n) { return malloc(n); }

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
        str = readstring();
        /* if (Mydebug) printf("[string: %s]",str); */

        KClval = newObject_d();
        KClval->tag = Sstring;
        KClval->lc.str = newString(strlen(str)+1);
        strcpy(KClval->lc.str,str);
    
        Saki = fsgetc(Inop);
        return(QUOTE);
        break;
      }
      if (Saki == '\'' ) {
        d = readchar();
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
      /*if (Mydebug) printf("identifier string=[%s]",name);*/
      if (isdigit(name[0])) {
        /****************************
      /**case : machine integer. 
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
      
      if (d = isReserved(name)) {
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
      

char *readstring() {
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
    if (c == '\"') {
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
  
void putchar0(int c)
{
  if (c > 0) putchar(c);
}

void printf0(char *s)
{
  int i = 0;
  while (s[i] != '\0') putchar0(s[i++]);
}

void printf1(char *s)
{
  int i = 0;
  /* putchar0('K'); */   /* do your own replacement */
  while (s[i] != '\0') putchar0(s[i++]);
}

int isReserved(char *s)
{
  char *r[] = {"auto","break","case","char","const","continue",
               "default","do","double","else","enum","extern",
               "float","for","goto","if","int","long","register",
               "return","short","signed","sizeof","static","struct",
               "switch","typedef","union","unsigned","volatile",
               "void","while",
               "print","module","local","def","sm1","load","Test","special",
               "class","super","operator","final","extends",
               "incetanceVariable","this","new","startOfThisClass",
               "sizeOfThisClass","PSfor","OutputPrompt"};
           
  int  val[]= {AUTO, BREAK, CASE, CHAR, CONST, CONTINUE, DEFAULT, DO, DOUBLE,
               ELSE, ENUM,
               EXTERN, FLOAT, FOR, GOTO, IF, INT, LONG, REGISTER,
               RETURN, SHORT, SIGNED, SIZEOF, STATIC, STRUCT, SWITCH,
               TYPEDEF, UNION,
               UNSIGNED, VOLATILE, VOID, WHILE,
               PRINT,MODULE,LOCAL,DEF,SM1,LOAD,TEST,SPECIAL,
               CLASS,SUPER,OPERATOR,FINAL,EXTENDS,INCETANCEVARIABLE,
               THIS, NEW, STARTOFTHISCLASS, SIZEOFTHISCLASS,PSFOR,PROMPT}; 
  int  n = 52; /* Length of the list above */
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

int shouldReplace(char *s)
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


int main(int argc,char *argv[]) {
  int c;
  if (argc <= 1) Inop = stdin;
  else {
    Inop = fopen(argv[1],"r");
    if (Inop == NULL) {
      fprintf(stderr,"File %s is not found.\n",argv[1]); return(-1);
    }
  }
  while ((c=KClex()) != EOF) {
    if (c == DEF) {
      c=KClex();
	  if (c != ID) {
		fprintf(stderr,"ID (identifier) is expected, but the token type is %d\n",c);
	  }else {
        /* printf("ID=%s",(KClval->lc).str); */
        printf("localf %s$\n",(KClval->lc).str); 
      }
    }
  }
  return(0);
}
