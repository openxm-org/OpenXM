/* $OpenXM: OpenXM/src/kxx/ox_texmacs.c,v 1.30 2006/03/03 02:30:09 takayama Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <signal.h>
#include "ox_kan.h"
#include "serversm.h"

#if defined(__CYGWIN__)
#define JMP_BUF sigjmp_buf
#define SETJMP(env)  sigsetjmp(env,1)
#define LONGJMP(env,p)  siglongjmp(env,p)
#else
#define JMP_BUF jmp_buf
#define SETJMP(env)  setjmp(env)
#define LONGJMP(env,p)  longjmp(env,p)
#endif

/*
#define DEBUG 
*/
/* #define DEBUG2 */

/* Type of View part (user interface engine) */
#define  GENERIC      0   /* DEBUG, xml */
#define  V_TEXMACS    1
#define  V_CFEP       2
int View       = V_TEXMACS ;

char *Data_begin_v[] = {
  "<S type=verbatim>",
  "\002verbatim:",
  "\002",
};
char *Data_begin_l[] = {
  "<S type=latex>",
  "\002latex:",
  "\002latex:"
};
char *Data_begin_p[] = {
  "<S type=prompt>",
  "\002prompt:",
  "\002prompt:"
};
char *Data_begin_ps[] = {
  "<S type=postscript>",
  "\002ps:",
  "\002ps:"
};
char *Data_end[] = {
  "</S>",
  "\005",
  "\n\005"    /* \n is not a part of the protocol. */
};

/* todo:  start_of_input */

char End_of_input[] = {
  0x5,              /* Use ^E and Return to end the input. */
  '\n',  /* TEXMACS_END_OF_INPUT. 0xd should be used for multiple lines. */
  0x5    /* CFEP_END_OF_INPUT    */
};

/* Table for the engine type. */
#define ASIR          1
#define SM1           2
#define K0            3

extern int Quiet;
extern JMP_BUF EnvOfStackMachine;
extern int Calling_ctrlC_hook;
extern int RestrictedMode, RestrictedMode_saved;
int Format=1;  /* 1 : latex mode */
int OutputLimit_for_TeXmacs = (1024*10);


int TM_Engine  = ASIR ;
int TM_asirStarted = 0;
int TM_sm1Started  = 0;
int TM_k0Started  = 0;
int TM_do_not_print = 0;

int Xm_noX = 0;
int NoCopyright = 0;
int Cpp = 0;                 /* Use cpp before sending to the engine. */ 
int EngineLogToStdout = 0;   /* Do not run the ox engine inside xterm. */

char *LanguageResource = NULL;

void ctrlC();
struct object KpoString(char *s);
char *KSpopString(void);

static char *readString(FILE *fp,char *prolog, char *eplog);
static void printv(char *s);
static void printl(char *s);
static void printp(char *s);
static void printps(char *s);
static void printCopyright(char *s);
static int startEngine(int type,char *msg);
static int isPS(char *s);
static int end_of_input(int c);
static void setDefaultParameterForCfep();

static void myEncoder(int c);
static void myEncoderS(unsigned char *s);
static void myEncoderSn(unsigned char *s,int n);
static void outputStringToTunnel0(int channel, unsigned char *s, int size, int view);
static void outputStringToTunnel(int channel, unsigned char *s,  int view);

static void flushSm1();

/* tail -f /tmp/debug-texmacs.txt 
   Debug output to understand the timing problem of pipe interface.
*/
FILE *Dfp;

