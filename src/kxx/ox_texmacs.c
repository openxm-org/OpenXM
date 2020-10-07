/* $OpenXM: OpenXM/src/kxx/ox_texmacs.c,v 1.47 2019/09/25 06:30:37 takayama Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <signal.h>
#include "ox_kan.h"
#include "serversm.h"
#include "ox_pathfinder.h"

#if defined(__CYGWIN__) || defined(__MSYS__)
#define JMP_BUF sigjmp_buf
#define SETJMP(env)  MYSIGSETJMP(env,1)
#define LONGJMP(env,p)  MYSIGLONGJMP(env,p)
#else
#define JMP_BUF jmp_buf
#define SETJMP(env)  MYSETJMP(env)
#define LONGJMP(env,p)  MYLONGJMP(env,p)
#endif

void KSstart(); // kan96xx/Kan/datatype.h
void KSstart_quiet(); // kan96xx/Kan/datatype.h
int KSexecuteString(char *s); // kan96xx/Kan/datatype.h


/*
#define DEBUG 
*/
/*
#define DEBUG2
*/

/* Type of View part (user interface engine) */
#define  GENERIC      0   /* DEBUG, xml */
#define  V_TEXMACS    1
#define  V_CFEP       2
#define  V_QFEP       3
#define  V_SAGE       4
#define  V_JUPYTER       5
int View       = V_TEXMACS ;

char *Data_begin_v[] = {
  "<S type=verbatim>",
  "\002verbatim:",
  "\002",
  "",
  "",
  ""
};
char *Data_begin_l[] = {
  "<S type=latex>",
  "\002latex:",
  "\002latex:",
  "",
  "",
  ""
};
char *Data_begin_p[] = {
  "<S type=prompt>",
  "\002prompt:",
  "\002prompt:",
  "",
  "",
  ""
};
char *Data_begin_ps[] = {
  "<S type=postscript>",
  "\002ps:",
  "\002ps:",
  "",
  "",
  ""
};
char *Data_end[] = {
  "</S>",
  "\005",
  "\n\005",    /* \n is not a part of the protocol. */
  "",
  "",          /* SAGE */
  "\n"
};

/* todo:  start_of_input */

char End_of_input[] = {
  0x5,              /* Use ^E and Return to end the input. */
  '\n',  /* TEXMACS_END_OF_INPUT. 0xd should be used for multiple lines. */
  0x5,    /* CFEP_END_OF_INPUT    */
  0x5,
  '\n',   /* SAGE cf. __init__ */
  '\n'
};

/* Note! if you give more than 10 Views, increase the number 10  below. 
   See the option --prompt which should be given after --view is set.
*/
char *Prompt[10]={
  "generic>", /* generic */
  "", /* texmacs */
  "",    /* cfep */
  "",   /* qfep */
  "\nasir>",    /* SAGE */
  "\nPEXPECT_PROMPT>"
};


/* Table for the engine type. */
#define ASIR          1
#define SM1           2
#define K0            3

extern int Quiet;
extern JMP_BUF EnvOfStackMachine;
extern int Calling_ctrlC_hook;
extern int RestrictedMode, RestrictedMode_saved;
int Format=1;  /* 1 : latex mode, 2: cfep_png */
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

char *AsirInitFile = NULL;

char *LanguageResource = NULL;
int TM_quiet = 0;

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
static void setDefaultParameterForSage();
static void setDefaultParameterForJupyter();

static void myEncoder(int c);
static void myEncoderS(unsigned char *s);
static void myEncoderSn(unsigned char *s,int n);
static void outputStringToTunnel0(int channel, unsigned char *s, int size, int view);
static void outputStringToTunnel(int channel, unsigned char *s,  int view);

static void pngSendFile(char *path);
static int pngCheck(void);
static void pngNotAvailable(void);
static char *pngGetResult();

static void flushSm1();
static int is_substr_of(char a[],char s[]);
static void hexout(FILE *fp,char s[]);
static int mystrncmp(char a[],char s[]);


/* tail -f /tmp/debug-texmacs.txt 
   Debug output to understand the timing problem of pipe interface.
*/
FILE *Dfp;

