#include <stdio.h>

main() {
  char s[1024];
  int count = 0;
  printf("char *SymbolTableOfsm1[]={\n");
  while (scanf("%s",s) != -1) {
    count++;
    printf("   \"%s\",\n",s);
  }
  printf("  \"zzzzzzzzDUMMYsYMBOLeND\"};\n");
  count++;
  printf("int SizeOfSymbolTableOfsm1 = %d;\n",count);
  exit(0);
}

