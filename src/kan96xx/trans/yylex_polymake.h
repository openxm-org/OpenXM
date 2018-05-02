/* $OpenXM: OpenXM/src/kan96xx/trans/yylex_polymake.h,v 1.1 2003/11/24 02:33:39 takayama Exp $ */
#include "gc.h"
struct pmObject {
  int tag;
  void *body;
};
typedef struct pmObject *pmObjectp ;
extern pmObjectp PMlval;  /* Yacc on FreeBSD 4.8 does not generate this declaration in yy_polymake.tab.h
                             Yacc on Debian potato generates this declaration in yy_poymake.tab.h
                             No problem with makeing two extern declarations. */

#define YYSTYPE pmObjectp
#define PM_unknown -2
#define PM_noToken -1

#define PMobject_str 1
#define PMobject_list 2
#define PMobject_tree 3

struct pmList {
  pmObjectp left;
  struct pmList *right;
};

struct pmTree {
  char *nodeName;
  pmObjectp attrList;
  pmObjectp childs;
};

int pmSetS(char *s);
int pmPreprocess(void);
char *pmPutstr(int c);
pmObjectp pmNewStrObject(char *s);
pmObjectp pmNewListObject(pmObjectp a);
pmObjectp pmCons(pmObjectp a,struct pmList *b);
int pmListLength(struct pmList *list);
pmObjectp pmNewTreeObject(char *s);
pmObjectp pmNewTreeObjecto(pmObjectp s);
pmObjectp pmAddAttr(pmObjectp c,pmObjectp a);
pmObjectp pmAddChild(pmObjectp c,pmObjectp a);
void pmPrintObject(FILE *fp,pmObjectp a);
char *pmObjectToStr(pmObjectp p);
char *pmObjectToStr_aux(pmObjectp p);
void *sGC_malloc(int n);

