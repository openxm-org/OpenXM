/* $OpenXM: OpenXM/src/kan96xx/Kan/shell.c,v 1.16 2013/09/20 06:02:19 takayama Exp $ */
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "datatype.h"
#include "stackm.h"
#include "extern.h"
#include "extern2.h"
#include <signal.h>
#include "plugin.h"
#include "kclass.h"
#include <ctype.h>
#include "ox_pathfinder.h"

static struct object KoxShell_test1(struct object ob);
static struct object oxsExecuteBlocked(struct object ob);
static struct object oxsSetenv(struct object ob);
static char *oxsIsURI(char *s);
static char *oxsURIgetVarName(char *s);
static char *oxsURIgetExtension(char *s);
static char *oxsURIgetFileName(char *s);
static char *oxsRemoveOpt(char *s);
static char *oxsGetOpt(char *s);
static char *oxsVarToFile(char *v,char *extension,char *comamnd,int tmp);
static int oxsFileToVar(char *v,char *fname);
static char **oxsBuildArgv(struct object ob);
static char **oxsBuildArgvRedirect(char **argv);
static struct object testmain(struct object ob);

#define mymalloc(n)  sGC_malloc(n)
#define nomemory(n)  errorKan1("%s\n","No more memory in shell.c");

#define MAXFILES  256
static char *AfterReadFile[MAXFILES];
static char *AfterSetVar[MAXFILES];
static int AfterPt=0;
static char *AfterDeleteFile[MAXFILES];
static int AfterD=0;

static int KeepTmpFiles = 0;

extern int OX_P_stdin;
extern int OX_P_stdout;
extern int OX_P_stderr;

struct object KoxShell(struct object ob) {
  return KoxShell_test1(ob);
}

/* A temporary help system */
void KoxShellHelp(char *key,FILE *fp) {
  char *keys[]={"command","export","keep_tmp_files",
                "killall","redirect","which","@@@@gatekeeper"};
  int i;
#define HSIZE 20
  char *s[HSIZE];
  if (key == NULL) {
    fprintf(fp,"\n");
    for (i=0; strcmp(keys[i],"@@@@gatekeeper") != 0; i++) {
      fprintf(fp,"%s\n",keys[i]);
      KoxShellHelp(keys[i],fp);
      fprintf(fp,"%s\n",keys[i]);
    }
    return;
  }
  for (i=0; i<HSIZE; i++) s[i] = NULL;
  if (strcmp(key,"export")==0) {
    s[0] = "export env_name  =  value";
    s[1] = "export env_name = ";
    s[2] = "Example: [(export) (PATH) (=) (/usr/new/bin:${PATH})] oxshell";
    s[3] = NULL;
  }else if (strcmp(key,"which")==0) {
    s[0] = "which cmd_name";
    s[1] = "which cmd_name path";
    s[2] = "Example: [(which) (ls)] oxshell";
    s[3] = NULL;
  }else if (strcmp(key,"command")==0) {
    s[0] = "Executing a command";
    s[1] = "cmdname arg1 arg2 ... ";
    s[2] = "Example 1: /afo (Hello! ) def [(cat) (stringIn://afo)] oxshell";
    s[3] = "Example 2: [(polymake) (stringInOut://afo.poly) (FACETS)] oxshell";
    s[4] = "A temporary file afo.poly* with the contents of the variable afo.poly is generated under $TMP and \"polymake $TMP FACETS\" will be executed. cf. kan96xx/trans/doPolymake.sm1, Doc/oxshell.oxw, Doc/changelog-ja.tex";
    s[5] = "Example 3: [(ls) (-l) (>) (stringOut://ff)] oxshell";
    s[6] = NULL;
  }else if (strcmp(key,"redirect")==0) {
    s[0] = "The following redirect operators are implemented.";
    s[1] = "< > 2>";
    s[2] = "Example 1: [(ls) (hoge) (2>) (stringOut://afo)] oxshell\n    afo ::";
    s[3] = "Example 2: [(cp) ] addStdoutStderr oxshell\n      [@@@stdout @@@stderr] ::";
    s[4] = NULL;
  }else if (strcmp(key,"killall")==0) {
    s[0] = "Kill all the processes envoked by oxshell";
    s[1] = NULL;
  }else if (strcmp(key,"keep_tmp_files")==0) {
    s[0] = "keep_tmp_files value";
    s[1] = "If value is zero, then temporary files are removed after execution.";
    s[2] = "Example 1: [(keep_tmp_files) (1)] oxshell ; temporary files will be kept in /tmp or $TMP or ...";
    s[3] = NULL;
  }else{
  }
  i = 0;
  while (s[i] != NULL) {
    fprintf(fp,"%s\n",s[i++]);
  }
}


