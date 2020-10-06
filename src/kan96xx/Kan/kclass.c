/* $OpenXM: OpenXM/src/kan96xx/Kan/kclass.c,v 1.6 2006/01/30 10:38:36 takayama Exp $ */
/* kclass.c,  1997, 3/1
   This module handles class data base.
   This is a top level and provides an interface for sm1 for Sclass objects.
   Main processing is done in Kclass/*
                                      See, Kclass/sample.h, Kclass/sample.c  ;
                                      grep the keyword CLASSNAME_sampleClass
*/
#include <stdio.h>
#include <string.h>
#include "datatype.h"
#include "stackm.h"
#include "extern.h"
#include "gradedset.h"
#include "extern2.h"
#include "kclass.h"

#define CLASSTABLE_SIZE 4096


struct object * ClassDictionaries[CLASSTABLE_SIZE];
char *ClassNames[CLASSTABLE_SIZE];
int ClassTypes[CLASSTABLE_SIZE];

void initClassDataBase() {
  int i;
  for (i=0; i<CLASSTABLE_SIZE; i++) {
    ClassTypes[i] = CLASS_NOT_USED;
  }
  /* Initialize CLASS_INTERNAL */
  ClassTypes[CLASSNAME_OPERANDSTACK] = CLASS_INTERNAL;
  ClassNames[CLASSNAME_OPERANDSTACK] = "Class.OperandStack";
  ClassDictionaries[CLASSNAME_OPERANDSTACK] = (struct object *)NULL;
  /* We have to creat new dictionary in a future. */

  ClassTypes[CLASSNAME_ERROR_PACKET] = CLASS_OBJ;
  ClassNames[CLASSNAME_ERROR_PACKET] = "Class.ErrorPacket";
  ClassDictionaries[CLASSNAME_ERROR_PACKET] = (struct object *)NULL;
  /* We have to creat new dictionary in a future. */

  ClassTypes[CLASSNAME_CONTEXT] = CLASS_INTERNAL;
  ClassNames[CLASSNAME_CONTEXT] = "Class.Context";
  ClassDictionaries[CLASSNAME_CONTEXT] = (struct object *)NULL;
  /* We have to creat new dictionary in a future. */

  ClassTypes[CLASSNAME_GradedPolySet] = CLASS_INTERNAL;
  ClassNames[CLASSNAME_GradedPolySet] = "Class.GradedPolySet";
  ClassDictionaries[CLASSNAME_GradedPolySet] = (struct object *)NULL;
  /* We have to creat new dictionary in a future. */

  ClassTypes[CLASSNAME_mathcap] = CLASS_OBJ;
  ClassNames[CLASSNAME_mathcap] = "Class.mathcap";
  ClassDictionaries[CLASSNAME_mathcap] = (struct object *)NULL;
  /* We have to creat new dictionary in a future. */

  ClassTypes[CLASSNAME_CMO] = CLASS_OBJ;
  ClassNames[CLASSNAME_CMO] = "Class.CMO";
  ClassDictionaries[CLASSNAME_CMO] = (struct object *)NULL;
  /* We have to creat new dictionary in a future. */

  ClassTypes[CLASSNAME_indeterminate] = CLASS_OBJ;
  ClassNames[CLASSNAME_indeterminate] = "Class.indeterminate";
  ClassDictionaries[CLASSNAME_indeterminate] = (struct object *)NULL;

  ClassTypes[CLASSNAME_tree] = CLASS_OBJ;
  ClassNames[CLASSNAME_tree] = "Class.tree";
  ClassDictionaries[CLASSNAME_tree] = (struct object *)NULL;

  ClassTypes[CLASSNAME_recursivePolynomial] = CLASS_OBJ;
  ClassNames[CLASSNAME_recursivePolynomial] = "Class.recursivePolynomial";
  ClassDictionaries[CLASSNAME_recursivePolynomial] = (struct object *)NULL;

  ClassTypes[CLASSNAME_polynomialInOneVariable] = CLASS_OBJ;
  ClassNames[CLASSNAME_polynomialInOneVariable] = "Class.polynomialInOneVariable";
  ClassDictionaries[CLASSNAME_polynomialInOneVariable] = (struct object *)NULL;

  ClassTypes[CLASSNAME_sampleClass] = CLASS_OBJ;
  ClassNames[CLASSNAME_sampleClass] = "Class.sampleClass";
  ClassDictionaries[CLASSNAME_sampleClass] = (struct object *)NULL;
  /* We have to creat new dictionary in a future. */

}


