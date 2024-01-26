/* $OpenXM: OpenXM/src/kan96xx/Kan/extern.h,v 1.35 2006/02/01 00:30:05 takayama Exp $ */
/*********** global variables for module stackmachine***************/
extern int VerboseStack;
extern int DebugStack;
extern FILE *Fstack;
#if defined(__CYGWIN__)
extern sigjmp_buf EnvOfStackMachine;
#else
extern jmp_buf EnvOfStackMachine;
#endif
extern struct object NullObject;
extern struct object NoObject;
extern int Lookup[TYPES][TYPES];


/* scanner.c */
/*struct tokens getokenSM(actionType kind,char *str);
struct tokens *decomposeToTokens(char *str,int *size);*/
struct tokens getokenSM();
struct tokens *decomposeToTokens();
char *getLOAD_SM1_PATH();
char *traceShowScannerBuf();

/* stackmachine.c  object */
struct object *newObject(void);
struct object newObjectArray(int size);
int isNullObject(struct object obj);

/* stackmachine.c  dictionary */
int putSystemDictionary(char *str,struct object ob);
int findSystemDictionary(char *str);
int putUserDictionary(char *str,int h0,int h1,struct object ob,
		      struct dictionary *dic);
struct object KputUserDictionary(char *str,struct object ob);
struct object findUserDictionary(char *str,int h0,int h1,
				 struct context *cp);
struct object KfindUserDictionary(char *str);
int putPrimitiveFuction(char *str,int number);
int hash0(char *s);
int hash1(char *s);
void hashInitialize(struct dictionary *dic);
struct tokens lookupTokens(struct tokens t);
struct object lookupLiteralString(char *s);
int putUserDictionary2(char *str,int h0,int h1,int attr,
		       struct dictionary *dic);
void KdefinePrimitiveFunctions(void);
struct object showSystemDictionary(int f);
int showUserDictionary(void) ;


/* stackmachine.c  stack */
struct object Kpop(void); /* Kpush and Kpop may be used only in stackmachine.c
			     and primitive.c */
int Kpush(struct object ob);
struct object peek(int k);
void printOperandStack(void);
struct object newOperandStack(int size);
void setOperandStack(struct object ob);
void stdOperandStack(void);

/* stackmachine.c  printing */
void printObject(struct object ob,int nl,FILE *fp) ;
void printObjectArray(struct object ob,int nl,FILE *fp);

/* stackmachine.c  misc*/
void scanner(void);
void ctrlC(int sig);
void cancelAlarm();
int executeToken(struct tokens token);
int executePrimitive(struct object ob);
int isLiteral(char *str);
void lockCtrlCForOx();
void unlockCtrlCForOx();
void restoreCtrlCForOx();
int executeExecutableArray(struct object ob,char *fname,int withGotoP);

/* stackmachine.c  export */
int KSdefineMacros(void);
struct object KSpop(void);
void KSpush(struct object ob);
char * KSstringPop(void);
int KSstackPointer(void);
struct object KSdupErrors(void);
struct object KSpeek(int k);
struct object KSnewObjectArray(int k);

/* stackmachine.c context */
struct context *newContext0(struct context *super,char *name);
struct object KnewContext(struct object superobj, char *name);
void fprintContext(FILE *fp,struct context *c);
void KsetContext(struct object contextobj);
void contextControl(actionOfContextControl ctl);
struct object getSuperContext(struct object contextObj) ;

/* stackmachine.c back-trace */
void tracePushName(char *s);
void traceClearStack(void);
char *tracePopName(void);
char *traceShowStack(void); 
struct object *traceNameStackToArrayp(void);

/* kclass.c */
void initClassDataBase();
void fprintClass(FILE *fp,struct object ob);
int KclassEqualQ(struct object ob1,struct object ob2);
struct object KcreateClassIncetance(struct object ob1,
				    struct object ob2,
				    struct object ob3);
struct object KclassDataConversion(struct object ob1,struct object ob2);
/* dataConversion is also used for creating new object. */
/* Arithmetic functions. */
struct object Kclass_ooAdd(struct object ob1,struct object ob2);

/* ErrorStack */
int pushErrorStack(struct object obj);
struct object popErrorStack(void);
int KScheckErrorStack(void);
struct object KnewErrorPacket(int serial,int no,char *message);
struct object KnewErrorPacketObj(struct object obj);

/** usage.c */
void Kusage(struct object ob);
void Kusage2(FILE *fp,char *s);

