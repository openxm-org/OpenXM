/* $OpenXM: OpenXM/src/util/ox_pathfinder.c,v 1.36 2015/10/13 10:16:53 takayama Exp $ */
/* Moved from misc-2003/07/cygwin/test.c */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#if  (!defined(__MINGW32__) &&  !defined(__MINGW64__))
#include <sys/wait.h>
#else
#include <_mingw.h>
#endif
#include <signal.h>
#include <ctype.h>
#include <time.h>
#include <string.h>
#include "ox_pathfinder.h"

void *sGC_malloc(int);

int OX_P_stdin = -1;
int OX_P_stdout = -1;
int OX_P_stderr = -1;

extern char **environ;

static int getOStypei();
static char *addSlash();
static char *get_sm1_lib_path();
static char *get_k0_lib_path();
static char *get_ox_asir_path();
static char *get_ox_sm1_path();
static char *get_ox_path();
static char *get_oxc_path();
static char *get_oxlog_path();
static int getFileSize(char *fn);
static void errorPathFinder(char *s);
static void msgPathFinder(char *s);
static char *cygname(char *s);

static int Verbose_get_home = 0;
static int Verbose = 1;
static int NoX = 0;
static int ErrorVerbose = 1;
static int EngineLogToStdout = 0;

#define nomemory(a) {fprintf(stderr,"(%p) no more memory.\n",(void *)a);exit(10);}
#define mymalloc(a)  sGC_malloc(a)

static char *cygname(char *s) {
  char *name;
#if defined(__MSYS__)  
  return(s);
#else
  name = mymalloc(strlen(s)+16);
  strcpy(name,"/cygdrive");
  return(strcat(name,s));
#endif
}
void pathFinderErrorVerbose(int k) {
  static int prev;
  if (k >= 0) {
	prev = ErrorVerbose;
	ErrorVerbose = k;
  }else{
	ErrorVerbose = prev;
  }
}
static void errorPathFinder(char *s) {
  /* Todo; we need to return the error message to the client if it is used
     in ox_shell */
  if (ErrorVerbose) fprintf(stderr,"Error: %s",s);
}
static void msgPathFinder(char *s) {
  /* Todo; we need to return the error message to the client if it is used
     in ox_shell */
  fprintf(stderr,"Log(ox_pathfinder): %s",s);
}

int ox_pathfinderNoX(int f) {
  if (f < 0) return NoX;
  NoX = f;
  return f;
}
int ox_pathfinderVerbose(int f) {
  extern int Verbose_get_home;
  if (f < 0) return Verbose_get_home;
  Verbose_get_home = f;
  return f;
} /* cf. ox_pathfinder_quiet() */

/* test main   */
/*
main() {
  char *outfile;
  char *cpp;
  char *argv[10];
  int n;
  char *sfile = "ox_pathfinder.c";
  if (getFileSize(sfile) < 0) {
    fprintf(stderr,"The source file is not found.\n"); return -1;
  }
  cpp = getCppPath();
  if (cpp == NULL) {
    fprintf(stderr,"cpp is not found.\n"); return -1;
  }
  printf("%s\n",cpp);
  outfile = generateTMPfileName("seed");
  if (outfile == NULL) {
    fprintf(stderr,"Failed to generate a temporary file name.\n"); return -1;
  }
  printf("%s\n",outfile);
  if ((char *)strstr(cpp,"/asir/bin/cpp.exe") == NULL) {
    argv[0] = cpp;
    argv[1] = "-P";
    argv[2] = "-lang-c++";
    argv[3] = sfile;
    argv[4] = outfile;
    argv[5] = NULL;
  }else{
    argv[0] = cpp;
    argv[1] = cygwinPathToWinPath(sfile);
    argv[2] = cygwinPathToWinPath(outfile);
    argv[3] = NULL;
  }
  n=oxForkExecBlocked(argv);
}

void *sGC_malloc(int s) { return (void *) malloc(s); }
*/

/* -------- end of test main ----------- */

/* See kxx/ox100start.c for main */

#define MYFORKCP_SIZE 100
static int Myforkchildren[MYFORKCP_SIZE];
static int Myforkcp=0;
#if  (!defined(__MINGW32__)  && !defined(__MINGW64__))
static void myforkwait() {
  int status;
  int pid;
  int i,j;
  /* signal(SIGCHLD,SIG_IGN);  It is not allowed in posix */
  pid = wait(&status);
  if (Verbose) fprintf(stderr,"Child process %d is exiting.\n",pid);
  if (pid < 0) {
    perror("wait");
  }
  for (i=0; i<Myforkcp; i++) {
    if (Myforkchildren[i]  == pid) {
      for (j=i; j<Myforkcp-1; j++) {
        Myforkchildren[j] = Myforkchildren[j+1];
      }
      if (Myforkcp > 0) Myforkcp--;
    }
  }
  signal(SIGCHLD,myforkwait);
}

