/* $OpenXM$ */
#include <stdio.h>
#include <sys/param.h>
#include "oxasir.h"

char *asir_ox_pop_string();

main() {
  char *s;
  asir_ox_init(0);  /* Use the network byte order */

  /* Loading the file xm */
  /* This part generates a core dump. I'm trying to find a reason. */
  asir_ox_push_string("xm");
  asir_ox_push_int32(1);
  asir_ox_push_string("load");
  asir_ox_push_cmd(SM_executeFunction);
  

  asir_ox_push_string("fctr(x^10-1);");
  asir_ox_push_cmd(SM_executeStringByLocalParser);

  /*
  asir_ox_push_string("x^10-1;");
  asir_ox_push_cmd(SM_executeStringByLocalParser);
  asir_ox_push_int32(1);
  asir_ox_push_string("fctr");
  asir_ox_push_cmd(SM_executeFunction);
  */

  s = asir_ox_pop_string();
  printf("%s\n",s);

  printf("------------------------------------------\n");
  asir_ox_push_object_given_by_a_string("12345;");
  s = asir_ox_pop_string();
  printf("%s\n",s);

}

asir_ox_push_int32(int k) {
  static unsigned char p[8];
  *((int *)p) = htonl((int) CMO_INT32);
  *((int *)(p+4)) = htonl(k);
  debug_packet(p,8);
  asir_ox_push_cmo(p);
}

asir_ox_push_string(char *s) {
  unsigned char *p;
  p = (unsigned char *)GC_malloc(strlen(s)+8);
  if (sizeof(int) != 4) {
	fprintf(stderr,"sizeof(int) is not equal to 4.\n"); exit(1);
  }
  *((int *)p) = htonl((int) CMO_STRING);
  *((int *)(p+4)) = htonl((int ) strlen(s));
  strcpy(p+8,s);
  debug_packet(p,strlen(s)+8);
  asir_ox_push_cmo(p);
}

asir_ox_push_object_given_by_a_string(char *s) {
  asir_ox_push_string(s);
  asir_ox_push_cmd(SM_executeStringByLocalParser);
}

char *asir_ox_pop_string() {
  int size;
  char *s;
  int tag;
  asir_ox_push_int32(1);
  asir_ox_push_string("rtostr");
  asir_ox_push_cmd(SM_executeFunction);

  size = asir_ox_peek_cmo_size();
  s = (char *)GC_malloc(size+1);
  asir_ox_pop_cmo(s,size+1);
  s[size] = 0;
  tag = ntohl(*((int *)s));
  if (tag != CMO_STRING) {
	fprintf(stderr,"asir_ox_pop_string: the cmo object is not a string. Returns the empty string.\n");
	return("");
  }else{
	return(s+8);
  }
}
  
debug_packet(char *p,int size) {
  int i;
  for (i=0; i<size; i++) {
    printf(" %2x ",p[i]);
	if ((p[i] >= ' ') && (p[i] < 0x7f)) {
	  printf("(%c) ",p[i]);
	}
  }
  printf("\n");
}

