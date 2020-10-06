/* $OpenXM: OpenXM/src/kan96xx/Kan/primitive.c,v 1.25 2018/09/07 00:15:44 takayama Exp $ */
/*   primitive.c */
/*  The functions in this module were in stackmachine.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include "datatype.h"
#include "stackm.h"
#include "extern.h"
#include "extern2.h"
#include "gradedset.h"
#include "kclass.h"
#include <sys/types.h>
#include <sys/times.h>
#include "mysig.h"

int PrintDollar = 1;         /* flag for printObject() */
int PrintComma  = 1;         /* flag for printObject() */
int InSendmsg2 = 0;
#define OB_ARRAY_MAX   (AGLIMIT+100)

extern int GotoP;
extern char *GotoLabel;
extern int Osp;
extern int Sdp;
extern int ClassTypes[];   /* kclass.c */
extern struct context *PrimitiveContextp;
extern struct context *CurrentContextp;
extern struct dictionary *SystemDictionary;
extern int QuoteMode;

static char *operatorType(int i);

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

#define evalEA(ob1) if (ob1.tag == SexecutableArray) {\
	  executeExecutableArray(ob1,(char *)NULL,0); ob1 = Kpop();}

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
#define Sshow_systemdictionary 15
#define Slength            16
#define Sfor               17
#define Sroll              18
#define Squit              19
#define Stest              20      /* this is used for test of new function*/
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
#define Sdiv 81
#define Sgoto 82
#define Sextension 83
#define Snewstack 84
#define Ssetstack 85
#define Sstdstack 86
#define Slc 87
#define Src 88
#define Sgbext 89
#define Snewcontext 90
#define Ssetcontext 91
#define Ssupercontext 92
#define Ssendmsg 93
#define Serror 94
#define Smpzext 95
#define Ssendmsg2 96
#define Sprimmsg 97
#define Ssupmsg2 98
#define Scclass 99
#define Scoeff2 100
#define Stlimit 101
#define Soxshell 102
/***********************************************/
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
    case SuniversalNumber:
      fprintf(fp,"<universalNumber> ");
      break;
    case Sclass:
      fprintf(fp,"<class> ");
      break;
    case SrationalFunction:
      fprintf(fp,"<rationalFunction> ");
      break;
    case Sdouble:
      fprintf(fp,"<double> ");
      break;
    case SbyteArray:
      fprintf(fp,"<byteArray> ");
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
      fprintf(fp,"%s",ob.lc.str);
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
      case DOLLAR:
        fprintf(fp,"<<STRING(DOLLAR)>>%s ",(ta[i]).token);
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
    fprintf(fp,"Name=%s, FILE *=%p ",ob.lc.str,ob.rc.file);
    break;
  case Sring:
    fprintf(fp,"Ring."); KshowRing(KopRingp(ob));
    break;
  case SuniversalNumber:
    fprintf(fp,"%s",coeffToString(ob.lc.universalNumber));
    break;
  case SrationalFunction:
    fprintf(fp,"("); printObject(*(Knumerator(ob)),nl,fp);
    fprintf(fp,")/(");printObject(*(Kdenominator(ob)),nl,fp);
    fprintf(fp,")");
    break;
  case Sclass:
    /* fprintf(fp,"Class: "); */
    fprintClass(fp,ob);
    break;
  case Sdouble:
    fprintf(fp,"%f",KopDouble(ob));
    break;
  case SbyteArray:
    printObject(byteArrayToArray(ob),nl,fp); /* Todo: I should save memory.*/
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
  extern char *LeftBracket, *RightBracket;
  size = ob.lc.ival;
  fprintf(fp,"%s  ",LeftBracket);
  for (i=0; i<size; i++) {
    if (PrintComma && (i != 0)) {
      fprintf(fp," , ");
    }else{
      fprintf(fp,"  ");
    }
    printObject((ob.rc.op)[i],0,fp);
  }
  fprintf(fp," %s ",RightBracket);
  if (nl) fprintf(fp,"\n");
}