int oxForkExec(char **argv) {
  int pid;
  char **eee;
  int m;
  m = 0;
  if (argv == NULL) {
    fprintf(stderr,"Cannot fork and exec.\n"); return -1;
  }
  if ((pid = fork()) > 0) {
    oxResetRedirect();
    if (m&2) {
      /* Do not call singal to turn around a trouble on cygwin. BUG. */
    }else{
      signal(SIGCHLD,myforkwait); /* to kill Zombie */
    }
    Myforkchildren[Myforkcp++] = pid;
    if (Myforkcp >= MYFORKCP_SIZE-1) {
      fprintf(stderr,"Child process table is full.\n");
      Myforkcp = 0;
    }
  }else{
    /* close the specified files */
    if (m&1) {
       sigset_t sss;
       sigemptyset(&sss);
       sigaddset(&sss,SIGINT);
       sigprocmask(SIG_BLOCK,&sss,NULL);
    }
    if (NoX && (!EngineLogToStdout)) {
      FILE *null;
      null = fopen("/dev/null","wb");
      if (OX_P_stdout >= 0) dup2(OX_P_stdout,1); else dup2(fileno(null),1);
      if (OX_P_stderr >= 0) dup2(OX_P_stderr,2); else dup2(fileno(null),2);
    }else{
      if (OX_P_stdout >= 0) dup2(OX_P_stdout,1);
      if (OX_P_stderr >= 0) dup2(OX_P_stderr,2);
    }
    if (OX_P_stdin >= 0) dup2(OX_P_stdin,0);
    execve(argv[0],argv,environ);
    /* This place will never be reached unless execv fails. */
    fprintf(stderr,"oxForkExec fails: ");
    exit(3);
  }
}

int oxForkExecBlocked(char **argv) {
  int pid;
  char **eee;
  int m;
  int status;
  if (argv == NULL) {
    fprintf(stderr,"Cannot fork and exec.\n"); return -1;
  }
  if ((pid = fork()) > 0) {
    oxResetRedirect();
    Myforkchildren[Myforkcp++] = pid;
    if (Myforkcp >= MYFORKCP_SIZE-1) {
      fprintf(stderr,"Child process table is full.\n");
      Myforkcp = 0;
    }
    if (waitpid(pid,&status,0) < 0) {  /* blocked */
      perror("waitpid");
    }
    Myforkcp--;
    return status;
  }else{
    /* close the specified files */
    if (m&1) {
       sigset_t sss;
       sigemptyset(&sss);
       sigaddset(&sss,SIGINT);
       sigprocmask(SIG_BLOCK,&sss,NULL);
    }
    if (NoX && (!EngineLogToStdout)) {
      FILE *null;
      null = fopen("/dev/null","wb");
      if (OX_P_stdout >= 0) dup2(OX_P_stdout,1); else dup2(fileno(null),1);
      if (OX_P_stderr >= 0) dup2(OX_P_stderr,2); else dup2(fileno(null),2);
    }else{
      if (OX_P_stdout >= 0) dup2(OX_P_stdout,1);
      if (OX_P_stderr >= 0) dup2(OX_P_stderr,2);
    }
    if (OX_P_stdin >= 0) dup2(OX_P_stdin,0);
    execve(argv[0],argv,environ);
    /* This place will never be reached unless execv fails. */
    fprintf(stderr,"oxForkExecBlock fails: ");
    exit(3);
  }
}
#else
static void myforkwait() {
  fprintf(stderr,"ERROR: myforkwait is not implented in mingw\n");
  return;
}
int oxForkExec(char **argv) {
  /* cf. Kan/shell.c */
  fprintf(stderr,"ERROR: oxForkExec is not implented in mingw\n");
  return(-1);
}
int oxForkExecBlocked(char **argv) {
  fprintf(stderr,"ERROR: oxForkExecBlocked is not implented in mingw\n");
  return(-1);
}
#endif

static int getOStypei() {
  /*
     0  unix
     1  windows-cygwin
     2  windows-cygwin-on-X
     3  windows-native
     4  windows-msys
     5  windows-msys-on-x
  */
  int ostype;
  char *s,*s2,*s3;
#if defined(__CYGWIN__) || defined(__MSYS__)
  ostype = 1;
#else
  ostype = 0;
#endif
  if (ostype == 0) return ostype;
  /* Heuristic method */
  s = getenv("MSYSTEM");
  if (s != NULL) {
    if (strcmp(s,"MSYS")==0) return(4);
    s = (char *)getenv("WINDOWID");
    if (s != NULL) {
      return 5;  // not tested.
    }
  }
  s = (char *)getenv("WINDOWID");
  if (s != NULL) {
    return 2;
  }
  s = (char *)getenv("OSTYPE");
  s2 = (char *)getenv("MACHTYPE");
  s3 = (char *)getenv("PWD");
  if ((s != NULL) || (s2 != NULL) || (s3 != NULL)) {
    return 1;
  }
  return 3;
}

char *getOStypes() {
  int ostype;
  ostype = getOStypei();

  if (ostype == 1) {
    return("Windows-cygwin");
  }else if (ostype == 2) {
    return("Windows-cygwin-on-X");
  }else if (ostype == 3) {
    return("Windows-native");
  }else if (ostype == 4) {
    return("Windows-msys");
  }else if (ostype == 5) {
    return("Windows-msys-on-X");
  }else{
    return("unix");
  }
}

/*
  kan96xx/Kan/ext.c
*/
/*
 -1          : no file
 non-negative: there is a regular file or a directory
*/
static int getFileSize(char *s) { 
   struct stat buf;
   int m;
   if (s == NULL) return -1;
   m = stat(s,&buf);
   if (m == -1) {
     return -1;
      /* fail */
   }else{
      /* success */
     return (int) buf.st_size;
   }
}

static char *addSlash(char *p) {
  char *p2;
   if ((strlen(p) == 0) || (p == NULL)) return(p);
    if (p[strlen(p)-1] == '/') return(p);
    /* Add / */
    p2 = (char *) mymalloc(sizeof(char)*(strlen(p)+3));
    if (p2 == NULL) { fprintf(stderr,"No more memory.\n"); exit(10); }
    strcpy(p2,p); strcat(p2,"/");
    return(p2);
 
}

