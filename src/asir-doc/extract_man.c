/* $OpenXM: OpenXM/src/asir-doc/extract_man.c,v 1.2 2009/02/22 16:40:05 ohara Exp $ */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int
main(argc,argv)
int argc;
char **argv;
{
	int c;
	FILE *in,*fp;
	char buf[BUFSIZ*100];
	char *ok,*ng,*bok,*eok,*bng,*eng;

	if ( argc != 3 )
		goto usage;
	if ( !strcmp(argv[1],"ja") || !strcmp(argv[1],"JP") ) {
		ok = "\\JP";
		bok = "\\BJP";
		eok = "\\E";
		ng = "\\EG";
		bng = "\\BEG";
		eng = "\\E";
	} else if ( !strcmp(argv[1],"en") || !strcmp(argv[1],"EG") ) {
		ok = "\\EG";
		bok = "\\BEG";
		eok = "\\E";
		ng = "\\JP";
		bng = "\\BJP";
		eng = "\\E";
	} else
		goto usage;

	in = fopen(argv[2],"rb");
	if ( !in ) {
		fprintf(stderr,"%s : not found",argv[2]);
		exit(0);
	}
	while ( 1 ) {
		if ( !fgets(buf,BUFSIZ,in) )
			exit(0);
		if ( !strncmp(buf,ok,3) )
			fputs(buf+4,stdout);
		else if ( !strncmp(buf,bok,4) ) {
			while ( 1 ) {
				if  ( !fgets(buf,BUFSIZ,in) ) {
					fprintf(stderr,"%s : EOF while %s is active.",argv[2],bok);
					exit(0);
				}
				if ( !strncmp(buf,eok,2) )
					break;
				else
					fputs(buf,stdout);
			}
		} else if ( !strncmp(buf,bng,4) ) {
			while ( 1 ) {
				if  ( !fgets(buf,BUFSIZ,in) ) {
					fprintf(stderr,"%s : EOF while %s is active.",argv[2],bng);
					exit(0);
				}
				if ( !strncmp(buf,eng,2) )
					break;
			}
		} else if ( strncmp(buf,ng,3) )
			fputs(buf,stdout);
	}
usage:
	fprintf(stderr,"usage : extract_man JP|EG texinfofile\n");
}