void  KdefinePrimitiveFunctions() {
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
  putPrimitiveFunction("print_switch_status",Sprint_switch_status);
  putPrimitiveFunction("quit",Squit);
  putPrimitiveFunction("file",Sfileopen);
  putPrimitiveFunction("closefile",Sclosefile);
  putPrimitiveFunction("idiv",Sidiv);
  putPrimitiveFunction("reduction",Sreduction);
  putPrimitiveFunction("replace",Sreplace);
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
  putPrimitiveFunction("test",Stest);
  putPrimitiveFunction("tlimit",Stlimit);
  putPrimitiveFunction("oxshell",Soxshell);
  putPrimitiveFunction("map",Smap);
  putPrimitiveFunction("to_records",Sto_records);
  putPrimitiveFunction("Usage",Susage);
  putPrimitiveFunction("load",Sload);
  putPrimitiveFunction("writestring",Swritestring);
  putPrimitiveFunction("eval",Seval);
  putPrimitiveFunction("homogenize",Shomogenize);
  putPrimitiveFunction("principal",Sprincipal);
  putPrimitiveFunction("pushfile",Spushfile);
  putPrimitiveFunction("div",Sdiv);
  putPrimitiveFunction("goto",Sgoto);
  putPrimitiveFunction("extension",Sextension);
  putPrimitiveFunction("newstack",Snewstack);
  putPrimitiveFunction("setstack",Ssetstack);
  putPrimitiveFunction("stdstack",Sstdstack);
  putPrimitiveFunction("lc",Slc);
  putPrimitiveFunction("rc",Src);
  putPrimitiveFunction("gbext",Sgbext);
  putPrimitiveFunction("newcontext",Snewcontext);
  putPrimitiveFunction("setcontext",Ssetcontext);
  putPrimitiveFunction("supercontext",Ssupercontext);
  putPrimitiveFunction("sendmsg",Ssendmsg);
  putPrimitiveFunction("error",Serror);
  putPrimitiveFunction("mpzext",Smpzext);
  putPrimitiveFunction("sendmsg2",Ssendmsg2);
  putPrimitiveFunction("primmsg",Sprimmsg);
  putPrimitiveFunction("supmsg2",Ssupmsg2);
  putPrimitiveFunction("cclass",Scclass);
  putPrimitiveFunction("coeff",Scoeff2);
}

int executePrimitive(ob) 
     struct object ob;
{
  struct object ob1 = OINIT;
  struct object ob2 = OINIT;
  struct object ob3 = OINIT;
  struct object ob4 = OINIT;
  struct object ob5 = OINIT;
  struct object rob = OINIT;
  struct object obArray[OB_ARRAY_MAX];
  struct object obArray2[OB_ARRAY_MAX];
  int size;
  int i,j,k,n;
  int status;
  int infixOn;
  struct tokens infixToken;
  struct tokens *tokenArray;
  struct tokens token;
  FILE *fp;
  char *fname;
  int rank;
  struct object oMat = OINIT;
  static int timerStart = 1;
  static struct tms before, after;
  static time_t before_real, after_real;
  struct object oInput = OINIT;
  char *str;
  int ccflag = 0;
  extern int KeepInput;
  extern int History;
  extern struct ring *CurrentRingp;
  extern int TimerOn;
  extern int SecureMode;
  extern int RestrictedMode;

  infixOn = 0;

  if (DebugStack >= 2) {
    fprintf(Fstack,"In execute %d\n",ob.lc.ival); printOperandStack();
  }

  if (RestrictedMode) {
    switch(ob.lc.ival) {
    case SleftBrace:
    case SrightBrace:
    case Sexec:
      break;
    default:
      fprintf(stderr,"primitive No = %d : ", ob.lc.ival);
      errorStackmachine("You cannot use this primitive in the RestrictedMode.\n");
    }
  }