static void msg_get_home(int t,char *s) {
  extern int Verbose_get_home;
  if (!Verbose_get_home) return;
  if (t == 1) {
    fprintf(stderr,"getOpenXM_HOME(): ");
  }else if (t == 2) {
    fprintf(stderr,"getServerEnv(): ");
  }else if (t == 3) {
    fprintf(stderr,"?? ");
  }else if (t == 4) {
    fprintf(stderr,"cygwinPathToWinPath(): ");
  }else if (t == 5) {
    fprintf(stderr,"catArgv(): ");
  }else{
    fprintf(stderr,"getting path...: ");
  }
  if (s != NULL) {
    fprintf(stderr,"%s\n",s);
  }else{
    fprintf(stderr," --NULL-- \n");
  }
}
/* cf. k097/d.c    getLOAD_K_PATH();
       kan96xx/Kan/scanner.c   getLOAD_SM1_PATH();
 */
char *getOpenXM_HOME() {
  char *p;
  char *h;
  p = getOStypes();
  msg_get_home(1,p);
  
  p = (char *) getenv("OpenXM_HOME");
  if (getFileSize(p) != -1) return addSlash(p);
  msg_get_home(1,"OpenXM_HOME is not found.");

  p = (char *) getenv("OPENXM_HOME");
  if (getFileSize(p) != -1) return addSlash(p);
  msg_get_home(1,"OPENXM_HOME is not found.");

  if (getOStypei() == 3) { /* cygwin-native */
    p = (char *) getenv("OpenXM_HOME_WIN");
    if (getFileSize(p) != -1) return addSlash(p);
    msg_get_home(1,"OpenXM_HOME_WIN is not found.");

    p = (char *) getenv("OPENXMHOMEWIN");
    if (getFileSize(p) != -1) return addSlash(p);
    msg_get_home(1,"OPENXMHOMEWIN is not found.");
  }

  /* Try to find default directories */
  h = (char *)getenv("HOME");
  if (h != NULL) {
    p = (char *)mymalloc(strlen(h)+100);
    if (p == NULL) {
      fprintf(stderr,"No more memory.\n"); exit(100);
    }
    strcat(h,"/OpenXM");
    p = h;
    if (getFileSize(p) != -1) return addSlash(p);
    msg_get_home(1,"OpenXM is not found under the home directory.");
  }

  if (getOStypei() != 3) {
    p = "/usr/local/OpenXM";
  }else{
    p = cygname("/c/usr/local/OpenXM");
  }
  if (getFileSize(p) != -1) return addSlash(p);
  msg_get_home(1,"OpenXM is not found under /usr/local");

  if (getOStypei() != 0) {
    p = cygname("/c/OpenXM");
    if (getFileSize(p) != -1) return addSlash(p);
    msg_get_home(1,"OpenXM is not found under c:\\");

    p = cygname("/c/OpenXM-win");
    if (getFileSize(p) != -1) return addSlash(p);
    msg_get_home(1,"OpenXM-win is not found under c:\\");

    p = cygname("/c/Program Files/OpenXM");
    if (getFileSize(p) != -1) return addSlash(p);
    msg_get_home(1,"OpenXM is not found under c:\\Program Files");

    p = cygname("/c/Program Files/OpenXM-win");
    if (getFileSize(p) != -1) return addSlash(p);
    msg_get_home(1,"OpenXM-win is not found under c:\\Program Files");

  }

  msg_get_home(1,"Giving up!");
  return NULL;
  
}

static char *get_k0_lib_path() {
  char *oxhome;
  char *p;


  p = (char *)getenv("LOAD_K_PATH");
  if (p != NULL) {
    if (getFileSize(p) != -1) return addSlash(p);
    msg_get_home(1,"LOAD_K0_PATH is not found.");
  }

  oxhome = getOpenXM_HOME();
  if (oxhome == NULL) return (char *)NULL;
  p = (char *) mymalloc(strlen(oxhome)+100);
  if (p == NULL) {fprintf(stderr,"No more memory.\n"); exit(100);}

  strcpy(p,oxhome);
  strcat(p,"lib/k097");
  if (getFileSize(p) != -1) return addSlash(p);
  msg_get_home(1,oxhome);
  msg_get_home(1,"     is found, but ");
  msg_get_home(1,p);
  msg_get_home(1,"     is not found.");
    
  msg_get_home(1,"Giving up!");
  return NULL;
}

static char *get_sm1_lib_path() {
  char *oxhome;
  char *p;

  p = (char *)getenv("LOAD_SM1_PATH");
  if (p != NULL) {
    if (getFileSize(p) != -1) return addSlash(p);
    msg_get_home(1,"LOAD_SM1_PATH is not found.");
  }

  oxhome = getOpenXM_HOME();
  if (oxhome == NULL) return NULL;
  p = (char *) mymalloc(strlen(oxhome)+100);
  if (p == NULL) {fprintf(stderr,"No more memory.\n"); exit(100);}

  strcpy(p,oxhome);
  strcat(p,"lib/sm1");
  if (getFileSize(p) != -1) return addSlash(p);
  msg_get_home(1,oxhome);
  msg_get_home(1,"     is found, but ");
  msg_get_home(1,p);
  msg_get_home(1,"     is not found.");
    
  msg_get_home(1,"Giving up!");
  return NULL;
}

