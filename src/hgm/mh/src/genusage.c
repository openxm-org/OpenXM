#include <stdio.h>
#include <string.h>
#define SIZE 4098
int main() {
  char s[SIZE];
  int i;
  printf("/* This is automatically generated. Do not edit it.*/\n");
  while (fgets(s,SIZE-1,stdin)!=NULL) {
	if (s[0] == '!') {
	  printf("%s",&(s[1]));
	  continue;
	}
	if (s[0] == '$') {
	  if (strncmp(&(s[1]),"Open",4)==0) {
	    continue;
	  }
	}
	printf("oxprintfe(\"");
	for (i=0; i<strlen(s); i++) {
	  if (s[i] == '\\') printf("\\\\");
	  else if (s[i] == 0xa) printf("\\n");
	  else if (s[i] == '%') printf("%%%%");
	  else if (s[i] == '"') printf("\\\"");
	  else putchar(s[i]);
	}
	printf("\");\n");
  }
  return(0);
}
