/* $OpenXM: OpenXM/src/kan96xx/Kan/switch.c,v 1.4 2003/07/17 09:10:54 takayama Exp $ */
#include <stdio.h>
#include <string.h>
#include "datatype.h"
#include "extern2.h"
#include "gradedset.h"

/********** flags for switch status **************/
char *F_mmLarger = "???";
char *F_mpMult = "???";
char *F_monomialAdd = "???";
char *F_red = "???";
char *F_sp = "???";
char *F_groebner = "???";
char *F_grade = "???";
char *F_isSameComponent = "???";


void print_switch_status(void) {
  printf("------------------------------------\n");
  printf("mmLarger [matrix,lexicographic,tower,module_matrix]  %s\n",F_mmLarger);
  printf("mpMult   [poly,diff,difference]                %s\n",F_mpMult);
  printf("monomialAdd [poly]                             %s\n",F_monomialAdd);
  printf("red@      [standard,module1,module2,module1rev,ecart]\n");
  printf("                                               %s\n",F_red);
  printf("isSameComponent [x,xd]                         %s\n",F_isSameComponent);
  printf("sp        [standard]                           %s\n",F_sp);
  printf("grade     [standard,firstvec,module1,module1v] %s\n",F_grade);
  printf("groebner  [standard,gm]                        %s\n",F_groebner);
}

/* called from stackmachine.c,
   ex. $sp$ $so$ switch_function
 or    (report) (function) switch_function value(string)
*/
char *switch_function(fun,arg)
     char *fun;
     char *arg;
{
  char *ans = NULL;
  if (strcmp(fun,"mmLarger")==0) {
    switch_mmLarger(arg);
  }else if (strcmp(fun,"mpMult")==0) {
    switch_mpMult(arg);
  }else if (strcmp(fun,"monomialAdd")==0) {
    switch_monomialAdd(arg);
  }else if (strcmp(fun,"red@")==0) {
    switch_red(arg);
  }else if (strcmp(fun,"sp")==0) {
    switch_sp(arg);
  }else if (strcmp(fun,"isSameComponent")==0) {
    switch_isSameComponent(arg);
  }else if (strcmp(fun,"groebner")==0) {
    switch_groebner(arg);
  }else if (strcmp(fun,"grade")==0) {
    switch_grade(arg);
  }else if (strcmp(fun,"report")==0) {
    ans = (char *)sGC_malloc(128); /* 128 >= max(strlen(F_*))+1 */
    ans[0] = '\0';
    if (strcmp(arg,"mmLarger")==0) {
      strcpy(ans,F_mmLarger);
    }else if (strcmp(arg,"mpMult")==0) {
      strcpy(ans,F_mpMult);
    }else if (strcmp(arg,"monomialAdd")==0) {
      strcpy(ans,F_monomialAdd);
    }else if (strcmp(arg,"red@")==0) {
      strcpy(ans,F_red);
    }else if (strcmp(arg,"isSameComponent")==0) {
      strcpy(ans,F_isSameComponent);
    }else if (strcmp(arg,"sp")==0) {
      strcpy(ans,F_sp);
    }else if (strcmp(arg,"groebner")==0) {
      strcpy(ans,F_groebner);
    }else if (strcmp(arg,"grade")==0) {
      strcpy(ans,F_grade);
    }else {
      fprintf(stderr,"Unknown argument for switch_function():report\n");
    }    
  }else {
    fprintf(stderr,"Unknown argument for switch_function()\n");
  }
  return(ans);
}

/* 1. called from KinitKan() (poly.c)
*/
void switch_init(void) {
  switch_mmLarger("matrix");
  switch_mpMult("poly");
  switch_monomialAdd("poly");
  switch_red("standard");
  switch_isSameComponent("x");
  switch_sp("standard");
  switch_groebner("standard");
  switch_grade("standard");
}

/* 
  1. called from switch_init(), switch_function();
  2. setOrderByMatrix().
*/

void switch_mmLarger(arg)
     char *arg;
{
  if (strcmp(arg,"matrix") == 0) {
    mmLarger = mmLarger_matrix;
    F_mmLarger = "matrix";
  } else if (strcmp(arg,"lexicographic") == 0) {
    mmLarger = mmLarger_pureLexicographic;
    F_mmLarger = "lexicographic";
  } else if (strcmp(arg,"tower") == 0) {
    mmLarger = mmLarger_tower;
    F_mmLarger = "tower";
  } else if (strcmp(arg,"module_matrix")==0) {
    mmLarger = mmLarger_module_matrix;
    F_mmLarger = "module_matrix";
    init_module_matrix_mode(3); // default rank is 3.
  }else ;
}

