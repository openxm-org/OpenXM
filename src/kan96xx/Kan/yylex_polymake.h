/* $OpenXM: OpenXM/src/kan96xx/Kan/yylex_polymake.h,v 1.2 2003/11/20 00:06:50 takayama Exp $ */
struct pmObject {
  int tag;
  void *body;
};
typedef struct pmObject *pmObjectp ;
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

pmObjectp pmNewStrObject(char *s);
pmObjectp pmNewListObject(pmObjectp a);
pmObjectp pmCons(pmObjectp a,struct pmList *b);
int pmListLength(struct pmList *list);
pmObjectp pmNewTreeObject(char *s);
pmObjectp pmNewTreeObjecto(pmObjectp s);
pmObjectp pmAddAttr(pmObjectp c,pmObjectp a);
pmObjectp pmAddChild(pmObjectp c,pmObjectp a);
void pmPrintObject(FILE *fp,pmObjectp a);
