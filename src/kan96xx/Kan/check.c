/*  work7/check.c  1996, 3/19 */
/*   should move to SSkan/Kan/check.c */
#include <stdio.h>
#define N0 4
struct ring {
  int dummy;
};

struct exps {
  int x;
  int D;
};

struct monomial {
  struct ring *ringp;
  struct exps e[N0];
};

/* --------------------------------------- */
struct monom {
  struct ring *ringp;
  int ee[N0*2];
};

main() {
  int i;
  struct monomial *mp;
  if (sizeof(struct monomial) != sizeof(struct monom)) {
    fprintf(stderr,"Error: monomial != monom \n");
  }

  mp = (struct monomial *)malloc(sizeof(struct monomial));
  for (i=0; i<N0; i++) {
    mp->e[i].x = i;
    mp->e[i].D = -i;
  }
  for (i=0; i<N0*2; i++) {
    printf(" %d ",((struct monom *)mp)->ee[i]);
  }
  printf("\n\n");
  for (i=0; i<N0*2; i++) {
    ((struct monom *)mp)->ee[i] = -i;
  }
  for (i=0; i<N0; i++) {
    printf(" %d ",mp->e[i].x);
    printf(" %d ",mp->e[i].D);
  }
  printf("\n\n");
}
  
    