static char *get_ox_asir_path() {
  char *oxhome;
  char *p;

  oxhome = getOpenXM_HOME();
  if (oxhome == NULL) return NULL;
  p = (char *) mymalloc(strlen(oxhome)+100);
  if (p == NULL) {fprintf(stderr,"No more memory.\n"); exit(100);}

  strcpy(p,oxhome);
  strcat(p,"bin/ox_asir");
  if (getFileSize(p) != -1) return p;
  msg_get_home(1,oxhome);
  msg_get_home(1,"     is found, but ");
  msg_get_home(1,p);
  msg_get_home(1,"     is not found.");

  strcpy(p,oxhome);
  strcat(p,"lib/asir/ox_asir");
  if (getFileSize(p) != -1) return p;
  msg_get_home(1,oxhome);
  msg_get_home(1,"     is found, but ");
  msg_get_home(1,p);
  msg_get_home(1,"     is not found.");
    
  msg_get_home(1,"Giving up!");
  return NULL;
}

static char *get_ox_path() {
  char *oxhome;
  char *p;

  oxhome = getOpenXM_HOME();
  if (oxhome == NULL) return NULL;
  p = (char *) mymalloc(strlen(oxhome)+100);
  if (p == NULL) {fprintf(stderr,"No more memory.\n"); exit(100);}

  strcpy(p,oxhome);
  strcat(p,"bin/ox");
  if (getFileSize(p) != -1) return p;
  msg_get_home(1,oxhome);
  msg_get_home(1,"     is found, but ");
  msg_get_home(1,p);
  msg_get_home(1,"     is not found.");

  msg_get_home(1,"Giving up!");
  return NULL;
}


static char *get_oxc_path() {
  char *oxhome;
  char *p;

  oxhome = getOpenXM_HOME();
  if (oxhome == NULL) return NULL;
  p = (char *) mymalloc(strlen(oxhome)+100);
  if (p == NULL) {fprintf(stderr,"No more memory.\n"); exit(100);}

  strcpy(p,oxhome);
  strcat(p,"bin/oxc");
  if (getFileSize(p) != -1) return p;
  msg_get_home(1,oxhome);
  msg_get_home(1,"     is found, but ");
  msg_get_home(1,p);
  msg_get_home(1,"     is not found.");

  msg_get_home(1,"Giving up!");
  return NULL;
}

static char *get_oxlog_path() {
  char *oxhome;
  char *p;

  oxhome = getOpenXM_HOME();
  if (oxhome == NULL) return NULL;
  p = (char *) mymalloc(strlen(oxhome)+100);
  if (p == NULL) {fprintf(stderr,"No more memory.\n"); exit(100);}

  strcpy(p,oxhome);
  strcat(p,"bin/oxlog");
  if (getFileSize(p) != -1) return p;
  msg_get_home(1,oxhome);
  msg_get_home(1,"     is found, but ");
  msg_get_home(1,p);
  msg_get_home(1,"     is not found.");

  msg_get_home(1,"Giving up!");
  return NULL;
}

char *cygwinPathToWinPath(char *s) {
  char *pos;
  char *ans;
  int i;
  msg_get_home(4,s);
  if (s == NULL) return NULL;
  if (strlen(s) == 0) return s;

  ans = (char *) mymalloc(strlen(s) + 32);
  if (ans == NULL) nomemory(0);

  pos = (char *)strstr(s,"/cygdrive/");
  if (pos == s) {
    strcpy(ans,&(s[9]));
    ans[0] = s[10]; ans[1] = ':'; ans[2] = '\\';
  }else{
    strcpy(ans,s);
  }
#if defined(__MSYS__)
  pos = (char *)strstr(s,"/c/");
  if (pos == s) {
    strcpy(ans,&(s[1]));
    ans[0] = s[1]; ans[1] = ':'; ans[2] = '\\';
  }else{
    strcpy(ans,s);
  }
#endif

  if (ans[0] == '/') {
#if defined(__MSYS__) && defined(__x86_64__)
    strcpy(ans,"C:\\msys64");
#elif defined(__CYGWIN64__) || (defined(__CYGWIN__) && defined(__x86_64__))
    strcpy(ans,"C:\\cygwin64");
#else
    strcpy(ans,"C:\\cygwin");
#endif
    strcat(ans,s);
  }


  for (i=0; i <strlen(ans); i++) {
    if (ans[i] == '/') ans[i] = '\\';
  }
  return ans;
}

