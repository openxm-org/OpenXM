/* $OpenXM: OpenXM/src/kan96xx/Kan/shell.c,v 1.1 2003/11/24 12:38:17 takayama Exp $ */
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

struct object KoxShell(struct object ob) {
  return KoxShell_test1(ob);
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
  if (strcmp(cmd,"which")==0) {
	if (n == 2) {
	  rob = KoxWhich(getoa(ob,1),KpoInteger(0));
	}else if (n==3) {
	  rob = KoxWhich(getoa(ob,1),getoa(ob,2));
	}else{
	  errorKan1("%s\n","shell: << which command-name >> or << which command-name path >>");
	}
	return(rob);
  }else if (strcmp(cmd,"export")==0) {
  }else{
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