void fprintClass(FILE *fp,struct object obj) {
  int tag;
  tag = ectag(obj);
  if (tag == -1) {
    return ;
  }
  if (ClassTypes[tag] != CLASS_NOT_USED) {
    fprintf(fp,"%s ",ClassNames[tag]);
  }
  switch(tag) {
  case CLASSNAME_OPERANDSTACK:
    break;
  case CLASSNAME_ERROR_PACKET:
    fprintErrorPacket(fp,KopErrorPacket(obj));
    break;
  case CLASSNAME_CONTEXT:
    fprintContext(fp,KopContext(obj));
    break;
  case CLASSNAME_GradedPolySet:
    outputGradedPolySet(KopGradedPolySet(obj),0);
    break;
  case CLASSNAME_mathcap:
    fprintMathCap(fp,KopMathCap(obj));
    break;
  case CLASSNAME_CMO:
    fprintCMOClass(fp,obj);
    break;
  case CLASSNAME_indeterminate:
    fprintIndeterminate(fp,obj);
    break;
  case CLASSNAME_tree:
    fprintTree(fp,obj);
    break;
  case CLASSNAME_recursivePolynomial:
    fprintRecursivePolynomial(fp,obj);
    break;
  case CLASSNAME_polynomialInOneVariable:
    fprintPolynomialInOneVariable(fp,obj);
    break;
  case CLASSNAME_sampleClass:
    fprintSampleClass(fp,KopSampleClass(obj));
    break;
  default:
    fprintf(fp,"Unknown class tag=%d.\n",tag);
    break;
  }
}

int KclassEqualQ(struct object ob1,struct object ob2) {
  if (ectag(ob1) != ectag(ob2)) return(0);
  switch(ectag(ob1)) {
  case CLASSNAME_OPERANDSTACK:
  case CLASSNAME_CONTEXT:
    if (ecbody(ob1) == ecbody(ob2)) return(1);
    else return(0);
    break;
  case CLASSNAME_sampleClass:
    return(eqSampleClass(KopSampleClass(ob1),KopSampleClass(ob2)));
    break;
  case CLASSNAME_indeterminate:
    return(KooEqualQ(KopIndeterminate(ob1),KopIndeterminate(ob2)));
    break;
  default:
    errorKan1("%s\n","kclass.c (KclassEqualQ cannot compare these objects.)");
    break;
  }
}

void fprintErrorPacket(FILE *fp,struct object *op)
{
  printObject(*op,0,fp);
}

void fprintMathCap(FILE *fp,struct object *op)
{
  printObject(*op,0,fp);
}

struct object KpoMathCap(struct object *obp) {
  struct object rob = OINIT;
  struct object *newobp;

  newobp = (struct object *) sGC_malloc(sizeof(struct object));
  /* Yes! You can call KpoMathCap(&localVar) */
  if (newobp == NULL) errorKan1("%s\n","kclass.c, no more memory.");
  *newobp = *obp;

  rob.tag = Sclass;
  rob.lc.ival = CLASSNAME_mathcap;
  rob.rc.voidp = newobp;
  return(rob);
}

/* try
  [ 1 2 3] [(class) (sampleClass)] dc ::
*/
struct object KclassDataConversion(struct object ob1,struct object ob2)
{ /*  It is called from primitive.c  data_conversion. */
  /*  This function handles the following situnation.
      (This is not yet documented.)
      ob1                 [(class) (class-name)]  dc  :  method=1
      ob1(with class tag) [(class) (class-name)]  dc  :  method=2
      ob1(with class tag) (usual flag)            dc  :  method=3
      It also create a new class object.
      ob1 (error) dc --> ErrorObject with the contents ob1

      ob1(with class tag) (body) dc --> [left tag, right body of ob1.]
  */
  struct object rob = NullObject;
  int method ;
  struct object ff0 = OINIT;
  struct object ff1 = OINIT;
  struct object ob3 = OINIT;  /* for work.*/
  struct object ob4 = OINIT;
  char *ccc;
  char *key;