main(int argc,char *argv[]) {
  char *s;
  char *r;
  char *sys;
  struct object ob = OINIT;
  int irt=0;
  int vmode=1;
  char *openxm_home;
  char *asir_config;
  int i;


  
#ifdef DEBUG2
  Dfp = fopen("/tmp/debug-texmacs.txt","w");
#endif
  
  /* Set consts */
  Quiet = 1;
  for (i=1; i<argc; i++) {
    if (strcmp(argv[i],"--view") == 0) {
      i++;
      if (strcmp(argv[i],"texmacs") == 0) {
        View = V_TEXMACS;
      }else if (strcmp(argv[i],"cfep")==0) {
        View = V_CFEP; setDefaultParameterForCfep();
      }else{
        View = GENERIC;
        /* printv("Unknown view type.\n"); */
      }
    } else if (strcmp(argv[i],"--sm1") == 0) {
      TM_Engine = SM1;
    }else if (strcmp(argv[i],"--asir") == 0) {
      TM_Engine = ASIR;
    }else if (strcmp(argv[i],"--k0") == 0) {
      TM_Engine = K0;
    }else if (strcmp(argv[i],"--outputLimit") == 0) {
      i++;
      sscanf(argv[i],"%d",&OutputLimit_for_TeXmacs);
    }else if (strcmp(argv[i],"--noLogWindow") == 0) {
      Xm_noX = 1;
    }else if (strcmp(argv[i],"--noCopyright") == 0) {
      NoCopyright = 1;
    }else if (strcmp(argv[i],"--cpp") == 0) {
      Cpp = 1;
    }else if (strcmp(argv[i],"--engineLogToStdout") == 0) {
      EngineLogToStdout = 1;
    }else if (strcmp(argv[i],"--languageResource") == 0) {
      i++;
      LanguageResource = (char *)sGC_malloc(strlen(argv[i])+80);
      sprintf(LanguageResource,
			  " /localizedString.file (%s) def localizedString.load ",argv[i]);
    }else{
      /* printv("Unknown option\n"); */
    }
  }

  openxm_home = (char *) getenv("OpenXM_HOME");
  asir_config = (char *) getenv("ASIR_CONFIG");
  if (openxm_home == NULL)  {
    printv("Error. The environmental variable OpenXM_HOME is not set.\nStart the texmacs with openxm texmacs or ox_texmacs by openxm ox_texmacs\nBye...");
    exit(10);
  }

  /* Initialize kanlib (gc is also initialized) */
  KSstart();

  /* Main loop */
  printf("%s",Data_begin_v[View]);
  if ((asir_config == NULL) && (View == V_TEXMACS)) {
    printf("Warning. The environmental variable ASIR_CONFIG is not set.\nStart the texmacs with openxm texmacs or ox_texmacs by openxm ox_texmacs.\nOtherwise, tex translation will not be performed\n");
    
  }

  printCopyright("");


  /* Load ox engine here */
  /* engine id should be set to ox.engine */
  KSexecuteString(" [(parse) (ox.sm1) pushfile] extension ");
  if (Xm_noX) KSexecuteString(" /Xm_noX 1 def ");
  if (EngineLogToStdout) KSexecuteString(" /Xm_engineLogToStdout 1 def ");
  if (LanguageResource != NULL) KSexecuteString(LanguageResource);
  startEngine(TM_Engine," ");

  if (signal(SIGINT,SIG_IGN) != SIG_IGN) {
    signal(SIGINT,ctrlC);
  }

  irt = 0;
  while(1) {
    /* printp(sys);  no prompt */
    if (SETJMP(EnvOfStackMachine)) {
      if (!Calling_ctrlC_hook) {
        Calling_ctrlC_hook = 1; RestrictedMode = 0;
        KSexecuteString(" ctrlC-hook "); /* Execute User Defined functions. */
        RestrictedMode = RestrictedMode_saved;
      }
      Calling_ctrlC_hook = 0; 
      if (signal(SIGINT,SIG_IGN) != SIG_IGN) {
        signal(SIGINT,ctrlC);
      }
      irt = 1;
      continue;
    } else {  }
    if (!irt) {
      printf("%s",Data_end[View]); fflush(stdout);
    }
    irt = 0;

    /* Reading the input. */
    if (TM_Engine == K0) {
      s=readString(stdin, " ", " "); /* see test data */
    }else if (TM_Engine == SM1) {
      s=readString(stdin, " ", " "); /* see test data */
    }else{
      s=readString(stdin, "if (1) { ", " ; }else{ }"); /* see test data */
    }

    if (s == NULL) { irt = 1; continue; }
    if (!irt) printf("%s",Data_begin_v[View]);
    /* Evaluate the input on the engine */
    KSexecuteString(" ox.engine oxclearstack ");
    KSexecuteString(" ox.engine ");
    ob = KpoString(s);  
    KSpush(ob);
    KSexecuteString(" oxsubmit ");

	if (TM_Engine == SM1) flushSm1();
    
    /* Get the result in string for cfep. */
    if (View != V_TEXMACS) {
      KSexecuteString(" ox.engine oxgeterrors "); 
      ob = KSpop();
      if (ob.tag == Sarray) {
        if (getoaSize(ob) > 0) {
          ob = getoa(ob,0);
          KSpush(ob);
          KSexecuteString(" translateErrorForCfep ");
          r = KSpopString();
          outputStringToTunnel(0,(unsigned char *)r,View);
        }
      }
      if (!TM_do_not_print) {
        KSexecuteString(" ox.engine oxpopstring ");
        r = KSpopString();
        printv(r);
      }else{
        KSexecuteString(" ox.engine 1 oxpops ");
        KSexecuteString(" ox.engine 0 oxpushcmo ox.engine oxpopcmo ");
        ob = KSpop();
        printv("");
      }
      continue;
    }
    /* Get the result in string for texmacs  */
    if (Format == 1 && (! TM_do_not_print)) {
      /* translate to latex form */
      KSexecuteString(" ox.engine oxpushcmotag ox.engine oxpopcmo ");
      ob = KSpop();
      vmode = 0;
      /* printf("id=%d\n",ob.tag); bug: matrix return 17 instead of Sinteger
       or error. */
      if (ob.tag == Sinteger) {
        /* printf("cmotag=%d\n",ob.lc.ival);*/
        if (ob.lc.ival == CMO_ERROR2) {
          vmode = 1;
        }
        if (ob.lc.ival == CMO_STRING) {
          vmode = 1;
        }
      }
      if (vmode) {
        KSexecuteString(" ox.engine oxpopstring ");
        r = KSpopString();
      }else{
        KSexecuteString(" ox.engine 1 oxpushcmo ox.engine (print_tex_form) oxexec  ");
        KSexecuteString(" ox.engine oxpopstring ");
        r = KSpopString();
      }
      if (isPS(r)) {
        printps(r);
      }else{
        if (vmode) printv(r);
        else{
          if (strlen(r) < OutputLimit_for_TeXmacs) {
            printl(r);
          } else printv("Output is too large.\n");
        }
      }
    }else{
      if (!TM_do_not_print) {
        KSexecuteString(" ox.engine oxpopstring ");
        r = KSpopString();
        printv(r);
      }else{
        KSexecuteString(" ox.engine 1 oxpops "); /* Discard the result. */
        /* Push and pop dummy data to wait until the computation finishes. */
        KSexecuteString(" ox.engine 0 oxpushcmo ox.engine oxpopcmo ");
        ob = KSpop();
        printv("");
      }
    }
    /* note that there is continue above. */
  }
}

