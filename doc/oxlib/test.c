/* $OpenXM$ */
#include <asir/ox.h>

main() {
	char ibuf[BUFSIZ];
	char *s,*buf;
	int len,len0;

	asir_ox_init(1);  /* Use the network byte order */

	len0 = BUFSIZ;
	buf = (char *)malloc(len0);
	while ( 1 ) {
		fgets(ibuf,BUFSIZ,stdin);
		if ( !strncmp(ibuf,"bye",3) )
			exit(0);
		asir_ox_execute_string(ibuf);
		len = asir_ox_peek_cmo_string_length();
		if ( len > len0 ) {
			len0 = len;
			buf = (char *)realloc(buf,len0);
		}
		asir_ox_pop_string(buf,len0);
		printf("%s\n",buf);
	}
}

