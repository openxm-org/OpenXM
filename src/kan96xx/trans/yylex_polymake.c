/* $OpenXM: OpenXM/src/kan96xx/trans/yylex_polymake.c,v 1.6 2018/05/03 13:20:35 takayama Exp $ */
/* parser for polymake output */
/* This program requires


*/

#include <stdio.h>
#include "yylex_polymake.h"
#include "yy_polymake.tab.h"

#define mymalloc(n) sGC_malloc(n)
/*
#define mymalloc(n) malloc(n)
*/
/* pm = PolyMake */
#define PM_emptyLineCode 1


static char *S=NULL;
/* It is assumed that the newline code is only \n (preprocessed).
   Empty line is marked with 0x1. More than one empty line does not appear. */
static int Pt=-1;
static int PMdebug = 0;

/* char *PMlval; */

/* The function putstr() uses static variables inside,
  so if it is under use, it must not be initialized.
  The function putstr2() is for the second use and is identical
  function with putstr().
*/
static char *putstr(int c);
static char *putstr2(int c);
static char *putstr2s(char *s);

char *pmPutstr(int c) {
  return putstr(c);
}

int pmSetS(char *s) {
  S = s;
  return 0;
}

int PMlex() {
  int type;
  type = PMlex_aux();

  if (PMdebug) {
	printf("type=%d ",type);
	if ((type == PM_number) || (type == PM_keyword)) {
	  printf("value="); pmPrintObject(stdout,PMlval);
	}
	printf("\n");
  }
  
  return type;
}
int PMlex_aux() {
  if (Pt < 0) {
	return PM_noToken;
  }
  if (S[Pt] == 0) { Pt=-1; return PM_noToken; }
  while ((S[Pt]<=' ') && (S[Pt]!='\n') && (S[Pt] != PM_emptyLineCode)) Pt++;
  if (S[Pt] == '\n') { Pt++; return PM_newline; }
  if (S[Pt] == PM_emptyLineCode) {Pt++; return PM_emptyLine; }
  if (S[Pt] == '{') { Pt++; return PM_LCurryBrace; }
  if (S[Pt] == '}') { Pt++; return PM_RCurryBrace; }
  if (S[Pt] == '<') { Pt++; return PM_LAngle; }
  if (S[Pt] == '>') { Pt++; return PM_RAngle; }
  if (S[Pt] == '(') { Pt++; return PM_LBrace; }
  if (S[Pt] == ')') { Pt++; return PM_RBrace; }
  if (S[Pt] == '[') { Pt++; return PM_LBracket; }
  if (S[Pt] == ']') { Pt++; return PM_RBracket; }
  if (S[Pt] == ':') { Pt++; return PM_colon; }
  if (S[Pt] == '=') { Pt++; return PM_eq; }
  if (((S[Pt] >= '0') && (S[Pt] <= '9')) || (S[Pt] == '-')) {
	putstr(-1); putstr(S[Pt++]);
	while (((S[Pt]>='0') && (S[Pt]<='9')) || (S[Pt] == '/')) putstr(S[Pt++]);
	PMlval = pmNewStrObject(putstr(0));
	return PM_number;
  }
  if (((S[Pt] >= 'A') && (S[Pt] <= 'Z')) ||
      ((S[Pt] >= 'a') && (S[Pt] <= 'z')) ||
      (S[Pt] == '_') || S[Pt] == '!') {
    if (S[Pt] != '!') {putstr(-1); putstr(S[Pt++]);}
    else {
	  putstr(-1); Pt++;
	  putstr('N'); putstr('O'); putstr('T'); putstr('_'); putstr('_');
	}
    while (((S[Pt] >= 'A') && (S[Pt] <= 'Z')) ||
           ((S[Pt] >= 'a') && (S[Pt] <= 'z')) ||
           (S[Pt] == '_')) putstr(S[Pt++]);
    PMlval = pmNewStrObject(putstr(0));
    return PM_keyword;
  }
  Pt++;  return PM_unknown;
}

