/*
** call.c                                 Birk Huber, 2/99 
**   -- main calling file for tigers code
**  
**
** TiGERS,  Toric Groebner Basis Enumeration by Reverse Search 
** copyright (c) 1999  Birk Huber
**
*/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include "utils.h"
#include "gset.h"
#include "matrices.h"

int NT = 1;

/*
** write a description of program usage to stderr
*/
usage(prog)
char *prog;
{
  static char *helpmsg[] = {
  "Function: Enumerate all Groebner bases of a toric ideal I_A.",
  "          \n",
  "Input: \n ",
  "  A) A ring description (for now just a number of variables)\n",
  "     followed by a reduced Groebner basis for I_A.\n",
  "     example:\n",
  "     R: 5\n",
  "     G: {a^2*c-b^2*e, a^2*d-b*e^2, b*d-c*e}\n",
  "  or \n",
  "  B) An integer matrix A (defining I_A).\n",
  "     example:\n",
  "     M: { 3 5 : 2 1 1 1 1 1 \n",
  "                3 0 1 2 1 0 \n",
  "                4 0 0 1 2 1}\n",
  " Note: Lines beginning with a % are comments and are ignored.\n",
  "       Comment lines should only occur at the beginning of the file.\n",
  "     : When binomials are printed out (and read in) they can be \n",
  "       preceeded by the characters !, or # \n",
  "       ! means the binomial is known not to be a facet.\n",
  "       # means the binomial is known to be a facet.\n",
  " \n",
  "Options:\n",
  "    -h            print this message\n"
  "    -i (filename) set file name for input  [default: stdin]\n",
  "    -o (filename) set file name for output [default: stdout]\n",
  "    -R            only compute root of tree \n",
  "    -r            compute all grobner bases [done by default]\n",
  "    -C            turn partial caching on   [done by default]\n", 
  "    -c            turn partial caching off \n",
  "    -T            print edges of search tree \n",
  "    -t            do not print edges of search tree [assumed by default]\n",  "    -L            print vertices by giving initial ideals\n",
  "                     and printing facet biomials.\n",
  "    -l            print vertices as grobner bases   [done by default]\n",
  "    -F            Use only linear algebra when testing facets [default]\n",
  "    -f            use FLIPPABILITY test first when determining facets\n",
  "    -e            use exhaustive search instead of reverse search\n",
  "    -E            use reverse search                   [default]\n",
  NULL
  };
  char **p=helpmsg;

  fprintf(stderr,"Usage: %s {Options} \n",prog);
  while(*p)(void) fputs(*p++,stderr);
  exit(-1);
}

FILE *infile=stdin, *outfile=stdout;
extern int rsearch_cache;
extern int print_tree;
extern int  print_init;
extern int stats_minfacets;
extern int stats_minelts;
extern int stats_mindeg;
extern int stats_maxfacets;
extern int stats_maxelts;
extern int stats_maxdeg;
extern int stats_ecounter;
extern int stats_tdepth;
extern int lptest;
int root_only=FALSE;
int compGB=FALSE;
int use_exsearch=FALSE;

