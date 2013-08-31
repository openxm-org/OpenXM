/* $OpenXM: OpenXM/src/asir-doc/html_tools/gen_hh.c,v 1.2 2009/02/22 22:48:15 ohara Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if !defined(_MSC_VER)
#include <dirent.h>
#else 
#include <windows.h>
#endif

void gen_hhp(char *out, int n, char *htmldir)
{
	int i;
	FILE *outf;

	outf = fopen(out,"w");
	fprintf(outf,"[OPTIONS]\n");
	fprintf(outf,"Compatibility=1.1 or later\n");
	fprintf(outf,"Compiled file=asirhelp.chm\n");
	fprintf(outf,"Contents file=asirhelp.hhc\n");
	fprintf(outf,"Default topic=%s\\man_toc.html\n",htmldir);
	fprintf(outf,"Display compile progress=No\n");
	fprintf(outf,"Index file=asirhelp.hhk\n");
	fprintf(outf,"Language=0x411 “ú–{Œê\n\n\n[FILES]\n");

	fprintf(outf,"%s\\man_toc.html\n",htmldir);
	for ( i = 1; i <= n; i++ )
		fprintf(outf,"%s\\man_%d.html\n",htmldir,i);

	fprintf(outf,"\n[INFOTYPES]\n");
}

void conv_toc(char *in,char *out, char *htmldir)
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
			fprintf(outf,"<param name=\"Local\" value=\"%s\\%s\">\n",htmldir,fname);
			fprintf(outf,"</OBJECT>\n");
		}
	}
	fprintf(outf,"</UL>\n</BODY></HTML>\n");
}

void conv_index(char *in, char *out, char *htmldir)
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
			fprintf(outf,"<param name=\"Local\" value=\"%s\\%s\">\n",htmldir,fname);
			fprintf(outf,"</OBJECT>\n");
		}
	}
	fprintf(outf,"</BODY></HTML>\n");
}

#if !defined(_MSC_VER)
int find_files(char *indir,char *prefix) 
{
	DIR *d;
	struct dirent *dent;
	int n=0,n1;
	char *ptr,*ptr1;
	char name[BUFSIZ];
	int len=strlen(prefix);
	d = opendir(indir);
	if(!d) {
		exit(1);
	}
	while ( dent = readdir(d) ) {
		strcpy(name,dent->d_name);
		ptr = name+len;
		ptr = strchr(ptr,'_');
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
	closedir(d);
	return n;
}
#else
int find_files(char *indir,char *prefix) 
{
    HANDLE h;
    WIN32_FIND_DATA fd;
    char *ptr,*ptr1;
    char pattern[BUFSIZ];
    char name[BUFSIZ];
	int n=0,n1;
    int len=strlen(prefix);
    sprintf(pattern, "%s\\%s*_*.*", indir, prefix);
    h = FindFirstFileEx(pattern, FindExInfoStandard, &fd, FindExSearchNameMatch, NULL, 0);
    if(h == INVALID_HANDLE_VALUE) {
        exit(1);
    }
    do {
        strcpy(name,fd.cFileName);
        ptr = name+len;
        ptr = strchr(ptr,'_') + 1;
        ptr1 = strchr(ptr,'.');
        *ptr1 = 0;
        if ( !strcmp(ptr,"toc") )
            continue;
        n1 = atoi(ptr);
        if ( n1 > n )
            n = n1;
    } while(FindNextFile(h, &fd));
    return n;
}
#endif

int main(int argc, char *argv[])
{
	int n;
	char *indir,*outdir;
	char in[BUFSIZ],out[BUFSIZ];
	char *htmldir="html";

	indir = argv[1];
	outdir = argv[2];
	if(argc>3) {
		htmldir=argv[3];
	}

	sprintf(in,"%s/man_toc.html",indir);
	sprintf(out,"%s/asirhelp.hhc",outdir);
	conv_toc(in,out,htmldir);
	n = find_files(indir, "");

	sprintf(in,"%s/man_%d.html",indir,n);
	sprintf(out,"%s/asirhelp.hhk",outdir);
	conv_index(in,out,htmldir);
	sprintf(out,"%s/asirhelp.hhp",outdir);
	gen_hhp(out,n,htmldir);
	exit(0);
}