static struct object KoxShell_test1(struct object ob) {
  /* A simple shell. It does not implement a parser. */
  struct object rob = OINIT;
  char *cmd;
  char *arg1,*arg2;
  int i,n;
  rob = NullObject; cmd = NULL; arg1=NULL; arg2=NULL;
  if (ob.tag != Sarray) errorKan1("%s\n","KoxShell requires an array as an argument.");
  n = getoaSize(ob);
  for (i=0; i<n; i++) {
    if (getoa(ob,i).tag != Sdollar) errorKan1("%s\n","KoxShell requires an array of string as an argument.");
  }

  if (n == 0) return(rob);
  cmd = KopString(getoa(ob,0));
  if (strcmp(cmd,"testmain")==0) {
    rob = testmain(ob);
  }else if (strcmp(cmd,"which")==0) {
    if (n == 2) {
      pathFinderErrorVerbose(0); 
      rob = KoxWhich(getoa(ob,1),KpoInteger(0));
      pathFinderErrorVerbose(-1); 
    }else if (n==3) {
      pathFinderErrorVerbose(0); 
      rob = KoxWhich(getoa(ob,1),getoa(ob,2));
      pathFinderErrorVerbose(-1); 
    }else{
      errorKan1("%s\n","shell: << which command-name >> or << which command-name path >>");
    }
    return(rob);
  }else if (strcmp(cmd,"export")==0) {
    rob=oxsSetenv(ob);
  }else if (strcmp(cmd,"keep_tmp_files")==0) {
    if (n != 2) errorKan1("%s\n","shell: << keep_tmp_files value >>");
    if (strcmp("0",KopString(getoa(ob,1))) == 0) {
      KeepTmpFiles = 0;
    }else{
      KeepTmpFiles = 1;
    }
    rob = KpoInteger(KeepTmpFiles);
  }else if (strcmp(cmd,"killall")==0) {
	/* It is called from ctrl-C hook of oxrfc103.sm1 */
	fprintf(stderr,"Killing all child processes (oxshell) ...");
    rob = KpoInteger(oxKillAll());
	fprintf(stderr,"\nDone.\n");
  }else{
    rob = oxsExecuteBlocked(ob);
  }
  return(rob);
}

/* Functions for ox_shell */
struct object KoxWhich(struct object cmdo,struct object patho) {
  struct object rob = OINIT;
  char *sss;
  rob = NullObject;
  if (cmdo.tag != Sdollar) errorKan1("%s\n","KoxWhich(str-obj,str-obj)");
  if (patho.tag == Sdollar) {
    sss=oxWhich(KopString(cmdo),KopString(patho));
    if (sss != NULL) rob=KpoString(sss);
    else{
      sss=getCommandPath(KopString(cmdo));
      if (sss != NULL) rob=KpoString(sss);
    }
  }else{
    sss=getCommandPath(KopString(cmdo));
    if (sss != NULL) rob=KpoString(sss);
  }
  return(rob);
}

static int mysetenv(char *name, char *value, int overwrite);
static int myunsetenv(char *name);
static int mysetenv(char *name, char *value, int overwrite) {
  char *s;
  char *orig;
  s = (char *)getenv(name);
  if ((s == NULL) || overwrite) {
	s = (char *) mymalloc(strlen(name)+strlen(value)+5);
	if (s == 0) { fprintf(stderr,"No more memory.\n"); exit(10); }
    strcpy(s,name);
	strcat(s,"="); strcat(s,value);
	return(putenv(s));
  }
  return (0);
} 

