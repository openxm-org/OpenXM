/* $OpenXM: OpenXM/doc/oxlib/test3.c,v 1.2 2000/03/16 07:41:41 noro Exp $ */
#include <asir/ox.h>
#include <signal.h>

main(int argc, char **argv)
{
	char buf[BUFSIZ+1];
	int c;
	unsigned char sendbuf[BUFSIZ+10];
	unsigned char *result;
	unsigned char h[3];
	int len,i,j;
	static int result_len = 0;
	char *kwd,*bdy;
	unsigned int cmd;

	signal(SIGINT,SIG_IGN);
	asir_ox_init(1); /* 1: network byte order; 0: native byte order */
	result_len = BUFSIZ;
	result = (void *)malloc(BUFSIZ);
	while ( 1 ) {
		printf("Input>"); fflush(stdout);
		fgets(buf,BUFSIZ,stdin);
		for ( i = 0; buf[i] && isspace(buf[i]); i++ );
		if ( !buf[i] )
			continue;
		kwd = buf+i;
		for ( ; buf[i] && !isspace(buf[i]); i++ );
		buf[i] = 0;
		bdy = buf+i+1;
		if ( !strcmp(kwd,"asir") ) {
			sprintf(sendbuf,"%s;",bdy);
			asir_ox_execute_string(sendbuf);
		} else if ( !strcmp(kwd,"push") ) {
			h[0] = 0;
			h[2] = 0;
			j = 0;
			while ( 1 ) {
				for ( ; (c= *bdy) && isspace(c); bdy++ );
				if ( !c )
					break;
				else if ( h[0] ) {
					h[1] = c;
					sendbuf[j++] = strtoul(h,0,16);
					h[0] = 0;
				} else
					h[0] = c;
				bdy++;
			}
			if ( h[0] )
				fprintf(stderr,"Number of characters is odd.\n");
			else {
				sendbuf[j] = 0;
				asir_ox_push_cmo(sendbuf);
			}
		} else if ( !strcmp(kwd,"cmd") ) {
			cmd = atoi(bdy);
			asir_ox_push_cmd(cmd);
		} else if ( !strcmp(kwd,"pop") ) {
			len = asir_ox_peek_cmo_size();
			if ( !len )
				continue;
			if ( len > result_len ) {
				result = (char *)realloc(result,len);
				result_len = len;
			}
			asir_ox_pop_cmo(result,len);
			printf("Output>"); fflush(stdout);
			printf("\n");
			for ( i = 0; i < len; ) {
				printf("%02x ",result[i]);
				i++;
				if ( !(i%16) )
					printf("\n");
			}
			printf("\n");
		}
	}
}
