/* $OpenXM$ */
#include <stdio.h>
#include "oxasir.h"

main() {
  hoge();
}

hoge() {
  /*  (CMO_ZZ,12); */
  unsigned char cmo0[]=
  {00, 00, 00, 0x14,
   00, 00, 00, 01, 00, 00, 00, 0xc};

  /* (CMO_ZZ,8) */
  unsigned char cmo1[] =
  {00, 00, 00, 0x14,
   00, 00, 00, 01, 00, 00, 00, 8};  

  /* (CMO_INT32,2); */
  unsigned char cmo2[] =
  { 00, 00, 00, 02, 00, 00, 00, 02}; 

  /* (CMO_STRING,"igcd") */
  unsigned char cmo3[] =
  {00, 00, 00, 04, 00, 00, 00, 04,
   0x69,0x67,0x63,0x64 };
  unsigned char cmo[1024];
  int i;

  asir_ox_init(0);
  asir_ox_push_cmo(cmo0);
  asir_ox_push_cmo(cmo1);
  asir_ox_push_cmo(cmo2);
  asir_ox_push_cmo(cmo3);

  asir_ox_push_cmd(SM_executeFunction);  /* execute function */
  
  asir_ox_pop_cmo(cmo, 20);

  for (i=0; i<20; i++) {
	printf(" %2x ",cmo[i]);
  }
  printf("\n");
}