#define SB_SIZE 1024
#define INC_BUF     if (n >= limit-3) { \
      tmp = s; \
      limit *= 2;  \
      s = (char *) sGC_malloc(limit); \
      if (s == NULL) { \
        fprintf(stderr,"No more memory.\n"); \
        exit(10); \
      } \
      strcpy(s,tmp); \
    } 
/*   */
static char *readString(FILE *fp, char *prolog, char *epilog) {
  int n = 0;
  static int limit = 0;
  static char *s;
  int c;
  char *tmp;
  int i;
  int m;
  int start;
  struct object ob = OINIT;
  if (limit == 0) {
    limit = 1024;
    s = (char *)sGC_malloc(limit);
    if (s == NULL) {
      fprintf(stderr,"No more memory.\n");
      exit(10);
    }
  }
  s[0] = 0; n = 0; m = 0;
  for (i=0; i < strlen(prolog); i++) {
    s[n++] = prolog[i];  s[n] = 0;
    INC_BUF ;
  }
  start = n;
  while ((c = fgetc(fp)) != EOF) {
#ifdef DEBUG2
    fprintf(Dfp,"[%x] ",c); fflush(Dfp); 
#endif
    if (end_of_input(c)) {
      /* If there remains data in the stream,
         read the remaining data. (for debug) */
	  /*
      if (oxSocketSelect0(0,1)) {
        if (c == '\n') c=' ';
        s[n++] = c; s[n] = 0;  m++;
        INC_BUF ;
        continue;
      }
      */
      break;
    }
    if ( c == '\v') c=' ';
    s[n++] = c; s[n] = 0;  m++;
    INC_BUF ;
  }
  /* Check the escape sequence */
  if (strcmp(&(s[start]),"!quit;") == 0) {
    printv("Terminated the process ox_texmacs.\n"); 
    exit(0);
  }
  /* Check the escape sequence to change the global env. */
  if (strcmp(&(s[start]),"!verbatim;") == 0) {
    printv("Output mode is changed to verbatim mode.");
    Format=0;
    return NULL;
  }
  if (strcmp(&(s[start]),"!latex;") == 0) {
    printv("Output mode is changed to latex/verbose.");
    Format = 1;
    return NULL;
  }
  if (strcmp(&(s[start]),"!asir;") == 0) {
    Format=1;
    TM_Engine=ASIR; startEngine(TM_Engine,"Asir");
    return NULL;
  }
  if (strcmp(&(s[start]),"!sm1;") == 0) {
    Format=0;
    TM_Engine=SM1; startEngine(TM_Engine,"sm1");
    return NULL;
  }
  if (strcmp(&(s[start]),"!k0;") == 0) {
    Format=0;
    TM_Engine=K0; startEngine(TM_Engine,"k0");
    return NULL;
  }
  if (strcmp(&(s[start]),"!reset;") == 0) {
	printf("%s",Data_begin_v[View]);
    KSexecuteString(" ox.engine oxreset ox.engine oxpopcmo ");
	ob = KSpop();
	printf("%s",Data_end[View]); fflush(stdout);
    return NULL;
  }

  /* Set TM_do_no_print */
  if (s[n-1] == '$' && TM_Engine == ASIR) {
	TM_do_not_print = 1; s[n-1] = ' ';
  } else if (s[n-1] == ';' && TM_Engine == SM1) {
	TM_do_not_print = 1; s[n-1] = ' ';
  } else TM_do_not_print = 0;

  for (i=0; i < strlen(epilog); i++) {
    s[n++] = epilog[i];  s[n] = 0;
    INC_BUF ;
  }
  return s;
}