char **getServerEnv(char *oxServer) {
  int ostype;
  char *p;
  char *oxhome;
  char *oxterm;
  int  oxtermType=0;
  char *oxtermOpt;
  char *oxlog;
  char *load_sm1_path;
  char *load_k0_path;
  char *openXM_HOME;
#define ARGV_SIZE 100
  char *argv[ARGV_SIZE];
  int i,k;
  char **aaa;

  if (Verbose_get_home) {
    if (oxServer == NULL) {
      fprintf(stderr,"Server name is NULL.\n");
    }else{
      fprintf(stderr,"Server name is %s\n",oxServer);
    }
  }
  
  if (oxServer == NULL) return NULL;
  i = 0;
  argv[i] = NULL;
  
  ostype = getOStypei();

  oxhome = getOpenXM_HOME();
  if (oxhome == NULL) return NULL;
  p = (char *) mymalloc(strlen(oxhome)+strlen(oxServer)+100);
  if (p == NULL) {fprintf(stderr,"No more memory.\n"); exit(100);}

  strcpy(p,oxhome);
  strcat(p,oxServer);
  if (getFileSize(p) == -1) {
    msg_get_home(2,oxhome);
    msg_get_home(2,"     is found, but ");
    msg_get_home(2,p);
    msg_get_home(2,"     is not found.");
    return (NULL);
  }
  oxServer = (char *) mymalloc(strlen(oxhome)+strlen(oxServer)+100);
  if (oxServer == NULL) {fprintf(stderr,"No more memory.\n"); exit(100);}
  strcpy(oxServer,p);

  if ((ostype == 0) || (ostype == 2)) {
    oxlog = get_oxlog_path();
	if (!NoX) {
	  oxterm = oxTermWhich_unix(&oxtermType);
	  if (oxterm == NULL) {
        msg_get_home(2,"oxterm, rxvt, xterm is not found. NoX is automatically set.");
        NoX = 1;
	  }
	  if (oxtermType == T_XTERM) oxtermOpt = "-icon";
	  else  oxtermOpt = "-iconic";
	}
    argv[i] = oxlog; i++; argv[i] = NULL;
    if (!NoX) {
      argv[i] = oxterm ; i++; argv[i] = NULL;
      if (((char *)getenv("OX_XTERM_GEOMETRY")) != NULL) {
        /* ex. OX_XTERM_GEOMETRY=80x20+0+0 */
    	argv[i] = "-geometry"; i++; argv[i] = NULL;
		argv[i] = (char *) getenv("OX_XTERM_GEOMETRY"); i++; argv[i] = NULL;
      }else{
        argv[i] = oxtermOpt; i++; argv[i] = NULL;
      }
	  /* dirty hack for buggy international xterm cf. OpenXM FAQ */
      argv[i] = "-xrm"; i++; argv[i] = NULL;
      argv[i] = "XTerm*locale:false"; i++; argv[i] = NULL;

	  if (((char *)getenv("OX_XTERM_SCROLL")) != NULL) {
		argv[i] = "-sb"; i++; argv[i] = NULL;
		argv[i] = "-sl"; i++; argv[i] = NULL;
		argv[i] = (char *) getenv("OX_XTERM_SCROLL"); i++; argv[i] = NULL;
	  }
      argv[i] = "-e"; i++; argv[i] = NULL;
    }
    argv[i] = get_ox_path(); i++; argv[i] = NULL;
    argv[i] = "-ox"; i++; argv[i] = NULL;
    argv[i] = oxServer; i++; argv[i] = NULL;
  }else if ((ostype == 1) || (ostype == 3)) {  // cygwin or windows-native
    if (!NoX) {
      if (getFileSize("/cygdrive/c/winnt/system32/cmd.exe") >= 0) {
        oxterm = "/cygdrive/c/winnt/system32/cmd.exe /c start /min ";
        argv[i] = "/cygdrive/c/winnt/system32/cmd.exe"; i++; argv[i] = NULL;
      }else if (getFileSize("/cygdrive/c/windows/system32/cmd.exe") >= 0) {
        oxterm = "/cygdrive/c/windows/system32/cmd.exe  /c start /min ";
        argv[i] = "/cygdrive/c/windows/system32/cmd.exe"; i++; argv[i] = NULL;
      }else{
        msg_get_home(2,"cmd.exe is not found. NoX is automatically set.");
        NoX = 1;
      }
    }
    printf("oxterm=%s\n",oxterm); fflush(NULL);
    oxlog = " ";
    if (!NoX) {
      argv[i] = "/c"; i++; argv[i] = NULL;
      argv[i] = "start"; i++; argv[i] = NULL;
      argv[i] = "/min"; i++; argv[i] = NULL;
    }
    argv[i] = cygwinPathToWinPath(get_ox_path()); i++; argv[i] = NULL;
    argv[i] = "-ox"; i++; argv[i] = NULL;
    argv[i] = oxServer; i++; argv[i] = NULL;
  }else {
    /* msys with mintty*/
    if (!NoX) {
      oxterm = "/usr/bin/mintty";
    }
    if (!NoX) {
      argv[i] = oxterm ; i++; argv[i] = NULL;
      argv[i] = "--exec"; i++; argv[i] = NULL;
    }
    argv[i] = get_ox_path(); i++; argv[i] = NULL;
    argv[i] = "-ox"; i++; argv[i] = NULL;
    argv[i] = oxServer; i++; argv[i] = NULL;
  }


  aaa = (char **) mymalloc(sizeof(char*)*(i+1));
  if (aaa == NULL) nomemory(0);
  msg_get_home(2,"--------- Result --------------");
  for (k=0; k<i; k++) {
    aaa[k] = argv[k];
    msg_get_home(2,aaa[k]);
    aaa[k+1] = NULL;
  }
  return aaa;
}

