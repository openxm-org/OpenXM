#include <stdio.h>
#include <string.h>

main ()
{
	char buf[BUFSIZ];
	char *fname,*ptr,*ptr1;
	int c;

	printf("<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML//EN\">\n");
	printf("<HTML>\n<HEAD>\n");
	printf("<meta name=\"GENERATOR\" content=\"Microsoft&reg; HTML Help Workshop 4.1\">\n");
	printf("<!-- Sitemap 1.0 -->\n</HEAD><BODY>\n");
	printf("<OBJECT type=\"text/site properties\">\n");
	printf("<param name=\"ImageType\" value=\"Folder\">\n");
	printf("</OBJECT>\n<UL>\n");

	while ( 1 ) {
		c = fgetc(stdin);
		if ( c == EOF )
			break;
		ungetc(c,stdin);
		fgets(buf,BUFSIZ,stdin);
		if ( !strncmp(buf,"Jump to:",strlen("Jump to:")) )
			break;
		if ( fname = strstr(buf,"man_") ) {
			ptr = strchr(buf,'#');
			*ptr = 0;
			ptr = strchr(ptr+1,'>');
			ptr++;
			ptr1 = strchr(ptr,'<');
			*ptr1 = 0;
			printf("<LI><OBJECT type=\"text/sitemap\">\n");
			printf("<param name=\"Name\" value=\"%s\">\n",ptr);
			printf("<param name=\"Local\" value=\"html\\%s\">\n",fname);
			printf("</OBJECT>\n");
		}
	}
	printf("</UL>\n</BODY></HTML>\n");
}

