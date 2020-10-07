/* $OpenXM: OpenXM/src/k097/d.c,v 1.20 2018/11/02 01:33:26 takayama Exp $ */
/* simple.c,  1996, 1/1 --- 1/5 */
#include <stdio.h>
#include <ctype.h>
#include <setjmp.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "d.h"
#include "simple.tab.h"
#include "ox_pathfinder.h"

#if defined(__CYGWIN__)
#define JMP_BUF sigjmp_buf
#define SETJMP(env)  sigsetjmp(env,1)
#define LONGJMP(env,p)  siglongjmp(env,p)
#else
#define JMP_BUF jmp_buf
#define SETJMP(env)  setjmp(env)
#define LONGJMP(env,p)  longjmp(env,p)
#endif

JMP_BUF KCenvOfParser;

int DebugMode = 1;
extern int K00_verbose;

static FILE *outfile;
int Linenumber = 0;
objectp Inop = (objectp) NULL;  /* Input stream */
int Saki = 0;   /* Look a head */

#define SSIZE 10
objectp InopStack[SSIZE];
int     LinenumberStack[SSIZE];
int     SakiStack[SSIZE];
int     Stackp = 0;

int Mydebug = 1;
int Replace = 0;
int Debug2 = 0;
/* If you need to check only the lexical analizer,
   change main2() ---> main()
          main() ----> main2()
*/

int Interactive = 1;

static int isThereStdin();
#define MARK_CHAR  3

void main2(int argc, char *argv[]) {
  FILE *f;
  FILE *outf;
  char name[1024];
  int n;
  int i;
  if (argc < 2) {
    repl(stdin,stdout);
  }else{
    for (i=1; i<argc; i++) {
      fprintf(stderr,"Input file=%s\n",argv[i]);
      f = fopen(argv[i],"r");
      if (f == (FILE *)NULL) {
	fprintf(stderr,"Error: No file %s\n",argv[i]);
	exit(10);
      }
      strcpy(name,argv[i]);
      n = strlen(name);
      name[n] = '.'; name[n+1] = 't'; name[n+2]='\0';
      outf = fopen(name,"w");
      if (f == (FILE *)NULL) {
	fprintf(stderr,"Error: failed to open the file %s\n",name);
	exit(10);
      }
      fprintf(stderr,"Output file=%s\n",name);
      repl(f,outf);
    }
  }
}

