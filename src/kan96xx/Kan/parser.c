/* $OpenXM: OpenXM/src/kan96xx/Kan/parser.c,v 1.12 2015/10/08 11:49:37 takayama Exp $ */
/*
  parser.c   parser for poly.c
*/

#include <stdio.h>
#include <string.h>
#include "datatype.h"
#include "setjmp.h"
#include "stackm.h"
#include "extern.h"
#include "extern2.h"


#define NUM  1       /* NUM means struct Bignum */
#define POL  0

static struct ring Ring0;

extern int SerialCurrent;

union valObject {
  MP_INT *ival;
  POLY p;
};
  
#if defined(__CYGWIN__)
static sigjmp_buf EnvOfParser;
#else
static jmp_buf EnvOfParser;
#endif

static char *String;
static int  StrPtr = 0;  /* String and StrPtr are used in getcharFromStr() */
static int Ch = ' ';
static int Symbol ;
static int Value;
static int Spt = 0;
static int Spv = 0;   /* stack pointer */
#define SSIZE 20000
static int TagStack[SSIZE];
static union valObject ValStack[SSIZE];
#define MY_NAME_MAX 2000
static char Name[MY_NAME_MAX];

static union valObject ValTmp;


#define BIGNUM_ILIMIT 10000
static char Bwork[BIGNUM_ILIMIT];
static int Bp = 0;
static MP_INT *BigValue;





static int ShowPass0 = 0;     /* show the out of parserpass0.c */


static int push(int tag,union valObject val);
static int tpop(void);
static union valObject vpop(void);
static int mytolower(int c);
static int getcharFromStr(void);
static int getoken(void);
static void parse(void);
static void expr(void);
static void term(void);
static void factor(void);
static void monom(void);
static int isSpace(int c);
static int isDigit(int c);
static int isAlphabetNum(int c);
static void errorParser(char *s);


static int push(tag,val) int tag; union valObject val; {
  if (Spv<0) {
    errorParser(" parser.c stack underflow.\n");
  }
  if (Spv != Spt) {
    errorParser(" parser.c push(). incompatible Spv,Spt values.\n");
  }
  TagStack[Spt] = tag;
  ValStack[Spv] = val;
  Spv++; Spt++;
  if (Spv>SSIZE) {
    errorParser(" parser.c stack overflow.\n");
  }
}

static int tpop() {
  if (Spt<0) {
    errorParser(" parser.c stack underflow.\n");
  }
  return(TagStack[--Spt]);
}
static union valObject vpop() {
  if (Spv<0) {
    errorParser(" parser.c stack underflow.\n");
  }
  return(ValStack[--Spv]);
}
/* Warning.  tpop() & vpop(). */


POLY stringToPOLY(s,ringp) 
     char *s;
     struct ring *ringp;
{ 
  /* call init() [ poly.c ] before you use strToPoly(). */
  POLY ppp;
  char *ss;
  int k;

