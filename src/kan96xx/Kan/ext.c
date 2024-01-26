/* $OpenXM: OpenXM/src/kan96xx/Kan/ext.c,v 1.49 2018/09/07 00:09:32 takayama Exp $ */
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <time.h>
#include "datatype.h"
#include "stackm.h"
#include "extern.h"
#include "extern2.h"
#include <signal.h>
#include "plugin.h"
#include "kclass.h"
#include <ctype.h>
#include <errno.h>
#include <regex.h>
#include "ox_pathfinder.h"
#include "mysig.h"

void cmoDumpCmo(struct object ob); /* defined in ../plugin/cmo0.h */

extern int Quiet;
extern char **environ;
extern char *MsgSourceTrace;

#define MYCP_SIZE 100
static int Mychildren[MYCP_SIZE];
static int Mycp = 0;
static int Verbose_mywait = 0;
static void mywait() {
  int status;
  int pid;
  int i,j;
  /* mysignal(SIGCHLD,SIG_IGN); */
  pid = wait(&status);
  if ((!Quiet) && (Verbose_mywait)) fprintf(stderr,"Child process %d is exiting.\n",pid);
  for (i=0; i<Mycp; i++) {
    if (Mychildren[i]  == pid) {
      for (j=i; j<Mycp-1; j++) {
        Mychildren[j] = Mychildren[j+1];
      }
      if (Mycp > 0) Mycp--;
    }
  }
  mysignal(SIGCHLD,mywait);
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

static struct object oregexec(struct object oregex,struct object ostrArray,struct object oflag);

struct object Kextension(struct object obj)
{
  char *key;
  int size;
  struct object keyo = OINIT;
  struct object rob = NullObject;
  struct object obj1 = OINIT;
  struct object obj2 = OINIT;
  struct object obj3 = OINIT;
  struct object obj4 = OINIT;
  int m,i,pid, uid;
  int argListc, fdListc;
  char *abc;
  char *abc2;
  extern struct context *CurrentContextp;
  struct timeval tm;
#if (__CYGWIN__)
  extern sigjmp_buf EnvOfStackMachine;
#else
  extern jmp_buf EnvOfStackMachine;
#endif
  extern void ctrlC();
  extern int SigIgn;
  extern int DebugCMO;
  extern int OXprintMessage;
  struct stat buf;
  char **argv;
  FILE *fp;
  void (*oldsig)();
  extern int SecureMode;
  extern char *UD_str;
  extern int UD_attr;

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
    oldsig = mysignal(SIGINT,SIG_IGN);
    popEnv(EnvOfStackMachine);
    /* OK! We passed the critical area. */
    mysignal(SIGINT,oldsig);
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
	/* if (!( m == 0 || m == PROTECT || m == ABSOLUTE_PROTECT || m == ATTR_INFIX))
	   errorKan1("%s\n","The number must be 0, 1 or 2.");*/
    putUserDictionary2((char *)NULL,0,0,m | SET_ATTR_FOR_ALL_WORDS,
                       CurrentContextp->userDictionary);
  }else if (strcmp(key,"or_attrs")==0) {
    if (size != 2) errorKan1("%s\n","[(or_attrs)  num] extension.");
    obj1 = getoa(obj,1);
    if (obj1.tag != Sinteger) errorKan1("%s\n","[(or_attrs)  num] extension.");
    m = KopInteger(obj1);
    putUserDictionary2((char *)NULL,0,0,m | OR_ATTR_FOR_ALL_WORDS,
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
  }else if (strcmp(key,"date")==0) {
    if (size != 1) errorKan1("%s\n","[(date)] extension.");
    gettimeofday(&tm,NULL);
    rob = KpoString(ctime((time_t *)&(tm.tv_sec)));
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
  }else if (strcmp(key,"gethostname")==0) {
    abc = (char *)sGC_malloc(sizeof(char)*1024);
    if (gethostname(abc,1023) < 0) {
	  errorKan1("%s\n","hostname could not be obtained.");
	}
	rob = KpoString(abc);
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
         mysignal(SIGCHLD,mywait); /* to kill Zombie */
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
  }else if (strcmp(key,"getAttributeList")==0) {
    if (size != 2) errorKan1("%s\n","[(getAttributeList) ob] extension rob");
    rob = KgetAttributeList(getoa(obj,1));
  }else if (strcmp(key,"setAttributeList")==0) {
    if (size != 3) errorKan1("%s\n","[(setAttributeList) ob attrlist] extension rob");
    rob = KsetAttributeList(getoa(obj,1), getoa(obj,2));
  }else if (strcmp(key,"getAttribute")==0) {
    if (size != 3) errorKan1("%s\n","[(getAttribute) ob key] extension rob");
    rob = KgetAttribute(getoa(obj,1),getoa(obj,2));
  }else if (strcmp(key,"setAttribute")==0) {
    if (size != 4) errorKan1("%s\n","[(setAttributeList) ob key value] extension rob");
    rob = KsetAttribute(getoa(obj,1), getoa(obj,2),getoa(obj,3));
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
	/* if (!( m == 0 || m == PROTECT || m == ABSOLUTE_PROTECT || m == ATTR_INFIX))
	   errorKan1("%s\n","The number must be 0, 1 or 2.");*/
    putUserDictionary2(obj2.lc.str,(obj2.rc.op->lc).ival,(obj2.rc.op->rc).ival,
                       m,CurrentContextp->userDictionary);
  }else if (strcmp(key,"or_attr")==0) {
    if (size != 3) errorKan1("%s\n","[(or_attr)  num symbol] extension.");
    obj1 = getoa(obj,1);
    obj2 = getoa(obj,2);
    if (obj1.tag != Sinteger) errorKan1("%s\n","[(or_attr)  num symbol] extension.");
    if (obj2.tag != Sstring)  errorKan1("%s\n","[(or_attr)  num symbol] extension.");
    m = KopInteger(obj1);
    rob = KfindUserDictionary(obj2.lc.str);
    if (rob.tag != NoObject.tag) {
      if (strcmp(UD_str,obj2.lc.str) == 0) {
        m |= UD_attr;
      }else errorKan1("%s\n","or_attr: internal error.");
    }
    rob = KpoInteger(m);
    putUserDictionary2(obj2.lc.str,(obj2.rc.op->lc).ival,(obj2.rc.op->rc).ival,
                       m,CurrentContextp->userDictionary);
  }else if (strcmp(key,"getattr")==0) {
    if (size != 2) errorKan1("%s\n","[(getattr) symbol] extension.");
    obj1 = getoa(obj,1);
    if (obj1.tag != Sstring)  errorKan1("%s\n","[(getattr) symbol] extension.");
    rob = KfindUserDictionary(obj1.lc.str);
	if (rob.tag != NoObject.tag) {
	  if (strcmp(UD_str,obj1.lc.str) == 0) {
		rob = KpoInteger(UD_attr);
	  }else errorKan1("%s\n","getattr: internal error.");
	}else rob = NullObject;
  }else if (strcmp(key,"getServerEnv")==0) {
    if (size != 2) errorKan1("%s\n","[(getServerEnv) serverName] extension.");
    obj1 = getoa(obj,1);
    if (obj1.tag != Sdollar) errorKan1("%s\n","[(getServerEnv) serverName] extension.");
    {
      char **se; int ii; int nn;
	  char **getServerEnv(char *);
      se = getServerEnv(KopString(obj1));
      if (se == NULL) {
        debugServerEnv(KopString(obj1));
        rob = NullObject;
      }else{
        for (ii=0,nn=0; se[ii] != NULL; ii++) nn++;
        rob = newObjectArray(nn);
        for (ii=0; ii<nn; ii++) {
          putoa(rob,ii,KpoString(se[ii]));
        }
      }
    }
  }else if (strcmp(key,"read")==0) {
    if (size != 3) errorKan1("%s\n","[(read) fd size] extension.");
    obj1 = getoa(obj,1);
    if (obj1.tag != Sinteger) errorKan1("%s\n","[(read) fd size] extension. fd must be an integer.");
    obj2 = getoa(obj,2);
    if (obj2.tag != Sinteger) errorKan1("%s\n","[(read) fd size] extension. size must be an integer.");
    {
      int total, n, fd;
      char *s; char *s0;
      fd = KopInteger(obj1);
      total = KopInteger(obj2);
      if (total <= 0) errorKan1("%s\n","[(read) ...]; negative size has not yet been implemented.");
      /* Return a string. todo: implement SbyteArray case. */
      s0 = s = (char *) sGC_malloc(total+1);
      if (s0 == NULL) errorKan1("%s\n","[(read) ...]; no more memory."); 
      while (total >0) {
        n = read(fd, s, total);
        if (n < 0) { perror("read"); errorKan1("%s\n","[(read) ...]; read error.");}
        s[n] = 0;
        total -= n;  s = &(s[n]);
      }
      rob = KpoString(s0);
    }
  }else if (strcmp(key,"regionMatches")==0) {
    if (size != 3) errorKan1("%s\n","[(regionMatches) str strArray] extension.");
    obj1 = getoa(obj,1);
    if (obj1.tag != Sdollar) errorKan1("%s\n","[(regionMatches) str strArray] extension. str must be a string.");
	obj2 = getoa(obj,2);
    if (obj2.tag != Sarray) errorKan1("%s\n","[(regionMatches) str strArray] extension. strArray must be an array.");
    rob = KregionMatches(obj1,obj2);
  }else if (strcmp(key,"newVector")==0) {
    if (size != 2) errorKan1("%s\n","[(newVector) m] extension.");
    obj1 = getoa(obj,1);
    if (obj1.tag != Sinteger) errorKan1("%s\n","[(newVector) m] extension. m must be an integer.");
    rob = newObjectArray(KopInteger(obj1));
  }else if (strcmp(key,"newMatrix")==0) {
    if (size != 3) errorKan1("%s\n","[(newMatrix) m n] extension.");
    obj1 = getoa(obj,1);
    if (obj1.tag != Sinteger) errorKan1("%s\n","[(newMatrix) m n] extension. m must be an integer.");
	obj2 = getoa(obj,2);
    if (obj2.tag != Sinteger) errorKan1("%s\n","[(newMatrix) m n] extension. n must be an integer.");
    rob = newObjectArray(KopInteger(obj1));
	for (i=0; i<KopInteger(obj1); i++) {
      putoa(rob,i,newObjectArray(KopInteger(obj2)));
	}
  }else if (strcmp(key,"addModuleOrder")==0) {
    obj1 = getoa(obj,1);
    rob = KaddModuleOrder(obj1);
  }else if (strcmp(key,"ooPower")==0) {
    if (size != 3) errorKan1("%s\n","[(ooPower) a b] extension.");
    obj1 = getoa(obj,1);
	obj2 = getoa(obj,2);
    rob = KooPower(obj1,obj2);
  }else if (strcmp(key,"Krest")==0) {
    if (size != 2) errorKan1("%s\n","[(Krest) a] extension b");
    obj1 = getoa(obj,1);
    rob = Krest(obj1);
  }else if (strcmp(key,"Kjoin")==0) {
    if (size != 3) errorKan1("%s\n","[(Kjoin) a b] extension c");
    obj1 = getoa(obj,1);
	obj2 = getoa(obj,2);
    rob = Kjoin(obj1,obj2);
  }else if (strcmp(key,"ostype")==0) {
    /* Hard encode the OS type.  cpp -dM /dev/null */
#if defined(__CYGWIN__)
    rob = newObjectArray(1);
    putoa(rob,0,KpoString("windows"));
#else
    rob = newObjectArray(2);
    putoa(rob,0,KpoString("unix"));
#if defined(__APPLE__)
    putoa(rob,1,KpoString("mac"));
#else
    putoa(rob,1,KpoString("generic"));
#endif
#endif
  }else if (strcmp(key,"stringToArgv")==0) {
    if (size != 2) errorKan1("%s\n","[(stringToArgv) a ] extension b");
	obj1 = getoa(obj,1);
	if (obj1.tag != Sdollar) errorKan1("%s\n","[(stringToArgv) a ] extension b,  a must be a string.");
    rob = KstringToArgv(obj1);
  }else if (strcmp(key,"stringToArgv2")==0) {
    if (size != 3) errorKan1("%s\n","[(stringToArgv2) str separator] extension b");
	obj1 = getoa(obj,1);
	obj2 = getoa(obj,2);
    rob = KstringToArgv2(obj1,obj2);
  }else if (strcmp(key,"traceClearStack")==0) {
    traceClearStack();
    rob = NullObject;
  }else if (strcmp(key,"traceShowStack")==0) {
    char *ssst;
    ssst = traceShowStack();
    if (ssst != NULL) {
      rob = KpoString(ssst);
    }else{
      rob = NullObject;
    }
  }else if (strcmp(key,"traceShowScannerBuf")==0) {
    char *ssst;
    ssst = MsgSourceTrace;
    if (ssst != NULL) {
      rob = KpoString(ssst);
    }else{
      rob = NullObject;
    }
  }else if (strcmp(key,"regexec")==0) {
    if ((size != 3) && (size != 4)) errorKan1("%s\n","[(regexec) reg strArray flag(optional)] extension b");
    obj1 = getoa(obj,1);
    if (obj1.tag != Sdollar) errorKan1("%s\n","regexec, the first argument should be a string (regular expression).");
    obj2 = getoa(obj,2); 
    if (obj2.tag != Sarray) errorKan1("%s\n","regexec, the second argument should be an array of a string.");
    if (size == 3) obj3 = newObjectArray(0);
    else obj3 = getoa(obj,3);
    rob = oregexec(obj1,obj2,obj3);
  }else if (strcmp(key,"unlink")==0) {
    if (size != 2) errorKan1("%s\n","[(unlink) filename] extension b");
    obj1 = getoa(obj,1);
    if (obj1.tag != Sdollar) errorKan1("%s\n","unlink, the first argument should be a string (filename).");
    rob = KpoInteger(oxDeleteFile(KopString(obj1)));
  }else if (strcmp(key,"quiet")==0) {
    obj1 = getoa(obj,1);
    if (obj1.tag != Sinteger) errorKan1("%s\n","quiet, the first argument should be an integer.");
    Quiet = KopInteger(obj1);
    rob = obj1;
  }
#include "plugin.hh"
#include "Kclass/tree.hh"
  else{
	fprintf(stderr,"key=%s; ",key);
    errorKan1("%s\n","Unknown key for extension.");
  }


  return(rob);
}

struct object KregionMatches(struct object sobj, struct object keyArray)
{
  struct object rob = OINIT;
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

static struct object oregexec(struct object oregex,struct object ostrArray,struct object oflag) {
  struct object rob = OINIT;
  struct object ob = OINIT;
  int n,i,j,m,keyn,cflag,eflag,er;
  char *regex;
  regex_t preg;
  char *s;
  char *mbuf; int mbufSize;
#define REGMATCH_SIZE 100
  regmatch_t pmatch[100]; size_t nmatch;
  int size;

  nmatch = (size_t) REGMATCH_SIZE;
  rob = newObjectArray(0);
  mbufSize = 1024;

  if (oregex.tag != Sdollar) return rob;
  if (ostrArray.tag != Sarray) return rob;
  n = getoaSize(ostrArray);
  for (i=0; i<n; i++) {
    if (getoa(ostrArray,i).tag != Sdollar) { return rob; }
  }
  if (oflag.tag != Sarray) errorKan1("%s\n","oregexec: oflag should be an array of integers.");
  cflag = eflag = 0;
  oflag = Kto_int32(oflag);
  for (i=0; i<getoaSize(oflag); i++) {
    ob = getoa(oflag,i);
    if (ob.tag != Sinteger) errorKan1("%s\n","oregexec: oflag is not an array of integers.");
    if (i == 0) cflag = KopInteger(ob);
    if (i == 1) eflag = KopInteger(ob);
  }

  regex = KopString(oregex);
  if (er=regcomp(&preg,regex,cflag)) {
    mbuf = (char *) sGC_malloc(mbufSize);
    if (mbuf == NULL) errorKan1("%s\n","No more memory.");
    regerror(er,&preg,mbuf,mbufSize-1);
    errorKan1("regcomp error: %s\n",mbuf);
  }

  size = 0; /* We should use list instead of counting the size. */
  for (i=0; i<n; i++) {
    s = KopString(getoa(ostrArray,i));
    er=regexec(&preg,s,nmatch,pmatch,eflag);
    if ((er != 0) && (er != REG_NOMATCH)) { 
      mbuf = (char *) sGC_malloc(mbufSize);
      if (mbuf == NULL) errorKan1("%s\n","No more memory.");
      regerror(er,&preg,mbuf,mbufSize-1);
      errorKan1("regcomp error: %s\n",mbuf);
    }
    if (er == 0) size++;
  }

  rob = newObjectArray(size);
  size = 0;
  for (i=0; i<n; i++) {
    s = KopString(getoa(ostrArray,i));
    er=regexec(&preg,s,nmatch,pmatch,eflag);
    if ((er != 0) && (er != REG_NOMATCH)) { 
      mbuf = (char *) sGC_malloc(mbufSize);
      if (mbuf == NULL) errorKan1("%s\n","No more memory.");
      regerror(er,&preg,mbuf,mbufSize-1);
      errorKan1("regcomp error: %s\n",mbuf);
    }
    if (er == 0) {
      ob = newObjectArray(3);
      putoa(ob,0,KpoString(s));
      /* temporary */
      putoa(ob,1,KpoInteger((int) (pmatch[0].rm_so)));
      putoa(ob,2,KpoInteger((int) (pmatch[0].rm_eo)));
      putoa(rob,size,ob);
      size++;
    }
  }
  regfree(&preg);
  return rob;
}
