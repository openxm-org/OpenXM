/* $OpenXM: OpenXM/src/kan96xx/Kan/ext.c,v 1.13 2002/11/10 07:00:05 takayama Exp $ */
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
#include <ctype.h>

extern char **environ;

#define MYCP_SIZE 100
static int Mychildren[MYCP_SIZE];
static int Mycp = 0;
static void mywait() {
  int status;
  int pid;
  int i,j;
  signal(SIGCHLD,SIG_IGN);
  pid = wait(&status);
  fprintf(stderr,"Child process %d is exiting.\n",pid);
  for (i=0; i<Mycp; i++) {
    if (Mychildren[i]  == pid) {
      for (j=i; j<Mycp-1; j++) {
        Mychildren[j] = Mychildren[j+1];
      }
      if (Mycp > 0) Mycp--;
    }
  }
  signal(SIGCHLD,mywait);
}

#define SIZE_OF_ENVSTACK 5
#if defined(__CYGWIN__)
#define JMP_BUF sigjmp_buf
#else
#define JMP_BUF jmp_buf
#endif
static JMP_BUF EnvStack[SIZE_OF_ENVSTACK];
static int Envp = 0;
static void pushEnv(JMP_BUF jb) {
  if (Envp < SIZE_OF_ENVSTACK) {
    *(EnvStack[Envp]) = *jb;
    Envp++;
  }else{
    fprintf(stderr,"Overflow of  EnvStack.\n");
    exit(2);
  }
}
static void popEnv(JMP_BUF jbp) {
  if (Envp <= 0) {
    fprintf(stderr,"Underflow of EnvStack.\n");
    exit(3);
  }else{
    Envp--;
    *jbp = *EnvStack[Envp];
  }
}

static char *ext_generateUniqueFileName(char *s)
{
  char *t;
  int i;
  struct stat statbuf;
  t = (char *)sGC_malloc(sizeof(char)*strlen(s)+4+2);
  for (i=0; i<1000; i++) {
    /* Give up if we failed for 1000 names. */
    sprintf(t,"%s.%d",s,i); 
    /* if (phc_overwrite) return(t); */
    if (stat(t,&statbuf) < 0) {
      return(t);
    }
  }
  errorKan1("%s\n","ext_generateUniqueFileName: could not generate a unique file name. Exhausted all the names.");
  return(NULL);
}

struct object Kextension(struct object obj)
{
  char *key;
  int size;
  struct object keyo;
  struct object rob = NullObject;
  struct object obj1,obj2,obj3,obj4;
  int m,i,pid, uid;
  int argListc, fdListc;
  char *abc;
  char *abc2;
  extern struct context *CurrentContextp;
#if (__CYGWIN__)
  extern sigjmp_buf EnvOfStackMachine;
#else
  extern jmp_buf EnvOfStackMachine;
#endif
  extern void ctrlC();
  extern int SigIgn;
  extern errno;
  extern int DebugCMO;
  extern int OXprintMessage;
  struct stat buf;
  char **argv;
  FILE *fp;
  void (*oldsig)();
  extern SecureMode;

  if (obj.tag != Sarray) errorKan1("%s\n","Kextension(): The argument must be an array.");
  size = getoaSize(obj);
  if (size < 1) errorKan1("%s\n","Kextension(): Empty array.");
  keyo = getoa(obj,0);
  if (keyo.tag != Sdollar) errorKan1("%s\n","Kextension(): No key word.");
  key = KopString(keyo);

