/* $OpenXM: OpenXM/src/kan96xx/trans/yymain_polymake.c,v 1.7 2020/10/06 11:33:48 takayama Exp $ */
#include <stdio.h>
#include "yylex_polymake.h"
#include "yy_polymake.tab.h"


char *SS=NULL;
int main_t() {
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
  return 0;
}

int PMparse();

int main() {
  int c,type;
  GC_INIT();  
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
  return 0;
}

void *sGC_malloc(int n) {
  return GC_malloc(n);
}

void PMerror() {
  fprintf(stderr,"Parse error. cf. yylex_polymake.c: PM_debug=1;\n");
}
