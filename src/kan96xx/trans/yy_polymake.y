/* $OpenXM: OpenXM/src/kan96xx/trans/yy_polymake.y,v 1.3 2004/07/15 07:50:40 takayama Exp $ */
/* yacc -d -b yy_polymake -p PM yy_polymake.y */
/* yacc -d -b yy_polymake -p PM yy_polymake.y ; gcc yylex_polymake.c  yy_polymake.tab.c*/
%{
#include <stdio.h>
#include "yylex_polymake.h"
%}


%token PM_emptyLine PM_keyword PM_LCurryBrace PM_RCurryBrace PM_LAngle PM_RAngle PM_LBracket PM_RBracket PM_colon PM_LBrace PM_RBrace
%token PM_number PM_newline

%%
program
:
programList {
  pmPrintObject(stdout,$1);
  fprintf(stdout,"\n");
} 
| pmemptyLine programList {
  pmPrintObject(stdout,$2);
  fprintf(stdout,"\n");
}
;


programList
:
pmdata {
  pmObjectp ob;
  ob = pmNewTreeObject("data");  
  ob = pmAddChild($1,ob);
  $$ = ob;
}
| pmdata programList {
  pmObjectp ob;
  ob = $2;
  ob = pmAddChild($1,ob);
  $$ = ob;
}
;

pmnumberList
:
PM_number {
  $$=pmNewListObject($1);
}
| PM_number pmnumberList {
  pmObjectp t;
  t = $2;
  if (t->tag != PMobject_list) {
	fprintf(stderr,"pmnumberList: list is broken.");
	$$=$1;
  }else{
	$$=pmCons($1,(struct pmList *)(t->body));
  }
}
;

pmline
: dataUnitList PM_newline {
  $$=$1;
}
;

dataUnitList
: dataUnit {
  $$=$1;
}
| dataUnit dataUnitList {
  pmObjectp t;
  t=pmNewListObject($2);
  $$=pmCons($1,(struct pmList *)(t->body));
}
| dataUnitList PM_colon dataUnitList { /* a : b --> COLON(a,b) */
  pmObjectp t;
  pmObjectp r;
  r = pmNewTreeObject("_colon");
  r = pmAddChild($3,r);
  r = pmAddChild($1,r);
  $$=r;
}
;

dataUnit
: pmnumberList {
  $$=$1;
}
| PM_LCurryBrace dataUnitList PM_RCurryBrace {
  $$=pmAddChild($2,pmNewTreeObject("_set")); /* set */
  /*printf("{}");pmPrintObject(stdout,$2);*/
}
| PM_LAngle dataUnitList PM_RAngle {
  $$=pmAddChild($2,pmNewTreeObject("_pairs"));   /* pairs */
  /* printf("<>");pmPrintObject(stdout,$2); */
}
| PM_LBracket dataUnitList PM_RBracket {
  $$=pmAddChild($2,pmNewTreeObject("_bracket"));  /* bracket */
  /* printf("[]");pmPrintObject(stdout,$2); */
}
| PM_LBrace dataUnitList PM_RBrace {
  $$=pmAddChild($2,pmNewTreeObject("_tuple"));  /* tuple */
  /* printf("()");pmPrintObject(stdout,$2); */
}
| PM_LCurryBrace  PM_RCurryBrace {
  $$=pmNewTreeObject("_set"); 
}
| PM_LAngle PM_RAngle {
  $$=pmNewTreeObject("_pairs"); 
}
| PM_LBracket PM_RBracket {
  $$=pmNewTreeObject("_bracket");
}
| PM_LBrace PM_RBrace {
  $$=pmNewTreeObject("_tuple");
}
;

pmlineList
:
pmline {
  $$=pmNewListObject($1);
}
| pmline pmlineList {
  pmObjectp t;
  t = $2;
  if (t->tag != PMobject_list) {
	fprintf(stderr,"pmlineList: list is broken.");
	$$=$1;
  }else{
    $$=pmCons($1,(struct pmList *)(t->body));
  }
}
;


pmdata
:  
PM_keyword   PM_newline pmemptyLine {
  $$=pmNewTreeObjecto($1);
}
| PM_keyword PM_newline pmlineList pmemptyLine {
  pmObjectp ob;
  ob = pmNewTreeObjecto($1);
  ob = pmAddChild($3,ob);
  $$=ob;
}
;


pmemptyLine
:
PM_emptyLine
;
	   
