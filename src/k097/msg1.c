#include <stdio.h>
#include "datatype.h"
#include "stackm.h"
#include "extern.h"
#include "extern2.h"

#include "ak0.h"

static int debug1 = 0;

void KasirKanExecute0(char *s) {
  char *ans;
  if (debug1) {
    fprintf(stderr,"This is KasirKanExecute0 in mgs1.c.\n");
    fprintf(stderr,"Now execute .. <<%s>> as the asir command.\n",s);
  }
  ans = KasirKanExecuteString(s);
  if (debug1) {
    fprintf(stderr,"Asir returns .. %s \n",ans);
  }
  ans = KasirKanConvert(ans);
  if (debug1) {
    fprintf(stderr,"Input to sm1 .. %s \n",ans);
  }
  KSexecuteString(ans); /* put the result on the stack. */
  if (debug1) {
    fprintf(stderr,"\nDone.\n");
  }
}


