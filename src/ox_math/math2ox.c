/* -*- mode: C; coding: euc-japan -*- */
/* $OpenXM$ */
/* $Id$ */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <mathlink.h>
#include <unistd.h>
#include <signal.h>

#include "ox.h"

static char *host    = "localhost";
static char *ctlserv = "ox";
static char *oxprog  = "ox_sm1";

ox_file_t sv;

/* Mathematica から直接呼び出される関数の定義. */
/* 呼び出しの方法は math2ox.tm で定義される.   */
int OX_executeStringByLocalParser(const char *str)
{
    ox_executeStringByLocalParser(sv, str);
    return 0;
}

char *OX_popString()
{
    return ox_popString(sv, sv->stream);
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


int OX_start(char* s)
{
    if (s != NULL && s[0] != '\0') {
        oxprog = s;
    }
    sv = ox_start(host, ctlserv, oxprog);
    fprintf(stderr, "open (%s)\n", "localhost");
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
    MLMain(argc, argv);
}
