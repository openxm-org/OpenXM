/* $OpenXM: OpenXM/src/asir-doc/html_tools/gen_hh.c,v 1.3 2013/08/31 13:45:59 ohara Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if !defined(_MSC_VER)
#include <dirent.h>
#else 
#include <windows.h>
#endif

void gen_hhp(char *out, int n, char *indir, char *prefix_, char *help)
{
	int i;
	FILE *outf;

	outf = fopen(out,"w");
	fprintf(outf,"[OPTIONS]\n");
	fprintf(outf,"Compatibility=1.1 or later\n");
	fprintf(outf,"Compiled file=%s.chm\n",help);
	fprintf(outf,"Contents file=%s.hhc\n",help);
	fprintf(outf,"Default topic=%s\\%s_toc.html\n",indir,prefix_);
	fprintf(outf,"Display compile progress=No\n");
	fprintf(outf,"Index file=%s.hhk\n",help);
	fprintf(outf,"Language=0x411 “ú–{Œê\n\n\n[FILES]\n");

	fprintf(outf,"%s\\%s_toc.html\n",indir,prefix_);
	for ( i = 1; i <= n; i++ )
		fprintf(outf,"%s\\%s_%d.html\n",indir,prefix_,i);

	fprintf(outf,"\n[INFOTYPES]\n");
}

void conv_toc(char *in, char *out, char *prefix_, char *indir)
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
		if ( fname = strstr(buf,prefix_) ) {
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
			fprintf(outf,"<param name=\"Local\" value=\"%s\\%s\">\n",indir,fname);
			fprintf(outf,"</OBJECT>\n");
		}
	}
	fprintf(outf,"</UL>\n</BODY></HTML>\n");
}

void conv_index(char *in, char *out, char *prefix_, char *indir)
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
		if ( fname = strstr(buf,prefix_) ) {
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
			fprintf(outf,"<param name=\"Local\" value=\"%s\\%s\">\n",indir,fname);
			fprintf(outf,"</OBJECT>\n");
		}
	}
	fprintf(outf,"</BODY></HTML>\n");
}

#if !defined(_MSC_VER)
int find_files(char *indir,char *base) 
{
	DIR *d;
	struct dirent *dent;
	int n=0,n1;
	char *ptr,*ptr1;
	char name[BUFSIZ];
	int len=strlen(base);
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
int find_files(char *indir,char *base) 
{
    HANDLE h;
    WIN32_FIND_DATA fd;
    char *ptr,*ptr1;
    char pattern[BUFSIZ];
    char name[BUFSIZ];
	int n=0,n1;
    int len=strlen(base);
    sprintf(pattern, "%s\\%s_*.*", indir, base);
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
	char in[BUFSIZ],out[BUFSIZ],prefix_[BUFSIZ];
	char *prefix,*lang;
	char help[BUFSIZ],base[BUFSIZ];

    indir = argv[1];
    outdir = argv[2];
    prefix = indir;
    if(argc>3) {
        lang = argv[3];
        sprintf(base,"%s-%s",prefix,lang);
    }else {
        sprintf(base,"%s", "man");
    }
	sprintf(help,"%shelp",prefix);
	sprintf(prefix_,"%s_", base);
	sprintf(in,"%s/%s_toc.html",indir,base);
	sprintf(out,"%s/%s.hhc",outdir,help);
	conv_toc(in,out,prefix_,indir);
	n = find_files(indir, base);
	sprintf(in,"%s/%s_%d.html",indir,base,n);
	sprintf(out,"%s/%s.hhk",outdir,help);
	conv_index(in,out,prefix_,indir);
	sprintf(out,"%s/%s.hhp",outdir,help);
	gen_hhp(out,n,indir,base,help);
	return 0;
}
