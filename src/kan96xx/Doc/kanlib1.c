/* 1996, 1/5
   kanlib1.c : an example of how to use kan from your C-program.
*/

#include <stdio.h>
#include "datatype.h"
#include "stackm.h"
#include "extern.h"
#include "extern2.h"
#include "lookup.h"
#include "matrix.h"
#include "gradedset.h"

main() {
  struct object of;
  POLY f;
  extern int Quiet;
  Quiet = 1;
  KSstart();   /********  initialize ***********/
  /* Call setjmp(EnvOfStackMachine) for error handling. cf. scanner() in
   Kan/stackmachine.c */

  /* First, define ring Q[x,y] and put ff = Expand[(x+y)^10]; */
  KSexecuteString("[(x,y) ring_of_polynomials ( ) elimination_order 0] define_ring");
  KSexecuteString("( (x+y)^10 ).  /ff set  ff print newline");

  /* Put f = ff and compute f*f by calling ppMult() in kanlib.a. */
  printf("\n\n");
  KSexecuteString("ff"); of = KSpop();
  f = of.lc.poly;
  f = ppMult(f,f); /* Now, f = (x+y)^10 * (x+y)^10. */
  printf("(x+y)^20= %s\n",POLYToString(f,' ',0));


  KSstop(); /* closing */
}
  
   