/* bug on Solaris. It does not unsetenv.
   libc4, libc5, glibc. It does unsetenv. */
static int myunsetenv(char *name) {
  return(putenv(name));
}

/* Example. [(export)  (PATH)  (=)  (/usr/new/bin:$PATH)] */
static struct object oxsSetenv(struct object ob) {
  struct object rob = OINIT;
  int i,n;
  char *envp;
  char *new;
  int r;
  rob = NullObject;
  if (ob.tag != Sarray) errorKan1("%s\n","oxsSetenv requires an array of srings.");
  n = getoaSize(ob);
  if ((n != 4) && (n != 3)) errorKan1("%s\n","oxsSetenv requires an array of srings. Length must be 3 or 4.");
  for (i=0; i<n; i++) {
    if (getoa(ob,i).tag != Sdollar) errorKan1("%s\n","oxsSetenv requires an array of srings. Length must be 3 or 4.");
  }

  if (strcmp(KopString(getoa(ob,2)),"=") != 0) {
    errorKan1("%s\n","oxsSetenv, example [(export)  (PATH)  (=)  (/usr/new/bin:$PATH)] oxshell");
  }
  envp = KopString(getoa(ob,1));
  if (n == 4) {
    new = KopString(getoa(ob,3));
    /* printf("%s\n",new); */
    new = oxEvalEnvVar(new);
    /* printf("%s\n",new); */
    r = mysetenv(envp,new,1);
  }else{
    myunsetenv(envp); r = 0;
    /* bug: On Solaris, unsetenv will not work. */
  }
  if (r != 0) errorKan1("%s\n","setenv failed.");
  new = (char *) getenv(envp);
  if (new != NULL) {
    rob = KpoString((char *) getenv(envp));
  }
  return(rob);
}

/* s="stringIn://abc.poly"   ==> stringIn
   s="stringInOut://abc.poly" ==> stringInOut
*/
char *oxsIsURI(char *s) {
  int n,i,j;
  char *u;
  if (s == NULL) return((char *)NULL);
  s = oxsRemoveOpt(s);
  n = strlen(s);
  for (i=0; i<n-3;i++) {
    if ((s[i] == ':') && (s[i+1] == '/') && (s[i+2] == '/')) {
      u = (char *) mymalloc(i+1);
      if (u == NULL) nomemory(1);
      u[0]=0;
      for (j=0; j<i; j++) {
        u[j] = s[j]; u[j+1]=0;
      }
      return(u);
    }
  }
  return(NULL);
}

/* s="stringIn://abc.poly"   ==> abc
   s="stringInOut://abc.poly" ==> abc
   s="stringInOut://abc" ==> abc
*/
char *oxsURIgetVarName(char *s) {
  int n,i,j;
  char *u;
  if (s == NULL) return((char *)NULL);
  s = oxsRemoveOpt(s);
  n = strlen(s);
  for (i=0; i<n-3;i++) {
    if ((s[i] == ':') && (s[i+1] == '/') && (s[i+2] == '/')) {
      u = (char *) mymalloc(n-i+1);
      if (u == NULL) nomemory(1);
      u[0]=0;
      for (j=i+3; j<n; j++) {
        if (s[j] == '.') break;
        u[j-i-3] = s[j]; u[j-i-3+1]=0;
      }
      return(u);
    }
  }
  return(NULL);
}

/* s="stringIn://abc.poly"   ==> poly
   s="stringInOut://abc.poly" ==> poly
   s="stringInOut://abc" ==> NULL
*/
char *oxsURIgetExtension(char *s) {
  int n,i,j,k;
  char *u;
  if (s == NULL) return((char *)NULL);
  s = oxsRemoveOpt(s);
  n = strlen(s);
  for (i=0; i<n-3;i++) {
    if ((s[i] == ':') && (s[i+1] == '/') && (s[i+2] == '/')) {
      for (j=i+3; j<n; j++) {
        if (s[j] == '.') {
          u = (char *) mymalloc(n-j+2);
          if (u == NULL) nomemory(1);
          u[0]=0;
          for (k=j+1; k<n; k++) {
            u[k-j-1] = s[k]; u[k-j] = 0;
          }
          return(u);
        }
      }
    }
  }
  return(NULL);
}