/*  list.c */
struct object *newList(struct object *op);
int klength(struct object *op);
struct object *vJoin(struct object *list1,struct object *list2);
struct object car(struct object *list);
struct object *cdr(struct object *list);
void printObjectList(struct object *op);
int memberQ(struct object *list1,struct object list2);

struct object KvJoin(struct object listo1,struct object listo2);
struct object Kcar(struct object listo);
struct object Kcdr(struct object listo);
struct object KlistToArray(struct object listo);
struct object KarrayToList(struct object ob);

/* list:conversion */
struct object listToArray(struct object *op);
struct object *arrayToList(struct object ob);
struct object newByteArray(int size,struct object obj);
struct object newByteArrayFromStr(char *s,int size);
struct object byteArrayToArray(struct object obj);

/* kanExport0.c */
/** arithmetic **/
struct object KooAdd(struct object ob1, struct object ob2);
struct object KooSub(struct object ob1, struct object ob2);
struct object KooMult(struct object ob1,struct object ob2);
struct object KooDiv(struct object ob1,struct object ob2);
struct object KaoMult(struct object aa,struct object bb);
struct object KooDiv2(struct object ob1,struct object ob2);
struct object KoNegate(struct object ob1);
struct object KreduceRationalFunction(struct object ob);
struct object KooPower(struct object ob1,struct object ob2);

/* for rational function object */
struct object KoInverse(struct object ob);
int KisInvalidRational(objectp ob);
objectp copyObjectp(objectp op);
struct object KnewRationalFunction0(objectp op1, objectp op2);
int KisZeroObject(objectp ob);
int KisOneObject(objectp ob);
int KisMinusOneObject(objectp ob);


/** relation */
struct object KoIsPositive(struct object ob);
int KooEqualQ(struct object o1,struct object o2);
struct object KooGreater(struct object o1,struct object o2);
struct object KooLess(struct object o1,struct object o2);


/** Conversion */
struct object KdataConversion(struct object ob1, char *key);
struct object Kto_int32(struct object ob);
struct object KpoInteger(int i);
struct object KpoString(char *s);
struct object KpoPOLY(POLY f);
struct object KpoArrayOfPOLY(struct arrayOfPOLY *ap);
struct object KpoMatrixOfPOLY(struct matrixOfPOLY *mp);
struct object KpoRingp(struct ring *ringp);
struct object KpoDouble(double a);
struct object KpoUniversalNumber(struct coeff *u);
struct object KintToUniversalNumber(int n);
#define KopInteger(o) ( (o).lc.ival )
#define KopString(o)  ( (o).lc.str )
#define KopPOLY(o)    ( (o).lc.poly )
#define KopArrayOfPOLY(o) ( *((o).lc.arrayp) )
#define KopMatrixOfPOLY(o) ( *((o).lc.matrixp))
#define KopArrayOfPOLYp(o) ( (o).lc.arrayp )
#define KopMatrixOfPOLYp(o) ( (o).lc.matrixp)
#define KopRingp(o)  ( (o).lc.ringp )
#define KopUniversalNumber(o)  ( (o).lc.universalNumber )
#define Knumerator(o)  ( (o).lc.op )
#define Kdenominator(o) ( (o).rc.op )
#define KopDouble(ob)  (*((ob).lc.dbl))
#define KopByteArray(o) ( (o).lc.bytes )
#define getByteArraySize(o) ( (o).rc.ival )
char *KPOLYToString(POLY f);
/** conversion 2 **/
struct object arrayOfPOLYToArray(struct arrayOfPOLY *aa);
struct object matrixOfPOLYToArray(struct matrixOfPOLY *pmat);
struct arrayOfPOLY *arrayToArrayOfPOLY(struct object oa);
struct matrixOfPOLY *arrayToMatrixOfPOLY(struct object oa);
double toDouble0(struct object ob);

