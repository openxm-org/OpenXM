#include <stdio.h>

main ()
{
	int c,c1,i,j;
	char buf[BUFSIZ],fname[BUFSIZ],kname[BUFSIZ];

	while ( 1 ) {
		c = getchar();
		if ( c == EOF )
			exit(0);
		if ( c == '<' ) {
			c1 = getchar();
			if ( c1 == 'A' ) {
				fgets(buf,BUFSIZ,stdin);
				for ( i = 0; buf[i] != '='; i++ );
				i++; i++;
				for ( j = 0; buf[i] != '#'; i++, j++ ) fname[j] = buf[i];
				fname[j] = 0;
				for ( ; buf[i] != '>'; i++ );
				i++;
				if ( buf[i] == '<' ) {
					for ( ; buf[i] != '>'; i++ );
					i++;
				}
				for ( j = 0; buf[i] != '<'; i++, j++ ) kname[j] = buf[i];
				kname[j] = 0;
				printf("<OBJECT type=\"text/sitemap\">\n");
				printf("<param name=\"Name\" value=\"%s\">\n",kname);
				printf("<param name=\"Local\" value=\"html\\%s\">\n",fname);
				printf("</OBJECT>\n");
			} else {
				putchar(c);
				putchar(c1);
				do putchar(c=getchar()); while ( c != '>' );
			}
		} else putchar(c);
	}
}

