/* $OpenXM$ */
union cell {
  int ival;
  char *str;
  struct Object *op;
  FILE *file;
  struct stringBuf *sbuf;
  void *voidp;
};

struct Object{
  int tag;                /* class identifier */
  union cell lc;          /* left cell */
  union cell rc;          /* right cell */
  struct object *attr;
};

struct object{         /* must be compatible with stackm.h */
  int tag;                /* class identifier */
  union cell lc;          /* left cell */
  union cell rc;          /* right cell */
  struct object *attr;
};
typedef struct Object * objectp;

/**** data types (class identifiers) ************/
#define Snull             0
#define Sinteger          1     /* integer */
#define Sstring           2     /* pointer to a string */
#define SexecutableArray  3     /* executable array */
#define Soperator         4     /* operators defined in the system dic */
#define Sdollar           5     /* pointer to a string obtained from $...$ */
#define Sarray            6     /* lc.ival is the size of array,
				   (rc.op)[0], ..., (rc.op)[k] is the array
				   of object */
#define SleftBraceTag    7     /* [ */
#define SrightBraceTag   8     /* ] */
#define Spoly            9
#define SarrayOfPOLY     10
#define SmatrixOfPOLY    11
#define Slist            12   /* list of object */
#define Sfile            13
#define Sring            14
#define SuniversalNumber 15
#define SrationalFunction 16
#define Sclass          17   /* class, for extension */
#define Sdouble         18

#define TYPES            19   /* number of data types. */
/* NOTE! If you change the above, you need to change mklookup.c too. */
/* Change also dr.sm1 : datatype constants. */

/* The following tags are not in stackm.h, but we use them. */
#define CLASSNAME_CONTEXT 258


#define ID 258
#define QUOTE 259
#define SINGLEQUOTE 260
#define NUMBER 261
#define CLASS 262
#define SUPER 263
#define OPERATOR 264
#define FINAL 265
#define EXTENDS 266
#define INCETANCEVARIABLE 267
#define THIS 268
#define NEW 269
#define SIZEOFTHISCLASS 270
#define STARTOFTHISCLASS 271
#define MODULE 272
#define PRINT 273
#define LOCAL 274
#define DEF 275
#define SM1 276
#define LOAD 277
#define TEST 278
#define SPECIAL 279
#define AUTO 280
#define BREAK 281
#define CASE 282
#define CHAR 283
#define CONST 284
#define CONTINUE 285
#define DEFAULT 286
#define DO 287
#define DOUBLE 288
#define ELSE 289
#define ENUM 290
#define EXTERN 291
#define FLOAT 292
#define FOR 293
#define GOTO 294
#define IF 295
#define INT 296
#define LONG 297
#define REGISTER 298
#define RETURN 299
#define SHORT 300
#define SIGNED 301
#define SIZEOF 302
#define STATIC 303
#define STRUCT 304
#define SWITCH 305
#define TYPEDEF 306
#define UNION 307
#define UNSIGNED 308
#define VOLATILE 309
#define VOID 310
#define WHILE 311
#define PSFOR 312
#define PROMPT 313
#define RESIDUEPUT 314
#define NEGATEPUT 315
#define MULTPUT 316
#define PUT 317
#define OR 318
#define AND 319
#define NOTEQUAL 320
#define EQUAL 321
#define GREATEREQUAL 322
#define LESSEQUAL 323
#define RIGHTSHIFT 324
#define LEFTSHIFT 325
#define DECREMENT 326
#define INCREMENT 327
#define UNARYMINUS 328
#define MEMBER 329