void repl(FILE *inFile,FILE *outFile) {
  int c;
  int t;
  Inop = newObject_d();
  Inop->tag = Sfile;
  Inop->lc.file = inFile;
  Inop->rc.sbuf = newStringBuf((char *)NULL);

  outfile = outFile;
  while ( (c = KClex()) != EOF) {
    if (Mydebug) {
      if ( c > ' ' && c < 0x7f ) {
	printf("\nKClex returns --- %x [ %c ].",c,c);
      }else{
	switch(c) {
	case ID:    printf("\nKClex returns --- ID.[%s]",
			   KClval->lc.str); break;
	case QUOTE: printf("\nKClex returns --- QUOTE. [%s]",
			   KClval->lc.str); break;
	case SINGLEQUOTE:    printf("\nKClex returns --- SINGLEQUOTE.[%x]",
				    KClval->lc.ival); break;
	/*case NUMBER:    printf("\nKClex returns --- NUMBER.[%d]",
			       KClval->lc.ival); break;*/
	case NUMBER:    printf("\nKClex returns --- NUMBER.[%s]",
			       KClval->lc.str); break;

	case CLASS:    printf("\nKClex returns --- CLASS."); break;
	case SUPER:    printf("\nKClex returns --- SUPER."); break;
	case OPERATOR:    printf("\nKClex returns --- OPERATOR."); break;
	case FINAL:    printf("\nKClex returns --- FINAL."); break;
	case EXTENDS:    printf("\nKClex returns --- EXTENDS."); break;
	case INCETANCEVARIABLE:
	  printf("\nKClex returns --- INCETANCEVARIABLE."); break;
	case MODULE:    printf("\nKClex returns --- MODULE."); break;
        case SM1:       printf("\nKClex returns --- SM1."); break;
	case PRINT: printf("\nKClex returns --- PRINT."); break;
	case LOCAL: printf("\nKClex returns --- LOCAL."); break;
	case DEF: printf("\nKClex returns --- DEF."); break;
	case BREAK: printf("\nKClex returns --- BREAK."); break;
	case CASE:    printf("\nKClex returns --- CASE."); break;
	case CHAR:    printf("\nKClex returns --- CHAR."); break;
	case CONST:    printf("\nKClex returns --- CONST."); break;
	case CONTINUE:    printf("\nKClex returns --- CONTINUE."); break;
	case DEFAULT:    printf("\nKClex returns --- DEFAULT."); break;
	case DO:    printf("\nKClex returns --- DO."); break;
	case DOUBLE:    printf("\nKClex returns --- DOUBLE."); break;
	case ELSE:    printf("\nKClex returns --- ELSE."); break;
	case ENUM:    printf("\nKClex returns --- ENUM."); break;
	case EXTERN:    printf("\nKClex returns --- EXTERN."); break;
	case FLOAT:    printf("\nKClex returns --- FLOAT."); break;
	case FOR:    printf("\nKClex returns --- FOR."); break;
	case GOTO:    printf("\nKClex returns --- GOTO."); break;
	case IF:    printf("\nKClex returns --- IF."); break;
	case INT:    printf("\nKClex returns --- INT."); break;
	case LONG:    printf("\nKClex returns --- LONG."); break;
	case REGISTER:    printf("\nKClex returns --- REGISTER."); break;
	case RETURN:    printf("\nKClex returns --- RETURN."); break;
	case SHORT:    printf("\nKClex returns --- SHORT."); break;
	case SIGNED:    printf("\nKClex returns --- SIGNED."); break;
	case SIZEOF:    printf("\nKClex returns --- SIZEOF."); break;
	case STATIC:    printf("\nKClex returns --- STATIC."); break;
	case STRUCT:    printf("\nKClex returns --- SWITCH."); break;
	case TYPEDEF:    printf("\nKClex returns --- TYPEDEF."); break;
	case UNION:    printf("\nKClex returns --- UNION."); break;
	case UNSIGNED:    printf("\nKClex returns --- UNSIGNED."); break;
	case VOLATILE: printf("\nKClex returns --- VOLATILE."); break;
	case VOID: printf("\nKClex returns --- VOID."); break;
	case WHILE: printf("\nKClex returns --- WHILE."); break;

	case EQUAL: printf("\nKClex returns --- ==."); break;
	case LESSEQUAL: printf("\nKClex returns --- <=."); break;
	case LEFTSHIFT: printf("\nKClex returns --- <<."); break;
	case GREATEREQUAL: printf("\nKClex returns --- >=."); break;
	case RIGHTSHIFT: printf("\nKClex returns --- >>."); break;
	case AND: printf("\nKClex returns --- &&."); break;
	case OR: printf("\nKClex returns --- ||."); break;
	case NOTEQUAL: printf("\nKClex returns --- !=."); break;
	case PUT: printf("\nKClex returns --- :=."); break;
	case INCREMENT: printf("\nKClex returns --- ++."); break;
	case DECREMENT: printf("\nKClex returns --- --."); break;
	case MEMBER: printf("\nKClex returns --- ->."); break;
	case RESIDUEPUT: printf("\nKClex returns --- %%=."); break;
	case NEGATEPUT: printf("\nKClex returns --- ^=."); break;
	case MULTPUT: printf("\nKClex returns --- *=."); break;

	default:
	  printf("\nKClex returns --- %x(%d)[%c].",c,c,c);
	}
      }
      printf(" line=%d.\n",Linenumber);
    }
  }
}

/* --------------------------------------------------------- */
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
  case CLASSNAME_CONTEXT:
    K00fprintContext(fp,K00objectToContext(op));
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
    sprintf(tmp,"%d",op->lc.ival);
    pkkan(tmp);
    return; break;
  case Sstring:
    pkkan(op->lc.str);
    return; break;
  default:
    fprintf(stderr,"(printObjectSymbol(): Sorry. Not implemented.)");
    break;
  }
}

