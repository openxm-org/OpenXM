/* -*- mode: C; coding: euc-japan -*- */
/* $OpenXM: OpenXM/src/ox_math/math2ox.c,v 1.9 1999/12/14 09:31:55 ohara Exp $ */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <mathlink.h>
#include <unistd.h>
#include <signal.h>
#include <mathlink.h>

#include "ox.h"
#include "parse.h"
#include "serv2.h"

static char *host    = "localhost";
static char *ctlserv = "ox";
static char *oxprog  = "ox_sm1";

ox_file_t sv;
static ox_file_t *svs = NULL;
static int len_svs = 0;
static int max_process = 0;

/* Mathematica から直接呼び出される関数の定義. */
/* 呼び出しの方法は math2ox.tm で定義される.   */
void OX_get()
{
    cmo *c = NULL;

    receive_ox_tag(sv->stream);
    c = receive_cmo(sv->stream);
#ifdef DEBUG
    fprintf(stderr, "ox message is received in OxGet[].\n");
    print_cmo(c);
    fflush(stderr);
#endif
    send_mlo(c);
	ml_flush();
}

int OX_execute_string(const char *str)
{
    ox_execute_string(sv, str);
    return 0;
}

char *OX_popString()
{
    return ox_popString(sv);
}

void OX_popCMO()
{
    cmo *c = ox_pop_cmo(sv);
    send_mlo(c);
	ml_flush();
}

int OX_close()
{
    ox_close(sv);
    return 0;
}

int OX_reset()
{
    ox_reset(sv);
    return 0;
}

/* 文字列 s を parse() にかけて生成された cmo を サーバに送る. */
int OX_parse(char *s)
{
    cmo *m;
    symbol *symp;
    int len = strlen(s);
    init_parser(s);

    if(s != NULL && len > 0 && (m = parse()) != NULL) {
        if (m->tag == OX_DATA) {
            send_ox_cmo(sv->stream, ((ox_data *)m)->cmo);
        }else if (m->tag == OX_COMMAND) {
            send_ox_command(sv->stream, ((ox_command *)m)->command);
        }else {
            send_ox_cmo(sv->stream, m);     
        }
        return 0;
    }
    return -1; /* 失敗した場合 */
}

int OX_start(char* s)
{
    if (s != NULL && s[0] != '\0') {
        oxprog = s;
    }
	if (++max_process < len_svs) {
		sv = ox_start(host, ctlserv, oxprog);
		fprintf(stderr, "open (%s)\n", "localhost");
		svs[max_process] = sv;
		return max_process;
	}
	return -1;
}

int OX_start_insecure(char *host, int portCtl, int portDat)
{
    if (host != NULL || host[0] == '\0') {
        host = "localhost";
    }
    if (portCtl == 0) {
        portCtl = 1200;
    }
    if (portDat == 0) {
        portDat = 1300;
    }
    
    sv = ox_start_insecure_nonreverse(host, portCtl, portDat);
    fprintf(stderr, "math2ox :: connect to \"%s\" with (ctl, dat) = (%d, %d)\n", host, portCtl, portDat);

    return 0;
}

static char *cp_str(char *src)
{
    char *dest = malloc(strlen(src)+1);
    strcpy(dest, src);
    return dest;
}

int OX_setClientParam(char *h, char* c, char* p)
{
    host    = cp_str(h);
    ctlserv = cp_str(c);
    oxprog  = cp_str(p);
    return 0;
}

int main(int argc, char *argv[])
{
    /* 構文解析器の設定 */
    setflag_parse(PFLAG_ADDREV);
	len_svs = 20;
	svs = (ox_file_t *)malloc(sizeof(ox_file_t)*len_svs);
    
    MLMain(argc, argv);
}