#define PUTSTR_INIT 10
static char *putstr(int c) {
  static char *s=NULL;
  static int pt=0;
  static int limit=0;
  int i;
  char *old;
  if (c < 0) {
	s = (char *)mymalloc(PUTSTR_INIT);
	if (s == NULL) {fprintf(stderr,"No more memory.\n"); exit(10);}
	limit = PUTSTR_INIT;
	pt = 0; s[pt] = 0;
	return s;
  }
  if (s == NULL) putstr(-1);
  if (pt < limit-1) {
	s[pt++]=c; s[pt]=0;
	return s;
  }else{
	old = s;
	limit = 2*limit;
	s = (char *)mymalloc(limit);
	if (s == NULL) {fprintf(stderr,"No more memory.\n"); exit(10);}
	for (i=0; i<=pt; i++) {
	  s[i] = old[i];
	}
	return putstr(c);
  }
}
static char *putstr2(int c) {
  static char *s=NULL;
  static int pt=0;
  static int limit=0;
  int i;
  char *old;
  if (c < 0) {
	s = (char *)mymalloc(PUTSTR_INIT);
	if (s == NULL) {fprintf(stderr,"No more memory.\n"); exit(10);}
	limit = PUTSTR_INIT;
	pt = 0; s[pt] = 0;
	return s;
  }
  if (s == NULL) putstr2(-1);
  if (pt < limit-1) {
	s[pt++]=c; s[pt]=0;
	return s;
  }else{
	old = s;
	limit = 2*limit;
	s = (char *)mymalloc(limit);
	if (s == NULL) {fprintf(stderr,"No more memory.\n"); exit(10);}
	for (i=0; i<=pt; i++) {
	  s[i] = old[i];
	}
	return putstr2(c);
  }
}
static char *putstr2s(char *s) {
  int i;
  char *ss;
  for (i=0; i<strlen(s); i++) {
	ss = putstr2(s[i]);
  }
  return ss;
}

int pmPreprocess() {
  int newp,oldp;
  int state;
  int i,j;
  if (S == NULL) return -1;
  Pt = 0; 
  for (oldp = newp = 0; S[oldp] != 0; oldp++) {
	if ((S[oldp] == 0xd) && (S[oldp] == 0xa)) { /* Windows 0d 0a */
	  S[newp++] = '\n'; oldp++;  
	}else{
	  S[newp++] = S[oldp];
	}
  }
  S[newp] = '\0';

  for (oldp = newp = 0; S[oldp] != 0; oldp++) {
	if (S[oldp] == 0xd) { /* Replace for mac 0xd to 0xa */
	  S[newp++] = '\n';   
	}else{
	  S[newp++] = S[oldp];
	}
  }
  S[newp] = '\0';

  /* Remove #, empty lines, ... */
  state = 1; newp=0;
  for (oldp=0; S[oldp] != 0; oldp++) {
	/* printf("oldp=%d, state=%d, char=%c\n",oldp,state,S[oldp]); */
	switch(state) {
	case 0:
	  if (S[oldp] == '\n') {state=1; newp=oldp+1;}
	  break;
	case 1:
	  if ((S[oldp] == ' ') || (S[oldp] == '\t')) break;
	  if ((S[oldp] == '#') || ((S[oldp]=='_') && (oldp == 0))
		|| ((S[oldp]=='_') && (S[oldp-1]<' '))) {
		/* skip the rest of the line, state=1; */
		for (; S[oldp] != 0 ; oldp++) {
		  if (S[oldp] == '\n') {oldp--; break;}
		}
		if (S[oldp] == 0) oldp--;
	  }else if (S[oldp] == '\n') {
		/* replace the empty line by PM_emptyLine */
		S[newp]= PM_emptyLineCode; j=oldp+1;
		for (i=newp+1; S[j] != 0; i++) {
		  S[i] = S[j]; j++;
		}
		oldp=newp;
		S[i] = 0;
	  }else{
		state = 0;
	  }
	  break;
	default:
	  break;
	}
  }
}	  

/* --------------- pmObjects --------------------- */
pmObjectp pmNewStrObject(char *s) {
  pmObjectp ob;
  ob = (pmObjectp) mymalloc(sizeof(struct pmObject));
  if (ob == NULL) {
	fprintf(stderr,"No more memory.\n");
	exit(10);
  }
  ob->tag = PMobject_str;
  ob->body = s;
  return ob;
}
pmObjectp pmNewListObject(pmObjectp a) {
  pmObjectp ob;
  struct pmList *aa;
  ob = (pmObjectp) mymalloc(sizeof(struct pmObject));
  if (ob == NULL) {
	fprintf(stderr,"No more memory.\n");
	exit(10);
  }
  aa= (struct pmList *)mymalloc(sizeof(struct pmList));
  if (aa == NULL) {
	fprintf(stderr,"No more memory.\n");
	exit(10);
  }
  aa->left=a;
  aa->right=NULL;
  ob->tag = PMobject_list;
  ob->body = aa;
  return ob;
}
pmObjectp pmCons(pmObjectp a,struct pmList *b) {
  pmObjectp ob;
  struct pmList *t;
  ob = pmNewListObject(a);
  t = ob->body;
  t->right = b;
  return ob;
}