void main(int argc,char *argv[]) {
  char *s;
  char *r;
  char *sys;
  struct object ob = OINIT;
  int irt=0;
  int vmode=1;
  char *openxm_home;
  char *asir_config;
  char *path;
  int i;


  
#ifdef DEBUG2
  Dfp = fopen("/tmp/debug-texmacs.txt","w");
#endif

  TM_quiet=0;
  for (i=1; i<argc; i++) {
    if (strcmp(argv[i],"--quiet")==0) {
      TM_quiet=1;
    }
  }
  /* Initialize kanlib (gc is also initialized) */
  if (TM_quiet) {
    KSstart_quiet();
  }else{
    KSstart();
  }
  /* Set consts */
  Quiet = 1;
  for (i=1; i<argc; i++) {
    if (strcmp(argv[i],"--view") == 0) {
      i++;
      if (strcmp(argv[i],"texmacs") == 0) {
        View = V_TEXMACS;
      }else if (strcmp(argv[i],"cfep")==0) {
        View = V_CFEP; setDefaultParameterForCfep();
      }else if (strcmp(argv[i],"qfep")==0) {
	View = V_QFEP; setDefaultParameterForCfep();
      }else if (strcmp(argv[i],"sage")==0) {
	View = V_SAGE; setDefaultParameterForSage();
      }else if (strcmp(argv[i],"jupyter")==0) {
	View = V_JUPYTER; setDefaultParameterForJupyter();
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
    }else if (strcmp(argv[i],"--asirInitFile") == 0) {
      i++;
      AsirInitFile = (char *)sGC_malloc(strlen(argv[i])+80);
      sprintf(AsirInitFile,"%s",argv[i]);
    }else if (strcmp(argv[i],"--quiet") == 0) {
      /* it is already set. Ignore. */
    }else if (strcmp(argv[i],"--prompt")==0) {
      i++;
      Prompt[View] = (char *)sGC_malloc(strlen(argv[i])+2);
      strcpy(Prompt[View],argv[i]);
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

  if (mysignal(SIGINT,SIG_IGN) != SIG_IGN) {
    mysignal(SIGINT,ctrlC);
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
      if (mysignal(SIGINT,SIG_IGN) != SIG_IGN) {
        mysignal(SIGINT,ctrlC);
      }
      irt = 1;
      continue;
    } else {  }
    if (!irt) {
      if (View != V_SAGE) printf("%s",Data_end[View]); fflush(stdout);
    }
    irt = 0;

    /* Reading the input. */
    if ((View == V_SAGE) || (View == V_JUPYTER)) {
      if (s != NULL) {printf("%s",Prompt[View]); fflush(stdout);}
      else {printf("%s",&(Prompt[View][1])); fflush(stdout);}
    }else{
      printf("%s",Prompt[View]); fflush(stdout);
    }
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
        if (Format == 2) { /* png mode */
          r = pngGetResult();
        }else{
          KSexecuteString(" ox.engine oxpopstring ");
          r = KSpopString();
        }
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
    if ((Format == 1) && (! TM_do_not_print)) {
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

static char *pngGetResult(void) {
  struct object ob;
  int vmode;
  char *r;
  char *path;

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
    KSexecuteString(" ox.engine 1 oxpushcmo ox.engine (print_png_form2) oxexec  ");
    KSexecuteString(" ox.engine oxpopcmo /tmp_ox_texmacs set tmp_ox_texmacs 0 get ");
    r = KSpopString();  /* input form */
    KSexecuteString(" tmp_ox_texmacs 1 get ");
    path = KSpopString(); /* path name of the png file. */
    pngSendFile(path);
  }
  return r;
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
static char *Sss=NULL;
static int Sss_size=10240;

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
    fprintf(Dfp,"[%x]%c ",c,c); fflush(Dfp); 
#endif
    if (end_of_input(c)) {
      /* If there remains data in the stream,
         read the remaining data. (for debug) */
      /*
      if ((View == V_JUPYTER) || (View == V_SAGE)) {
        if (oxSocketSelect0(0,1)) {
#ifdef DEBUG2
          fprintf(Dfp,"<%x>%c ",c,c); fflush(Dfp); 
#endif
          if (c == '\n') c=' ';
          s[n++] = c; s[n] = 0;  m++;
          INC_BUF ;
          continue;
        }
      }
      */
      if (View == V_JUPYTER) {
        s[n++] = c; s[n] = 0;  m++;
        INC_BUF ;
      }
      break;
    }
    if ( c == '\v') c=' ';
    s[n++] = c; s[n] = 0;  m++;
    INC_BUF ;
  }
  if ((c == EOF) && (start == n)) exit(0); 
  /* Check the escape sequence */
  if (mystrncmp(&(s[start]),"!quit;") == 0) {
    printv("Terminated the process ox_texmacs.\n"); 
    exit(0);
  }
  if (((View == V_JUPYTER) || (View == V_SAGE)) && 
      ((mystrncmp(&(s[start]),"quit")==0) || (mystrncmp(&(s[start]),"quit()")==0))) {
    printv("Terminated the process ox_texmacs.\n"); 
    exit(0);
  }
  if ((View == V_JUPYTER) && 
      (mystrncmp(&(s[start]),"version")==0)) {
    strcpy(&(s[start]),"version");
    s[n=strlen(s)]='('; s[++n]=0; INC_BUF;
    s[n++]=')'; s[n]=0; INC_BUF;
  }
  /* Check the escape sequence to change the global env. */
  if (strcmp(&(s[start]),"!verbatim;") == 0) {
    if (View != V_CFEP) printv("Output mode is changed to verbatim mode.");
    Format=0;
    return NULL;
  }
  if (strcmp(&(s[start]),"!latex;") == 0) {
    printv("Output mode is changed to latex/verbose.");
    Format = 1;
    return NULL;
  }
  if (strcmp(&(s[start]),"!cfep_png;") == 0) {
	if (pngCheck()) {
	  if (View != V_CFEP) printv("Output mode is changed to cfep_png/verbose.");
      Format = 2;
    } else pngNotAvailable();
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

  /* remove end$ or end; */
  if (TM_Engine == ASIR) {
    for (i=n-1; i>=4; i--) {
	  if ((s[i-4]=='\n') && (s[i-3]=='e') && (s[i-2]=='n') && (s[i-1]=='d') && (s[i]=='$')) {
        s[i-3] = s[i-2] = s[i-1] = s[i] = ' '; break;
      }
	  if ((s[i-4]=='\n') && (s[i-3]=='e') && (s[i-2]=='n') && (s[i-1]=='d') && (s[i]==';')) {
        s[i-3] = s[i-2] = s[i-1] = s[i] = ' '; break;
      }
    }

    /* 2019.03.28 for jupyter */
    if ((View == V_JUPYTER) && (mystrncmp(&(s[start]),"base_prompt") != 0)
        &&(mystrncmp(&(s[start]),"version()") != 0)) {
      if (Sss == NULL) {
          Sss = (char *)sGC_malloc(Sss_size);
          Sss[0] = 0;
      }
      if (Sss_size < strlen(Sss)+strlen(s)+strlen(epilog)-10) {
        Sss_size = 2*Sss_size;
        if (Sss_size < 0) { fprintf(stderr,"Fatal error: too big string\n"); exit(-1); }	
        char *sss_tmp = (char *)sGC_malloc(Sss_size);
	strcpy(sss_tmp,Sss);
        Sss=sss_tmp;
      }
      strcpy(&(Sss[strlen(Sss)]),&(s[start]));
      for (i=strlen(Sss); i>=0; i--) {
	if (Sss[i] < 0x20) continue;
        if ((Sss[i] == ';') && (Sss[i-1] == ';')) {
          /* construct s and return s */
          Sss[i] = ' ';
          s = (char *) sGC_malloc(strlen(Sss)+strlen(prolog)+strlen(epilog)+10);
          sprintf(s,"%s%s%s",prolog,Sss,epilog);
          Sss[0]=0;
#ifdef DEBUG2
          fprintf(Dfp,"#By Sss: %s#\n",s); fflush(Dfp);
#endif
          return s;
        }else break;
      }
      return NULL;
    }
  }else{
#ifdef DEBUG2
    fprintf(Dfp,"#base_prompt or version()#\n",s); fflush(Dfp);
#endif
  }
  /* end 2019.03.28 for jupyter. */

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
#ifdef DEBUG2
  fprintf(Dfp,"#By normal readString:%s\n",s); hexout(Dfp,s); fprintf(Dfp,"\n"); fflush(Dfp);
#endif
  return s;
}

static int end_of_input(int c) {
  if (c == End_of_input[View]) return 1;
  else return 0;
}

static void setDefaultParameterForCfep() {
  Format = 0;
}
static void setDefaultParameterForSage() {
  Format = 0;
}
static void setDefaultParameterForJupyter() {
  Format = 0;
}

static void printv(char *s) {
  int i;
  printf("%s",Data_begin_v[View]);
  printf("%s",s);
  printf("%s",Data_end[View]);
#ifdef DEBUG2
  fprintf(Dfp,"printv:<%s>",s); fflush(Dfp);
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
  if (! NoCopyright) {
    printf("%s",Data_begin_v[View]);
    printf("OpenXM engine (ox engine) interface with TeXmacs protocol.\n2004 (C) openxm.org");
    printf(" under the BSD license.  !asir; !sm1; !k0; !verbatim; !quit;\n");
    printf("Type in      !reset;     when the engine gets confused. ");
    printf("%s",s);
    printf("%s",Data_end[View]);
  }
  fflush(NULL);
}

static void flushSm1() {
  KSexecuteString(" oxsm1.ccc ( [(flush)] extension pop ) oxsubmit ");
}
static int startEngine(int type,char *msg) {
  struct object ob = OINIT;
  if (!TM_quiet) printf("%s",Data_begin_v[View]);
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
    if (AsirInitFile) {  /* cf. asir-contrib/packages/src/cfep-init.rr */
	  unsigned char *ss;
	  ss = (unsigned char *)sGC_malloc(strlen(AsirInitFile)+256);
	  sprintf((char *)ss," oxasir.ccc (load(\"%s\");) oxsubmit oxasir.ccc oxgeterrors length 0 gt { (Error in loading asirInitFile) message} { } ifelse ",AsirInitFile);
	  /* printf("Loading --asirInitFile %s\n",AsirInitFile); */
	  KSexecuteString(ss);
    }
    KSexecuteString(" oxasir.ccc (if(1) {  Xm_server_mode = 1; Xm_helpdir = \"help-en\";  } else { ; } ;) oxsubmit oxasir.ccc oxpopcmo ");
    KSexecuteString(" oxasir.ccc (if(1) {  ctrl(\"message\",0);  } else { ; } ;) oxsubmit oxasir.ccc oxpopcmo ");
    /* bug; if ctrl is written with Xm_helpdir = ... without oxpopcmo, then it does
       not work. */
    KSexecuteString(" oxasir.ccc (print(\"----------- Messages from asir ------------------------------\")$ ) oxsubmit oxasir.ccc oxpopcmo ");
  }
  if (!TM_quiet) printf("%s",Data_end[View]);
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
  n = strlen((char *)s);
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
    sprintf((char *)ts,"{%d<%d ",channel,n+1);
    myEncoderS(ts);
    myEncoderSn(s,n); myEncoder(0); 
    myEncoderS((unsigned char *)">}");
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


static void pngSendFile(char *path) {
  char s[1024];
  if (strlen(path) > 1000) return;
  sprintf(s,"showFile,%s",path);
  outputStringToTunnel0(10,(unsigned char *)s,strlen(s),View);
  printf("\n");
  fflush(NULL);
}
static int pngCheck(void) {
  static int checked = 0;
  static int ans = 0;
  if (checked) return ans;
  checked = 1;
  if (!getCommandPath("latex")) return 0;
  if (!getCommandPath("dvipng")) return 0;
  ans = 1;  return 1;
}
static void pngNotAvailable(void) {
  char *s = "notAvailable";
  outputStringToTunnel0(10,(unsigned char *)s,strlen(s),View);
  fflush(NULL);
}
  
/* Check if a is a substring of s */
static int is_substr_of(char a[],char s[]) {
  int n,m,i,j;
  n = strlen(s);
  m = strlen(a);
  for (i=0; i<n-m; i++) {
        for (j=0; j<m; j++) {
          if (s[i+j] != a[j]) break;
          if (j == m-1) return 1;
        }
  }
  return 0;
}

static void hexout(FILE *fp,char s[]) {
  int i;
  for (i=0; i<strlen(s); i++) fprintf(fp,"%2x ",(unsigned char)s[i]);
}
static int mystrncmp(char a[],char s[]) {
  int r;
  r=strncmp(a,s,strlen(s));
#ifdef DEBUG2
  fprintf(Dfp,"mystrncmp(%s,%s)=%d\n",a,s,r); fflush(Dfp); 
#endif
  return r;
}
      
