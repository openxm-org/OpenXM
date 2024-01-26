/* $OpenXM: OpenXM/src/kan96xx/Kan/option.c,v 1.20 2016/03/31 05:27:34 takayama Exp $ */
#include <stdio.h>
#include <string.h>
#include "datatype.h"
#include "stackm.h"
#include "extern.h"
#include "gradedset.h"
#include "kclass.h"
#include "lookup.h"
#include <signal.h>
#include "mysig.h"

extern void ctrlC();
void warningOption(char *str);


struct object KsystemVariable(ob)
     struct object ob; /* Sarray */
{
  /* Don't forget to write the keys in usage.c */
  extern int PrintDollar;
  extern int Wrap;
  extern int COutput;
  extern struct ring *CurrentRingp;
  extern int Verbose;
  extern int UseCriterion1;
  extern int UseCriterion2B;
  extern int ReduceLowerTerms;
  extern int CheckHomogenization;
  extern int Homogenize;
  extern int Statistics;
  extern int AutoReduce;
  extern int Osp;
  extern struct operandStack StandardStack;
  extern struct operandStack ErrorStack;
  extern int ErrorMessageMode;
  extern int WarningMessageMode;
  extern int CatchCtrlC;
  extern int Strict;
  extern struct context *CurrentContextp;
  extern struct context *PrimitiveContextp;
  extern int Strict2;
  extern int SigIgn;
  extern int KSPushEnvMode;
  extern int KanGBmessage;
  extern int TimerOn;
  extern int OutputStyle;
  extern int Sugar;
  extern int Homogenize_vec;
  extern int CmoDMSOutputOption;
  extern int DebugReductionRed; /* hidden option */
  extern int DebugReductionEcart; 
  extern char *VersionString;
  extern int AvoidTheSameRing;
  extern char *LeftBracket;
  extern char *RightBracket;
  extern int SecureMode;
  extern int Ecart;
  extern int EcartAutomaticHomogenization;
  extern int TraceLift;
  extern int DoCancel;
  extern int DebugContentReduction;
  extern int QuoteMode;
  extern int RestrictedMode, RestrictedMode_saved;
  extern int UseDsmall;

