#include <stdio.h>
#include <string.h>
#include <dirent.h>

gen_hhp(out,n,prefix,lang)
char *out;
int n;
char *prefix,*lang;
{
	int i;
	FILE *outf;

	outf = fopen(out,"w");
	fprintf(outf,"[OPTIONS]\n");
	fprintf(outf,"Compatibility=1.1 or later\n");
	fprintf(outf,"Compiled file=%shelp-%s.chm\n",prefix,lang);
	fprintf(outf,"Contents file=%shelp-%s.hhc\n",prefix,lang);
	fprintf(outf,"Default topic=html\\%s-%s_toc.html\n",prefix,lang);
	fprintf(outf,"Display compile progress=No\n");
	fprintf(outf,"Index file=%shelp-%s.hhk\n",prefix,lang);
	fprintf(outf,"Language=0x411 “ú–{Œê\n\n\n[FILES]\n");

	fprintf(outf,"html\\%s-%s_toc.html\n",prefix);
	for ( i = 1; i <= n; i++ )
		fprintf(outf,"html\\%s-%s_%d.html\n",prefix,lang,i);

	fprintf(outf,"\n[INFOTYPES]\n");
}

conv_toc(in,out,prefix_)
char *in,*out,*prefix_;
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
			fprintf(outf,"<param name=\"Local\" value=\"html\\%s\">\n",fname);
			fprintf(outf,"</OBJECT>\n");
		}
	}
	fprintf(outf,"</UL>\n</BODY></HTML>\n");
}

conv_index (in,out,prefix_)
char *in,*out,*prefix_;
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
	char in[BUFSIZ],out[BUFSIZ],name[BUFSIZ],prefix_[BUFSIZ];
	char *prefix,*lang;

	indir = argv[1];
	outdir = argv[2];
	prefix = argv[3];
	lang = argv[4];
	sprintf(in,"%s/%s-%s_toc.html",indir,prefix,lang);
	sprintf(out,"%s/%shelp-%s.hhc",outdir,prefix,lang);
	sprintf(prefix_,"%s-%s_",prefix,lang);
	conv_toc(in,out,prefix_);
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
	sprintf(in,"%s/%s-%s_%d.html",indir,prefix,lang,n);
	sprintf(out,"%s/%shelp-%s.hhk",outdir,prefix,lang);
	conv_index(in,out,prefix_);
	sprintf(out,"%s/%shelp-%s.hhp",outdir,prefix,lang);
	gen_hhp(out,n,prefix,lang);
	exit(0);
}
