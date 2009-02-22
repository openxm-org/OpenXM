/* $OpenXM$ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

gen_hhp(out,n)
char *out;
int n;
{
	int i;
	FILE *outf;

	outf = fopen(out,"w");
	fprintf(outf,"[OPTIONS]\n");
	fprintf(outf,"Compatibility=1.1 or later\n");
	fprintf(outf,"Compiled file=asirhelp.chm\n");
	fprintf(outf,"Contents file=asirhelp.hhc\n");
	fprintf(outf,"Default topic=html\\man_toc.html\n");
	fprintf(outf,"Display compile progress=No\n");
	fprintf(outf,"Index file=asirhelp.hhk\n");
	fprintf(outf,"Language=0x411 “ú–{Œê\n\n\n[FILES]\n");

	fprintf(outf,"html\\man_toc.html\n");
	for ( i = 1; i <= n; i++ )
		fprintf(outf,"html\\man_%d.html\n",i);

	fprintf(outf,"\n[INFOTYPES]\n");
}

conv_toc(in,out)
char *in,*out;
{
	char buf[BUFSIZ];
	char *fname,*ptr,*ptr1;
	int c;
	FILE *inf,*outf;

	inf = fopen(in,"r");
	outf = fopen(out,"w");
	fprintf(outf,"<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML//EN\">\n");
	fprintf(outf,"<HTML>\n<HEAD>\n");
	fprintf(outf,"<meta name=\"GENERATOR\" content=\"Microsoft&reg; HTML Help Workshop 4.1\">\n");
	fprintf(outf,"<!-- Sitemap 1.0 -->\n</HEAD><BODY>\n");
	fprintf(outf,"<OBJECT type=\"text/site properties\">\n");
	fprintf(outf,"<param name=\"ImageType\" value=\"Folder\">\n");
	fprintf(outf,"</OBJECT>\n<UL>\n");

	while ( 1 ) {
		c = fgetc(inf);
		if ( c == EOF )
			break;
		ungetc(c,inf);
		fgets(buf,BUFSIZ,inf);
		if ( !strncmp(buf,"Jump to:",strlen("Jump to:")) )
			break;
		if ( fname = strstr(buf,"man_") ) {
			ptr = strchr(buf,'#');
			*ptr = 0;
			ptr = strchr(ptr+1,'>');
			ptr++;
			if ( *ptr == '<' )
				ptr = strchr(ptr+1,'>')+1;
			ptr1 = strchr(ptr,'<');
			*ptr1 = 0;
			fprintf(outf,"<LI><OBJECT type=\"text/sitemap\">\n");
			fprintf(outf,"<param name=\"Name\" value=\"%s\">\n",ptr);
			fprintf(outf,"<param name=\"Local\" value=\"html\\%s\">\n",fname);
			fprintf(outf,"</OBJECT>\n");
		}
	}
	fprintf(outf,"</UL>\n</BODY></HTML>\n");
}

conv_index (in,out)
char *in,*out;
{
	char buf[BUFSIZ];
	char *fname,*ptr,*ptr1;
	FILE *inf,*outf;

	inf = fopen(in,"r");
	outf = fopen(out,"w");

	fprintf(outf,"<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML//EN\">\n");
	fprintf(outf,"<HTML>\n<HEAD>\n");
	fprintf(outf,"<meta name=\"GENERATOR\" content=\"Microsoft&reg; HTML Help Workshop 4.1\">\n");
	fprintf(outf,"<!-- Sitemap 1.0 -->\n</HEAD><BODY>\n");
	fprintf(outf,"</HEAD><BODY>\n");

	while ( 1 ) {
		fgets(buf,BUFSIZ,inf);
		if ( !strncmp(buf,"Jump to:",strlen("Jump to:")) )
			break;
	}
	while ( 1 ) {
		fgets(buf,BUFSIZ,inf);
		if ( !strncmp(buf,"Jump to:",strlen("Jump to:")) )
			break;
		if ( fname = strstr(buf,"man_") ) {
			ptr = strchr(buf,'#');
			*ptr = 0;
			ptr = strchr(ptr+1,'>');
			ptr++;
			if ( *ptr == '<' )
				ptr = strchr(ptr+1,'>')+1;
			ptr1 = strchr(ptr,'<');
			*ptr1 = 0;
			fprintf(outf,"<LI><OBJECT type=\"text/sitemap\">\n");
			fprintf(outf,"<param name=\"Name\" value=\"%s\">\n",ptr);
			fprintf(outf,"<param name=\"Local\" value=\"html\\%s\">\n",fname);
			fprintf(outf,"</OBJECT>\n");
		}
	}
	fprintf(outf,"</BODY></HTML>\n");
}

main(argc,argv)
int argc;
char **argv;
{
	DIR *d;
	struct dirent *dent;
	int i,n,n1;
	char *ptr,*ptr1;
	char *indir,*outdir;
	char in[BUFSIZ],out[BUFSIZ],name[BUFSIZ];

	indir = argv[1];
	outdir = argv[2];
	sprintf(in,"%s/man_toc.html",indir);
	sprintf(out,"%s/asirhelp.hhc",outdir);
	conv_toc(in,out);
	d = opendir(indir);
	n = 0;
	while ( dent = readdir(d) ) {
		strcpy(name,dent->d_name);
		ptr = strchr(name,'_');
		if ( !ptr )
			continue;
		ptr++;
		ptr1 = strchr(ptr,'.');
		if ( !ptr1 )
			continue;
		*ptr1 = 0;
		if ( !strcmp(ptr,"toc") )
			continue;
		n1 = atoi(ptr);
		if ( n1 > n )
			n = n1;
	}
	sprintf(in,"%s/man_%d.html",indir,n);
	sprintf(out,"%s/asirhelp.hhk",outdir);
	conv_index(in,out);
	sprintf(out,"%s/asirhelp.hhp",outdir);
	gen_hhp(out,n);
	exit(0);
}