void printTens(objectp op)
{
  static char tmp[1024];
  int n;
  int i;
  if (op == (objectp) NULL) {
    printf(" <null objectp> ");
    return;
  }
  switch(op->tag) {
  case Sstring:
    n = strlen(op->lc.str);
    if (n > 1022) {
      fprintf(stderr,"printTens(): Too long string. ");
      return;
    }
    tmp[0] = '1';
    for (i=1; i<=n; i++) tmp[i] = '0';
    tmp[n+1] = 0;
    pkkan(tmp);
    return; break;
  default:
    fprintf(stderr,"(printTens(): Sorry. Not implemented.)");
    break;
  }
}

char *objectSymbolToString(objectp op)
{
  static char tmp[1024];
  char *r = (char *)NULL;
  if (op == (objectp) NULL) {
    printf(" <null objectp> ");
    return (r);
  }
  switch(op->tag) {
  case Sinteger:
    sprintf(tmp,"%d",op->lc.ival);
    r = (char *)GC_malloc(sizeof(char)*(strlen(tmp)+2));
    strcpy(r,tmp);
    break;
  case Sstring:
    r = (char *)GC_malloc(sizeof(char)*(strlen(op->lc.str)+2));
    strcpy(r,op->lc.str);
    break;
  default:
    fprintf(stderr,"(objectSymbolToString(): Sorry. Not implemented.)");
    break;
  }
  return(r);
}

objectp ooAdd(objectp a,objectp b)
{
  objectp r;
  if (a == (objectp)NULL || b == (objectp)NULL) return((objectp) NULL);
  if (a->tag == Sinteger && b->tag == Sinteger) {
    r = newObject_d();
    r->tag = Sinteger;
    r->lc.ival = a->lc.ival + b->lc.ival;
    return(r);
  }else{
    fprintf(stderr,"ooAdd(): Sorry. Not implemented.");
    return((objectp)NULL);
  }
}

objectp ooMult(objectp a,objectp b)
{
  objectp r;
  if (a == (objectp)NULL || b == (objectp)NULL) return((objectp) NULL);
  if (a->tag == Sinteger && b->tag == Sinteger) {
    r = newObject_d();
    r->tag = Sinteger;
    r->lc.ival = a->lc.ival * b->lc.ival;
    return(r);
  }else{
    fprintf(stderr,"ooMult(): Sorry. Not implemented.");
    return((objectp)NULL);
  }
}