/** :ring :kan **/
int objArrayToOrderMatrix(struct object oA,int order[],int n,int oasize);
int KsetOrderByObjArray(struct object oA);
struct object oGetOrderMatrix(struct ring *ringp); /* order.c */
struct object KgetOrderMatrixOfCurrentRing();
struct object oRingToOXringStructure(struct ring *ringp); /* order.c */
int KsetUpRing(struct object ob1,struct object ob2, struct object ob3,struct object ob4,struct object ob5);
void KshowRing(struct ring *ringp);
struct object KdefaultPolyRing(struct object num);
struct object KswitchFunction(struct object ob1,struct object ob2);
void KprintSwitchStatus(void);
struct object  KoReplace(struct object ob,struct object rule);
struct object Kparts(struct object f,struct object v);
struct object Kparts2(struct object f,struct object v);
struct object parts2(POLY f,POLY g);
struct object Kdegree(struct object f,struct object v);
struct object KringMap(struct object ob);
struct object Ksp(struct object ob1,struct object ob2);
struct object Khead(struct object ob);
int KpolyLength(POLY f);
struct object KsetOutputOrder(struct object obj,struct ring *rp);
struct object KsetVariableNames(struct object obj,struct ring *rp);
/* KgetRingInfo has not yet been implemented. */
struct object KgetRingInfo(struct object ringObj, struct object ofunc);
struct object KaddModuleOrder(struct object ob);

/* :eval */
struct object Keval(struct object obj);

/** misc **/
struct object KtoRecords(struct object ob1);
struct object KstringToArgv(struct object ob1);
struct object KstringToArgv2(struct object ob1,struct object ob2);
void Kclose(void);
int warningKan(char *s);
int warningKanNoStrictMode(char *s);
int errorKan1(char *s,char *m);
struct object test(struct object ob);
struct object Kextension(struct object ob);
struct object KgbExtension(struct object ob);
struct object KmpzExtension(struct object ob);
struct object Krest(struct object ob);
struct object Kjoin(struct object ob1, struct object ob2);
struct object Kget(struct object ob1, struct object ob2);

/** Utilities */
char *KremoveSpace(char *s);
int KtoArgvbyCurryBrace(char *str,char *argv[],int limit);

/** Attribute */
struct object KgetAttributeList(struct object ob);
struct object  KsetAttributeList(struct object ob,struct object attr);
struct object KgetAttribute(struct object ob,struct object key);
struct object KsetAttribute(struct object ob,struct object key,struct object value); 
/*  ob (key) (value) setAttribute /ob set. They are not destructive. */



/** kanExport1.c */
/* :ring :kan */
struct object Kreduction(struct object f,struct object set);
struct object Kgroebner(struct object ob);

/* :conversion */
struct object gradedPolySetToGradedArray(struct gradedPolySet *gps,
					 int keepRedundant);
struct object polySetToArray(struct polySet *ps,int keepRedundant);
struct object gradedPolySetToArray(struct gradedPolySet *g,int keepRedundant);
struct gradedPolySet *arrayToGradedPolySet(struct object ob);
struct object syzPolyToArray(int size,POLY f,struct gradedPolySet *grG);
struct object getBackwardArray(struct gradedPolySet *grG);
POLY arrayToPOLY(struct object ob);
struct object POLYToArray(POLY f);
struct object oPrincipalPart(struct object ob);
struct object oInitW(struct object ob,struct object oWeight);
struct object KordWsAll(struct object ob,struct object oWeight);

/* :misc  */
struct object homogenizeObject(struct object ob,int *gradep);
struct object homogenizeObject_vec(struct object ob,int *gradep);
struct object homogenizeObject_go(struct object ob,int *gradep);
void KresetDegreeShift(void);
int oGrade(struct object ob);
struct ring *oRingp(struct object ob);
struct object KisOrdered(struct object of);
struct object KvectorToSchreyer_es(struct object obarray);
POLY objArrayToPOLY(struct object ob);
struct object POLYtoObjArray(POLY f,int size);
struct object KgetExponents(struct object obPoly,struct object type);

/* hilbert.c */
struct object hilberto(struct object obgb,struct object obvlist);

/* option.c */
struct object KsystemVariable(struct object ob);

void KasirKanExecute0(char *s);

struct object KbinaryToObject(int size, char *data);
char *KobjectToBinary(struct object ob,int *size);

int mmLarger_tower3(POLY ff,POLY gg,struct object *gbList); /* order.c */
struct object KschreyerSkelton(struct object g);  

/* misc */
struct object KregionMatches(struct object sobj, struct object keyArray);
int objToInteger(struct object ob);

/* shell.c */
struct object KoxWhich(struct object cmdo,struct object patho);
struct object KoxShell(struct object ob);
void KoxShellHelp(char *key,FILE *fp);

/* ---- */
void KinitKan(void);
int stackmachine_close(void);
void stackmachine_init(void);
int memberQ(struct object *list1,struct object obj2);
int warningStackmachine(char *str);
void errorStackmachine(char *str);
int putPrimitiveFunction(char *str,int number);