static int end_of_input(int c) {
  if (c == End_of_input[View]) return 1;
  else return 0;
}

static void setDefaultParameterForCfep() {
  Format = 0;
}

static void printv(char *s) {
  int i;
  printf("%s",Data_begin_v[View]);
  printf("%s",s);
  printf("%s",Data_end[View]);
#ifdef DEBUG2
  fprintf(Dfp,"<%s>",s); fflush(Dfp);
#endif
  fflush(NULL);
}
static void printl(char *s) {
  printf("%s",Data_begin_l[View]);
  printf(" $ %s $ ",s);
  printf("%s",Data_end[View]);
  fflush(NULL);
}
static int isPS(char *s) {
  if (s[0] != '%') return 0;
  if (s[1] != '%') return 0;
  if (s[2] != '!') return 0;
  if (s[3] != 'P') return 0;
  if (s[4] != 'S') return 0;
  return 1;
}
static void printps(char *s) {
  printf("%s",Data_begin_ps[View]);
  printf("%s",s);
  printf("%s",Data_end[View]);
  fflush(NULL);
}
static void printp(char *s) {
  printf("%s",Data_begin_p[View]);
  printf("%s",Data_end[View]);
  printf("%s] ",s);
  fflush(NULL);
}
static void printCopyright(char *s) {
  printf("%s",Data_begin_v[View]);
  if (! NoCopyright) {
    printf("OpenXM engine (ox engine) interface with TeXmacs protocol.\n2004 (C) openxm.org");
    printf(" under the BSD license.  !asir; !sm1; !k0; !verbatim; !quit;\n");
    printf("Type in      !reset;     when the engine gets confused. ");
    printf("%s",s);
  }
  printf("%s",Data_end[View]);
  fflush(NULL);
}

