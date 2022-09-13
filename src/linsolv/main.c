#include "lin.h"

extern int Verbose;
extern struct node InAns;

int main(int argc,char *argv[]) {
  Ring r;
  int i;
  int nv=5; int chr=1;
  char **v=NULL;
  int quiet=1;
  int count=1;
  /*
  v=define_ring_vars("t2.txt"); i=0;
  while (v[i] != NULL) printf("<%s>\n",v[i++]);
  return 0;
  */

  init_calc(nv,chr,v); /*For initialize gc. It is again executed after getting options. */
  Verbose=0;
  for (i=1; i<argc; i++) {
    if (strcmp(argv[i],"--nv")==0) {
      i++;
      sscanf(argv[i],"%d",&nv);
    }else if (strcmp(argv[i],"--verbose")==0) {
      Verbose=1;
    }else if (strcmp(argv[i],"--char")==0) {
      i++;
      sscanf(argv[i],"%d",&chr);
      if (chr==0) chr=1; /* Q */
      if (chr<0) chr=0;  /* Z */
    }else if (strcmp(argv[i],"--quiet")==0) {
      i++;
      sscanf(argv[i],"%d",&quiet);
    }else if (strcmp(argv[i],"--vars")==0) {
      i++;
      v=define_ring_vars(argv[i]);
    }else{
      fprintf(stderr,"Unknown option. --nv number_of_vars --char characteristic\n --vars fileName --quiet val");
      usage();
      return(-1);
    }
  }
  init_calc(nv,chr,v);
  if (!quiet) { show_ring(CurrentRing); printf("\n"); }
  Input = stdin;
  while ( 1 ) {
    InAns.next=NULL;
    parse_string = 0;
    yyparse();
    printf("Ans%d=",count);
    print_node(mylist); printf("$\n");
    if (InAns.next != NULL) {
      printf("InAns%d=",count); print_node(InAns.next); printf("$\n");
    }
    count++;
  }
  return(0);
}

int usage() {
   printf("\nUsage: \n");
   printf("linsolv [--nv number_of_vars] [--char characteristic]\n   [--vars fileName] [--quiet val] [--verbose]\n");
   printf("  Defaults are --nv 5 --char 0  --quiet 1\n\n");
   printf("Examples:\n");
   printf(" - linsolv --quiet 0            shows current ring\n");
   printf(" - Load tmp-MM.rr on asir and run test6n() for F_C. tmp-mm*vars.txt\n");
   printf("    is an example of the file to define a ring by --vars.\n");
   printf("    tmp-mm*vars.txt : [a_1,a_2,a_3,a_4,a_5,a_6,c1_0,c1_1,c1_2,c1_3],\n");
   printf("    where the order is a_1 > ... > c1_3\n");
   printf(" - A=[x1+x2,x2+x3]; E(A);  computes the reduced GB for A. \n");
   printf("   A;       Note that A is changed.\n");
/*   printf(" - bug:  B=[E(A),A] causes a segmentation fault.\n");
   printf("         B=[E(A),x3+x4] is OK.\n"); */
   printf(" - syntax: All variables are list type. X=x1, then X will become [x1]\n");
   printf(" - F(R) translates the reduced GB (rref) to the rule. a* variables are set to 0\n");
   printf("   R=E([x1+x2,x2+x3]); F(R); \n");
   printf("\n");
}