/* 
  1. called from switch_init(), switch_function();
*/

void switch_mpMult(arg)
     char *arg;
{
  if (strcmp(arg,"poly") == 0) {
    mpMult = mpMult_poly;
    F_mpMult = "poly";
  } if (strcmp(arg,"diff") == 0 ) {
    mpMult = mpMult_diff;
    F_mpMult = "diff";
  } if (strcmp(arg,"difference") == 0 ) {
    mpMult = mpMult_difference;
    F_mpMult = "difference";
    /* cf. a dirty trick in mpMult_difference() in poly3.c. */
  }else ;
}

/* 
  1. called from switch_init(), switch_function();
*/

void switch_monomialAdd(arg)
     char *arg;
{
  if (strcmp(arg,"poly") == 0) {
    monomialAdd = monomialAdd_poly;
    F_monomialAdd = "poly";
  }else ;
}

void switch_red(arg)
     char *arg;
{
  if (strcmp(arg,"standard") == 0) {
    switch_sp("standard");
    isReducible = isReducible_gen;
    reduction1 = reduction1_gen;
    reduction = reduction_gen;

    isCdrReducible = isCdrReducible_gen;
    reduction1Cdr = reduction1Cdr_gen;
    reductionCdr = reductionCdr_gen;

    lcm = lcm_gen;
    switch_grade("standard");
    F_red = "standard";
  }else if (strcmp(arg,"module1") == 0) {
    switch_sp("standard");
    isReducible = isReducible_module;
    reduction1 = reduction1_gen;
    reduction = reduction_gen;

    isCdrReducible = isCdrReducible_gen;
    reduction1Cdr = reduction1Cdr_gen;
    reductionCdr = reductionCdr_gen;

    lcm = lcm_module;
    switch_grade("module1");
    F_red = "module1";
  }else if (strcmp(arg,"module1rev") == 0) {
    switch_sp("standard");
    isReducible = isReducible_module;
    reduction1 = reduction1_gen;
    reduction = reduction_gen_rev;

    isCdrReducible = isCdrReducible_gen;
    reduction1Cdr = reduction1Cdr_gen;
    reductionCdr = reductionCdr_gen;

    lcm = lcm_module;
    switch_grade("module1");
    F_red = "module1rev";
  }else if (strcmp(arg,"module2") == 0) {
    switch_sp("standard");
    isReducible = isReducible_module;
    reduction1 = reduction1_gen;
    reduction = reduction_gen;

    isCdrReducible = isCdrReducible_gen;
    reduction1Cdr = reduction1Cdr_gen;
    reductionCdr = reductionCdr_gen;

    lcm = lcm_module;
    switch_grade("standard");
    F_red = "module2";
  }else if (strcmp(arg,"ecart") == 0) {
    switch_sp("standard");
    isReducible = isReducible_module;
    reduction1 = reduction1_gen;
    reduction = reduction_ecart;

    isCdrReducible = isCdrReducible_gen;
    reduction1Cdr = reduction1Cdr_gen;
    reductionCdr = reductionCdr_gen;

    lcm = lcm_module;
    switch_grade("module1");
    F_red = "ecart";
  }else if (strcmp(arg,"debug") == 0) {
    reduction1 = reduction1_gen_debug;
    F_red = "debug";
  }else ;
}

void switch_groebner(arg)
     char *arg;
{
  if (strcmp(arg,"standard") == 0) {
    groebner = groebner_gen;
    F_groebner = "standard";
  } else if (strcmp(arg,"gm") == 0) {
    groebner = groebner_gm;
    F_groebner = "gm";
  } else ;
}

/* called from switch_init */
void switch_isSameComponent(arg)
     char *arg;
{
  if (strcmp(arg,"x") == 0) {
    isSameComponent = isSameComponent_x;
    F_isSameComponent = "x";
  } else if (strcmp(arg,"xd") == 0) {
    isSameComponent = isSameComponent_xd;
    F_isSameComponent = "xd";
  } else ;
}

void switch_sp(arg)
     char *arg;
{
  if (strcmp(arg,"standard") == 0) {
    sp = sp_gen;
    F_sp = "standard";
  } else ;
}


void switch_grade(arg)
     char *arg;
{
  if (strcmp(arg,"standard")==0) {
    grade = grade_gen;
    F_grade = "standard";
  }else if (strcmp(arg,"firstvec")==0) {
    grade = grade_firstvec;
    F_grade = "firstvec";
  }else if (strcmp(arg,"module1")==0) {
    grade = grade_module1;
    F_grade = "module1";
  }else if (strcmp(arg,"module1v")==0) {
    grade = grade_module1v;
    F_grade = "module1v";
  }else ;
    
}
