/* $OpenXM: OpenXM/src/kan96xx/Kan/shell.c,v 1.3 2003/12/03 09:00:46 takayama Exp $ */
#include <stdio.h>
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
static char *oxsVarToFile(char *v,char *extension,char *comamnd,int tmp);
static int oxsFileToVar(char *v,char *fname);
static char **oxsBuildArgv(struct object ob);
static struct object testmain(struct object ob);

#define mymalloc(n)  sGC_malloc(n)
#define nomemory(n)  errorKan1("%s\n","No more memory in shell.c");

#define MAXFILES  256
static char *AfterReadFile[MAXFILES];
static char *AfterSetVar[MAXFILES];
static int AfterPt=0;
static char *AfterDeleteFile[MAXFILES];
static int AfterD=0;

struct object KoxShell(struct object ob) {
  return KoxShell_test1(ob);
}

/* A temporary help system */
void KoxShellHelp(char *key,FILE *fp) {
  char *keys[]={"command","export","which","@@@@gatekeeper"};
  int i;
#define HSIZE 20
  char *s[HSIZE];
  if (key == NULL) {
    for (i=0; strcmp(keys[i],"@@@@gatekeeper") != 0; i++) {
	  fprintf(fp,"%s\n",keys[i]);
	  KoxShellHelp(keys[i],fp);
	  fprintf(fp,"\n",keys[i]);
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
    s[4] = NULL;
  }else{
  }
  i = 0;
  while (s[i] != NULL) {
	fprintf(fp,"%s\n",s[i++]);
  }
}


static struct object KoxShell_test1(struct object ob) {
  /* A simple shell. It does not implement a parser. */
  struct object rob;
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
	  rob = KoxWhich(getoa(ob,1),KpoInteger(0));
	}else if (n==3) {
	  rob = KoxWhich(getoa(ob,1),getoa(ob,2));
	}else{
	  errorKan1("%s\n","shell: << which command-name >> or << which command-name path >>");
	}
	return(rob);
  }else if (strcmp(cmd,"export")==0) {
	rob=oxsSetenv(ob);
  }else{
	rob = oxsExecuteBlocked(ob);
  }
  return(rob);
}

/* Functions for ox_shell */
struct object KoxWhich(struct object cmdo,struct object patho) {
  struct object rob;
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

/* Example. [(export)  (PATH)  (=)  (/usr/new/bin:$PATH)] */
static struct object oxsSetenv(struct object ob) {
  struct object rob;
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
	r = setenv(envp,new,1);
  }else{
	unsetenv(envp); r = 0;
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

static struct object testmain(struct object ob) {
  struct object rob;
  char *s;
  struct object ot;
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
  struct object vv;

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

  if (v == NULL) errorKan1("%s\n","oxsFileToVar(), v is NULL.");
  limit = 1024;
  fp = fopen(fname,"r");
  if (fp == NULL) {
	fprintf(stderr,"Filename=%s\n",fname);
	errorKan1("%s\n","oxsFileToVar(), the file cannot be opened.");
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
  struct object ocmd;

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

  /* bug: Set stdout, stderr to result variables. */
  argv = oxsBuildArgv(ob);  
  r=oxForkExecBlocked(argv);  /* bug: what happen when NoX? */
  /*
  if (1) {
	for (i=0; argv[i] != NULL; i++) {
	  fprintf(stderr,"argv[%d]=%s\n",i,argv[i]);
	}
	errorKan1("%s\n","ForkExecBlocked failed.");
  }
  */
  if (AfterPt > 0) {
	for (i=0; i< AfterPt; i++) {
	  oxsFileToVar(AfterSetVar[i],AfterReadFile[i]);
	}
  }
  AfterPt = 0;

  if (AfterD > 0) {
	for (i=0; i< AfterD; i++) {
	  /* oxDeleteFile(AfterDeleteFile[i]);  not implemented. */
	}
  }
  AfterD = 0;

  return(KpoInteger(r));
}