  if (GotoP) return(0);
  switch (ob.lc.ival) {
    /* Postscript primitives :stack */
  case Sgoto:
    ob1 = Kpop();
    if (ob1.tag != Sstring) {
      if (DebugStack>=2) printObject(ob1,0,Fstack);
      errorStackmachine("Usage:goto");
    }
    GotoLabel = ob1.lc.str;
    GotoP = 1;
    break;
  case Spop:
    ob1 = Kpop();
    break;

  case Sdup:
    ob1 = Kpop();
    Kpush(ob1); Kpush(ob1);
    break;
  case Scopy:  /* copy values. cf. dup */
    ob1 = Kpop();
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
        str = (char *)sGC_malloc(strlen(ob2.lc.str)+3);
        if (str == (char *)NULL) errorStackmachine("No memory (copy)");
        strcpy(str,ob2.lc.str);
        Kpush(KpoString(str));
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
        Kpush(ob3);
        break;
      case SbyteArray:
        n = getByteArraySize(ob2);
        ob3 = newByteArray(n,ob2);
        Kpush(ob3);
        break;
      default:
        Kpush(ob2);
        break;
      }
      k++;
    }
    break;
  case Sroll:
    ob1 = Kpop();
    ob2 = Kpop();
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
        obArray[i] = Kpop();
      }else{
        errorStackmachine("exceeded OB_ARRAY_MAX (roll)\n");
      }
    }
    for (i=0; i<n; i++) {
      k = (j-1)%n;
      k = (k>=0?k: k+n);
      Kpush(obArray[k]);
      j--;
    }
    break;
  case Spstack:
    printOperandStack();
    break;

    /* Postscript primitives :arithmetic */
  case Sadd:
    ob1 = Kpop();  
    ob2 = Kpop();
    evalEA(ob1); evalEA(ob2);
    rob = KooAdd(ob1,ob2);
    Kpush(rob);
    break;
  case Ssub:
    ob2 = Kpop();
    ob1 = Kpop();
    evalEA(ob1); evalEA(ob2);
    rob = KooSub(ob1,ob2);
    Kpush(rob);
    break;
  case Smult:
    ob2 = Kpop();
    ob1 = Kpop();
    evalEA(ob1); evalEA(ob2);
    rob = KooMult(ob1,ob2);
    Kpush(rob);
    break;
  case Sidiv:
    ob2 = Kpop(); ob1 = Kpop();
    evalEA(ob1); evalEA(ob2);
    rob = KooDiv(ob1,ob2);
    Kpush(rob);
    break;

  case Sdiv:
    ob2 = Kpop(); ob1 = Kpop();
    evalEA(ob1); evalEA(ob2);
    rob = KooDiv2(ob1,ob2);
    Kpush(rob);
    break;

    /* Postscript primitives :array */
  case SleftBrace:
    rob.tag = SleftBraceTag;
    Kpush(rob);
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
          Kpop();
        }
        break;
      }
      size++;
      ob1 = peek(size);
    }
    Kpush(rob);
    break;
    
  case Sget:
    /* [a_0 ... a_{n-1}] i   get a_i */
    /* ob2               ob1 get     */
    ob1 = Kpop();
    ob2 = Kpop();
    Kpush(Kget(ob2,ob1));
    break;

  case Sput:
    /*    [a_0 ... a_{n-1}] index any put */
    /* ob3               ob2   ob1 put     */
    /* Or;  [[a_00 ....] [a_10 ....] ....] [1 0] any put. MultiIndex. */
    ob1 = Kpop(); ob2 = Kpop(); ob3 = Kpop();
    switch(ob2.tag) {
    case SuniversalNumber:
      ob2 = Kto_int32(ob2); /* do not break and go to Sinteger */ 
    case Sinteger: 
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
      case SbyteArray:
        i = ob2.lc.ival;
        size = getByteArraySize(ob3);
        if ((0 <= i) && (i<size)) {
		  if (ob1.tag != Sinteger) ob1 = Kto_int32(ob1);
          if (ob1.tag != Sinteger) errorStackmachine("One can put only integer.\n");
          KopByteArray(ob3)[i] = KopInteger(ob1);
        }else{
          errorStackmachine("Index is out of bound. (put)\n");
        }
        break;
      default: errorStackmachine("Usage:put");
      }
      break;
    case Sarray:
      ob5 = ob3;
      n = getoaSize(ob2);
      for (i=0; i<n; i++) {
        if (ob5.tag != Sarray)
          errorStackmachine("Object pointed by the multi-index is not array (put)\n");
        ob4 = getoa(ob2,i);
        if (ob4.tag == SuniversalNumber) ob4 = Kto_int32(ob4);
        if (ob4.tag != Sinteger)
          errorStackmachine("Index has to be an integer. (put)\n");
        k = ob4.lc.ival;
        size = getoaSize(ob5);
        if ((0 <= k) && (k<size)) {
          if (i == n-1) {
            getoa(ob5,k) = ob1;
          }else{
            ob5 = getoa(ob5,k);
          }
        }else{
          errorStackmachine("Index is out of bound for the multi-index. (put)\n");
        }
      }
      break;
    default: errorStackmachine("Usage:put");
    }
    break;

  case Sindex:
    ob1 = Kpop();
    switch(ob1.tag) {
    case Sinteger: break;
    default: errorStackmachine("Usage:index");
    }
    size = ob1.lc.ival;
    Kpush(peek(size-1));
    break;

  case Saload:
    /* [a1 a2 ... an] aload a1 a2 ... an [a1 ... an] */
    ob1 = Kpop();
    switch(ob1.tag) {
    case Sarray: break;
    default:
      errorStackmachine("Usage:aload");
    }
    size = getoaSize(ob1);
    for (i=0; i<size; i++) {
      Kpush(getoa(ob1,i));
    }
    Kpush(ob1);
    
    break;

  case Slength:
    /* [a_0 ... a_{n-1}] length n */
    /*               ob1 length rob */
    ob1 = Kpop();
    switch(ob1.tag) {
    case Sarray:
      size = getoaSize(ob1);
      Kpush(KpoInteger(size));
      break;
    case Sdollar:
      Kpush(KpoInteger(strlen(ob1.lc.str)));
      break;
    case Spoly:
      Kpush(KpoInteger(KpolyLength(KopPOLY(ob1))));
      break;
    case SbyteArray:
      Kpush(KpoInteger(getByteArraySize(ob1)));
      break;
    default: errorStackmachine("Usage:length");
    }
    break;

    /* Postscript primitives :relation */
  case Sequal:
    /* obj1 obj2 == bool */
    ob2 = Kpop();
    ob1 = Kpop();
    if(KooEqualQ(ob1,ob2)) {
      Kpush(KpoInteger(1));
    }else{
      Kpush(KpoInteger(0));
    }
    break;

  case Sless:
    /* obj1 obj2 < bool */
    ob2 = Kpop();
    ob1 = Kpop();
    Kpush(KooLess(ob1,ob2));
    break;

  case Sgreater:
    /* obj1 obj2 < bool */
    ob2 = Kpop();
    ob1 = Kpop();
    Kpush(KooGreater(ob1,ob2));
    break;


    /* Postscript primitives :controle */
  case Sloop:
    /* { .... exit .....} loop */
    ob1 = Kpop();
    switch(ob1.tag) {
    case SexecutableArray: break;
    default:
      errorStackmachine("Usage:loop");
      break;
    }
    while (1) {
      status = executeExecutableArray(ob1,(char *)NULL,1);
      if ((status & STATUS_BREAK) || GotoP) break;
      /* here, do not return 1. Do not propagate exit signal outside of the
         loop. */
    }
    break;

  case Sfor:
    /* init inc limit { } for */
    /* ob4  ob3 ob2   ob1 */
    ob1 =Kpop(); ob2 = Kpop(); ob3 = Kpop(); ob4 = Kpop();
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
          Kpush(KpoInteger(i));
          status = executeExecutableArray(ob1,(char *)NULL,1);
          if ((status & STATUS_BREAK) || GotoP) goto xyz;
		}
      }else{
        /*
          if (lim > i) errorStackmachine("The initial value must not be less than limit value (for).\n");
        */
        for ( ; i>=lim; i += inc) {
          Kpush(KpoInteger(i));
          status = executeExecutableArray(ob1,(char *)NULL,1);
          if ((status & STATUS_BREAK) || GotoP) goto xyz;
        }
      }
    xyz:  ;
    }
    break;

  case Smap:
    ob2 = Kpop(); ob1 = Kpop();
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
    Kpush(rob);

    for (i=0; i<osize; i++) {
      Kpush(getoa(ob1,i));
      status = executeExecutableArray(ob2,(char *)NULL,0);
      if (status & STATUS_BREAK) goto foor;
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
            Kpop();
          }
          break;
        }
        size++;
        ob1 = peek(size);
      }
      Kpush(rob);
    }
    }
    break;


  case Sifelse:
    /* bool { } { } ifelse */
    ob1 = Kpop();
    ob2 = Kpop();
    ob3 = Kpop();
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
    status = executeExecutableArray(ob1,(char *)NULL,0);
    if (status & STATUS_BREAK) return(status);

    break;

  case Sexec:
    /* { .........} exec */
    ob1 = Kpop();
    switch(ob1.tag) {
    case SexecutableArray: break;
    default: errorStackmachine("Usage:exec");
    }
	status = executeExecutableArray(ob1,(char *)NULL,0);
    break;

    /* Postscript primitives :dictionary */    
  case Sdef:
    ob2 = Kpop();
    ob1 = Kpop();
    /* type check */
    switch(ob1.tag) {
    case Sstring: break;
    default:
      errorStackmachine("Usage:def");
      break;
    }
    k=putUserDictionary(ob1.lc.str,(ob1.rc.op->lc).ival,
                        (ob1.rc.op->rc).ival,ob2,
                        CurrentContextp->userDictionary);
    if (k < 0) {
      str = (char *)sGC_malloc(sizeof(char)*(strlen(ob1.lc.str) + 256));
      if (str == (char *)NULL) {
        errorStackmachine("No memory.\n");
      }
      if (k == -PROTECT) {
        sprintf(str,"You rewrited the protected symbol %s.\n",ob1.lc.str);
        /*  cf. [(chattr) num sym] extension */
        warningStackmachine(str);
      } else if (k == -ABSOLUTE_PROTECT) {
        sprintf(str,"You cannot rewrite the protected symbol %s.\n",ob1.lc.str);
        errorStackmachine(str);
      } else errorStackmachine("Unknown return value of putUserDictioanry\n");
    }
    break;
    
  case Sload:
    ob1 = Kpop();
    switch(ob1.tag) {
    case Sstring: break;
    default: errorStackmachine("Usage:load");
    }
    ob1 =  findUserDictionary(ob1.lc.str,
                              (ob1.rc.op->lc).ival,
                              (ob1.rc.op->rc).ival,
                              CurrentContextp);
    if (ob1.tag == -1) Kpush(NullObject);
    else Kpush(ob1);
    
    break;

  case Sset:
    ob1 = Kpop();
    ob2 = Kpop();
    switch(ob1.tag) {
    case Sstring: break;
    default: errorStackmachine("Usage:set");
    }
    k= putUserDictionary(ob1.lc.str,(ob1.rc.op->lc).ival,
                         (ob1.rc.op->rc).ival,ob2,
                         CurrentContextp->userDictionary);
    if (k < 0) {
      str = (char *)sGC_malloc(sizeof(char)*(strlen(ob1.lc.str) + 256));
      if (str == (char *)NULL) {
        errorStackmachine("No memory.\n");
      }
      if (k == -PROTECT) {
        sprintf(str,"You rewrited the protected symbol %s. \n",ob1.lc.str);
        warningStackmachine(str);
      } else if (k == -ABSOLUTE_PROTECT) {
        sprintf(str,"You cannot rewrite the protected symbol %s.\n",ob1.lc.str);
        errorStackmachine(str);
      } else errorStackmachine("Unknown return value of putUserDictioanry\n");
    }
    break;


  case Sshow_systemdictionary:
    fprintf(Fstack,"------------- system dictionary -------------------\n");
    showSystemDictionary(0);
    break;

  case Sshow_user_dictionary:
    showUserDictionary();
    break;



    /* Postscript primitives : convert */
  case Sdata_conversion:
    ob2 = Kpop();
    ob1 = Kpop();
    switch(ob2.tag) {
    case Sdollar:
      if (ob1.tag != Sclass) {
        rob = KdataConversion(ob1,ob2.lc.str);
      }else{
        rob = KclassDataConversion(ob1,ob2);
      }
      break;
    case Sarray:
      rob = KclassDataConversion(ob1,ob2); break;
    default: errorStackmachine("Usage:data_conversion");
    }
    Kpush(rob);
    break;
    

    /* Postscript ptimitives :file */
  case Srun:
    ob1 = Kpop();
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
    ob1 = Kpop();
    printObject(ob1,0,Fstack);
    break;

  case Sfileopen: /* filename  mode   file  descripter */
    /* ob2       ob1  */
    ob1 = Kpop();
    ob2 = Kpop();
    if (SecureMode) errorStackmachine("Security violation: you cannot open a file.");
    switch(ob1.tag) {
    case Sdollar: break;
    default: errorStackmachine("Usage:file");
    }
    switch(ob2.tag) {
    case Sinteger: break;
    case Sdollar: break;
    default:errorStackmachine("Usage:file");
    }
    rob = NullObject;
    if (ob2.tag == Sdollar) {
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
    }else {
      rob.rc.file = fdopen(ob2.lc.ival,ob1.lc.str);
      if ( rob.rc.file != (FILE *)NULL) {
        rob.tag = Sfile; rob.lc.ival = ob2.lc.ival;
      }else{
        errorStackmachine("I cannot fdopen the given fd.");
      }
    }

    Kpush(rob);
    break;


  case Swritestring:
    /* file string writestring
       ob2  ob1
    */
    ob1 = Kpop();
    ob2 = Kpop();
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
    ob1 = Kpop();
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
    ob2 = Kpop();
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
      if (ob1.rc.file == (FILE *)NULL) {
        char fname2[1024];
        strcpy(fname2,getLOAD_SM1_PATH());
        strcat(fname2,ob2.lc.str);
        ob1.rc.file = fopen(fname2,"r");
        if (ob1.rc.file == (FILE *)NULL) {
          strcpy(fname2,LOAD_SM1_PATH);
          strcat(fname2,ob2.lc.str);
          ob1.rc.file = fopen(fname2,"r");
          if (ob1.rc.file == (FILE *)NULL) {
            fprintf(stderr,"Warning: Cannot open the file <<%s>> for loading in the current directory nor the library directories %s and %s.\n",ob2.lc.str,getLOAD_SM1_PATH(),LOAD_SM1_PATH);
            errorStackmachine("I cannot open the file.");
          }
        }
      }
    }

    /* read the strings
     */
    n = 256; j=0;
    rob.tag = Sdollar; rob.lc.str = (char *) sGC_malloc(sizeof(char)*n);
    if (rob.lc.str == (char *)NULL) errorStackmachine("No more memory.");
    while ((i = fgetc(ob1.rc.file)) != EOF) {
      if (j >= n-1) {
        n = 2*n;
        if (n <= 0) errorStackmachine("Too large file to put on the stack.");
        str = (char *)sGC_malloc(sizeof(char)*n);
        if (str == (char *)NULL) errorStackmachine("No more memory.");
        for (k=0; k< n/2; k++) str[k] = (rob.lc.str)[k];
        rob.lc.str = str;
      }
      (rob.lc.str)[j] = i; (rob.lc.str)[j+1] = '\0';
      j++;
    }
      
    fclose(ob1.rc.file);
    Kpush(rob);
    break;

    /* Postscript primitives :misc */
  case Squit:
    Kclose(); stackmachine_close();
    exit(0);
    break;

  case Ssystem:
    ob1 = Kpop();
    switch(ob1.tag) {
    case Sdollar: break;
    case Sstring: break;
    default: errorStackmachine("Usage:system");
    }
    if (SecureMode) errorStackmachine("Security violation.");
    {int rr; rr=system( ob1.lc.str );}
    break;

  case Scat_n:
    ob1 = Kpop();
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
      default:  errorStackmachine("Usage:cat_n");
      }
      k += strlen(ob2.lc.str);
    }
    ob1.tag = Sdollar;
    ob1.lc.str = (char *)sGC_malloc(sizeof(char)*(k+1));
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
      ob2 = Kpop();
    }
    Kpush(ob1);
    break;

  case Sset_timer:
    /* 118p */
    if (timerStart) {
      before_real = time(&before_real);
      times(&before);
      timerStart = 0; TimerOn = 1;
    }else{
      times(&after);
      after_real = time(&after_real);
      if (TimerOn) {
        printf("User time: %f seconds, System time: %f seconds, Real time: %d s\n",
               ((double)(after.tms_utime - before.tms_utime)) /100.0,
               ((double)(after.tms_stime - before.tms_stime)) /100.0,
               (int) (after_real-before_real));
        /* In cases of Solaris and Linux, the unit of tms_utime seems to
           be given 0.01 seconds. */

      }
      timerStart = 1; TimerOn = 0;
    }
    break;

  case Susage:
    ob1 = Kpop();
    Kusage(ob1);
    break;

  case Sto_records:
    ob1 = Kpop();
    switch(ob1.tag) {
    case Sdollar: break;
    default:  errorStackmachine("Usage:to_records");
    }
    ob2 = KtoRecords(ob1);
    size = getoaSize(ob2);
    for (i=0; i<size; i++) {
      Kpush(getoa(ob2,i));
    }
    rob.tag = Sinteger;
    rob.lc.ival = size;
    Kpush(rob);
    break;

  case Ssystem_variable:
    ob1 = Kpop();
    switch(ob1.tag) {
    case Sarray: break;
    default: errorStackmachine("Usage:system_variable");
    }
    Kpush(KsystemVariable(ob1));
    break;

    /* kan primitives :kan :ring */
  case Sset_order_by_matrix:
    ob1 = Kpop();
    KsetOrderByObjArray(ob1);
    break;
  case Sset_up_ring:
	KresetDegreeShift();
    ob5 = Kpop(); ob4=Kpop(); ob3=Kpop(); ob2=Kpop(); ob1=Kpop();
    KsetUpRing(ob1,ob2,ob3,ob4,ob5);
    break;
  case Sshow_ring:
    KshowRing(CurrentRingp);
    break;
  case Sswitch_function:
    ob1 = Kpop();
    ob2 = Kpop();
    ob3 = KswitchFunction(ob2,ob1);
    if (!isNullObject(ob3)) {
      Kpush(ob3);
    }
    break;
  case Sprint_switch_status:
    KprintSwitchStatus();
    break;
  case Sreplace:
    ob2 = Kpop();
    ob1 = Kpop();
    Kpush(KoReplace(ob1,ob2));
    break;
    
  case Scoefficients:
    ob2 = Kpop();
    ob1 = Kpop();
    Kpush(Kparts(ob1,ob2));
    break;

  case Scoeff2:
    ob2 = Kpop();
    ob1 = Kpop();
    Kpush(Kparts2(ob1,ob2));
    break;

  case Sdegree:
    ob2 = Kpop();
    ob1 = Kpop();
    Kpush(Kdegree(ob1,ob2));
    break;
  case Sspol:
    ob2 = Kpop();
    ob1 = Kpop();
    Kpush(Ksp(ob1,ob2));
    break;

  case Seval:
    ob1 = Kpop();
    Kpush(Keval(ob1));
    break;

  case Sreduction:
    ob2 = Kpop();
    ob1 = Kpop();
    Kpush(Kreduction(ob1,ob2));
    break;
    
  case Sgroebner :
    ob1 = Kpop();
    Kpush(Kgroebner(ob1));
    break;

  case Shomogenize :
    ob1 = Kpop();
    Kpush(homogenizeObject(ob1,&i));
    break;

  case Sprincipal :
    ob1 = Kpop();
    Kpush(oPrincipalPart(ob1));
    break;

  case Sinit:
    ob2 = Kpop();
    if (ob2.tag != Sarray) {
      Kpush(Khead(ob2));
    }else{
      if (getoaSize(ob2) > 0) {
        if (getoa(ob2,getoaSize(ob2)-1).tag == Spoly) {
          Kpush(oInitW(ob2,newObjectArray(0)));
        }else{
          ob1 = Kpop();
          Kpush(oInitW(ob1,ob2));
        }
      }else{
        ob1 = Kpop();
        Kpush(oInitW(ob1,ob2));
      }
    }
    break;

  case Sextension:
    ob1 = Kpop();
    Kpush(Kextension(ob1));
    break;
    
  case Sgbext:
    ob1 = Kpop();
    Kpush(KgbExtension(ob1));
    break;
    
  case Snewstack:
    ob1 = Kpop();
    switch(ob1.tag) {
    case Sinteger:
      Kpush(newOperandStack(ob1.lc.ival));
      break;
    default:
      errorStackmachine("Usage:newstack");
      break;
    }
    break;
    
  case Ssetstack:
    ob1 = Kpop();
    switch(ob1.tag) {
    case Sclass:
      setOperandStack(ob1);
      break;
    default:
      errorStackmachine("Usage:setstack");
      break;
    }
    break;
    
  case Sstdstack:
    stdOperandStack();
    break;
    
  case Slc:
    ob1 = Kpop();
    switch (ob1.tag) {
    case Sclass:
      Kpush(KpoInteger(ob1.lc.ival));
      break;
    default:
      errorStackmachine("Usage:lc");
      break;
    }
    break;
    
  case Src:
    ob1 = Kpop();
    switch (ob1.tag) {
    case Sclass:
      if (ClassTypes[ob1.lc.ival] == CLASS_OBJ) {
        Kpush(*(ob1.rc.op));
      }else{
        warningStackmachine("<<obj rc >> works only for a class object with CLASS_OBJ attribute.\n");
        Kpush(ob1);
      }
      break;
    default:
      errorStackmachine("Usage:rc");
      break;
    }
    break;
    
  case Snewcontext:
    ob1 = Kpop();
    ob2 = Kpop();
    switch(ob1.tag) {
    case Sclass:
      if (ob2.tag == Sdollar) {
        Kpush(KnewContext(ob1,KopString(ob2)));
      }else  errorStackmachine("Usage:newcontext");
      break;
    default:
      errorStackmachine("Usage:newcontext");
      break;
    }
    break;
    
  case Ssetcontext:
    ob1 = Kpop();
    switch(ob1.tag) {
    case Sclass:
      KsetContext(ob1);
      break;
    default:
      errorStackmachine("Usage:setcontext");
      break;
    }
    break;

  case Ssupercontext:
    ob1 = Kpop();
    switch(ob1.tag) {
    case Sclass:
      Kpush(getSuperContext(ob1));
      break;
    default:
      errorStackmachine("Usage:supercontext");
      break;
    }
    break;

  case Ssendmsg:
    /* ob2 { .........} sendmsg */
    /* cf. debug/kobj.sm1    */
    ob1 = Kpop();
    ob2 = Kpop();
    switch(ob1.tag) {
    case SexecutableArray: break;
    default: errorStackmachine("Usage:sendmsg");
    }
    ccflag = 0;
    if (ob2.tag == Sarray ) {
      if (getoaSize(ob2) >= 1) {
        ob3 = getoa(ob2,0);
        if (ectag(ob3) == CLASSNAME_CONTEXT) {
          contextControl(CCPUSH); ccflag = 1; /* push the current context. */
          CurrentContextp = (struct context *)ecbody(ob3);
        }
      }
    }
    if (!ccflag) {
      contextControl(CCPUSH); ccflag = 1;
      CurrentContextp = PrimitiveContextp;
    }
    /* normal exec. */ 
    Kpush(ob2);
    status = executeExecutableArray(ob1,(char *)NULL,0);

    if (ccflag) {
      contextControl(CCPOP); ccflag = 0; /* recover the Current context. */
    }

    break;
  case Ssendmsg2:
    /* ob2 ob4 { .........} sendmsg2 */
    /* Context is determined by ob2 or ob1 */
    ob1 = Kpop();
    ob4 = Kpop();
    ob2 = Kpop();
    switch(ob1.tag) {
    case SexecutableArray: break;
    default: errorStackmachine("Usage:sendmsg2");
    }
    ccflag = 0;
    if (ob2.tag == Sarray ) {
      if (getoaSize(ob2) >= 1) {
        ob3 = getoa(ob2,0);
        if (ectag(ob3) == CLASSNAME_CONTEXT) {
          contextControl(CCPUSH); ccflag = 1; /* push the current context. */
          CurrentContextp = (struct context *)ecbody(ob3);
        }
      }
    }
    if (!ccflag && ob4.tag == Sarray) {
      if (getoaSize(ob4) >= 1) {
        ob3 = getoa(ob4,0);
        if (ectag(ob3) == CLASSNAME_CONTEXT) {
          contextControl(CCPUSH); ccflag = 1; /* push the current context. */
          CurrentContextp = (struct context *)ecbody(ob3);
        }
      }
    }
    if (!ccflag) {
      contextControl(CCPUSH); ccflag = 1;
      CurrentContextp = PrimitiveContextp;
    }
    /* normal exec. */ 
    Kpush(ob2); Kpush(ob4);

    /* We cannot use executeExecutableArray(ob1,(char *)NULL) because of
       the quote mode. Think about it later. */
    tokenArray = ob1.lc.tokenArray;
    size = ob1.rc.ival;
    for (i=0; i<size; i++) {
      token = tokenArray[i];
      InSendmsg2 = 1;
      status = executeToken(token);
      InSendmsg2 = 0;

      if (status & STATUS_INFIX) {
		if (status & DO_QUOTE) errorStackmachine("infix op with DO_QUOTE\n");
        if (i == size-1) errorStackmachine("infix operator at the end(sendmsg2).\n");
        infixOn = 1; infixToken = tokenArray[i];
        infixToken.tflag |= NO_DELAY; continue;
      }else if (infixOn) {
        infixOn = 0; status = executeToken(infixToken);
        if (status & STATUS_BREAK) break;
      }

      if (QuoteMode && (status & DO_QUOTE)) {
        /* generate tree object, for kan/k0 */
        struct object qob = OINIT;
        struct object qattr = OINIT;
        struct object qattr2 = OINIT;
        if (i==0) { Kpop(); Kpop();}
        qob = newObjectArray(3);
        qattr = newObjectArray(1);
        qattr2 = newObjectArray(2);
		/* Set the node name of the tree. */
        if (token.kind == ID) {
          putoa(qob,0,KpoString(token.token));
        }else{
          putoa(qob,0,KpoString("unknown"));
        }
        /* Set the attibute list; class=className */
        if (ob2.tag == Sdollar) {
          putoa(qattr2,0,KpoString("cd"));
          putoa(qattr2,1,ob2);
        }else{
          putoa(qattr2,0,KpoString("class"));
          putoa(qattr2,1,KpoString(CurrentContextp->contextName));
        }
        putoa(qattr,0,qattr2);
        putoa(qob,1,qattr);
        putoa(qob,2,ob4);  /* Argument */
        qob = KpoTree(qob);
        Kpush(qob);
      } else if (status & STATUS_BREAK) break;

    }
    if (ccflag) {
      contextControl(CCPOP); ccflag = 0;
      /* recover the Current context. */
      /* Note that it is not recovered in case of error. */
    }
    
    break;
  case Sprimmsg:
    /* { .........} primmsg */
    /* Context is PrimitiveContext. */
    ob1 = Kpop();
    switch(ob1.tag) {
    case SexecutableArray: break;
    default: errorStackmachine("Usage:primmsg");
    }
    contextControl(CCPUSH); ccflag = 1;
    CurrentContextp = PrimitiveContextp;
    /* normal exec. */ 
    status = executeExecutableArray(ob1,(char *)NULL,0);
    contextControl(CCPOP); /* recover the Current context. */
    break;

  case Ssupmsg2:
    /* ob2 ob4 { .........} supmsg2 */
    /* Context is super class of ob2 */
    ob1 = Kpop();
    ob4 = Kpop();
    ob2 = Kpop();
    switch(ob1.tag) {
    case SexecutableArray: break;
    default: errorStackmachine("Usage:supmsg2");
    }
    ccflag = 0;
    if (ob2.tag == Sarray ) {
      if (getoaSize(ob2) >= 1) {
        ob3 = getoa(ob2,0);
        if (ectag(ob3) == CLASSNAME_CONTEXT) {
          if (((struct context *)ecbody(ob3))->super == NULL) {
            errorStackmachine("supmsg2: SuperClass is NIL.");
          }
          contextControl(CCPUSH); ccflag = 1; /* push the current context. */
          CurrentContextp = ((struct context *)ecbody(ob3))->super;
        }
      }
    }
    if (!ccflag && (ob4.tag == Sarray) ) {
      if (getoaSize(ob4) >= 1) {
        ob3 = getoa(ob4,0);
        if (ectag(ob3) == CLASSNAME_CONTEXT) {
          if (((struct context *)ecbody(ob3))->super == NULL) {
            errorStackmachine("supmsg2: SuperClass is NIL.");
          }
          contextControl(CCPUSH); ccflag = 1; /* push the current context. */
          CurrentContextp = ((struct context *)ecbody(ob3))->super;
        }
      }
    }
    if (!ccflag) {
      contextControl(CCPUSH); ccflag = 1;
      CurrentContextp = PrimitiveContextp;
    }
    /* normal exec. */ 
    Kpush(ob2); Kpush(ob4);
    status = executeExecutableArray(ob1,(char *)NULL,0);
    if (ccflag) {
      contextControl(CCPOP); ccflag = 0; /* recover the Current context. */
    }
    
    break;

  case Serror:
    ob1 = peek(0);
    if (ob1.tag == Sdollar) {
      /* compose error message */
      ob = Kpop();
      str = (char *) sGC_malloc(sizeof(char)*(strlen("error operator : ")+
                                              strlen(KopString(ob1))+ 10));
      if (str == NULL) errorStackmachine("No more memory.");
      strcpy(str,"error operator : ");
      strcat(str,KopString(ob1));
      errorStackmachine(str);
    }else{
      errorStackmachine("error operator.");
    }
    break;
  case Smpzext:
    ob1 = Kpop();
    Kpush(KmpzExtension(ob1));
    break;

  case Scclass:
    ob3 = Kpop();
    ob2 = Kpop();
    ob1 = Kpop();
    /* [class-tag super-obj] size [class-tag]  cclass */
    Kpush(KcreateClassIncetance(ob1,ob2,ob3));
    break;

  case Stest:
    /* test is used for a test of a new function. */
    ob2 = Kpop();
    ob1 = Kpop();
    Kpush(hilberto(ob1,ob2));
    /*
      {
      ob1 = Kpop();
      Kpush(test(ob1));

      }
    */
    break;

  case Soxshell:
    ob1 = Kpop();
    Kpush(KoxShell(ob1));
    break;

  case Stlimit:
    /* {   } time tlimit */
    ob2 = Kpop();
    ob1 = Kpop();
    switch(ob2.tag) {
	case Sinteger: break;
	default: errorStackmachine("Usage:tlimit"); break;
	}
    switch(ob1.tag) {
    case SexecutableArray: break;
    default:
      errorStackmachine("Usage:tlimit");
      break;
    }
	n = ob2.lc.ival;
	if (n > 0) {
	  mysignal(SIGALRM,ctrlC); alarm((unsigned int) n);
      status = executeExecutableArray(ob1,(char *)NULL,0);
	  cancelAlarm();
	}else{
      before_real = time(&before_real);
      times(&before);
      status = executeExecutableArray(ob1,(char *)NULL,0);
      times(&after);
      after_real = time(&after_real);
	  ob1 = newObjectArray(3);
	  putoa(ob1,0,KpoInteger((int) after.tms_utime - before.tms_utime));
	  putoa(ob1,1,KpoInteger((int) after.tms_stime - before.tms_stime));
	  putoa(ob1,2,KpoInteger((int) (after_real-before_real)));
	  Kpush(ob1);
    }
	break;
    
    
  default:
    errorStackmachine("Unknown Soperator type. \n");
  }
  return(0);  /* normal exit */
}

    