/* ------------------------------------------- */

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
	   Saki == '?' || Saki == '.') {
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
      }else{
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

int KCerror(char *s)   /* You need this function. Otherwise, you get core. */
{
  K00recoverFromError();
  fprintf(stderr,"\nSyntax Error in the line %d:%s\n",Linenumber,s);
  showStringBuff(Inop);
  /* Clean the junks. Try load("debug/buggy.k"); */
  if (isThereStdin()) {
	ungetc(MARK_CHAR,stdin);
	while (fsgetc(Inop) > MARK_CHAR) ;
  }
  return 0;
  LONGJMP(KCenvOfParser,2);
  exit(1); 
}

int readcomment() {
  int c;
  while (1) {
    c = fsgetc(Inop);
    if (c == EOF) {
      fprintf(stderr,"%d: Unexpected end of file in a comment.\n",Linenumber);
      fsungetc(c,Inop); /* should change */
      return 0;
    }
    if (c == '*') {
      c = fsgetc(Inop);
      if (c == '/') return 0;
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
      fsungetc(c,Inop); /* should change */
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
	fsungetc(c,Inop); /* should change */
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
      fsungetc(c,Inop); /* should change */
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
  if (c > 0) fputc(c,outfile);
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

/* --------------------------- protection of symbols ------------------ */
#include "Stable/sm1symbol.h"
int ips(objectp op) {
  return(isProtectedSymbol(op->lc.str));
}
int isProtectedSymbol(char *s)
{

  static char **sn = SymbolTableOfsm1;
  int size = SizeOfSymbolTableOfsm1;
  int start = 0;
  int end = size-1;
  int r;

  while (1) {
    /* binary search */
    if (start>end) break;
    r = strcmp(s,sn[start+(end-start)/2]);
    if (r == 0) {
      fprintf(stderr,"Warning: Protected symbol %s is used as user variable.\n",s);
      return(0);
    }
    if (r > 0) {
      start = start + (end-start)/2 + 1;
    } else if ( r < 0) {
      end = start + (end-start)/2-1;
    }
  }
  return(0);
}

/* ------------------ read from file ------------------- */
struct stringBuf *newStringBuf(char *initstr)
{
#define INITSIZE  1024
  struct stringBuf *stringBuf;
  int limit;
  stringBuf = mymalloc(sizeof(struct stringBuf));
  if (stringBuf == (struct stringBuf *)NULL) {
    fprintf(stderr,"No memory\n"); exit(10);
  }
  if (initstr == (char *)NULL) limit = INITSIZE;
  else limit = strlen(initstr)+11;
  stringBuf->str = mymalloc(sizeof(char)*limit);
  stringBuf->ptr = 0;
  stringBuf->limit = limit;
  if (stringBuf->str == (char *)NULL) {
    fprintf(stderr,"No memory\n"); exit(10);
  }
  if (initstr != (char *)NULL) {
    strcpy(stringBuf->str,initstr);
  } else (stringBuf->str)[0] = '\0';
  return(stringBuf);
}

void doublingStringBuf(struct stringBuf *obuf)
{
  char *t;
  t = mymalloc(sizeof(char)*(obuf->limit)*2);
  if (t == (char *)NULL) {
    fprintf(stderr,"No memory\n"); exit(10);
  }
  strcpy(t,obuf->str);
  obuf->str = t;
  obuf->limit *=  2;
}

void readlineFromFile(FILE *fp,struct stringBuf *obuf)
{
  int c;
  obuf->ptr = 0; (obuf->str)[obuf->ptr]='\0';
  while (1) {
    c = fgetc(fp);
    /* for debug. 
    if (c<' ') printf(" %x(%x) ",c,(int) fp); else printf(" <%c>(%x) ",c,(int) fp); fflush(NULL);
    if (c == EOF) {
      printf("Stackp=%d ",Stackp);
      printf("File=%x\n",(int) fp); fflush(NULL);
    }
    */ 
    if (c == EOF) {
      (obuf->str)[obuf->ptr]='\0';
      obuf->ptr = 0;
      return;
    }
    (obuf->str)[(obuf->ptr)++] = c;
    (obuf->str)[obuf->ptr] = '\0';
    if (c == '\n') {
      obuf->ptr = 0;
      return;
    }
    if (obuf->ptr > obuf->limit - 5) doublingStringBuf(obuf);
  }
}

static int popFile() {
  if (Inop->tag == Sfile) {
    if (Inop->lc.file != stdin) {
      fclose(Inop->lc.file);
      /* sendKan(10);  output In[n]= */
    }
  }
  /* fprintf(stderr,"popFile(), Stackp=%d\n",Stackp); fflush(NULL);  */
  if (Stackp <= 1) return(EOF);
  Stackp--;
  Linenumber = LinenumberStack[Stackp];
  Inop = InopStack[Stackp];
  if (Inop == (objectp) NULL) {
    fprintf(stderr,"popFile: Inop==NULL\n"); fflush(stdout); /* was fflush(NULL); */
    return(EOF);
  }
  if (Inop->tag == Sfile) {
    if (Inop->lc.file != stdin) {
      Interactive = 0;
    }else{
      Interactive = 1;
    }
  }
  /* Saki = SakiStack[Stackp]; */
  Saki = '\n';
  return(Saki);
}

static int isThereStdin() {
  if (Stackp > 1 && (InopStack[1])->tag == Sfile
	  && (InopStack[1])->lc.file == stdin) {
	return(1);
  }else{
	return(0);
  }
}

int fsgetc(objectp op) {
  struct stringBuf *obuf;
  char *str;
  int ptr;
  if (op->tag == Sfile) {
    /* return(fgetc(op->lc.file));  non-buffered reading */
    obuf = op->rc.sbuf;
    str = obuf->str;
    ptr = obuf->ptr;
    if (str[ptr] == '\0') {
      readlineFromFile(op->lc.file,obuf);
      str = obuf->str;
      ptr = obuf->ptr;
    }
    if (str[ptr] == '\0') {
      return(popFile());
    } else return( str[(obuf->ptr)++] );
  }else if (op->tag == Slist) {
    obuf = op->lc.sbuf;
    str = obuf->str;
    ptr = obuf->ptr;
    if (str[ptr] == '\0') {
      return(popFile());
    } else return(str[(obuf->ptr)++]);
  }else {
    fprintf(stderr,"unknown tag?\n");
  }
  return(EOF);
}

int fsungetc(int c,objectp op) {
  fprintf(stderr,"Sorry. fsungetc has not yet implemented.\n");
}

void clearInop() {
  Saki = '\0';
  Inop = (objectp) NULL;
  Stackp = 0;
}

static void pushOld() {
  LinenumberStack[Stackp]=Linenumber;
  InopStack[Stackp] = Inop;
  SakiStack[Stackp] = Saki;
  /* fprintf(stderr,"pushOld, Stackp=%d, fp=%x\n",Stackp,(int)(Inop->lc.file)); fflush(NULL);  */
  Stackp++;
  if (Stackp >= SSIZE) {
    fprintf(stderr,"Stackp overflow.\n");
    exit(10);
  }
}

void parseAfile(FILE *fp) {
  int c;

  pushOld();

  Linenumber = 0;
  Inop = newObject_d();
  Inop->tag = Sfile;
  Inop->lc.file = fp;
  Inop->rc.sbuf = newStringBuf((char *)NULL);
  if (fp != stdin) {
    Interactive = 0;
  }

}

void parseAstring(char *s)
{
  int c;
  int prevLineNumber;
  objectp prevInop;
  int prevSaki;

  pushOld();

  Linenumber = 0;
  Inop = newObject_d();
  Inop->tag = Slist;
  Inop->lc.sbuf = newStringBuf(s);

}

objectp checkIfTheFileExists(objectp op) {
  FILE *fp;
  char fname[1024];
  char *s;
  objectp nullObj;
  nullObj = NULL;
  if (op->tag != Sstring) {
    fprintf(stderr,"File name must be given as an argment of load.\n");
    return nullObj;
  }
  if (strlen(op->lc.str) > 800) {
    fprintf(stderr,"Too long file name.\n");
    return nullObj;
  }
  strcpy(fname,op->lc.str);
  fp = fopen(fname,"r");
  if (fp == (FILE *)NULL) {
    strcpy(fname,getLOAD_K_PATH());
    strcat(fname,op->lc.str);
    fp = fopen(fname,"r");
    if (fp == (FILE *)NULL) {
      strcpy(fname,LOAD_K_PATH);
      strcat(fname,op->lc.str);
      fp = fopen(fname,"r");
      if (fp == (FILE *)NULL) {
		fprintf(stderr,"Cannot open the file <<%s>> for loading in the current directory nor the library directories %s and %s.\n",op->lc.str,getLOAD_K_PATH(),LOAD_K_PATH);
		return nullObj;
      }
    }
  }
  fclose(fp);
  op = newObject_d();
  op->tag = Sstring;
  s = (char *)GC_malloc(sizeof(char)*(strlen(fname)+1));
  if (s == NULL) fprintf(stderr,"No more memory.\n");
  strcpy(s,fname);
  op->lc.str = s;
  return(op);
}

void loadFile(objectp op)
{
  FILE *fp;
  char fname[1024];
  if (op->tag != Sstring) {
    fprintf(stderr,"File name must be given as an argment of load.\n");
    return;
  }
  op = checkIfTheFileExists(op);
  if (op == NULL) return;
  if (strlen(op->lc.str) > 1000) {
    fprintf(stderr,"Too long file name.\n");
    return;
  }
  fp = fopen(op->lc.str,"r");
  if (K00_verbose) fprintf(stderr,"Reading the file <<%s>>...  ",op->lc.str);
  parseAfile(fp);
  if (K00_verbose) fprintf(stderr,"\nClosed the file <<%s>>.\n",op->lc.str);
}

void loadFileWithCpp(objectp op)
{
  FILE *fp;
  char fname[1024];
  char tmpName[1024];
  int pid;
  objectp ob;
  char *outfile;
  char *cpp;
  char *argv[10];
  int n;
  char *sfile = NULL;
  if (op->tag != Sstring) {
    fprintf(stderr,"File name must be given as an argment of load.\n");
    return;
  }
  op = checkIfTheFileExists(op);
  if (op == NULL) return;
  if (strlen(op->lc.str) > 900) {
    fprintf(stderr,"Too long file name.\n");
    return;
  }
  /* Use gcc -v to know what symbols are defined. 
	 if  defined(linux) || defined(__linux__) 
	 Removed old codes. */

  sfile = op->lc.str;
  cpp = getCppPath();
  if (cpp == NULL) {
	fprintf(stderr,"cpp is not found.\n"); return;
  }
  /* printf("%s\n",cpp); */
  outfile = generateTMPfileName("k0-cpp");
  if (outfile == NULL) {
	fprintf(stderr,"Failed to generate a temporary file name.\n"); return;
  }
  /* printf("%s\n",outfile); */
  if ((char *)strstr(cpp,"/asir/bin/cpp.exe") == NULL) {
#if defined(__clang__) || defined(__FreeBSD__)
/* cpp of the FreeBSD is the cpp of the clang, but gcc is selected by configure.
   echo | gcc -dM -E -
*/
    sprintf(tmpName,"cpp -E -P %s | sed -e 's/^#.*//g' >%s",sfile,outfile);
#else
	argv[0] = cpp;
	argv[1] = "-P";
	argv[2] = "-lang-c++";
	argv[3] = sfile;
	argv[4] = outfile;
	argv[5] = NULL;
#endif
  }else{
	argv[0] = cpp;
	argv[1] = "-P";
	argv[2] = cygwinPathToWinPath(sfile);
	argv[3] = cygwinPathToWinPath(outfile);
	argv[4] = NULL;
  }
#if defined(__clang__) || defined(__FreeBSD__)
  system(tmpName);
#else
  n=oxForkExecBlocked(argv);
#endif

  ob = newObject_d();
  ob->tag = Sstring;
  ob->lc.str = outfile;
  loadFile(ob);
  unlink(outfile);
}

void showStringBuff(objectp op)
{
  struct stringBuf *sb;
  int i;
  int start;
  int ptr;
  if (op->tag == Sfile) {
    sb = op->rc.sbuf;
  }else if (op->tag == Slist) {
    sb = op->lc.sbuf;
  }else fprintf(stderr,"Unknown tag.\n");
  ptr = sb->ptr;
  if (K00_verbose) {
    fprintf(stderr,"stringBuff ptr = %d, ",ptr);
    fprintf(stderr,"sb[ptr] = %x,%d",(sb->str)[ptr],(sb->str)[ptr]);
    fprintf(stderr,"Saki(yomi) = %x,%d \n",Saki,Saki);
  }
  if (ptr == 0 && Saki == -1) {
    fprintf(stderr," ; was expected.\n");
  }
  start = (ptr-20<0?0:ptr-20);
  fprintf(stderr,"The error occured when the system is reading the line: ");
  for (i=start; i<ptr+20 && (sb->str)[i] >= ' '; i++) {
    fprintf(stderr,"%c",(sb->str)[i]);
    if (i==ptr-1) fprintf(stderr,"<<error>>  \n");
  }
  fprintf(stderr,"\n");
}



char *getLOAD_K_PATH() {
  return getLOAD_K_PATH2();
}
