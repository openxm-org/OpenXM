

/* kclass.h  1997, 3/1
   This module handles class data base.
   Extended class names.
*/

/* Format of extended class
   Sclass
   ob.lc.ival :  extended tag
   ob.rc.voidp : body
*/
/* The next macro  Extended Class tag returns -1 if the object
   is not in Sclass and returns extended class tags if the object
   is in Sclass.
   */
#define ectag(ob) ((ob).tag != Sclass? -1:(ob).lc.ival)
#define ecbody(ob) ((ob).rc.voidp)
/* Extended Class tags */
#define CLASSNAME_OPERANDSTACK 256
#define CLASSNAME_ERROR_PACKET 257
#define CLASSNAME_CONTEXT 258
#define CLASSNAME_GradedPolySet 259
#define CLASSNAME_mathcap  260
#define CLASSNAME_sampleClass   261

#define CLASSNAME_CMO     CMO    /* 1024 defined in stackm.h and plugin.h */
#define CLASSNAME_indeterminate         262
#define CLASSNAME_tree                  263
#define CLASSNAME_recursivePolynomial   264
#define CLASSNAME_polynomialInOneVariable   265


/*    */
#define  CLASS_NOT_USED  0
#define  CLASS_INTERNAL  1
#define  CLASS_OBJ     2

/* Conversion functions */
struct object KpoOperandStack(struct operandStack * op);
struct object KpoErrorPacket(struct object *op);
struct object KpoContext(struct context *ct);
struct object KpoGradedPolySet(struct gradedPolySet *grG);
struct object KpoMathCap(struct object *op);
struct object KpoCMOClass(struct object *op);
struct object KpoIndeterminate(struct object op);
struct object KpoTree(struct object op);
struct object KpoRecursivePolynomial(struct object op);
struct object KpoPolynomialInOneVariable(struct object op);
struct object KpoSampleClass(struct object *op);

/* Other constructors */
struct object KnewIndeterminate(char *s);

#define KopOperandStack(ob) ((struct operandStack *)(ob).rc.voidp)
#define KopErrorPacket(ob) ((struct object *)(ob).rc.voidp)
#define KopContext(ob) ((struct context *)(ob).rc.voidp)
#define KopGradedPolySet(ob) ((struct gradedPolySet *)(ob).rc.voidp)
#define KopMathCap(ob) ((struct object *)(ob).rc.voidp)
#define KopIndeterminate(ob) (*((struct object *)(ob).rc.voidp))
#define KopTree(ob) (*((struct object *)(ob).rc.voidp))
#define KopRecursivePolynomial(ob) (*((struct object *)(ob).rc.voidp))
#define KopPolynomialInOneVariable(ob) (*((struct object *)(ob).rc.voidp))
#define KopSampleClass(ob) ((struct object *)(ob).rc.voidp)


void fprintErrorPacket(FILE *fp, struct object *op);
void fprintMathCap(FILE *fp, struct object *op);

/* Functions in Kclass/sample.c */
void fprintSampleClass(FILE *fp,struct object *op);
int eqSampleClass(struct object *ob1, struct object *ob2);

/* Functions in Kclass/rawcmo.c */
void fprintCMOClass(FILE *fp,struct object op);

/* Functions in Kclass/indeterminate.c */
void fprintInderminate(FILE *fp,struct object ob);
int eqIndetermiante(struct object *ob1, struct object *ob2);
struct object addTree(struct object ob1,struct object ob2);
void fprintTree(FILE *fp,struct object ob);
int isTreeAdd(struct object ob);

void fprintRecursivePolynomial(FILE *fp,struct object ob);
void fprintPolynomialInOneVariable(FILE *fp,struct object ob);
struct object polyToRecursivePoly(struct object p);
struct object polyToRecursivePoly2(struct object p,struct object vlist,int vn);
struct object recursivePolyToPoly(struct object rp);
struct object coeffToObject(struct coeff *cp);
int isRecursivePolynomial2(struct object ob);






