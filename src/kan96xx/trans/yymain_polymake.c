/* $OpenXM: OpenXM/src/kan96xx/trans/yymain_polymake.c,v 1.2 2003/12/04 07:49:24 takayama Exp $ */
#include <stdio.h>
#include "yylex_polymake.h"
#include "yy_polymake.tab.h"


char *SS=NULL;
main_t() {
  int c,type;
  pmPutstr(-1);
  while ((c=getchar()) != EOF) {
	pmPutstr(c);
  }
  pmPutstr('\n');
  SS = pmPutstr(0);
  printf("%s\n",SS);
  pmSetS(SS);
  pmPreprocess();
  printf("--------------------------\n");
  printf("%s\n",SS);
  printf("--------------------------\n");
  while ((type=PMlex()) != PM_noToken) {
	printf("type=%d ",type);
	if ((type == PM_number) || (type == PM_keyword)) {
	  printf("value="); pmPrintObject(stdout,PMlval);
	}
	printf("\n");
  }
}

main() {
  int c,type;
  
  pmPutstr(-1);
  while ((c=getchar()) != EOF) {
	pmPutstr(c);
  }
  pmPutstr('\n');
  SS = pmPutstr(0);
#ifdef DEBUG
  printf("%s\n",SS);
#endif
  pmSetS(SS);
  pmPreprocess();
#ifdef DEBUG
  printf("--------------------------\n");
  printf("%s\n",SS);
  printf("--------------------------\n");
#endif
  PMparse();
}

sGC_malloc(int n) {
  return GC_malloc(n);
}

PMerror() {
}