  int n,i;
  struct object ob1 = OINIT;
  struct object ob2 = OINIT;
  struct object ob3 = OINIT;
  struct object ob4 = OINIT;
  struct object rob = NullObject;
  switch (getoaSize(ob)) {
  case 1:   /* get the value */
    ob1 = getoa(ob,0);
    switch(ob1.tag) {
    case Sdollar:
      if (strcmp(ob1.lc.str,"PrintDollar") == 0) {
        rob = KpoInteger(PrintDollar);
      }else if (strcmp(ob1.lc.str,"Wrap") == 0) {
        rob = KpoInteger(Wrap);
      }else if (strcmp(ob1.lc.str,"COutput") == 0) {
        rob = KpoInteger(COutput);
      }else if (strcmp(ob1.lc.str,"P") == 0) {
        rob = KpoInteger(CurrentRingp->p);
      }else if (strcmp(ob1.lc.str,"N") == 0) {
        rob = KpoInteger(CurrentRingp->n);
      }else if (strcmp(ob1.lc.str,"NN") == 0) {
        rob = KpoInteger(CurrentRingp->nn);
      }else if (strcmp(ob1.lc.str,"M") == 0) {
        rob = KpoInteger(CurrentRingp->m);
      }else if (strcmp(ob1.lc.str,"MM") == 0) {
        rob = KpoInteger(CurrentRingp->mm);
      }else if (strcmp(ob1.lc.str,"L") == 0) {
        rob = KpoInteger(CurrentRingp->l);
      }else if (strcmp(ob1.lc.str,"LL") == 0) {
        rob = KpoInteger(CurrentRingp->ll);
      }else if (strcmp(ob1.lc.str,"C") == 0) {
        rob = KpoInteger(CurrentRingp->c);
      }else if (strcmp(ob1.lc.str,"CC") == 0) {
        rob = KpoInteger(CurrentRingp->cc);
      }else if (strcmp(ob1.lc.str,"CurrentRingp") == 0) {
        rob = KpoRingp(CurrentRingp);
      }else if (strcmp(ob1.lc.str,"Verbose") == 0) {
        rob = KpoInteger(Verbose);
      }else if (strcmp(ob1.lc.str,"UseCriterion1") == 0) {
        rob = KpoInteger(UseCriterion1);
      }else if (strcmp(ob1.lc.str,"UseCriterion2B") == 0) {
        rob = KpoInteger(UseCriterion2B);
      }else if (strcmp(ob1.lc.str,"ReduceLowerTerms") == 0) {
        rob = KpoInteger(ReduceLowerTerms);
      }else if (strcmp(ob1.lc.str,"CheckHomogenization") == 0) {
        rob = KpoInteger(CheckHomogenization);
      }else if (strcmp(ob1.lc.str,"Homogenize") == 0) {
        rob = KpoInteger(Homogenize);
      }else if (strcmp(ob1.lc.str,"Statistics") == 0) {
        rob = KpoInteger(Statistics);
      }else if (strcmp(ob1.lc.str,"AutoReduce") == 0) {
        rob = KpoInteger(AutoReduce);
      }else if (strcmp(ob1.lc.str,"StackPointer") == 0) {
        rob = KpoInteger(Osp);
      }else if (strcmp(ob1.lc.str,"StandardOperandStack") == 0) {
        rob.tag = Sclass;
        rob.lc.ival = CLASSNAME_OPERANDSTACK;
        rob.rc.voidp = &StandardStack;
      }else if (strcmp(ob1.lc.str,"ErrorStack") == 0) {
        rob.tag = Sclass;
        rob.lc.ival = CLASSNAME_OPERANDSTACK;
        rob.rc.voidp = &ErrorStack;
      }else if (strcmp(ob1.lc.str,"ErrorMessageMode") == 0) {
        rob = KpoInteger(ErrorMessageMode);
      }else if (strcmp(ob1.lc.str,"WarningMessageMode") == 0) {
        rob = KpoInteger(WarningMessageMode);
      }else if (strcmp(ob1.lc.str,"CatchCtrlC") == 0) {
        rob = KpoInteger(CatchCtrlC);
        /* If you catch ctrlc in KSexecuteString. */
      }else if (strcmp(ob1.lc.str,"Strict") == 0) {
        rob = KpoInteger(Strict);
      }else if (strcmp(ob1.lc.str,"CurrentContextp") == 0) {
        rob.tag = Sclass;
        rob.lc.ival = CLASSNAME_CONTEXT;
        rob.rc.voidp = CurrentContextp;
      }else if (strcmp(ob1.lc.str,"PrimitiveContextp") == 0) {
        rob.tag = Sclass;
        rob.lc.ival = CLASSNAME_CONTEXT;
        rob.rc.voidp = PrimitiveContextp;
      }else if (strcmp(ob1.lc.str,"NullContextp") == 0) {
        rob.tag = Sclass;
        rob.lc.ival = CLASSNAME_CONTEXT;
        rob.rc.voidp = (struct context *)NULL;
      }else if (strcmp(ob1.lc.str,"Strict2") == 0) {
        rob = KpoInteger(Strict2);
      }else if (strcmp(ob1.lc.str,"SigIgn") == 0) {
        rob = KpoInteger(SigIgn);
      }else if (strcmp(ob1.lc.str,"KSPushEnvMode") == 0) {
        rob = KpoInteger(KSPushEnvMode);
      }else if (strcmp(ob1.lc.str,"KanGBmessage") == 0) {
        rob = KpoInteger(KanGBmessage);
      }else if (strcmp(ob1.lc.str,"TimerOn") == 0) {
        rob = KpoInteger(TimerOn);
      }else if (strcmp(ob1.lc.str,"orderMatrix") == 0) {
        rob = KgetOrderMatrixOfCurrentRing();
      }else if (strcmp(ob1.lc.str,"gbListTower") == 0) {
        if (CurrentRingp->gbListTower == NULL) rob = NullObject;
        else rob = *((struct object *)(CurrentRingp->gbListTower));
      }else if (strcmp(ob1.lc.str,"outputOrder") == 0) {
        n = CurrentRingp->n;
        ob1 = newObjectArray(n*2); 
        for (i=0; i<2*n; i++) {
          putoa(ob1,i,KpoInteger(CurrentRingp->outputOrder[i]));
        }
        rob = ob1;
      }else if (strcmp(ob1.lc.str,"multSymbol") == 0) {
        rob = KpoInteger(OutputStyle);
      }else if (strcmp(ob1.lc.str,"Sugar") == 0) {
        rob = KpoInteger(Sugar);
      }else if (strcmp(ob1.lc.str,"Homogenize_vec") == 0) {
        rob = KpoInteger(Homogenize_vec);
      }else if (strcmp(ob1.lc.str,"Schreyer")==0) {
        rob = KpoInteger( CurrentRingp->schreyer );
      }else if (strcmp(ob1.lc.str,"ringName")==0) {
        rob = KpoString( CurrentRingp->name );
      }else if (strcmp(ob1.lc.str,"CmoDMSOutputOption")==0) {
        rob = KpoInteger( CmoDMSOutputOption );
      }else if (strcmp(ob1.lc.str,"Version")==0) {
        rob = KpoString(VersionString);
      }else if (strcmp(ob1.lc.str,"RingStack")==0) {
        KsetUpRing(NullObject,NullObject,NullObject,NullObject,NullObject);
        rob = KSpop(); /* This is exceptional style */
      }else if (strcmp(ob1.lc.str,"AvoidTheSameRing")==0) {
        rob = KpoInteger(AvoidTheSameRing);
      }else if (strcmp(ob1.lc.str,"LeftBracket")==0) {
        rob = KpoString(LeftBracket);
      }else if (strcmp(ob1.lc.str,"RightBracket")==0) {
        rob = KpoString(RightBracket);
      }else if (strcmp(ob1.lc.str,"SecureMode")==0) {
        rob = KpoInteger(SecureMode);
      }else if (strcmp(ob1.lc.str,"RestrictedMode")==0) {
        rob = KpoInteger(RestrictedMode);
      }else if (strcmp(ob1.lc.str,"Ecart")==0) {
        rob = KpoInteger(Ecart);
      }else if (strcmp(ob1.lc.str,"EcartAutomaticHomogenization")==0) {
        rob = KpoInteger(EcartAutomaticHomogenization);
      }else if (strcmp(ob1.lc.str,"TraceLift")==0) {
        rob = KpoInteger(TraceLift);
      }else if (strcmp(ob1.lc.str,"DoCancel")==0) {
        rob = KpoInteger(DoCancel);
      }else if (strcmp(ob1.lc.str,"DebugContentReduction")==0) {
        rob = KpoInteger(DebugContentReduction);
      }else if (strcmp(ob1.lc.str,"QuoteMode")==0) {
        rob = KpoInteger(QuoteMode);
      }else if (strcmp(ob1.lc.str,"UseDsmall")==0) {
        rob = KpoInteger(UseDsmall);
      }else{
        warningKan("KsystemVariable():Unknown key word.\n");
      }
      break;
    default: 
      warningKan("KsystemVariable():Invalid argument\n");
      break;
    }
    break;
  case 2: /* set value */
    ob1 = getoa(ob,0);
    ob2 = getoa(ob,1);
    switch (Lookup[ob1.tag][ob2.tag]) {
    case SdollarSinteger:
      if (strcmp(ob1.lc.str,"PrintDollar") == 0) {
        PrintDollar = ob2.lc.ival;
        rob = KpoInteger(PrintDollar);
      }else if (strcmp(ob1.lc.str,"Wrap") == 0) {
        Wrap = ob2.lc.ival;
        rob = KpoInteger(Wrap);
        /*}else if (strcmp(ob1.lc.str,"P") == 0) {
          P = ob2.lc.ival;  Q should be set here too. 
          CurrentRingp->p = P;
          rob = KpoInteger(P); */
      }else if (strcmp(ob1.lc.str,"COutput") == 0) {
        COutput = ob2.lc.ival;
        rob = KpoInteger(COutput);
      }else if (strcmp(ob1.lc.str,"NN") == 0) {
        if (ob2.lc.ival <= CurrentRingp->n && ob2.lc.ival >= CurrentRingp->m) {
          CurrentRingp->nn = ob2.lc.ival;
        }else{
          warningKan("New value of NN is out of bound.");
        }
        rob = KpoInteger(ob1.lc.ival);
      }else if (strcmp(ob1.lc.str,"Verbose") == 0) {
        Verbose = ob2.lc.ival;
        rob = KpoInteger(Verbose);
      }else if (strcmp(ob1.lc.str,"UseCriterion1") == 0) {
        UseCriterion1 = ob2.lc.ival;
        rob = KpoInteger(UseCriterion1);
      }else if (strcmp(ob1.lc.str,"UseCriterion2B") == 0) {
        UseCriterion2B = ob2.lc.ival;
        rob = KpoInteger(UseCriterion2B);
      }else if (strcmp(ob1.lc.str,"ReduceLowerTerms") == 0) {
        ReduceLowerTerms = ob2.lc.ival;
        rob = KpoInteger(ReduceLowerTerms);
      }else if (strcmp(ob1.lc.str,"CheckHomogenization") == 0) {
        CheckHomogenization = ob2.lc.ival;
        rob = KpoInteger(CheckHomogenization);
      }else if (strcmp(ob1.lc.str,"Homogenize") == 0) {
        Homogenize = ob2.lc.ival;
        rob = KpoInteger(Homogenize);
      }else if (strcmp(ob1.lc.str,"Statistics") == 0) {
        Statistics = ob2.lc.ival;
        rob = KpoInteger(Statistics);
      }else if (strcmp(ob1.lc.str,"AutoReduce") == 0) {
        AutoReduce = ob2.lc.ival;
        rob = KpoInteger(AutoReduce);
      }else if (strcmp(ob1.lc.str,"ErrorMessageMode") == 0) {
        ErrorMessageMode = ob2.lc.ival;
        rob = KpoInteger(ErrorMessageMode);
      }else if (strcmp(ob1.lc.str,"WarningMessageMode") == 0) {
        WarningMessageMode = ob2.lc.ival;
        rob = KpoInteger(WarningMessageMode);
      }else if (strcmp(ob1.lc.str,"CatchCtrlC") == 0) {
        CatchCtrlC = ob2.lc.ival;
        rob = KpoInteger(CatchCtrlC);
      }else if (strcmp(ob1.lc.str,"Strict") == 0) {
        Strict = ob2.lc.ival;
        rob = KpoInteger(Strict);
      }else if (strcmp(ob1.lc.str,"Strict2") == 0) {
        Strict2 = ob2.lc.ival;
        rob = KpoInteger(Strict2);
      }else if (strcmp(ob1.lc.str,"SigIgn") == 0) {
        SigIgn = ob2.lc.ival;
        if (SigIgn) mysignal(SIGINT,SIG_IGN);
        else mysignal(SIGINT,ctrlC);
        rob = KpoInteger(SigIgn);
      }else if (strcmp(ob1.lc.str,"KSPushEnvMode") == 0) {
        KSPushEnvMode = ob2.lc.ival;
        rob = KpoInteger(KSPushEnvMode);
      }else if (strcmp(ob1.lc.str,"KanGBmessage") == 0) {
        KanGBmessage = ob2.lc.ival;
        rob = KpoInteger(KanGBmessage);
      }else if (strcmp(ob1.lc.str,"TimerOn") == 0) {
        TimerOn = ob2.lc.ival;
        rob = KpoInteger(TimerOn);
      }else if (strcmp(ob1.lc.str,"multSymbol") == 0) {
        OutputStyle = KopInteger(ob2);
        rob = KpoInteger(OutputStyle);
      }else if (strcmp(ob1.lc.str,"Sugar") == 0) {
        Sugar = KopInteger(ob2);
        if (Sugar && ReduceLowerTerms) {
          ReduceLowerTerms = 0;
          warningKan("ReduceLowerTerms is automatically set to 0, because Sugar = 1.");
          /* You cannot use both ReduceLowerTerms and sugar.
             See gb.c, reduction_sugar. */
        }
        rob = KpoInteger(Sugar);
      }else if (strcmp(ob1.lc.str,"Homogenize_vec") == 0) {
        Homogenize_vec = KopInteger(ob2);
        rob = KpoInteger(Homogenize_vec);
      }else if (strcmp(ob1.lc.str,"CmoDMSOutputOption") == 0) {
        CmoDMSOutputOption = KopInteger(ob2);
        rob = KpoInteger(CmoDMSOutputOption);
      }else if (strcmp(ob1.lc.str,"DebugReductionRed") == 0) {
        DebugReductionRed = KopInteger(ob2);
        rob = KpoInteger(DebugReductionRed);
      }else if (strcmp(ob1.lc.str,"DebugReductionEcart") == 0) {
        DebugReductionEcart = KopInteger(ob2);
        rob = KpoInteger(DebugReductionEcart);
      }else if (strcmp(ob1.lc.str,"AvoidTheSameRing") == 0) {
        AvoidTheSameRing = KopInteger(ob2);
        rob = KpoInteger(AvoidTheSameRing);
      }else if (strcmp(ob1.lc.str,"SecureMode") == 0) {
        if (KopInteger(ob2) >= SecureMode) {
          SecureMode = KopInteger(ob2);
        }else{
          errorKan1("%s\n","You cannot weaken the security level.");
        }
        rob = KpoInteger(SecureMode);
      }else if (strcmp(ob1.lc.str,"RestrictedMode") == 0) {
        if (KopInteger(ob2) >= RestrictedMode) {
          RestrictedMode = KopInteger(ob2);
          RestrictedMode_saved = RestrictedMode;
        }else{
          errorKan1("%s\n","You cannot weaken the RestrictedMode level.");
        }
        rob = KpoInteger(RestrictedMode);
      }else if (strcmp(ob1.lc.str,"Ecart") == 0) {
        Ecart = KopInteger(ob2);
        rob = KpoInteger(Ecart);
      }else if (strcmp(ob1.lc.str,"EcartAutomaticHomogenization") == 0) {
        EcartAutomaticHomogenization = KopInteger(ob2);
        rob = KpoInteger(EcartAutomaticHomogenization);
      }else if (strcmp(ob1.lc.str,"TraceLift") == 0) {
        TraceLift = KopInteger(ob2);
        rob = KpoInteger(TraceLift);
      }else if (strcmp(ob1.lc.str,"DoCancel") == 0) {
        DoCancel = KopInteger(ob2);
        rob = KpoInteger(DoCancel);
      }else if (strcmp(ob1.lc.str,"DebugContentReduction") == 0) {
        DebugContentReduction = KopInteger(ob2);
        rob = KpoInteger(DebugContentReduction);
      }else if (strcmp(ob1.lc.str,"QuoteMode") == 0) {
        QuoteMode = KopInteger(ob2);
        rob = KpoInteger(QuoteMode);
      }else if (strcmp(ob1.lc.str,"UseDsmall") == 0) {
        UseDsmall = KopInteger(ob2);
        rob = KpoInteger(UseDsmall);
      }else{
        warningKan("KsystemVariable():Unknown key word.\n");
      }
      break;
    case SdollarSdollar:
      if (strcmp(ob1.lc.str,"ringName") == 0) {
        CurrentRingp->name = KopString(ob2);
        rob = KpoString(CurrentRingp->name);
      }else if (strcmp(ob1.lc.str,"LeftBracket") == 0) {
        LeftBracket = KopString(ob2);
        rob = KpoString(LeftBracket);
      }else if (strcmp(ob1.lc.str,"RightBracket") == 0) {
        RightBracket = KopString(ob2);
        rob = KpoString(RightBracket);
      }else{
        warningKan("KsystemVariable():Unknown key word.\n");
      }
      break;
    case SdollarSring:
      if (strcmp(ob1.lc.str,"CurrentRingp") == 0) {
        CurrentRingp = ob2.lc.ringp;
	if (CurrentRingp->module_rank) {
	  if (Verbose & 0x10000) warningKan("mmLarger is changed to module_matrix automatically by ring_def.\n");
	  KswitchFunction(KpoString("mmLarger"),KpoString("module_matrix"));
	}
        rob = KpoRingp(CurrentRingp);
      }else{
        warningKan("KsystemVariable():Unknown key word.\n");
      }
      break;
    case SdollarSclass:
      if (strcmp(ob1.lc.str,"PrimitiveContextp") == 0) {
        if (ectag(ob2) == CLASSNAME_CONTEXT) {
          PrimitiveContextp = (struct context *)ob2.rc.voidp;
          rob = ob2;
        }else{
          warningKan("The second argument must be class.context.\n");
          rob = NullObject;
        }
      }else {
        warningKan("KsystemVariable():Unknown key word.\n");
      }
      break;
    case SdollarSlist:
      if (strcmp(ob1.lc.str,"gbListTower") == 0) {
        if (AvoidTheSameRing)
          warningKan("Changing gbListTower may cause a trouble under AvoidTheSameRing == 1.");
        CurrentRingp->gbListTower = newObject();
        *((struct object *)(CurrentRingp->gbListTower)) = ob2;
        rob = *((struct object *)(CurrentRingp->gbListTower));
      }else {
        warningKan("KsystemVariable(): Unknown key word to set value.\n");
      }
      break;
    case SdollarSarray:
      if (strcmp(ob1.lc.str,"outputOrder") == 0) {
        rob = KsetOutputOrder(ob2,CurrentRingp);
      }else if (strcmp(ob1.lc.str,"variableNames") == 0) {
        rob = KsetVariableNames(ob2,CurrentRingp);
      }else {
        warningKan("KsystemVariable(): Unknown key word to set value.\n");
      }
      break;
    default:
      warningKan("KsystemVariable():Invalid argument.\n");
    }
    break;
  case 3:
    ob1 = getoa(ob,0); ob2 = getoa(ob,1); ob3 = getoa(ob,2);
    switch(Lookup[ob1.tag][ob2.tag]) {
    case SdollarSdollar:
      if (strcmp(ob2.lc.str,"var") == 0) {
        if (strcmp(ob1.lc.str,"x")==0) {
          if (ob3.tag != Sinteger) {
            warningKan("[$x$ $var$ ? ] The 3rd argument must be integer.");
            break;
          }
          if (ob3.lc.ival >= 0 && ob3.lc.ival < CurrentRingp->n) {
            rob = KpoString(CurrentRingp->x[ob3.lc.ival]);
          }else{
            warningKan("[$x$ $var$ ? ] The 3rd argument is out of range.");
            break;
          }
        }else if (strcmp(ob1.lc.str,"D")==0) {
          if (ob3.tag != Sinteger) {
            warningKan("[$D$ $var$ ? ] The 3rd argument must be integer.");
            break;
          }
          if (ob3.lc.ival >= 0 && ob3.lc.ival < CurrentRingp->n) {
            rob = KpoString(CurrentRingp->D[ob3.lc.ival]);
          }else{
            warningKan("[$D$ $var$ ? ] The 3rd argument is out of range.");
            break;
          }
        }
      }else{
        warningKan("KsystemVariable(): Invalid argument.\n");
      }
      break;
    default:
      warningKan("KsystemVariable(): Invalid argument.\n");
      break;
    }
    break;
  default:
    warningKan("KsystemVariable():Invalid argument.\n");
    break;
  }
  return(rob);
}

void warningOption(str)
     char *str;
{
  fprintf(stderr,"Warning(option.c): %s\n",str);
}
