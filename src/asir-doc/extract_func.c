/* $OpenXM: OpenXM/src/asir-doc/extract_func.c,v 1.1.1.1 1999/12/08 05:47:43 noro Exp $ */

#include <stdio.h>
#include <string.h>

main(argc,argv)
int argc;
char **argv;
{
	int c;
	FILE *in,*fp;
	char buf[BUFSIZ*100];
	char buf1[BUFSIZ*100];
	char *name[BUFSIZ];
	char cmd[BUFSIZ];
	char fn[BUFSIZ];
	char *file;
	int jis;
	int i,j;

	if ( (argc != 2) && (argc != 3) ) {
		fprintf(stderr,"usage : extract_func [-j] infofile\n");
		exit(0);
	}
	if ( argc == 3 ) {
		jis = 1;	
		file = argv[2];
	} else {
		jis = 0;
		file = argv[1];
	}
	in = fopen(file,"rb");
	if ( !in ) {
		fprintf(stderr,"%s : not found",file);
		exit(0);
	}
	fp = 0;
	while ( 1 ) {
		c = getc(in);
		if ( c == EOF )
			exit(0);
		else if ( !fp && c == '\n' )
			continue;
		ungetc(c,in);
		fgets(buf,BUFSIZ,in);
		if ( fname(buf,name) ) {
			fgets(buf1,BUFSIZ,in);
			for ( i = 0; buf1[i] && buf1[i] == '-'; i++ );
			if ( i >= 3 && buf1[i] == '\n' ) {
				fputs(buf,stderr);
				strcpy(fn,name[0]);
				fp = fopen(fn,"w");
				for ( j = 1; name[j]; j++ )
					symlink(fn,name[j]);
				fputs(buf,fp);
				fputs(buf1,fp);
			} else if ( fp ) {
				fputs(buf,fp);
				if ( buf1[0] == 0x1f ) {
					fclose(fp); fp = 0;
					if ( jis ) {
						sprintf(cmd,"nkf %s > %s.tmp; rm -f %s; mv %s.tmp %s",
							name[0],name[0],name[0]);
						system(cmd);
					}
				} else
					fputs(buf1,fp);
			}
		} else if ( fp )
			if ( buf[0] == 0x1f ) {
				fclose(fp); fp = 0;
				if ( jis ) {
					sprintf(cmd,"nkf %s > %s.tmp; rm -f %s; mv %s.tmp %s",
						fn,fn,fn,fn,fn);
					system(cmd);
				}
			} else
				fputs(buf,fp);
	}
}

int fname(buf,name)
char *buf;
char **name;
{
	int i,len;
	char *quote,*bquote,*comma;

	/* XXX */
	if ( *buf != '`' )
		return 0;

	i = 0;
	while ( 1 ) {
		/* search a back quote */
		bquote = index(buf,'`' );
		if ( !bquote )
			return 0;
		buf = bquote+1;

		/* buf points to a function; search a quote */
		quote = index(buf,'\'');
		if ( !quote )
			return 0;
		len = quote-buf;
		name[i] = (char *)malloc(len+1);
		strncpy(name[i],buf,len);
		name[i][len] = 0;
		i++;
		buf = quote+1;
		/* buf points to ',' or a space char; search a comma */
		comma = index(buf,',');
		if ( !comma ) {
			/* if the rest chars include a non-space char, then return 0 */
			while ( *buf && isspace(*buf) ) buf++;
			if ( *buf )
				return 0;
			else {
				name[i] = 0;
				return 1;
			}
		}
	}
}
