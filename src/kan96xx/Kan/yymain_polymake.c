/* $OpenXM$ */
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
  SS = pmPutstr(0);
  printf("%s\n",SS);
  pmSetS(SS);
  pmPreprocess();
  printf("--------------------------\n");
  printf("%s\n",SS);
  printf("--------------------------\n");
  PMparse();
}

sGC_malloc(int n) {
  return GC_malloc(n);
}
