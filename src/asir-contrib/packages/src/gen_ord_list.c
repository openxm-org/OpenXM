/*  $OpenXM$  */
#include <stdio.h>

/* This program generates 
   Sm1_ord_list for sm1
   */
#define NUMBERMAX  20
main() {
  int i;
  printf("Sm1_ord_list = [ ");

  /* d variables. */
  for (i=NUMBERMAX; i>=0; i--) {
	printf("dz%d, ",i);
  }
  for (i=NUMBERMAX; i>=0; i--) {
	printf("dy%d, ",i);
  }
  for (i=NUMBERMAX; i>=0; i--) {
	printf("dx%d, ",i);
  }
  printf("\n");
  for (i='z'; i>='a'; i--) {
	if (i != 'o' && i != 'd') {
	  /* You cannot use do and dd as a variable name. */
	  printf("d%c, ",i);
	}
  }
  printf("\n");

  /* x variables. This must be compatible with the phrase above. */
  for (i=NUMBERMAX; i>=0; i--) {
	printf("z%d, ",i);
  }
  for (i=NUMBERMAX; i>=0; i--) {
	printf("y%d, ",i);
  }
  for (i=NUMBERMAX; i>=0; i--) {
	printf("x%d, ",i);
  }
  printf("\n");
  for (i='z'; i>='a'; i--) {
	if (i != 'o' && i != 'd') {
	  /* You cannot use do and dd as a variable name. */
	  printf("%c, ",i);
	}
  }
  printf("\n");

  /* trailer */
  printf(" hoge ]$\n");
}

	
