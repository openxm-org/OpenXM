/* Kclass/sample.c   sample new class. */
/* How to add a new class to sm1. 
   1. create a new file Kclass/newclass-name.c and change
      Makefile
   2. Define new tags in kclass.h and put your prototypes.
       (Note. CMO is defined to be 1024 in stackm.h
        So, CMO+cmotag is used for the etag for raw cmo object.
        This should be changed near future.)
   3. Add entry points of your functions for newclass to kclass.c
*/
#include <stdio.h>
#include "../datatype.h"
#include "../stackm.h"
#include "../extern.h"
#include "../gradedset.h"
#include "../extern2.h"
#include "../kclass.h"


/* Data conversion function : see KclassDataConversion*/
struct object KpoSampleClass(struct object *obp) {
  struct object rob;
  struct object *newobp;
  rob.tag = Sclass;
  rob.lc.ival = CLASSNAME_sampleClass;
  newobp = (struct object *) sGC_malloc(sizeof(struct object));
  /* Yes! You can call KpoSampleClass(&localVar) */
  if (newobp == NULL) errorKan1("%s\n","Kclass/sample.c, no more memory.");
  *newobp = *obp;
  rob.rc.voidp = newobp;
  return(rob);
}
/* cf. KopSampleClass() in kclass.h */

/* Printing function : see fprintClass */
void fprintSampleClass(FILE *fp,struct object *op)
{
  printObject(*op,0,fp);
}

/* Comparison function : see classEqualQ */
int eqSampleClass(struct object *ob1, struct object *ob2)
{
  return(KooEqualQ(*ob1,*ob2));
}

/* Addition : see Kclass_ooAdd */