/* stringInOut://abc.poly=:file://pqr.txt */
static char *oxsRemoveOpt(char *s) {
  int n,i,j;
  char *u;
  if (s == NULL) return((char *)NULL);
  n = strlen(s);
  for (i=0; i<n-1;i++) {
    if ((s[i] == '=') && (s[i+1] == ':')) {
      u = (char *) mymalloc(i+1);
      if (u == NULL) nomemory(1);
      u[0]=0;
      for (j=0; j<i; j++) {
        u[j] = s[j]; u[j+1]=0;
      }
      return(u);
    }
  }
  return(s);
}

/* stringInOut://abc.poly=:file://pqr.txt */
/* stringInOut://abc.poly  */
char *oxsGetOpt(char *s) {
  int n,i,j;
  char *u;
  if (s == NULL) return((char *)NULL);
  n = strlen(s);
  for (i=0; i<n-1;i++) {
    if ((s[i] == '=') && (s[i+1] == ':')) {
      u = (char *) mymalloc(n-i+1);
      if (u == NULL) nomemory(1);
      u[0]=0;
      for (j=i+2; j<n; j++) {
        u[j-i-2] = s[j]; u[j-i-2+1]=0;
      }
      return(u);
    }
  }
  return(NULL);
}

char *oxsURIgetFileName(char *s) {
  int n,i,j;
  char *u;
  if (s == NULL) return((char *)NULL);
  s = oxsRemoveOpt(s);
  n = strlen(s);
  for (i=0; i<n-3;i++) {
    if ((s[i] == ':') && (s[i+1] == '/') && (s[i+2] == '/')) {
      u = (char *) mymalloc(n-i+1);
      if (u == NULL) nomemory(1);
      u[0]=0;
      for (j=i+3; j<n; j++) {
        u[j-i-3] = s[j]; u[j-i-3+1]=0;
      }
      return(u);
    }
  }
  return(NULL);
}


static struct object testmain(struct object ob) {
  struct object rob = OINIT;
  char *s;
  struct object ot = OINIT;
  char **av;
  int i;
  rob = NullObject;

  /* Try  sm1 -s " /hoge (hogehoge) def [(testmain)] oxshell afo message " */

  ot = newObjectArray(3);
  getoa(ot,0)=KpoString("cat");
  getoa(ot,1)=KpoString("stringInOut://hoge.poly");
  getoa(ot,2)=KpoString("${HOME}/t.t");
  av=oxsBuildArgv(ot);
  for (i=0; av[i] != NULL; i++) {
    printf("%s\n",av[i]);
  }
  printf("------------------------------\n");

  s=oxsVarToFile("hoge","poly","polymake",0);
  printf("%s\n",s);
  oxsFileToVar("afo",s);
  return(rob);
}

char *oxsVarToFile(char *v,char *ext,char *command,int usetmp) {
  char *fname;
  int winname;
  FILE *fp;
  int n,i,prevc,c;
  char *prog;
  struct object vv = OINIT;

  /*bug; winname must be automatically set by looking at command.
    If command is win32-native-application, then winname=1; else winname=0.
    For example, if command=="gnuplot32" then winmame=1; else winname=0;
  */ 
  winname = 0;

  fname = generateTMPfileName2(v,ext,usetmp,winname);
  if (fname == NULL) errorKan1("%s\n","generateTMPfileName2() is failed.");
  if (v == NULL) errorKan1("%s\n","oxsVarToFile(), v is NULL.");
  vv = KfindUserDictionary(v);
  if (vv.tag != Sdollar) errorKan1("%s\n","oxsVarToFile(), value is not a string object or the object is not found in the dictionary.");
  prog = KopString(vv);
  
  fp = fopen(fname,"w");
  if (fp == NULL) errorKan1("%s\n","oxsVarToFile(), fail to open a file.");
  n = strlen(prog);
  prevc = 0;
  for (i=0; i<n ; i++) {
    c = prog[i];
    if (winname) {
      if ((c == '\n') && (prevc != 0xd)) {
        fputc(0xd,fp); fputc(c,fp);
      }
    }else{
      fputc(c,fp);
    }
    prevc = c;
  }
  if (fclose(fp) != 0) errorKan1("%s\n","oxsVarToFile(), fail to close the file.");

  return(fname);
}

