/* $OpenXM$ */
/* Kclass/rawcmo.c   */
#include <stdio.h>
#include "../datatype.h"
#include "../stackm.h"
#include "../extern.h"
#include "../gradedset.h"
#include "../extern2.h"
#include "../kclass.h"

/* Printing function : see fprintClass */
void fprintCMOClass(FILE *fp,struct object op)
{
  if (fp != stdout) warningKan("fprintfCMOClass() can output only to stdout.\n");
  /* cmo functions are defined in the plugin. So, we use
     executeString */
  KSpush(op);
  KSexecuteString("[(cmoDumpCmo) 3 -1 roll ] extension pop ");
}