#define MATFOUND 1
#define GSETFOUND 2
main2(int argc, char **argv ){
 char *c,cc, *prog=argv[0], *ifname=0, *ofname=0;
 int tmp,acnt,stat=0,counter;
 gset G1=0,gset_toric_ideal();
 int **M=0,Mn,Mm;
 double tt;

  /* initialize parameters */
  root_only=FALSE;
  rsearch_cache=TRUE;
  print_tree=FALSE;
  print_init=FALSE;

  /* parse command line */
  while (--argc > 0 && (*++argv)[0] == '-'){
    acnt=0;
    for (c = argv[0]+1; *c != '\0'; c++){
      switch (*c) {
      case 'h': usage(prog);  break;
      case 'R': root_only=TRUE; break;    /* Root Flag On */      
      case 'r': root_only=FALSE; break;   /* Root Flag Off */      
      case 'C': rsearch_cache=TRUE; break;/* Turn partial caching on*/ 
      case 'c': rsearch_cache=FALSE;break;/* Turn partical caching off*/
      case 'T': print_tree=TRUE;  break;  /* Turn tree printing on */
      case 't': print_tree=FALSE; break;  /* Turn tree printing off*/
      case 'L': print_init=TRUE;  break;  /* Turn initial ideal printing on */
      case 'l': print_init=FALSE; break;  /* Turn initial ideal printing off */
      case 'f': lptest=3; break;          /*check facets with linalg and */
	                                  /*flipability tests */
      case 'F': lptest=1; break;          /*check facets only with linalg */
      case 'A': lptest=2; use_exsearch=TRUE;break;          /*check facets only for flipability*/
      case 'E': use_exsearch=FALSE;break; /*use reverse search to enumerate */
      case 'e': use_exsearch=TRUE; break; /*use exhaustive search */
      case 'i': case 'I':
         argc--;
	 ifname=strdup(argv[++acnt]); /* scan infile name */
          fprintf(stderr,"using filename %s for input\n",ifname);  
       break;
      case 'o': case 'O':argc--;
	  ofname=strdup(argv[++acnt]); /* scan infile name */
          fprintf(stderr,"using filename %s for output\n",ofname);  
       break;	/* scan outfile name */
        break;
        default:
           fprintf(stderr,"%s: illegal option %c\n",prog,*c);
      }
    }
    argv+=acnt;
  }
  if (argc != 0) usage(prog);
  
  /* open infile and outfile (if nesc) */
  if (ifname!=0 && (infile=fopen(ifname,"r"))==0){
      fprintf(stderr," %s: couldn't open %s for input\n",prog,ifname);
      exit(1);
  }
  if (ofname!=0 && (outfile=fopen(ofname,"w"))==0){
      fprintf(stderr," %s: couldn't open %s for output\n",prog,ofname);
      exit(1);
  }
 
  /* scan input from infile and outfile */
  
  eatwhite(infile);
  cc=getc(infile);
  if (cc=='R'){
    while((cc=getc(infile))!=':');
    if (ring_read(infile)==FALSE){
     fprintf(stderr,"%s: ring_read() failed\n",prog);
     exit(1);
    }
    eatwhite(infile);
    cc=getc(infile);
  }
  if (cc=='G'){
    while((cc=getc(infile))!=':');
    if (ring_N<0) {
       fprintf(stderr,"%s: ring not set\n",prog);
       exit(1);
    }
    G1=gset_new();
    if (gset_read(infile,G1)==FALSE){
     fprintf(stderr,"%s: gset_read() failed\n",prog);
     exit(1);      
    }
    stat=GSETFOUND;
  }
  else if (cc=='M'){
    while((cc=getc(infile))!=':');
    if ((M=imatrix_read(infile,&Mm,&Mn))==0){
     fprintf(stderr,"%s: imatrix_read() failed\n",prog);
     exit(1);      
    }
    if (ring_N==0) ring_set(Mn);
    else if (ring_N!=Mn) {
     fprintf(stderr,"%s: Matrix collum and ring dimensions must agree\n",prog);
     exit(1);
    }
    stat=MATFOUND;
  }
  else { 
   fprintf(stderr,"%s,: Input files contains neither a generating set\n",prog);
   fprintf(stderr,"     nor a matrix description of a toric ideal\n");
   exit(1);
}

  /* ensure we have root */
  if (stat==MATFOUND){
   G1=gset_toric_ideal(M,Mm,Mn); 
  }
  else {
   if (compGB==TRUE){gset_rgb(G1,monomial_lexcomp);}
   else {
     /* could put checks to make sure input is toric rgb */
     /* then use grobner walk to get to first rgb */
   }
  }

  if (NT == 1) {

    fprintf(outfile,"[\n");
    /* perform reverse search if desired*/  
    if (root_only==FALSE){
      if (use_exsearch==FALSE){
	/* should double check we are at root */
	tt=clock();
	counter=rsearch(G1);
	tt=(clock()-tt)/CLOCKS_PER_SEC;
	fprintf(outfile,"\n");
      } else {
	tt=clock();
	counter=exsearch(G1);
	tt=(clock()-tt)/CLOCKS_PER_SEC;
	fprintf(outfile,"\n");
      }
    }

    fprintf(outfile,"]\n");
    goto epilogue ;
  }
  /* if NT != 1, then start the followings. */
  /* output first GB if desired */
  fprintf(outfile,"%% starting GB:\n");
  fprintf(outfile,"R: %d\n",ring_N);
  fprintf(outfile,"G: ");
  gset_print(outfile,G1);
  fprintf(outfile,"\n");

  /* perform reverse search if desired*/  
  if (root_only==FALSE){
    if (use_exsearch==FALSE){
      /* should double check we are at root */
     fprintf(outfile,"\n Enumerating Groebner bases\n");
     fprintf(outfile,"   using reverse search\n");
     if (ifname!=0) fprintf(outfile,"   taking input from %s\n",ifname);
     if (rsearch_cache==FALSE) fprintf(outfile,"   without partial caching\n");
     else fprintf(outfile,"   with partial caching\n");
     switch(lptest){
     case 1: 
        fprintf(outfile,"   using only linear programing to test facets\n");
        break;
     case 3:  
        fprintf(outfile,"   using wall ideal pretest for facet checking\n");
        break;
     case 2: 
       fprintf(stderr,"Error: can not use -A option with reverse search\n");
       break;
     }
     tt=clock();
     counter=rsearch(G1);
     tt=(clock()-tt)/CLOCKS_PER_SEC;
     fprintf(outfile,"\n");
     fprintf(outfile,"Number of Groebner bases found %d\n",counter);
     fprintf(outfile,"Number of edges of state polytope %d\n",stats_ecounter);
     fprintf(outfile,"max caching depth      %d\n",stats_tdepth);
     fprintf(outfile,"max facet binomials    %d\n",stats_maxfacets);
     fprintf(outfile,"min facet binomials    %d\n",stats_minfacets);
     fprintf(outfile,"max elts               %d\n",stats_maxelts);
     fprintf(outfile,"min elts               %d\n",stats_minelts);
     fprintf(outfile,"max degree             %d\n",stats_maxdeg);
     fprintf(outfile,"min degree             %d\n",stats_mindeg);
     if (ifname!=0) fprintf(outfile,"%s: ",ifname);
     fprintf(outfile,"Reverse Search, ");
     switch(rsearch_cache){
     case TRUE: 
        fprintf(outfile,"  Caching,");
        break;
     case FALSE:  
        fprintf(outfile,"  NO Caching,");
        break;
      } 
     switch(lptest){
     case 1: 
        fprintf(outfile,"  LP only.\n");
        break;
     case 3:  
        fprintf(outfile,"  A-pretest,\n");
        break;
     }
     fprintf(outfile,"time used (in seconds) %4.2lf\n",tt);
     return 0;
    }
    else {
      fprintf(outfile,"\nEnumerating Groebner bases\n");
      fprintf(outfile,"using exhaustive searching");
      if (ifname!=0) fprintf(outfile,"taking input from %s\n",ifname);
      else fprintf(outfile,"\n");
     switch(lptest){
     case 1: 
        fprintf(outfile,"   using only linear programing to test facets\n");
        break;
     case 3:  
        fprintf(outfile,"   using wall ideal pretest for facet checking\n");
        break;
     case 2: 
       fprintf(outfile,"Using wall ideal pretest instead of facet checking\n");
       fprintf(outfile," FINDING ALL A-GRADED INITIALS CONNECTED TO ROOT\n");
       break;
     }
     tt=clock();
     counter=exsearch(G1);
     tt=(clock()-tt)/CLOCKS_PER_SEC;
     fprintf(outfile,"\n");
     fprintf(outfile,"Number of Groebner bases found %d\n",counter);
     fprintf(outfile,"Number of edges of state polytope %d\n",stats_ecounter);
     fprintf(outfile,"max facet binomials    %d\n",stats_maxfacets);
     fprintf(outfile,"min facet binomials    %d\n",stats_minfacets);
     fprintf(outfile,"max elts               %d\n",stats_maxelts);
     fprintf(outfile,"min elts               %d\n",stats_minelts);
     fprintf(outfile,"max degree             %d\n",stats_maxdeg);
     fprintf(outfile,"min degree             %d\n",stats_mindeg);
     if (ifname!=0) fprintf(outfile,"%s: ",ifname);
     fprintf(outfile,"Exhaustive search, ");
     switch(lptest){
     case 1: 
        fprintf(outfile,"  LP only\n");
        break;
     case 3:  
        fprintf(outfile,"  A-pretest\n");
        break;
     case 2: 
       fprintf(outfile," A-only\n");
       break;
     default:
        fprintf(outfile,"\n");
     } 
     fprintf(outfile,"time used (in seconds) %4.2lf\n",tt);
    return 0;
    }
  }

 epilogue: ;
  /* clean up */
  LP_free_space();
  if (G1!=0) gset_free(G1);
}