  /* Important NOTE.
     Parser assumes that the input is an element of Z[x].
     modulop() maps the input into R[x] where R=Z, Z/Zp or ringp->next.
     Z[x] <---> Ring0
     R[x] <---> ringp
     So, you cannot use @vname when ringp->next != NULL.
  */
  Ring0 = *ringp;
  Ring0.p = 0;
  Ring0.next = (struct ring *)NULL; 
#if defined(__CYGWIN__)
  if (MYSIGSETJMP(EnvOfParser,1)) {
#else  
  if (MYSETJMP(EnvOfParser)) {
#endif
    fprintf(stderr,"\nERROR: You have syntax errors in the expression: %s\n",s);
    errorKan1("%s\n"," parser.c : Syntax error in the input polynomial.");
    return( POLYNULL ); /* error */
  } else { }
  
  ss = (char *)sGC_malloc( strlen(s)+6 );  /* This parser think that an expression
                      -1+.. is error. ---->
                      Improved at 1992/03/04.*/
  if (ss == (char *)NULL)
    errorParser("No more space.");
  k=0; while (s[k] == ' ') k++;
  /* This method is quite adhoc. */
  /*
    if (s[k] == '+' || s[k] == '-') { 
    strcpy(ss,"0"); strcat(ss,&(s[k]));
    }else strcpy(ss,&(s[k]));
  */
  strcpy(ss,&(s[k])); /* we introduce new parser which recognize unary - */
  k = strlen(ss);
  ss[k] = ';'; ss[k+1] = '\0'; /* add ; by ad-hoc method */

  ss = str2strPass0(ss,&Ring0);
  if (ShowPass0) {
    printf("Pass0:  %s \n",ss);
  }
  /* add * and change names to xn and Dn */

  
  String = ss; StrPtr = 0;  /* initialization for getcharFromStr() */
  Ch = ' '; Spt = Spv = 0; /* initializetion for parser globals */
  getoken();
  parse();
  if (tpop()==POL) {
    ValTmp = vpop();
    ppp = (ValTmp).p;
  }else {
    ValTmp = vpop();
    ppp = bxx((ValTmp).ival,0,0,&Ring0);
  }

  ppp = modulop(ppp,ringp);
  if (ppp != POLYNULL && ppp->m->ringp != ringp) fprintf(stderr,"???\n");

  return( ppp );
}

static int mytolower(c) int c; {
  /*if (c>='A' && c<= 'Z') return( c + 0x20 );
    else return( c ); */
  return(c);   /* 1992/06/27  : do nothing now. ***/
}

static int getcharFromStr() {
  if (String[StrPtr] != '\0')
    return(mytolower(String[StrPtr++]));  /* All symbols are changed to lower*/
  else
    return(String[StrPtr]);
}

  
static int getoken() {
  int i;
  if (Ch == '\0') return( -1 );
  while (isSpace(Ch)) Ch = getcharFromStr();
  if (isDigit(Ch)) {
    Symbol = NUM; Bp = 0;
    do {
      Bwork[Bp] = Ch; Bwork[Bp+1] = '\0';
      Ch = getcharFromStr();
      Bp++;
      if (Bp >= BIGNUM_ILIMIT-1) {
        errorParser(" Too big big-num. ");
      }
    } while (isDigit(Ch));
    BigValue = newMP_INT();
    mpz_set_str(BigValue,Bwork,10);
    /* BigValue = strToBignum(Bwork); */
  } else if (Ch=='x' || Ch=='d') {
    Symbol = Ch;
    Ch = getcharFromStr();
    if (isDigit(Ch)) {
      /* Symbol = NUM; Don't do that. */
      Value = 0;
      do {
        Value = Value*10+(Ch-'0');
        Ch = getcharFromStr();
      } while (isDigit(Ch));
    }else errorParser(" Number is expected after x or d. ");
  } else if (Ch == '@') {
    Symbol = '@';
    i = 0;
    do {
      Name[i] = Ch; Name[i+1] = '\0'; i++;
      if (i+2 >= MY_NAME_MAX) {
        errorParser("Too long name begining with @.");
      }
      Ch = getcharFromStr();
    } while (isAlphabetNum(Ch));
  }else {
    Symbol = Ch;
    Ch = getcharFromStr();
  }

  /***/ /*
      if (Symbol == NUM) {
      fprintf(stderr,"\nToken type = number");
      } else {
      fprintf(stderr,"\nToken type = %c",Symbol);
      }
      fprintf(stderr,"  value is %d ",Value);
        */
  
}

static void parse() {
  expr();
  if (Symbol == ';') return;
  else errorParser(" ; is expected.");
}

static  void expr() {
  MP_INT *f,*g;
  int op;
  union valObject utmp;
  
  int gtype, ftype;   /* data type.  NUM or POL */
  POLY gp; POLY fp; POLY rp; /* g -> gp, f->fp, r -> rp , if they are
                                polynomials */
  POLY tmpp;
  
  term();

  while (Symbol == '+' || Symbol == '-') {
    op = Symbol;
    getoken();
    term();
 
    gtype = tpop(); ftype = tpop();
    if      (gtype == NUM) {
      ValTmp = vpop();
      g = (ValTmp).ival;
    } else if (gtype == POL) {
      ValTmp = vpop();
      gp= (ValTmp).p;
    }else ;
    if      (ftype == NUM) {
      ValTmp = vpop();
      f = (ValTmp).ival;
    } else if (ftype == POL) {
      ValTmp = vpop();
      fp= (ValTmp).p;
    }else ;

    /* debug */
    /* pWritef(gp,3); printf("\n\n"); sleep(5); */

    if (op == '+') {
      if (ftype == NUM && gtype == NUM) {
        mpz_add(f,f,g);
        utmp.ival = f;
        push(NUM,utmp);
      }else if (ftype== POL && gtype == NUM) {
        rp = ppAdd(fp,bxx(g,0,0,&Ring0));
        utmp.p = rp;
        push(POL, utmp);
      }else if (ftype==NUM && gtype == POL) {
        rp = ppAdd(bxx(f,0,0,&Ring0),gp);
        utmp.p = rp;
        push(POL,utmp);
      }else if (ftype==POL && gtype== POL) {
        rp = ppAdd(fp,gp); 
        utmp.p = rp;
        push(POL,utmp);
      }else ;
    
    }else {
      if (ftype == NUM && gtype == NUM) {
        mpz_sub(f,f,g);
        utmp.ival = f;
        push(NUM,utmp);
      }else if (ftype== POL && gtype == NUM) {
        rp = ppSub(fp,bxx(g,0,0,&Ring0));
        utmp.p = rp;
        push(POL, utmp);
      }else if (ftype==NUM && gtype == POL) {
        rp = ppSub(bxx(f,0,0,&Ring0),gp); 
        utmp.p = rp;
        push(POL,utmp);
      }else if (ftype==POL && gtype== POL) {
        rp = ppSub(fp,gp); 
        utmp.p = rp;
        push(POL,utmp);
      }else ;

    }
    
  }
}

static void term() {
  MP_INT *f,*g;
  int op;
  union valObject utmp;
  

  int gtype, ftype;   /* data type.  NUM or POL */
  POLY gp; POLY fp; POLY rp; /* g -> gp, f->fp, r -> rp , if they are
                                polynomials */
  POLY tmpp;


  /* Unary + and -.  For example -(1+6), -5*3, so on and so forth.
     term :          factor |
     ( + | - )factor |
     factor ( * | / ) factor ...  |
     ( + | - )factor ( * | / ) factor ...
  */
  if (Symbol == '+') {
    getoken();
    factor();
  } else if (Symbol == '-') {
    getoken();
    factor();
    /* We must change the sign */
    gtype = tpop();
    if (gtype == NUM) {
      ValTmp = vpop();
      g =  (ValTmp).ival;
      mpz_neg(g,g);   /* g = -g; */    
      utmp.ival = g;
      push(NUM,utmp);
    } else if (gtype == POL) {
      ValTmp = vpop();
      gp = (ValTmp).p;
      gp = ppMult(cxx(-1,0,0,&Ring0),gp);
      utmp.p = gp;
      push(POL,utmp);
    } else ;
  } else {
    factor();
  }
  while (Symbol=='*' || Symbol=='/') {
    op = Symbol;
    getoken();
    factor();

    gtype = tpop(); ftype = tpop();
    if      (gtype == NUM) {
      ValTmp = vpop();
      g = (ValTmp).ival;
    }else if (gtype == POL) {
      ValTmp = vpop();
      gp= (ValTmp).p;
    }else ;
    if      (ftype == NUM) {
      ValTmp = vpop();
      f = (ValTmp).ival;
    }else if (ftype == POL) {
      ValTmp = vpop();
      fp= (ValTmp).p;
    }else ;
    
    if (op == '*') {
      if (ftype == NUM && gtype == NUM) {
        mpz_mul(f,f,g);
        utmp.ival = f;
        push(NUM,utmp);
      }else if (ftype== POL && gtype == NUM) {
        fp = ppMult(bxx(g,0,0,&Ring0),fp);
        utmp.p = fp;
        push(POL, utmp);
      }else if (ftype==NUM && gtype == POL) {
        gp = ppMult(bxx(f,0,0,&Ring0),gp);
        utmp.p = gp;
        push(POL,utmp);
      }else if (ftype==POL && gtype== POL) {
        rp = ppMult(fp,gp); 
        utmp.p = rp;
        push(POL,utmp);
      }else ;
    
    }else {
      if (ftype == NUM && gtype == NUM) {
		errorParser("num/num is not supported yet.\n");
        mpz_div(f,f,g);
        utmp.ival = f;
        push(NUM,utmp);
      }else if (ftype== POL && gtype == NUM) {
        errorParser("POLY / num is not supported yet.\n"); 
        /*pvCoeffDiv(BbToCoeff(g),fp);
          utmp.p = fp;
          push(POL, utmp);*/
      }else if (ftype==NUM && gtype == POL) {
        errorParser(" / POLY is not supported yet.\n"); 
      }else if (ftype==POL && gtype== POL) {
        errorParser(" / POLY is not supported yet.\n");
      }else ;

    }


  }
}

static void factor() {
  MP_INT *f,*g;

  int gtype, ftype;   /* data type.  NUM or POL */
  POLY gp; POLY fp; POLY rp; /* g -> gp, f->fp, r -> rp , if they are
                                polynomials */
  POLY tmpp;
  union valObject utmp;

  monom();
  while (Symbol == '^') {
    getoken();
    monom();

    gtype = tpop(); ftype = tpop();
    if      (gtype == NUM) {
      ValTmp = vpop();
      g = (ValTmp).ival;
    }else if (gtype == POL) {
      ValTmp = vpop();
      gp= (ValTmp).p;
    }else ;
    if      (ftype == NUM) {
      ValTmp = vpop();
      f = (ValTmp).ival;
    }else if (ftype == POL) {
      ValTmp = vpop();
      fp= (ValTmp).p;
    }else ;
    
    if (1) {
      if (ftype == NUM && gtype == NUM) {
        /* printf("\nf=");mpz_out_str(stdout,10,f);
           printf("\ng=");mpz_out_str(stdout,10,g); */
        mpz_pow_ui(f,f,(unsigned long int) mpz_get_si(g));
        utmp.ival = f;
        push(NUM,utmp);
      }else if (ftype== POL && gtype == NUM) {
        rp = pPower(fp,(int)mpz_get_si(g));
        utmp.p = rp;
        push(POL,utmp); 
      }else if (ftype==NUM && gtype == POL) {
        errorParser("(   ) ^ Polynomial is not supported yet.\n");
      }else if (ftype==POL && gtype== POL) {
        errorParser("(   ) ^ Polynomial is not supported yet.\n");
      }else ;

    }


  }
}

static void monom() {
  union valObject utmp;
  struct object obj = OINIT;
  POLY f;
  extern struct context *CurrentContextp;
  if (Symbol == 'x' || Symbol == 'd') {
    if (Symbol == 'x') {
      utmp.p = cxx(1,Value,1,&Ring0);
      push(POL,utmp);
    }else {
      utmp.p = cdd(1,Value,1,&Ring0);
      push(POL,utmp);
    }

    getoken();

  }else if (Symbol == '@') {
    obj = findUserDictionary(&(Name[1]),hash0(&(Name[1])),hash1(&(Name[1])),
                             CurrentContextp);
    if (isNullObject(obj)) {
      fprintf(stderr,"%s",Name);
      errorParser(" cannot be found in the user dictionary.");
    }
    if (obj.tag != Spoly) {
      fprintf(stderr,"%s",Name);
      errorParser(" must be polynomial object.");
    }
    f = pcmCopy(obj.lc.poly);
    if (f==POLYNULL) {
      utmp.p = f;
      push(POL,utmp);
    }else{
      if (f->m->ringp->n != Ring0.n) {
        fprintf(stderr,"%s ",Name);
        errorParser("should be a polynomial in the current ring.\n");
      }
      utmp.p = modulo0(f,&Ring0);
      push(POL,utmp);
    }

    getoken();

  }else if (Symbol == NUM) {

    utmp.ival = BigValue;
    push(NUM,utmp);

    getoken();
  }else if (Symbol=='(') {
    getoken();
    expr();
    if (Symbol != ')') 
      errorParser(" ) is expectected. ");
    getoken();
  }else errorParser(" error in monom().");
}


static int isSpace(c) int c; {
  if (c == ' ' || c == '\t' || c == '\n') return( 1 );
  else return( 0 );
}

static int isDigit(c) int c; {
  if (c >='0' && c <= '9') return(1);
  else return(0);
}

static int isAlphabetNum(c) int c; {
  if (c>='A' && c<='Z') return(1);
  if (c>='a' && c<='z') return(1);
  if (c>='0' && c<='9') return(1);
  return(0);
}

    
  

static void errorParser(s) char s[]; {
  int i;
  extern char *GotoLabel;
  extern int GotoP;
  extern int ErrorMessageMode;
  extern int RestrictedMode, RestrictedMode_saved;
  int j;
  char tmpc[1024];
  RestrictedMode = RestrictedMode_saved;
  cancelAlarm();
  if (ErrorMessageMode == 1 || ErrorMessageMode == 2) {
    sprintf(tmpc,"\nError(parser.c): ");
    if (strlen(s) < 1000-strlen(tmpc)) {
      strcat(tmpc,s);
    }
    strcat(tmpc," The error occured around: ");
    j = strlen(tmpc);
    for (i=(((StrPtr-5) >= 0)?StrPtr-5:0)  ;
         (i<StrPtr+10) && (String[i] != '\0'); i++) {
      tmpc[j] = String[i]; j++;
      tmpc[j] = '\0';
      if (j >= 1023) break;
    }
    pushErrorStack(KnewErrorPacket(SerialCurrent,-1,tmpc));
  }
  if (ErrorMessageMode != 1) {
    fprintf(stderr,"%s\n",s);
    fprintf(stderr,"The error occured around:  ");
    for (i=(((StrPtr-5) >= 0)?StrPtr-5:0)  ;
         (i<StrPtr+10) && (String[i] != '\0'); i++)
      fprintf(stderr,"%c",String[i]);
    fprintf(stderr,"  ..... \n");
  }
  if (GotoP) {
    fprintf(Fstack,"The interpreter was looking for the label <<%s>>. It is also aborted.\n",GotoLabel);
    GotoP = 0;
  }
#if defined(__CYGWIN__)
  MYSIGLONGJMP(EnvOfParser,1);
#else
  MYLONGJMP(EnvOfParser,1);
#endif
}