  if (ob1.tag == Sclass && ob2.tag == Sarray) {
    method = 2;
  }else if (ob1.tag == Sclass && ob2.tag == Sdollar) {
    method = 3;
  }else if (ob1.tag != Sclass && ob2.tag == Sarray) {
    method = 1;
  }else{
    errorKan1("%s\n","kclass.c : KclassDataConversion() cannot make this data conversion.");
  }
  switch(method) {
  case 1:
    if (getoaSize(ob2) != 2) errorKan1("%s\n","kclass.c : KclassDataConversion() the second argument should be [(class) (class-name)]");
    ff0 = getoa(ob2,0); ff1 = getoa(ob2,1);
    if (ff0.tag != Sdollar || ff1.tag != Sdollar) 
      errorKan1("%s\n","kclass.c : KclassDataConversion() the second argument should be [(class) (class-name)]");
    if (strcmp("class",KopString(ff0)) != 0) 
      errorKan1("%s\n","kclass.c : KclassDataConversion() the second argument should be [(class) (class-name)] (class)!!!");

    ccc = KopString(ff1);  /* target data type */

    /* From primitive to Class object */

    if (strcmp(ccc,"sampleClass") == 0) {
      rob = KpoSampleClass(&ob1);
    }else if (strcmp(ccc,"errorPacket") == 0) {
      if (ob1.tag != Sarray) errorKan1("%s\n","kclass.c : KclassDataConversion , !array --> errorPacket is not supported.");
      if (getoaSize(ob1) != 3) errorKan1("%s\n","kclass.c : KclassDataConversion , only [integer, integer, string] --> errorPacket is supported.");
      if (getoa(ob1,0).tag != Sinteger) errorKan1("%s\n","kclass.c : KclassDataConversion , only [integer, integer, string] --> errorPacket is supported.");
      if (getoa(ob1,1).tag != Sinteger) errorKan1("%s\n","kclass.c : KclassDataConversion , only [integer, integer, string] --> errorPacket is supported.");
      if (getoa(ob1,2).tag != Sdollar) errorKan1("%s\n","kclass.c : KclassDataConversion , only [integer, integer, string] --> errorPacket is supported.");
      rob = KnewErrorPacketObj(ob1);
    }else if (strcmp(ccc,"indeterminate") == 0) {
      if (ob1.tag != Sdollar) errorKan1("%s\n","kclass.c : KclassDataConversion , !String --> indeterminate is not supported.");
      rob = KpoIndeterminate(ob1);
    }else if (strcmp(ccc,"mathcap") == 0) {
      /* You should check ob1 contains mathcap data or not.
         I've not yet written them.
      */
      rob = KpoMathCap(&ob1);
    }else if (strcmp(ccc,"tree") == 0) {
      if (ob1.tag != Sarray) errorKan1("%s\n","kclass.c : KclassDataConversion , !array --> tree is not supported.");
      rob = KpoTree(ob1);
    }else if (strcmp(ccc,"recursivePolynomial") == 0) {
      if (ob1.tag != Spoly) errorKan1("%s\n","kclass.c : KclassDataConversion , !poly --> recursivePoly is not supported.");
      rob = polyToRecursivePoly(ob1);
    }else{
      errorKan1("%s\n","KclassDataConversion: this type of data conversion from primitive object to class object is not supported.");
    }
    break;
  case 2:
    if (getoaSize(ob2) != 2) errorKan1("%s\n","kclass.c : KclassDataConversion() the second argument should be [(class) (class-name)]");
    ff0 = getoa(ob2,0); ff1 = getoa(ob2,1);
    if (ff0.tag != Sdollar || ff1.tag != Sdollar) 
      errorKan1("%s\n","kclass.c : KclassDataConversion() the second argument should be [(class) (class-name)]");
    if (strcmp("class",KopString(ff0)) != 0) 
      errorKan1("%s\n","kclass.c : KclassDataConversion() the second argument should be [(class) (class-name)] (class)!!!");

    ccc = KopString(ff1);  /* target data type. */
    switch(ectag(ob1)) {
    case CLASSNAME_sampleClass:
      if (strcmp(ccc,"sampleClass") == 0) {
        rob = KpoSampleClass(&ob1);
      }else{
        errorKan1("%s\n","KclassDataCOnversion: this type of data conversion from class object to class object is not supported.");
      }
      break;
    default:
      errorKan1("%s\n","KclassDataConversion: this type of data conversion from class object to class object is not supported.");
    }
    break;
  case 3:
    key = KopString(ob2);  /* target data type */
    if (key[0] == 't' || key[0] =='e') {
      if (strcmp(key,"type?")==0) {
        rob = KpoInteger(ob1.tag);
        return(rob);
      }else if (strcmp(key,"type??")==0) {
        if (ob1.tag != Sclass) {
          rob = KpoInteger(ob1.tag);
        }else {
          rob = KpoInteger(ectag(ob1));
        }
        return(rob);
      }else if (strcmp(key,"error")==0) {
        rob = KnewErrorPacketObj(ob1);
        return(rob);
      }
    }else if (key[0] == 'b') {
      if (strcmp(key,"body") == 0) {
        rob = newObjectArray(2);
        putoa(rob,0,KpoInteger(ectag(ob1)));
        putoa(rob,1,*((struct object *)(ecbody(ob1))));
        return rob;
      }
    }

    /* Class object to primtive Object */
    switch(ectag(ob1)) {
    case CLASSNAME_sampleClass:
      if (strcmp(key,"array") == 0) {
        rob = *(KopSampleClass(ob1));
      }else{
        errorKan1("%s\n","KclassDataCOnversion: this type of data conversion from class object to primitive object is not supported.");
      }
      break;
    case CLASSNAME_mathcap:
      if (strcmp(key,"array") == 0) {
        rob = newObjectArray(2);
        ob3 = KpoString("mathcap-object");
        putoa(rob,0,ob3);
        putoa(rob,1,*(KopMathCap(ob1)));
      }else{
        errorKan1("%s\n","KclassDataConversion: this type of data conversion from class object mathcap to primitive object is not supported.");
      }
      break;
    case CLASSNAME_indeterminate:
      if (strcmp(key,"string") == 0) {
        rob = KopIndeterminate(ob1);
      }else {
        errorKan1("%s\n","KclassDataConversion: interminate-->?? is not supported..");
      }
      break;
    case CLASSNAME_tree:
      if (strcmp(key,"array") == 0) {
        rob = KopTree(ob1);
      }else {
        errorKan1("%s\n","KclassDataConversion: tree-->?? is not supported..");
      }
      break;
    case CLASSNAME_recursivePolynomial:
      if (strcmp(key,"string") == 0) {
        errorKan1("%s\n","Translation of recursive polynomial to a string should be implemented.");
      }else if (strcmp(key,"poly") == 0) {
        rob = recursivePolyToPoly(ob1);
      }else if (strcmp(key,"array") == 0) {
        rob = KopRecursivePolynomial(ob1);
      }else {
        errorKan1("%s\n","KclassDataConversion: recursivePoly-->?? is not supported..");
      }
      break;
    default:
      errorKan1("%s\n","KclassDataConversion: this type of data conversion from class object to primitive object is not supported.");
    }
    break;
  }
  return(rob);
}

/* Arithmetic operations for class objects. */
struct object Kclass_ooAdd(struct object ob1, struct object ob2)
{
  /* It is called from ooAdd(). */
  /* ob1 or ob2 must have the class tag. */
  struct object rob = NullObject;

  /* Default action */
  rob = addTree(ob2,ob1);
  return(rob);
}  


      