/************************    NT ***************************/
static int findNextToken(char *is, int start)
{
  if (start == -1) goto endtext ;
  while ( is[start] <= ' ') {
    if (is[start] == '\0' ) goto endtext ;
    start++;
  }
  while ( is[start] > ' ') {
    if (is[start] == '\0') goto endtext ;
    start++;
  }
  while ( is[start] <= ' ') {
    if (is[start] == '\0' ) goto endtext ;
    start++;
  }
  return(start);
 endtext: ;
  fprintf(stderr,"findNextToken: end of the text.\n");
  return(-1);
}
int **imatrix_read_from_string(char *is,int *m, int *n) {
  char c;
  int i,j;
  int **M;
  int pt = 0;

  /*  2 3 :
      1 1 1
      0 1 2
  */
  /* read in matrix dimensions and initialize matrix */
  sscanf(is,"%d %d :",m,n);
  M=new_imatrix(*m,*n);
  pt = findNextToken(is,pt); /* n */
  pt = findNextToken(is,pt); /* : */
  
  /* read in matrix entrees */
  for(i=0;i<*m;i++){
    for(j=0;j<*n;j++){
      pt = findNextToken(is,pt);
      sscanf(&(is[pt]),"%d",&(IMref(M,i,j)));
    }
  }
  
  return M;
}

