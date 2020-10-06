/* $OpenXM: OpenXM/src/kan96xx/plugin/sample.c,v 1.3 2005/06/16 05:07:24 takayama Exp $ */
#include <stdio.h>
#include "../Kan/datatype.h"
#include "../Kan/stackm.h"
#include "../Kan/kclass.h"

#include "sample.h"

/*
  options for Kan/Makefile
PLUGIN = ../plugin
PLUGIN_PROTOTYPE  = $(PLUGIN)/sample.h
PLUGIN_LIB  = $(PLUGIN)/sample.a
PLUGIN_EXT = $(PLUGIN)/sample.hh
PLUGIN_SM1 =$(PLUGIN)/sample.sm1
PLUGIN_LINKFLAG = 
*/

static struct operandStack *SharedStack = NULL;
static struct object Kplugin_NullObject = OINIT;

/*  Sample is a very simple stack machine.
    There are only one operation "add" on the shared stack.
    Kplugin_sample(KPLUGIN_SAMPLE_INIT,
                   [ new-operand-stack-for-the-shared-stack ]);
    Pplugin_sample(KPLUGIN_SAMPLE_ADD, [ ] ); push the result on the shared
                                              stack.
*/
   
int Kplugin_sample(int opcode, struct object obj) {
  /* obj is assumed to be an array. */
  struct object ob1 = OINIT;
  struct object ob2 = OINIT;
  struct object ob3 = OINIT;
  static int initialized = 0;
  if (opcode != KPLUGIN_SAMPLE_INIT && !initialized) {
    errorSample("This component is not initialized.");
    return(-1);
  }
  switch(opcode) {
  case KPLUGIN_SAMPLE_INIT:
    if (getoaSize(obj) < 1) {
      errorSample("No argument for opcode=0 (initialization).");
      return(-1);
    }
    ob1 = getoa(obj,0);
    if (ectag(ob1) != CLASSNAME_OPERANDSTACK) {
      errorSample("Argument must be an operand stack.");
      return(-1);
    }
    SharedStack = (struct operandStack *) ecbody(ob1);
    fprintf(stderr,"Initializing plugin_sample\n");
    initialized = 1;
    Kplugin_NullObject.tag = Snull;
    break;
  case KPLUGIN_SAMPLE_SELECT:
    return(1); /* finished. */
    break;
  case KPLUGIN_SAMPLE_WAIT:
    return(0);
    break;
  case KPLUGIN_SAMPLE_ADD:
    ob2 = Kplugin_pop(SharedStack);  printf("%d\n",ob2.lc.ival);
    ob1 = Kplugin_pop(SharedStack);  printf("%d\n",ob1.lc.ival);
    ob1.lc.ival += ob2.lc.ival;
    Kplugin_push(ob1,SharedStack);
    break;
  default:
    errorSample("plugin_sample: unknown op-code.");
    return(-1);
    break;
  }
  return(0);
}

/* Should we use functions to handle operand stacks defined in 
   ../Kan/  or we should have our own functions to handle operand stack?
  We should copy functions to handle stacks from ../Kan.
*/
int Kplugin_push(struct object ob,struct operandStack *operandstack)
{
  (operandstack->ostack)[(operandstack->sp)++] = ob;
  if ((operandstack->sp) >= (operandstack->size)) {
    errorSample("Operand stack overflow.");
    (operandstack->sp)--;
    return(-1);
  }
  return(0);
}

struct object Kplugin_pop(struct operandStack *operandstack)
{
  if ((operandstack->sp) <= 0) {
    return( Kplugin_NullObject );
  }else{
    return( (operandstack->ostack)[--(operandstack->sp)]);
  }
}

struct object Kplugin_peek(int k,struct operandStack *operandstack)
{
  if (((operandstack->ostack)-k-1) < 0) {
    return( Kplugin_NullObject );
  }else{
    return( (operandstack->ostack)[(operandstack->sp)-k-1]);
  }
}


static void errorSample(char *s) {
  fprintf(stderr,"Error in plugin/sample: %s\n",s);
  /* or push error message in the sharedStack. */
}