static void flushSm1() {
  KSexecuteString(" oxsm1.ccc ( [(flush)] extension pop ) oxsubmit ");
}
static int startEngine(int type,char *msg) {
  struct object ob = OINIT;
  printf("%s",Data_begin_v[View]);
  if (type == SM1) {
    if (!TM_sm1Started) KSexecuteString(" sm1connectr ");
    KSexecuteString(" /ox.engine oxsm1.ccc def ");
    /* Initialize the setting of sm1. */
    KSexecuteString("  oxsm1.ccc ( [(cmoLispLike) 0] extension ) oxsubmit ");
    KSexecuteString("  oxsm1.ccc ( ox_server_mode ) oxsubmit ");
    KSexecuteString("  oxsm1.ccc ( ( ) message (------------- Message from sm1 ----------------)message ) oxsubmit ");
	flushSm1();
    TM_sm1Started = 1;
	/* Welcome message.  BUG. Copyright should be returned by a function. */
    if (! NoCopyright) {
      printf("Kan/StackMachine1                         1991 April --- 2004.\n");
      printf("This software may be freely distributed as is with no warranty expressed. \n");
      printf("See OpenXM/Copyright/Copyright.generic\n");
      printf("Info: http://www.math.kobe-u.ac.jp/KAN, kan@math.kobe-u.ac.jp.\n");
      printf("0 usages to show a list of functions. \n(keyword) usages to see a short description\n");
    }
    printf("%s\n",msg); fflush(NULL);
  }else if (type == K0) {
    if (!TM_k0Started) KSexecuteString(" k0connectr ");
    KSexecuteString(" /ox.engine oxk0.ccc def ");
    TM_k0Started = 1;
    printf("%s\n",msg);
  }else{
    if (!TM_asirStarted) KSexecuteString(" asirconnectr ");
    KSexecuteString(" /ox.engine oxasir.ccc def ");
    TM_asirStarted = 1;
    printf("%s\n",msg);
    if ((!NoCopyright) && ((char *)getenv("ASIR_CONFIG") != NULL)) {
      KSexecuteString(" oxasir.ccc (copyright()+asir_contrib_copyright();) oxsubmit oxasir.ccc oxpopstring ");
      ob = KSpop();
      if (ob.tag == Sdollar) {
        printf("%s",ob.lc.str);
      }
    }else if ((!NoCopyright) && ((char *)getenv("ASIR_CONFIG") == NULL)) {
      KSexecuteString(" oxasir.ccc (copyright();) oxsubmit oxasir.ccc oxpopstring ");
      ob = KSpop();
      if (ob.tag == Sdollar) {
        printf("%s",ob.lc.str);
      }
    }
    /* Initialize the setting of asir. */
    KSexecuteString(" oxasir.ccc (if(1) {  Xm_server_mode = 1; Xm_helpdir = \"help-eg\";  } else { ; } ;) oxsubmit oxasir.ccc oxpopcmo ");
    KSexecuteString(" oxasir.ccc (if(1) {  ctrl(\"message\",0);  } else { ; } ;) oxsubmit oxasir.ccc oxpopcmo ");
    /* bug; if ctrl is written with Xm_helpdir = ... without oxpopcmo, then it does
       not work. */
    KSexecuteString(" oxasir.ccc (print(\"----------- Messages from asir ------------------------------\")$ ) oxsubmit oxasir.ccc oxpopcmo ");
  }
  printf("%s",Data_end[View]);
  fflush(NULL);
}

/* test data

1.  print("hello"); print("afo");

    1+2;

2. def foo(N) { for (I=0; I<10; I++) {   --> error

   3+5;

4.  print("hello"); shift+return print("afo");

*/

static void myEncoder(int c) {
  putchar(0xf8 | (0x3 & (c >> 6)));
  putchar(0xf0 | (0x7 & (c >> 3)));
  putchar(0xf0 | (0x7 & c));
}
static void myEncoderS(unsigned char *s) {
  int i,n;
  n = strlen(s);
  for (i = 0; i<n ; i++) myEncoder(s[i]);
}
static void myEncoderSn(unsigned char *s,int n) {
  int i;
  for (i = 0; i<n ; i++) myEncoder(s[i]);
}
static void outputStringToTunnel0(int channel, unsigned char *s, int n,int view) {
  unsigned char ts[128];
  int i;
  if (view == GENERIC) {
    printf("{%d<%d ",channel,n+1);
    for (i=0; i<n; i++) putchar(s[i]);
    printf("%c>}",0);
  }else if (view == V_CFEP) {
    sprintf(ts,"{%d<%d ",channel,n+1);
    myEncoderS(ts);
    myEncoderSn(s,n); myEncoder(0); 
    myEncoderS(">}");
  }
  fflush(stdout);
}
#define LINE_FEED 0xc   /* ^L */
static void outputStringToTunnel(int channel, unsigned char *s, int view) {
  int start, i;
  start = i = 0;
  while (s[i] != 0) {
	if (s[i] == LINE_FEED) {
	  outputStringToTunnel0(channel,&(s[start]),i-start,view);
	  start = i+1;
	}
	i++;
  }
}



  
