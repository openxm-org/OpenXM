#include <stdio.h>
#include "hhh.h"

main() {
	char ibuf[BUFSIZ];
	char s[BUFSIZ+30];
	char *obuf;
	int len,len0;

	asir_ox_init(1);  /* Use the network byte order */

	len0 = BUFSIZ;
	obuf = (char *)malloc(len0);
	while ( 1 ) {
		printf("Input> ");
		fgets(ibuf,BUFSIZ,stdin);
		if ( !strncmp(ibuf,"bye",3) )
			exit(0);
		/* It works for ibuf with many newlines. Change fgets above.
		 */
		sprintf(s,"if (1) { %s ;} else { ; } ;",ibuf); 
		asir_ox_execute_string(ibuf);
		len = asir_ox_peek_cmo_string_length();
		if ( len > len0 ) {
			len0 = len;
			obuf = (char *)realloc(obuf,len0);
		}
		asir_ox_pop_string(obuf,len0);
		printf("Output> %s\n",obuf);
	}
}