int oxsFileToVar(char *v,char *fname) {
  FILE *fp;
  char *s, *sold;
  int limit;
  int c,i;

  if (v == NULL) {
	/* errorKan1("%s\n","oxsFileToVar(), v is NULL."); */
	fprintf(stderr,"oxsFileToVar(), v is NULL.");
	return(-1);
  }
  limit = 1024;
  fp = fopen(fname,"r");
  if (fp == NULL) {
    fprintf(stderr,"Filename=%s\n",fname);
    /* errorKan1("%s\n","oxsFileToVar(), the file cannot be opened."); */
	fprintf(stderr,"oxsFileToVar(), the file cannot be opened.");
	return(-1);
  }
  s = (char *)mymalloc(limit);
  if (s == NULL) errorKan1("%s\n","No more memory in oxsFileToVar().");

  i = 0;
  while ((c=fgetc(fp)) != EOF) {
    s[i] = c; s[i+1] = 0;
    if (i > limit - 10) {
      sold = s; limit *= 2;
      s = (char *)mymalloc(limit);
      if (s == NULL) errorKan1("%s\n","No more memory in oxsFileToVar().");
      strcpy(s,sold);
    }
    i++;
  }
  fclose(fp);

  KputUserDictionary(v,KpoString(s));
  return(0);
}

static char **oxsBuildArgv(struct object ob) {
  int n,i;
  char **argv;
  char *s;
  char *type;
  char *ext, *v;
  int usetmp=1;
  int win=0;
  struct object ocmd = OINIT;

  /* bug: win variable must be properly set on windows native. */

  if (ob.tag != Sarray) errorKan1("%s\n","oxsBuildArgv() requires an array as an argument.");
  n = getoaSize(ob);
  for (i=0; i<n; i++) {
    if (getoa(ob,i).tag != Sdollar) errorKan1("%s\n","oxsBuildArgv() requires an array of string as an argument.");
  }

  argv = (char **) mymalloc(sizeof(char *)*(n+2));
  argv[0] = (char *)NULL;
  if (n == 0) return(argv);

  s = KopString(getoa(ob,0));
  s = oxEvalEnvVar(s);
  ocmd = KoxWhich(KpoString(s),KpoInteger(0));
  if (ocmd.tag != Sdollar) {
    argv[0] = NULL;
  }else{
    argv[0] = KopString(ocmd);
  }
  argv[1] = (char *)NULL;
  if (argv[0] == NULL) {
    fprintf(stderr,"cmdname=%s\n",s);
    errorKan1("%s\n","oxsBuildArgv() Command is not found.\n");
  }
  for (i=1; i<n; i++) {
    argv[i] = argv[i+1] = NULL;
    s = KopString(getoa(ob,i));
    s = oxEvalEnvVar(s);
    type = oxsIsURI(s);
    if (type == NULL) {
      argv[i] = s;
    }else{
      /* Case when argv[i] is like "stringInOut:abc.poly" */
      v = oxsURIgetVarName(s);
      ext = oxsURIgetExtension(s);
      if (strcmp(type,"stringIn") == 0) {
        argv[i] = oxsVarToFile(v,ext,argv[0],usetmp);
        AfterDeleteFile[AfterD++] = argv[i];
      }else if (strcmp(type,"stringInOut") == 0) {
        argv[i] = oxsVarToFile(v,ext,argv[0],usetmp);
        AfterDeleteFile[AfterD++] = argv[i];
        AfterReadFile[AfterPt] = argv[i];
        AfterSetVar[AfterPt] = v;
        AfterPt++;
        if (AfterPt >= MAXFILES) {
          AfterPt=0;
          errorKan1("%s\n","oxsBuildArgv(), Too may files to open.");
        }
      }else if (strcmp(type,"stringOut") == 0) {
        argv[i] = generateTMPfileName2(v,ext,usetmp,win);
        AfterDeleteFile[AfterD++] = argv[i];
        AfterReadFile[AfterPt] = argv[i];
        AfterSetVar[AfterPt] = v;
        AfterPt++;
        if (AfterPt >= MAXFILES) {
          AfterPt=0;
          errorKan1("%s\n","oxsBuildArgv(), Too may files to open.");
        }
      }else {
        errorKan1("%s\n","This URI type has not yet been implemented.");
      }
      if (AfterD >= MAXFILES) {
          AfterD=0;
          errorKan1("%s\n","oxsBuildArgv(), Too may files to open.");
      }
    }
  }
  return(argv);
}

  
static struct object oxsExecuteBlocked(struct object ob)
{
  int r,i,n;
  char **argv;
  int errorf;