  /* branch by they key word. */
  if (strcmp(key,"parse")==0) {
    if (size != 2) errorKan1("%s\n","[(parse)  string] extension.");
    obj1 = getoa(obj,1);
    if (obj1.tag != Sdollar) errorKan1("%s\n","[(parse)  string] extension");

    pushEnv(EnvOfStackMachine);
    m = KSexecuteString(obj1.lc.str);
    /* This is critical area. If you catch ctrl-c here, program crashes. */
    oldsig = signal(SIGINT,SIG_IGN);
    popEnv(EnvOfStackMachine);
    /* OK! We passed the critical area. */
    signal(SIGINT,oldsig);
    rob = KpoInteger(m);
  }else if (strcmp(key,"getpid") == 0) {
    rob = KpoInteger( (int) getpid() );
  }else if (strcmp(key,"flush") == 0) {
    /* fflush(NULL); */
    fflush(stdout);
    rob.tag = Snull;
  }else if (strcmp(key,"chattrs")==0) {
    if (size != 2) errorKan1("%s\n","[(chattrs)  num] extension.");
    obj1 = getoa(obj,1);
    if (obj1.tag != Sinteger) errorKan1("%s\n","[(chattrs)  num] extension.");
    m = KopInteger(obj1);
    if (!( m == 0 || m == PROTECT || m == ABSOLUTE_PROTECT))
      errorKan1("%s\n","The number must be 0, 1 or 2.");
    putUserDictionary2((char *)NULL,0,0,m | SET_ATTR_FOR_ALL_WORDS,
                       CurrentContextp->userDictionary);
  }else if (strcmp(key,"keywords")==0) {
    if (size != 1) errorKan1("%s\n","[(keywords)] extension.");
    rob = showSystemDictionary(1);
    /*  }else if (strcmp(key,"fork0")==0) {
        if (size != 2) errorKan1("%s\n","[(fork0) sss] extension.");
        m = fork();
        if (m>0) { rob = KpoInteger(m); }
        else {
        system(KopString(getoa(obj,1))); exit(0);
        } */
  }else if (strcmp(key,"defaultPolyRing")==0) {
    if (size != 2) errorKan1("%s\n","[(defaultPolyRing) n] extension.");
    rob = KdefaultPolyRing(getoa(obj,1));
  }else if (strcmp(key,"getenv")==0) {
    if (size != 2) errorKan1("%s\n","[(getenv) envstr] extension.");
    obj1 = getoa(obj,1);
    if (obj1.tag != Sdollar) errorKan1("%s\n","[(getenv) envstr] extension");
    abc = getenv(KopString(obj1));
#if defined(__CYGWIN__)
    if (abc == NULL) {
	  abc2 = (char *)sGC_malloc(sizeof(char)*(strlen(KopString(obj1))+2));
      strcpy(abc2,KopString(obj1));
	  for (i=0; i<strlen(abc2); i++) {
		abc2[i] = toupper(abc2[i]);
	  }
      abc = getenv(abc2);
    }
#endif
    if (abc == NULL) {
      rob = NullObject;
    }else{
      abc2 = (char *)sGC_malloc(sizeof(char)*(strlen(abc)+2));
      strcpy(abc2,abc);
      rob = KpoString(abc2);
    }
  }else if (strcmp(key,"stat")==0) {
    if (size != 2) errorKan1("%s\n","[(stat) fname] extension.");
    obj1 = getoa(obj,1);
    if (obj1.tag != Sdollar) errorKan1("%s\n","[(stat) fname] extension ; string fname.");
    m = stat(KopString(obj1),&buf);
    rob = newObjectArray(2);
    if (m == -1) {
      /* fail */
      obj2 = NullObject;
      putoa(rob,0,obj2);
      obj3 = newObjectArray(2);
      putoa(obj3,0,KpoString("error no"));
      putoa(obj3,1,KpoInteger(errno));
      putoa(rob,1,obj3);
    }else{
      /* success */
      putoa(rob,0,KpoInteger(0));
      obj3 = newObjectArray(1);
      putoa(obj3,0,KpoInteger((int) buf.st_size));
      putoa(rob,1,obj3); /* We have not yet read buf fully */
    }
  }else if (strcmp(key,"forkExec")==0) {
    if (size != 4) errorKan1("%s\n","[(forkExec) argList fdList sigblock] extension.");
    obj1 = getoa(obj,1);
	if (obj1.tag == Sdollar) {
	  obj1 = KstringToArgv(obj1);
	}
    if (obj1.tag != Sarray) errorKan1("%s\n","[(forkExec) argList fdList sigblock] extension. array argList.");
	obj2 = getoa(obj,2);
    if (obj2.tag != Sarray) errorKan1("%s\n","[(forkExec) argList fdList sigblock] extension. array fdList.");
    obj3 = getoa(obj,3);
    if (obj3.tag != Sinteger) errorKan1("%s\n","[(forkExec) argList fdList sigblock] extension. integer sigblock.");
    m = KopInteger(obj3);  /* m&1 : block ctrl-C. */
    argListc = getoaSize(obj1);
    fdListc =  getoaSize(obj2);
    if ((pid = fork()) > 0) {
      /* parent */
      if (m&2) {
         /* Do not call singal to turn around a trouble on cygwin. BUG. */
      }else{
         signal(SIGCHLD,mywait); /* to kill Zombie */
      }
      Mychildren[Mycp++] = pid;
      if (Mycp >= MYCP_SIZE-1) {
        errorKan1("%s\n","Child process table is full.\n");
        Mycp = 0;
      }
      rob = KpoInteger(pid);
      /* Done */
    }else{
      /* Child */
      for (i=0; i<fdListc; i++) {
        /* close the specified files */
        close(KopInteger(getoa(obj2,i)));
      }
      /* execl */
      if (m&1) {
        {
          sigset_t sss;
          sigemptyset(&sss);
          sigaddset(&sss,SIGINT);
          sigprocmask(SIG_BLOCK,&sss,NULL);
        }
      }
      argv = (char **) sGC_malloc(sizeof(char *)*(argListc+1));
      if (argv == NULL) {
        fprintf(stderr," no more momory. forkExec --- exiting.\n");
        _exit(10);
      }
      for (i=0; i<argListc; i++) {
        argv[i] = KopString(getoa(obj1,i));
        argv[i+1] = NULL;
      }
      
      if (m&4) {
	fprintf(stderr,"execv %s\n",argv[0]);
	sleep(5);
        fprintf(stderr,">>>\n");
      }
      execve(argv[0],argv,environ);
      /* This place will never be reached unless execv fails. */
      fprintf(stderr,"forkExec fails: ");
      for (i=0; i<argListc; i++) {
        fprintf(stderr,"%s ",argv[i]);
      }
      fprintf(stderr,"\nExiting, but staying as Zombie.\n");
      _exit(10);
    }
  }else if (strcmp(key,"getchild")==0) {
    if (size != 1) errorKan1("%s\n","[(getchild)] extension.");
    rob = newObjectArray(Mycp);
    for (i=0; i<Mycp; i++) {
      putoa(rob,i,KpoInteger(Mychildren[i]));
    }
  }else if (strcmp(key,"getUniqueFileName")==0) {
    if (size != 2) errorKan1("%s\n","[(getUniqueFileName) path] extension.");
    obj1 = getoa(obj,1);
    if (obj1.tag != Sdollar) errorKan1("%s\n","[(getUniqueFileName) path] extension. path must be a string.");
    rob = KpoString(ext_generateUniqueFileName(KopString(obj1)));
  }else if (strcmp(key,"outputObjectToFile")==0) {
    if (size != 3) errorKan1("%s\n","[(outputObjectToFile) path obj] extension.");
    obj1 = getoa(obj,1);
    if (obj1.tag != Sdollar) errorKan1("%s\n","[(outputObjectToFile) path obj] extension. path must be a string.");
    obj2 = getoa(obj,2);
    fp = fopen(KopString(obj1),"w");
    if (fp == NULL) errorKan1("%s\n","[(outputObjectToFile) path object] extension : could not open the path.");
    printObject(obj2,0,fp);
    fclose(fp);
    rob = NullObject;
  }else if (strcmp(key,"hilbert")==0) {
    if (size != 3) errorKan1("%s\n","[(hilbert) obgb obvlist] extension.");
    rob = hilberto(getoa(obj,1),getoa(obj,2));
  }else if (strcmp(key,"nobody") == 0) {
      uid = getuid();
	  if (uid == 0) {
#define NOBODY 65534
		/* If I'm a super user, then change uid to nobody. */
		if (setuid(NOBODY) != 0) {
		  fprintf(stderr,"Failed to change uid to nobody (%d)\n",NOBODY);
		  exit(10);
		}
		fprintf(stderr,"uid is changed to nobody (%d).\n",NOBODY);
		rob.tag = Snull;
	  }
  }else if (strcmp(key,"chattr")==0) {
    if (size != 3) errorKan1("%s\n","[(chattr)  num symbol] extension.");
    obj1 = getoa(obj,1);
    obj2 = getoa(obj,2);
    if (obj1.tag != Sinteger) errorKan1("%s\n","[(chattr)  num symbol] extension.");
    if (obj2.tag != Sstring)  errorKan1("%s\n","[(chattr)  num symbol] extension.");
    m = KopInteger(obj1);
    if (!( m == 0 || m == PROTECT || m == ABSOLUTE_PROTECT))
      errorKan1("%s\n","The number must be 0, 1 or 2.");
    putUserDictionary2(obj2.lc.str,(obj2.rc.op->lc).ival,(obj2.rc.op->rc).ival,
                       m,CurrentContextp->userDictionary);
  }else if (strcmp(key,"regionMatches")==0) {
    if (size != 3) errorKan1("%s\n","[(regionMatches) str strArray] extension.");
    obj1 = getoa(obj,1);
    if (obj1.tag != Sdollar) errorKan1("%s\n","[(regionMatches) str strArray] extension. str must be a string.");
	obj2 = getoa(obj,2);
    if (obj2.tag != Sarray) errorKan1("%s\n","[(regionMatches) str strArray] extension. strArray must be an array.");
    rob = KregionMatches(obj1,obj2);
  }else if (strcmp(key,"ostype")==0) {
    rob = newObjectArray(1);
    /* Hard encode the OS type. */
#if defined(__CYGWIN__)
    putoa(rob,0,KpoString("windows"));
#else
    putoa(rob,0,KpoString("unix"));
#endif
  }
#include "plugin.hh"
  else{
    errorKan1("%s\n","Unknown tag for extension.");
  }


  return(rob);
}

struct object KregionMatches(struct object sobj, struct object keyArray)
{
  struct object rob;
  int n,i,j,m,keyn;
  char *s,*key;
  rob = newObjectArray(3);
  getoa(rob,0) = KpoInteger(-1);
  getoa(rob,1) = NullObject;
  getoa(rob,2) = NullObject;

  if (sobj.tag != Sdollar) return rob;
  if (keyArray.tag != Sarray) return rob;
  n = getoaSize(keyArray);
  for (i=0; i<n; i++) {
	if (getoa(keyArray,i).tag != Sdollar) { return rob; }
  }

  s = KopString(sobj);
  m = strlen(s);

  for (i=0; i<n; i++) {
	key = KopString(getoa(keyArray,i));
	keyn = strlen(key);
	for (j=0; j<m; j++) {
	  if (strncmp(&(s[j]),key,keyn) == 0) {
		getoa(rob,0) = KpoInteger(j);
		getoa(rob,1) = KpoString(key);
		getoa(rob,2) = KpoInteger(i);
		return rob;
	  }
	}
  }
  return rob;
}

