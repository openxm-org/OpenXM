/* $OpenXM$ */
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