char **setOXenv_old() {
  char *openXM_HOME;
  char *load_sm1_path;
  char *load_k0_path;
  char *asir_config;
  char *asir_libdir;
  char *asirloadpath;
  char *asir_rsh;
  char *ld_library_path;
  char **aaa;
  int n,i,k;

  /* set environmental variables */
  n = 20;  /* n must be larger than the number of env vars */
  n++;
  load_sm1_path = (char *) getenv("LOAD_SM1_PATH");
  load_k0_path = (char *) getenv("LOAD_SM1_PATH");
  asir_config = (char *) getenv("ASIR_CONFIG");
  asir_libdir = (char *) getenv("ASIR_LIBDIR");
  asirloadpath = (char *) getenv("ASIRLOADPATH");
  asir_rsh = (char *) getenv("ASIR_RSH");
  ld_library_path = (char *) getenv("LD_LIBRARY_PATH");

  openXM_HOME = getOpenXM_HOME();
  if (openXM_HOME != NULL) 
    openXM_HOME[strlen(openXM_HOME)-1] = '\0';
  /* How about ASIR... ? */

  msg_get_home(3,"OpenXM_HOME is"); msg_get_home(2,openXM_HOME);
  msg_get_home(3,"LOAD_SM1_PATH is"); msg_get_home(2,load_sm1_path);
  msg_get_home(3,"LOAD_K0_PATH is"); msg_get_home(2,load_k0_path);

  aaa = (char **) mymalloc(sizeof(char*)*n);
  if (aaa == NULL) nomemory(0);

  i = 0;
  if (openXM_HOME != NULL) {
    aaa[i] = openXM_HOME; i++; aaa[i] = NULL; if (i > n-2) return aaa; 
  }
  if (load_sm1_path != NULL) {
    aaa[i] = load_sm1_path; i++; aaa[i] = NULL; if (i > n-2) return aaa; 
  }
  if (load_k0_path != NULL) {
    aaa[i] = load_k0_path; i++; aaa[i] = NULL; if (i > n-2) return aaa; 
  }
  if (asir_config != NULL) {
    aaa[i] = asir_config; i++; aaa[i] = NULL; if (i > n-2) return aaa; 
  }
  if (asir_libdir != NULL) {
    aaa[i] = asir_libdir; i++; aaa[i] = NULL; if (i > n-2) return aaa; 
  }
  if (asirloadpath != NULL) {
    aaa[i] = asirloadpath; i++; aaa[i] = NULL; if (i > n-2) return aaa; 
  }
  if (asir_rsh != NULL) {
    aaa[i] = asir_rsh; i++; aaa[i] = NULL; if (i > n-2) return aaa; 
  }
  
  msg_get_home(3,"--------- Result --------------");
  for (k=0; k<n; k++) {
    if (aaa[k] == NULL) break;
    msg_get_home(3,aaa[k]);
  }

  return aaa;
}

char **debugServerEnv(char *oxServer)
{
  int t;
  char **aaa;
  t = Verbose_get_home;
  Verbose_get_home = 1;
  aaa = getServerEnv(oxServer);
  Verbose_get_home = t;
  return ( aaa );
}

char **catArgv(char **argv1,char **argv2)
{
  int i,n1,n2;
  char **argv;
  n1=0; 
  while (argv1[n1] != NULL) n1++;
  n2=0; 
  while (argv2[n2] != NULL) n2++;
  argv = (char **) mymalloc(sizeof(char *)*(n1+n2+1));
  if (argv == NULL) nomemory(0);
  for (i=0; i<n1; i++) argv[i] = argv1[i];
  for (i=0; i<n2; i++) argv[n1+i] = argv2[i];
  argv[n1+n2]=NULL;
  for (i=0; i<n1+n2; i++) {
    msg_get_home(5,argv[i]);
  }
  return argv;
}

char *getLOAD_SM1_PATH2() {
  char *p;
  p = get_sm1_lib_path();
  if (p == NULL) {
    return("/usr/local/lib/sm1/");
  }else{
    return p;
  }
}

char *getLOAD_K_PATH2(void) {
  char *p;
  p = get_k0_lib_path();
  if (p == NULL) {
    return("/usr/local/lib/kxx97/yacc/");
  }else{
    return p;
  }
}

char *winPathToCygwinPath(char *s) {
  char *new;
  int n,i;
  if (s == NULL) return s;
  new = (char *) mymalloc(strlen(s)+20);
  if (new == NULL) nomemory(new);
  if (strlen(s) >= 3) {
    /* case of c:\xxx ==> /cygdrive/c/xxx */
    if ((s[1] == ':') && (s[2] == '\\')) {
      sprintf(new,"/cygdrive/%c/%s",s[0],&(s[3]));
    }else{
      strcpy(new,s);
    }
  }else{
    strcpy(new,s);
  }
  n = strlen(s);
  for (i=0; i<n; i++) {
    if (new[i] == '\\') new[i] = '/';
  }
  return new;
}

char *generateTMPfileName(char *seed) {
  char *tmp;
  char *fname;
  char *tt;
  int num;
  static int prevnum=0; 
   /* Bugs for k0.
      (1) unlink does not work so, load["t.k"];; load["t.k"];; fails (only cygwin.
      (2) In case of  error, TMP file is not removed. cf KCerror().
     In case of cygwin, we can only load 90 times.
   */ 
  int i;
  int clean = 0;
  tmp = getenv("TMP");
  if (tmp == NULL) {
    tmp = getenv("TEMP");
  }
  if ((tmp == NULL) && (strcmp(getOStypes(),"Windows-native") != 0)) {
    tmp = "/tmp";
  }
  tmp = winPathToCygwinPath(tmp);
  if (tmp != NULL) {
    fname = (char *)mymalloc(strlen(tmp)+strlen(seed)+40);
    if (fname == NULL) nomemory(fname);
  }else{
    fname = (char *)mymalloc(strlen(seed)+40);
    if (fname == NULL) nomemory(fname);
  }
  for (num=prevnum+1; num <100; num++) {
    if (tmp != NULL) {
      sprintf(fname,"%s/%s-tmp-%d.txt",tmp,seed,num);
    }else{
      sprintf(fname,"%s-tmp-%d.txt",seed,num);
    }
    if (getFileSize(fname) < 0) {
      prevnum = num;
      return fname;
    } else {
      if ((num > 90) && (!clean)) {
        /* Clean the old garbages. */
        for (i=0; i<100; i++) {
          if (tmp != NULL) {
            sprintf(fname,"%s/%s-tmp-%d.txt",tmp,seed,i);
          }else{
            sprintf(fname,"%s-tmp-%d.txt",seed,i);
          }
          {
            struct stat buf;
            int m;
            m = stat(fname,&buf);
            if ((m >= 0) && (buf.st_mtime+120 < time(NULL))) {
              unlink(fname);
            }
          }
        }
        num = 0; clean=1; prevnum=0;
      }
    }
  }
  return NULL;
}

