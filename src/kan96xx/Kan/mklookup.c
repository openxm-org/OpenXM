/* $OpenXM: OpenXM/src/kan96xx/Kan/mklookup.c,v 1.5 2005/07/03 11:08:54 ohara Exp $ */
#include <stdio.h>
#include <stdlib.h>
#include "datatype.h"
#include "stackm.h"

/* Refer to global.c. You need to keep the orders of names and values */
int main() {
  int i,j;
  char *names[]={"Snull","Sinteger","Sstring","SexecutableArray","Soperator",
                 "Sdollar","Sarray","SleftBraceTag","SrightBraceTag","Spoly",
                 "SarrayOfPOLY","SmatrixOfPOLY","Slist","Sfile","Sring",
                 "SuniversalNumber","SrationalFunction","Sclass","Sdouble",
                 "SbyteArray"};
  int values[]={Snull,Sinteger,Sstring,SexecutableArray,Soperator,
                Sdollar,Sarray,SleftBraceTag,SrightBraceTag,Spoly,
                SarrayOfPOLY,SmatrixOfPOLY,Slist,Sfile,Sring,
                SuniversalNumber,SrationalFunction,Sclass,Sdouble,
                SbyteArray};
  for (i=0; i<TYPES; i++) {
    if (values[i] != i) {
      fprintf(stderr,"Errors in names and values\n");
      return 100;
    }
    for (j=0; j<TYPES; j++) {
      printf("#define %s%s %d\n",names[i],names[j],values[i]*TYPES+values[j]);
    }
  }
  return 0;
}

           
