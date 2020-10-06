/* global.c $OpenXM: OpenXM/src/kan96xx/Kan/global.c,v 1.37 2015/09/29 01:52:14 takayama Exp $ */
#include <stdio.h>
#include <setjmp.h>
#include "datatype.h"
#include "stackm.h"
#include "lookup.h"

void hashInitialize(struct dictionary *dic);  /* declared in extern.h */
void initClassDataBase(void); 
/*-------- variables to define  a ring ----------------*/
/*
  The numbers L,M, LL,MM,NN are set in setUpRing() in setupring.c now.

  Be careful when you do not use one of three type variables.
  Any setting must keep the relation: L<=M<=N.
  You must see mmwMult() in poly.c in the above case.
  Example. L=M=0 means there is no q-variables and difference variables.
  L=5, M=N=5 means all variables are q-variables (x0=q).
  L=2, M=2, N=5 means x0=q, x1 is q-variable and x2,,,,x4 are
  differential variables.
  L=0, M=2, N=5 means x0,x1 are difference variables and x2,,,x4
  are differential variables. */
/*#define   L     0     number of q-variables */
/*#define   M     0     number of difference variables */
/*Number of elimination variables  */
/*#define  LL     0     */
/*#define  MM     0    */
/* x_MM,...,x_{M-1} are eliminated difference variables */
/* MM < M if you want to use it. MM>=M if you do not use it */
/*#define  NN     9  */
/*x_NN,...,x_{N-1} are eliminated differential variables */
/* NN < N if you want to use it. NN>=N if you do not use it.
               See the eliminated_standard() in dbm3.c */


/* They are not used.
int N = 6;
int L = 1;
int M = 1;
int C = 1;
int NN = 5;
int MM = 1;
int LL = 1;
int CC = 1;
*/


/**********   These variables are set in setUpRing() ***********************/
int Md =  0;   /* if Md=1, then x_NN, ..., x_{N-1} are eliminated variables*/
int Med = 0;   /* if Med=1, then x_MM, ..., x_{M-1} are eliminated variables*/
int Mq  = 0;   /* if Mq=1, then x_LL, ..., x_{L-1} are eliminated variables*/

char **TransX; /* name of variables */
char **TransD;
int *Order;  /* order matrix */

int Msize = 0;
/*int P = 32609; */ /* characteristic */
/* ------------------ end of variables for the ring -------------- */

struct ring SmallRing;
struct ring *SmallRingp = &SmallRing;

struct ring *CurrentRingp = (struct ring *)0;
  
struct ring *SyzRingp = (struct ring *)0;

MP_INT *Mp_one;
MP_INT *Mp_zero;
MP_INT Mp_work_iiComb;
MP_INT Mp_work_iiPower;
MP_INT Mp_work_iiPoch;

struct coeff *UniversalZero;
struct coeff *UniversalOne;

/*******************************************************************/
int History = 1;   /* get a transformation matrix or not */

int Commutative = 0;
int UseCriterion1 = 0;
int UseCriterion2B = 0;
int OutputStyle = '*';
int COutput = 0; /* C sytle output of polynomial */
int Wrap = 0;  /* output.c */
int Verbose = 0;
int ReduceLowerTerms = 1;
int Criterion2B = 0;
int Criterion2M = 0;
int Criterion2F = 0;
int Statistics = 0;
int AutoReduce = 0;
int CheckHomogenization = 1;
int ErrorMessageMode = 0;
int WarningMessageMode = 0;
int CatchCtrlC = 0;
int Strict = 0;   /* It is used warning. */
int Strict2 = 0;  /* It is used in putDictionary */
extern struct dictionary UserDictionary[];
int SigIgn = 0;
int StopDegree = 0x7fff;
int KSPushEnvMode = 0;
int Sugar = 0;
int Homogenize_vec = 1; /*x + e (x+1)===>(1) x h + e (x+h) or (0)x + e(x+h) */
int CmoDMSOutputOption = 0;  /* Output with
                 (1) RING_BY_NAME or (0) DMS_OF_N_VARIABLES.
                 plugin/cmo.c */
int SecureMode = 0;
int RestrictedMode = 0;
int RestrictedMode_saved = 0;
int Ecart = 0;
int EcartAutomaticHomogenization = 0;
int TraceLift = 0;
struct ring *TraceLift_ringmod = NULL;
int DoCancel = 0;
int QuoteMode = 0;
int UseDsmall = 0;

/* global variables for stackmachine.c  */
int VerboseStack = 1;     /* 0 is quiet, 1 is standard, 2 ... */
int DebugStack   = 0;

FILE *Fstack = NULL; /* Initialized to standard output stream 
                        in stackmachine_init()
                        for module: stackmachine */

#if defined(sun)
/* in case of Solaris, use the following: */
int EnvOfStackMachine[2000];
#else
#if defined(__CYGIWN__)
sigjmp_buf EnvOfStackMachine;
#else
jmp_buf EnvOfStackMachine;
#endif
#endif

struct object NullObject;
struct object NoObject;

int Lookup[TYPES][TYPES];

int Quiet = 0;
int TimerOn = 0;

char *VersionString = "3.050615";

char *LeftBracket = NULL;
char *RightBracket = NULL;

int AvoidTheSameRing = 1;
int DebugCMO = 0;  /* plugin/cmo.c */
int OXprintMessage = 1; /* referred only from ox_sm1  plugin/ox.hh */

char *OxSystem = "ox_sm1.plain";         /* Example :  ox_sm1.plain */
char *OxSystemVersion = NULL;            /* Example :  0.1 */

/*  global variables for kanExport0.c */
int VerboseK = 1;  /* 1 is standard */
int DebugK   = 0;
FILE *Fk = NULL;  /* Initialized to stdout in stackmachine_init() */

void stackmachine_init() {
  int i,j;
  extern FILE *BaseFp;
  /* GC_init(); */
  GC_INIT();
  OxSystemVersion = VersionString;
  Fstack = stdout;  /* initialize output stream */
  Fk = stdout;
  BaseFp = stdin;
  LeftBracket = "["; RightBracket = "]";
  /* initialize null object */
  NullObject.tag = 0;
  NullObject.lc.op = NullObject.rc.op = (struct object *)NULL;
  NullObject.attr = (struct object *)NULL;
  /* findUserdictionary returns NoObject if it cannot find the key.*/
  NoObject.tag = -1;
  NoObject.lc.op = NoObject.rc.op = (struct object *)NULL;
  NoObject.attr = (struct object *)NULL;
  /* Initalize lookup table */
  for (i=0; i<TYPES; i++) {
    for (j=0; j<TYPES; j++)
      Lookup[i][j] = i*TYPES + j; /* Refer to mklookup.c */
  }
  /* Initialize hashing */
  hashInitialize(UserDictionary);

  initClassDataBase();

}


int stackmachine_close() {
  /* close output stream */
}



void Kclose() {
  /* close Fk */
}




  


