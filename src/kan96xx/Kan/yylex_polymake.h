/* $OpenXM$ */
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

struct pmList {
  pmObjectp left;
  struct pmList *right;
};

pmObjectp pmNewStrObject(char *s);
pmObjectp pmNewListObject(pmObjectp a);
pmObjectp pmCons(pmObjectp a,struct pmList *b);
void pmPrintObject(FILE *fp,pmObjectp a);