#define MAXTMP2  0xffffff
char *generateTMPfileName2(char *seed,char *ext,int usetmp,int win){
  char *tmp;
  char *fname;
  char *tt;
  int num;
  static int prevnum=0; 
  int i;
  int clean = 0;
  char *extold;
  if (ext == NULL) ext="";
  else {
    extold = ext;
    ext = (char *) mymalloc(strlen(ext)+3);
    if (ext == NULL) {fprintf(stderr,"No more memory.\n"); return NULL;}
    strcpy(ext,".");
    strcat(ext,extold);
  }
  if (usetmp) {
    tmp = getenv("TMP");
    if (tmp == NULL) {
      tmp = getenv("TEMP");
    }
    if ((tmp == NULL) && (strcmp(getOStypes(),"Windows-native") != 0)) {
      tmp = "/tmp";
    }
    tmp = winPathToCygwinPath(tmp);
  }else{
    tmp = NULL;
  }
  if (tmp != NULL) {
    fname = (char *)mymalloc(strlen(tmp)+strlen(seed)+40);
    if (fname == NULL) nomemory(fname);
  }else{
    fname = (char *)mymalloc(strlen(seed)+40);
    if (fname == NULL) nomemory(fname);
  }
  for (num=prevnum+1; num <MAXTMP2; num++) {
    if (tmp != NULL) {
      sprintf(fname,"%s/%s-tmp-%d%s",tmp,seed,num,ext);
    }else{
      sprintf(fname,"%s-tmp-%d%s",seed,num,ext);
    }
    if (getFileSize(fname) < 0) {
      prevnum = num;
      if (win) fname= cygwinPathToWinPath(fname);
      return fname;
    } else {
      if ((num > MAXTMP2-10) && (!clean)) {
        /* Clean the old garbages. */
        for (i=0; i<MAXTMP2; i++) {
          if (tmp != NULL) {
            sprintf(fname,"%s/%s-tmp-%d%s",tmp,seed,i,ext);
          }else{
            sprintf(fname,"%s-tmp-%d%s",seed,i,ext);
          }
          {
            struct stat buf;
            int m;
            m = stat(fname,&buf);
            if ((m >= 0) && (buf.st_mtime+120 < time(NULL))) {
              unlink(fname);
            }
          }
        }
        num = 0; clean=1; prevnum=0;
      }
    }
  }
  return NULL;
}

char *getCppPath(void) {
  static char *cpp = "/usr/local/bin/cpp"; 
  int ostype;
  char *oxhome;
  if ((cpp != NULL) && (getFileSize(cpp) >= 0)) return cpp;
  ostype = getOStypei();
  if (ostype < 3) {
    /* unix or cygwin env */
    cpp = "/lib/cpp";       /* on linux */ 
    if (getFileSize(cpp) >= 0) return cpp;
    cpp = "/usr/bin/cpp";   /* on freebsd, on cygwin */ 
    if (getFileSize(cpp) >= 0) return cpp;
    cpp = "/bin/cpp";     
    if (getFileSize(cpp) >= 0) return cpp;
    cpp = NULL; return cpp;
  }else {
    /* On Windows */
    oxhome = getOpenXM_HOME();
    if (oxhome == NULL) {
      cpp = NULL; return cpp;
    }
    cpp = (char *) mymalloc(strlen(oxhome)+strlen("/asir/bin/cpp.exe")+5);
    if (cpp == NULL) nomemory(cpp);
    sprintf(cpp,"%s/asir/bin/cpp.exe",oxhome);
    if (getFileSize(cpp) >= 0) return cpp;
    else return NULL;
  }
}

char *getCommandPath(char *cmdname)
{
  char *path;
  char *msg;
  char *path2;
  char *ss;
  int i,j;
  /* Use /cygdrive format always. */
  if (cmdname == NULL) return NULL;
  if (strlen(cmdname) < 1) {
    errorPathFinder("getCommandPath: cmdname is an empty string.\n");
    return NULL;
  }
  if (cmdname[0] == '/') {
    if (getFileSize(cmdname) >= 0) { /* Todo: isExecutableFile() */
    }else{
      msg = (char *)mymalloc(strlen(cmdname)+30);
      sprintf(msg,"getCommandPath: %s is not found.",cmdname);
      errorPathFinder(msg);
      return NULL;
    }
    return cmdname;
  }

  path = getOpenXM_HOME();  /* It will return /cygdrive for windows. */
  if (path != NULL) {
    path2 = (char *)mymalloc(strlen(path)+5);
    strcpy(path2,path);
    strcat(path2,"bin");
    ss = oxWhich(cmdname,path2);
    if (ss != NULL) return ss;
  }

  path = (char *)getenv("PATH");  /* Todo: it will not give /cygdrive format*/
  ss = oxWhich(cmdname,path);
  if (ss == NULL) {
    errorPathFinder("oxWhich_unix: could not find it in the path string.\n");
  }
  return ss;
}