  argv = oxsBuildArgv(ob);  
  argv = oxsBuildArgvRedirect(argv);  
  r=oxForkExecBlocked(argv);  /* bug: what happen when NoX? */
  /*
  if (1) {
    for (i=0; argv[i] != NULL; i++) {
      fprintf(stderr,"argv[%d]=%s\n",i,argv[i]);
    }
    errorKan1("%s\n","ForkExecBlocked failed.");
  }
  */
  errorf=0;
  if (AfterPt > 0) {
    for (i=0; i< AfterPt; i++) {
      if (oxsFileToVar(AfterSetVar[i],AfterReadFile[i]) != 0) {
        errorf=1;
      }
    }
  }
  AfterPt = 0;

  if (AfterD > 0) {
    for (i=0; i< AfterD; i++) {
      if (!KeepTmpFiles) {
        oxDeleteFile(AfterDeleteFile[i]); 
      }
    }
  }
  AfterD = 0;
  if (errorf) errorKan1("%s\n","Some errors in oxsFileToVar().");

  return(KpoInteger(r));
}

static char **oxsBuildArgvRedirect(char **argv) {
  char **newargv;
  int n,i,j;
  FILE *fp;
  char *fname;

  n = 0; while (argv[n] != NULL) n++;
  newargv = (char **) mymalloc(sizeof(char *)*(n+1));
  for (i=0; i<=n; i++) newargv[i]=(char *)NULL;
  j=0;
  /* bug: Critical area, do not make an interruption. */
  for (i=0; i<n; i++) {
    if (strcmp(argv[i],"<")==0) {
      fname=argv[i+1];
      OX_P_stdin = open(fname,O_RDONLY);
      if (OX_P_stdin < 0) {
        OX_P_stdin = -1;
        oxResetRedirect();
        errorKan1("%s\n","oxsBuildArgvRedirect fails to open the input file.");
      }
      i++;
    }else if (strcmp(argv[i],">")==0) {
      fname = argv[i+1];
      fp == NULL;
      if (fname != NULL) {
        fp = fopen(fname,"w");
      }
      if (fp == NULL) {
        oxResetRedirect();
        errorKan1("%s\n","oxsBuildArgvRedirect, cannot open the output file.\n");
      }
      fclose(fp); /* touch */
      OX_P_stdout = open(fname,O_WRONLY);
      i++;
    }else if (strcmp(argv[i],"2>") == 0) {
      fname = argv[i+1];
      fp == NULL;
      if (fname != NULL) {
        fp = fopen(fname,"w");
      }
      if (fp == NULL) {
        oxResetRedirect();
        errorKan1("%s\n","oxsBuildArgvRedirect, cannot open the output (stderr) file.\n");
      }
      fclose(fp); /* touch */
      OX_P_stderr = open(fname,O_WRONLY);
      i++;
    }else{
      newargv[j++] = argv[i]; 
    }
  }
  return( newargv );
}