tiger_executeString_M(char *is) {
  char *c,cc, *prog="tigers", *ifname=0, *ofname=0;
  int tmp,acnt,stat=0,counter;
  gset G1=0,gset_toric_ideal();
  int **M=0,Mn,Mm;
  double tt;

  /* initialize parameters */
  root_only=FALSE;
  rsearch_cache=TRUE;
  print_tree=FALSE;
  print_init=FALSE;

  if ((M=imatrix_read_from_string(is,&Mm,&Mn))==0){
     fprintf(stderr,"%s: imatrix_read() failed\n",prog);
     exit(1);      
  }
  if (ring_N==0) ring_set(Mn);
  else if (ring_N!=Mn) {
    fprintf(stderr,"%s: Matrix collum and ring dimensions must agree\n",prog);
    exit(1);
  }
  stat=MATFOUND;


  /* ensure we have root */
  if (stat==MATFOUND){
   G1=gset_toric_ideal(M,Mm,Mn);
  }

  fprintf(outfile,"[\n");
  /* perform reverse search if desired*/  
  if (root_only==FALSE){
    if (use_exsearch==FALSE){
      /* should double check we are at root */
      tt=clock();
      counter=rsearch(G1);
      tt=(clock()-tt)/CLOCKS_PER_SEC;
      fprintf(outfile,"\n");
    } else {
      tt=clock();
      counter=exsearch(G1);
      tt=(clock()-tt)/CLOCKS_PER_SEC;
      fprintf(outfile,"\n");
    }
  }

  fprintf(outfile,"]\n");
  LP_free_space();
  if (G1!=0) gset_free(G1);
}


main(int argc, char *argv[]) {
  if (argc > 1) {
    tiger_executeString_M(argv[1]);
  }else{
    tiger_executeString_M("2 4 : 1 1 1 1    0 1 2 3");
  }
}
