/* $OpenXM: OpenXM/doc/oxlib/test1.c,v 1.3 2000/03/16 07:34:37 noro Exp $ */
#include <ox/cmotag.h>
#include <ox/smCommand.h>


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
  int i,size;

  asir_ox_init(1);
  asir_ox_push_cmo(cmo0);
  asir_ox_push_cmo(cmo1);
  asir_ox_push_cmo(cmo2);
  asir_ox_push_cmo(cmo3);

  asir_ox_push_cmd(SM_executeFunction);  /* execute function */
  
  size = asir_ox_peek_cmo_size();
  if (size < 1024) {
	asir_ox_pop_cmo(cmo, size);
  }else{
	fprintf(stderr,"Too big cmo size.\n"); exit(1);
  }

  printf("gcd of 12 and 8, in the cmo format, is \n");
  for (i=0; i<size; i++) {
	printf(" %2x ",cmo[i]);
  }
  printf("\n");
}