int pmListLength(struct pmList *list) {
  struct pmList *t;
  int n;
  if (list == NULL) return 0;
  n = 0;
  t = list;
  while (t != NULL) {
	n++; t = t->right;
  }
  return n;
}

pmObjectp pmNewTreeObjecto(pmObjectp s)
{
  if ((s == NULL) || (s->tag != PMobject_str)) {
	warning_yylex_polymake("Invalid argument for pmNewObjecto\n");
	return pmNewTreeObject("?");
  }
  return pmNewTreeObject((char *)(s->body));
}
pmObjectp pmNewTreeObject(char *s) {
  pmObjectp ob;
  struct pmTree *aa;
  ob = (pmObjectp) mymalloc(sizeof(struct pmObject));
  if (ob == NULL) {
	fprintf(stderr,"No more memory.\n");
	exit(10);
  }
  aa= (struct pmTree *)mymalloc(sizeof(struct pmTree));
  if (aa == NULL) {
	fprintf(stderr,"No more memory.\n");
	exit(10);
  }
  aa->nodeName = s;
  aa->attrList = NULL;
  aa->childs = NULL;
  ob->tag = PMobject_tree;
  ob->body = aa;
  return ob;
}

pmObjectp pmAddAttr(pmObjectp c,pmObjectp a) {
  struct pmTree *tree;
  if (a->tag != PMobject_tree) {
	warning_yylex_polymake("pmAddAttr: the argument is not tree object.\n");
	return a;
  }
  tree = a->body;
  if (tree->attrList == NULL) {
	tree->attrList = pmNewListObject(c);
  }else{
	if (tree->attrList->tag == PMobject_list) {
	  tree->attrList = pmCons(c,(struct pmList *)(tree->attrList->body));
	}else{
	  warning_yylex_polymake("pmAddAttr: the attrbute list is broken.\n");
    }
  }
  return a;
}

/* Add c to the tree a */
pmObjectp pmAddChild(pmObjectp c,pmObjectp a) {
  struct pmTree *tree;
  if (a->tag != PMobject_tree) {
	warning_yylex_polymake("pmAddAttr: the argument is not tree object.\n");
	return a;
  }
  tree = a->body;
  if (tree->childs == NULL) {
	tree->childs = pmNewListObject(c);
  }else{
	if (tree->childs->tag == PMobject_list) {
	  tree->childs = pmCons(c,(struct pmList *)(tree->childs->body));
	}else{
	  warning_yylex_polymake("pmAddAttr: the child list is broken.\n");
    }
  }
  return a;
}

void warning_yylex_polymake(char *s) {
  fprintf(stderr,"Warning: %s",s);
}

void pmPrintObject(FILE *fp,pmObjectp p) {
  fprintf(fp,"%s",pmObjectToStr(p));
}
char *pmObjectToStr(pmObjectp p) {
  char *s;
  s=putstr2(-1);
  s=pmObjectToStr_aux(p);
  return putstr2(0);
}
char *pmObjectToStr_aux(pmObjectp p) {
  int n,i;
  struct pmList *list;
  struct pmList *t;
  struct pmTree *tree;
  char *ans;
  if (p == NULL) {
	/* fprintf(stderr,"NULL "); */
	return putstr2s("[]");
  }
  /* fprintf(stderr,"tag=%d ",p->tag); */
  switch (p->tag) {
  case PMobject_str:
    ans=putstr2s((char *)(p->body));
	break;
  case PMobject_list:
	list = (struct pmList *)(p->body);
	if (list == NULL) break;
	n = pmListLength(list);
	t = list;
	ans=putstr2s("[");
	for (i=0; i<n; i++) {
	  ans=pmObjectToStr_aux(t->left);
	  if (i != n-1) ans=putstr2s(",");
	  if (t == NULL) break; else t = t->right;
	}
	ans=putstr2s("]");
	break;
  case PMobject_tree:
	tree = p->body;
	ans=putstr2s("polymake."); ans=putstr2s(tree->nodeName);
    ans=putstr2s("(");
	/* Ignore attribute list */
	if (tree->childs == NULL) {n = 0; t = NULL; }
	else {
	  t = tree->childs->body;
	  n = pmListLength(t);
	}
	for (i=0; i<n; i++) {
	  ans=pmObjectToStr_aux(t->left);
	  t = t->right;
	  if (i != n-1) ans=putstr2(',');
	}
	ans = putstr2s(")");
	break;
  default:
	fprintf(stderr,"Unknown object tag %d.\n",p->tag);
	/* sleep(100);  to call debugger. */
	break;
  }
  return ans;
}

