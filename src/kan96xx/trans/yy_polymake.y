/* $OpenXM$ */
/* yacc -d -b yy_polymake -p PM yy_polymake.y */
/* yacc -d -b yy_polymake -p PM yy_polymake.y ; gcc yylex_polymake.c  yy_polymake.tab.c*/
%{
#include <stdio.h>
#include "yylex_polymake.h"
%}


%token PM_emptyLine PM_keyword PM_LCurryBrace PM_RCurryBrace
%token PM_number PM_newline

%%
program
:
programList {
  pmPrintObject(stdout,$1);
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
: pmnumberList  PM_newline {
  $$=$1;
}
| PM_LCurryBrace pmnumberList PM_RCurryBrace PM_newline {
  $$=$2;
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
	   