char *oxWhich(char *cmdname,char *path) {
  return(oxWhich_unix(cmdname,path));
}

char *oxWhich_unix(char *cmdname,char *path) {
  char *path2;
  int i,j;
  if (path == NULL) {
    return NULL;
  }

  path2 = (char *)mymalloc(strlen(path)+strlen(cmdname)+2);
  for (i=0, j=0; i <= strlen(path); i++) {
    path2[j] = 0;
    if ((path[i] == ':') || (path[i] == 0)) {
      strcat(path2,"/"); strcat(path2,cmdname);
      if (getFileSize(path2) >= 0) { /* Todo: isExecutableFile() */
        return path2;
      }
      j = 0; path2[j] = 0;
    }else{
      path2[j] = path[i]; j++; path2[j] = 0;
    }
  }
  return NULL;
}

char *oxEvalEnvVar(char *s) {
  int n, i,j;
  char *es;
  char *news;
  int flag,flag2;
  flag=-1;
  n = strlen(s);
  es = (char *)mymalloc(n+1); es[0] = 0;
  if (es == NULL) nomemory(1); 
  for (i=0; i<n; i++) {
    if ((s[i] == '$') && (s[i+1] == '{')) {
      for (j=0; ; j++) {
        if ((s[i+2+j] == 0) || (s[i+2+j] == '}')) {
          flag2 = i+2+j+1;
          break;
        }
        es[j] = s[i+2+j]; es[j+1]=0;
      }
      if (es[0] != 0) { flag=i; break; }
    }
  }
  if (flag >= 0) {
    es = (char *)getenv(es);
    if (es == NULL) es="";
    news = (char *) mymalloc(n+5+strlen(es));
    if (news == NULL) nomemory(1);
    j = 0;
    for (i=0; i<flag; i++) {
      news[j] = s[i]; j++;
    }
    for (i=0; i<strlen(es); i++) {
      news[j] = es[i]; j++;
    }
    for (i=flag2; i<strlen(s); i++) {
      news[j] = s[i]; j++;
    }
    news[j] = 0;
    return(oxEvalEnvVar(news));
  }else{
    return(s);
  }
}

void oxResetRedirect(void) {
  if (OX_P_stdin >= 0) close(OX_P_stdin);
  if (OX_P_stdout >= 0) close(OX_P_stdout);
  if (OX_P_stderr >= 0) close(OX_P_stderr);
  OX_P_stdin = OX_P_stdout = OX_P_stderr = -1;
}

int oxDeleteFile(char *fname) {
  if (getFileSize(fname) >= 0) {
    return(unlink(fname));
  }else{
    return(-1);
  }
}

/* This function just kills processes, so if there is a process which
   uses ox protocol, it is not relevant to use this functions.
*/
#if  (!defined(__MINGW32__) &&  !defined(__MINGW64__))
int oxKillAll(void) {
  int i;
  int pid;
  int status;
  for (i=0; i<Myforkcp; i++) {
    pid = Myforkchildren[i];
	if (Verbose) fprintf(stderr,"Sending signal to %d ... ",pid);
    kill(pid,SIGKILL);
	waitpid(pid,&status,0);
	if (Verbose) fprintf(stderr,"Gone.\n");
  }
  Myforkcp = 0;
  return(0);
}
#else
int oxKillAll(void) {
  fprintf(stderr,"ERROR: oxKillAll is not implented in mingw\n");
  return(-1);
}
#endif
void ox_pathfinder_quiet(void) {
  Verbose_get_home = 0;
  Verbose = 0;
}

char *oxTermWhich_unix(int *typep) {
  char *s;
  char *p;
  p = (char *) getenv("PATH");
  s = oxWhich("oxterm",p); *typep = T_OXTERM;
  if (s != NULL) return s;

/*  skip the search of rxvt  (temporary)
  s = oxWhich("rxvt",p); *typep = T_RXVT;
  if (s != NULL) return s;
*/

  s = oxWhich("xterm",p); *typep = T_XTERM;
  if (s != NULL) return s;

  return NULL;
}

int oxpSendStringAsFile(char *user,char *hostname, char *filename, char *str)
{
  FILE *fp;
  int i;
  char *comm;
  char *argv[10];
  mode_t oumask;
  oumask = umask((mode_t) (64-1));
  /* 077=111 111 */
  fp = fopen(filename,"w");
  umask(oumask);
  if (fp == NULL) {
	return -1;
  }
  for (i=0; i <strlen(str); i++) {
	fputc(str[i],fp);
  }
  fclose(fp);
  if (strcmp(hostname,"localhost") == 0) return 0;
  comm = (char *)malloc(strlen(user)+strlen(hostname)+strlen(filename)*2+1024);
  if (comm == NULL) return -2;

  argv[0] = getCommandPath("scp");
  if (argv[0] == NULL) return -3;
  argv[1] = filename;
  comm = (char *)malloc(strlen(user)+strlen(hostname)+strlen(filename)+256);
  sprintf(comm,"%s@%s:%s",user,hostname,filename);
  argv[2] = comm;
  argv[3] = NULL;
  return oxForkExec(argv);
}

char *oxpReadOneTimePasswordFromFile(char *filename) {
}

int ox_pathfinderEngineLogToStdout(int state) {
  EngineLogToStdout = state;
} 
